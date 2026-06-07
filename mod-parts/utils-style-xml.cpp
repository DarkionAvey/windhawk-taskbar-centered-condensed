#include <cwctype>

// ---- Minimize/maximize animation target correction -------------------------
// The taskbar overflow workaround can give the XAML taskbar a virtual width
// larger than the real monitor width. Explorer may then return a minimize
// animation rectangle in that virtual coordinate space. Keep the virtual
// layout intact, but transform HSHELL_GETMINRECT's rectangle back through the
// same offset/scale used for the visible taskbar island.
//
// IMPORTANT LIFETIME RULE:
// Do not use SetWindowSubclass here. Persistent subclasses attached to Explorer
// windows are unsafe for Windhawk disable/re-enable if removal ever misses. This
// implementation only uses Windhawk-restored function hooks and the taskbar
// wndproc hooks already present in this mod.
#ifndef HSHELL_GETMINRECT
#define HSHELL_GETMINRECT 5
#endif

struct SHELLHOOKINFO_TAI {
  HWND hwnd;
  RECT rc;
};

struct MinimizeAnimationMeasuredButtonTai {
  RECT visibleRectPx{};
  std::wstring automationName;
};

struct MinimizeAnimationCorrectionTai {
  double layoutOffsetXDip{0.0};
  double visualScale{1.0};
  double scaleCenterXDip{0.0};
  double rasterizationScale{1.0};
  RECT monitorRect{};
  RECT clampXRect{};
  bool hasClampXRect{false};
  std::vector<MinimizeAnimationMeasuredButtonTai> measuredButtons;
};

static std::mutex g_minimizeAnimationCorrectionMutexTai;
static std::unordered_map<std::wstring, MinimizeAnimationCorrectionTai> g_minimizeAnimationCorrectionByMonitorNameTai;
static std::atomic_bool g_minimizeAnimationCorrectionReadyTai{false};
static std::atomic_bool g_minimizeAnimationCorrectionUninitializingTai{false};

// Duplicate guard: the same HSHELL_GETMINRECT payload can be observed both by
// SendMessage* and by one of the taskbar wndproc hooks. Keep the guard
// thread-local to avoid locking in this hot message path.
static thread_local LPARAM g_minimizeAnimationLastCorrectedLParamTai = 0;
static thread_local HWND g_minimizeAnimationLastCorrectedHwndTai = nullptr;
static thread_local RECT g_minimizeAnimationLastCorrectedRawTai{};
static thread_local DWORD g_minimizeAnimationLastCorrectedTickTai = 0;

using SendMessageW_t = LRESULT(WINAPI*)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static SendMessageW_t SendMessageW_OriginalTai = nullptr;


static bool ReadShellHookInfoTai(LPARAM lParam, SHELLHOOKINFO_TAI* shellHookInfo) {
  if (!lParam || !shellHookInfo) {
    return false;
  }

#if defined(_MSC_VER)
  __try {
    *shellHookInfo = *reinterpret_cast<const SHELLHOOKINFO_TAI*>(lParam);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
#else
  *shellHookInfo = *reinterpret_cast<const SHELLHOOKINFO_TAI*>(lParam);
#endif

  return true;
}

static bool WriteShellHookRectTai(LPARAM lParam, const RECT& rect) {
  if (!lParam) {
    return false;
  }

#if defined(_MSC_VER)
  __try {
    reinterpret_cast<SHELLHOOKINFO_TAI*>(lParam)->rc = rect;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
#else
  reinterpret_cast<SHELLHOOKINFO_TAI*>(lParam)->rc = rect;
#endif

  return true;
}

static bool GetMonitorRectByNameTai(const std::wstring& monitorName, RECT* rect) {
  if (!rect || monitorName.empty()) {
    return false;
  }

  struct EnumContext {
    const std::wstring* monitorName;
    RECT* rect;
    bool found;
  } context{&monitorName, rect, false};

  EnumDisplayMonitors(
      nullptr,
      nullptr,
      [](HMONITOR monitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
        auto* context = reinterpret_cast<EnumContext*>(lParam);
        if (!context || !context->monitorName || !context->rect) {
          return TRUE;
        }

        MONITORINFOEXW monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor, &monitorInfo) &&
            _wcsicmp(monitorInfo.szDevice, context->monitorName->c_str()) == 0) {
          *context->rect = monitorInfo.rcMonitor;
          context->found = true;
          return FALSE;
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return context.found;
}

static int RectWidthTai(const RECT& rect) {
  return rect.right - rect.left;
}

static int ClampIntTai(int value, int minValue, int maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

static void ClampRectCenterXToBoundsTai(RECT* rect, const RECT& bounds) {
  if (!rect || IsRectEmpty(rect) || IsRectEmpty(&bounds)) {
    return;
  }

  const int width = std::max(1, RectWidthTai(*rect));
  const int currentCenterX = rect->left + width / 2;

  int minCenterX = bounds.left + width / 2;
  int maxCenterX = bounds.right - width / 2;
  if (minCenterX > maxCenterX) {
    minCenterX = maxCenterX = (bounds.left + bounds.right) / 2;
  }

  const int newCenterX = ClampIntTai(currentCenterX, minCenterX, maxCenterX);
  OffsetRect(rect, newCenterX - currentCenterX, 0);
}

static std::wstring ToLowerTai(std::wstring value) {
  std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
    return static_cast<wchar_t>(towlower(ch));
  });
  return value;
}

static std::wstring TrimTai(const std::wstring& value) {
  const auto first = value.find_first_not_of(L" \t\r\n");
  if (first == std::wstring::npos) {
    return L"";
  }
  const auto last = value.find_last_not_of(L" \t\r\n");
  return value.substr(first, last - first + 1);
}

static std::wstring GetWindowTextSafeTai(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) {
    return L"";
  }

  int length = GetWindowTextLengthW(hwnd);
  if (length <= 0 || length > 32767) {
    return L"";
  }

  std::wstring text(static_cast<size_t>(length) + 1, L'\0');
  int copied = GetWindowTextW(hwnd, text.data(), length + 1);
  if (copied <= 0) {
    return L"";
  }
  text.resize(static_cast<size_t>(copied));
  return TrimTai(text);
}

static std::wstring StripExeExtensionTai(std::wstring value) {
  value = TrimTai(value);
  if (value.size() > 4 && _wcsicmp(value.c_str() + value.size() - 4, L".exe") == 0) {
    value.resize(value.size() - 4);
  }
  return value;
}

static bool TryGetTargetWindowProcessBaseNameTai(HWND hwnd, std::wstring* processBaseName) {
  if (!hwnd || !processBaseName || !IsWindow(hwnd)) {
    return false;
  }

  DWORD processId = 0;
  GetWindowThreadProcessId(hwnd, &processId);
  if (!processId) {
    return false;
  }

  std::wstring processName = StripExeExtensionTai(GetProcessFileName(processId));
  if (processName.empty()) {
    return false;
  }

  *processBaseName = processName;
  return true;
}

static bool IsUsableMeasuredButtonRectTai(const RECT& rect) {
  return rect.right > rect.left && rect.bottom > rect.top;
}

static bool TextContainsTai(const std::wstring& haystack, const std::wstring& needle) {
  return !haystack.empty() && !needle.empty() && haystack.find(needle) != std::wstring::npos;
}

static const MinimizeAnimationMeasuredButtonTai* FindMeasuredButtonForTargetWindowTai(
    HWND targetWindow,
    const RECT& currentRect,
    const MinimizeAnimationCorrectionTai& correction,
    int* matchTier) {
  if (!targetWindow || !IsWindow(targetWindow) || correction.measuredButtons.empty()) {
    return nullptr;
  }

  const std::wstring titleLower = ToLowerTai(GetWindowTextSafeTai(targetWindow));
  std::wstring processLower;
  std::wstring processBaseName;
  if (TryGetTargetWindowProcessBaseNameTai(targetWindow, &processBaseName)) {
    processLower = ToLowerTai(processBaseName);
  }

  const int currentCenterX = currentRect.left + std::max(1, RectWidthTai(currentRect)) / 2;

  const MinimizeAnimationMeasuredButtonTai* best = nullptr;
  int bestTier = 0;
  LONG bestDistance = LONG_MAX;

  for (const auto& button : correction.measuredButtons) {
    if (!IsUsableMeasuredButtonRectTai(button.visibleRectPx)) {
      continue;
    }

    const std::wstring nameLower = ToLowerTai(TrimTai(button.automationName));
    if (nameLower.empty()) {
      continue;
    }

    int tier = 0;
    if (!titleLower.empty() && nameLower == titleLower) {
      tier = 3;
    } else if (!titleLower.empty() &&
               (TextContainsTai(nameLower, titleLower) || TextContainsTai(titleLower, nameLower))) {
      tier = 2;
    } else if (!processLower.empty() && TextContainsTai(nameLower, processLower)) {
      tier = 1;
    }

    if (tier <= 0) {
      continue;
    }

    const int buttonCenterX = button.visibleRectPx.left + RectWidthTai(button.visibleRectPx) / 2;
    const LONG distance = buttonCenterX >= currentCenterX ? buttonCenterX - currentCenterX : currentCenterX - buttonCenterX;
    if (!best || tier > bestTier || (tier == bestTier && distance < bestDistance)) {
      best = &button;
      bestTier = tier;
      bestDistance = distance;
    }
  }

  if (matchTier) {
    *matchTier = bestTier;
  }
  return best;
}

static bool TryUseMeasuredTaskbarButtonRectTai(
    HWND targetWindow,
    RECT* rect,
    const MinimizeAnimationCorrectionTai& correction,
    int* matchTier) {
  if (!rect || correction.measuredButtons.empty()) {
    return false;
  }

  const auto* button = FindMeasuredButtonForTargetWindowTai(targetWindow, *rect, correction, matchTier);
  if (!button || !IsUsableMeasuredButtonRectTai(button->visibleRectPx)) {
    return false;
  }

  rect->left = button->visibleRectPx.left;
  rect->right = button->visibleRectPx.right;
  return true;
}

static LONG TransformMinimizeRectXTai(LONG xPx, const MinimizeAnimationCorrectionTai& correction) {
  const double dpiScale = std::max(0.01, correction.rasterizationScale);
  double xDip = (static_cast<double>(xPx) - static_cast<double>(correction.monitorRect.left)) / dpiScale;

  // First apply the RootGrid translation that visually brings the virtual
  // taskbar surface back to the real screen, then apply the same island scale.
  xDip += correction.layoutOffsetXDip;
  xDip = correction.scaleCenterXDip + ((xDip - correction.scaleCenterXDip) * correction.visualScale);

  return static_cast<LONG>(std::lround(static_cast<double>(correction.monitorRect.left) + xDip * dpiScale));
}

static bool GetMinimizeAnimationCorrectionForWindowTai(
    HWND targetWindow,
    MinimizeAnimationCorrectionTai* correction) {
  if (!targetWindow || !correction ||
      !g_minimizeAnimationCorrectionReadyTai.load(std::memory_order_acquire) ||
      g_minimizeAnimationCorrectionUninitializingTai.load(std::memory_order_acquire)) {
    return false;
  }

  if (!IsWindow(targetWindow)) {
    return false;
  }

  HMONITOR monitor = MonitorFromWindow(targetWindow, MONITOR_DEFAULTTONEAREST);
  if (!monitor) {
    return false;
  }

  const std::wstring monitorName = GetMonitorName(monitor);
  if (monitorName.empty()) {
    return false;
  }

  std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
  auto it = g_minimizeAnimationCorrectionByMonitorNameTai.find(monitorName);
  if (it == g_minimizeAnimationCorrectionByMonitorNameTai.end()) {
    return false;
  }

  *correction = it->second;
  return true;
}

static bool WasMinimizeAnimationPayloadAlreadyCorrectedTai(
    LPARAM lParam,
    const SHELLHOOKINFO_TAI& shellHookInfo) {
  if (!lParam) {
    return false;
  }

  if (g_minimizeAnimationLastCorrectedLParamTai != lParam ||
      g_minimizeAnimationLastCorrectedHwndTai != shellHookInfo.hwnd ||
      !EqualRect(&g_minimizeAnimationLastCorrectedRawTai, &shellHookInfo.rc)) {
    return false;
  }

  return GetTickCount() - g_minimizeAnimationLastCorrectedTickTai < 1000;
}

static void RememberMinimizeAnimationCorrectedPayloadTai(
    LPARAM lParam,
    const SHELLHOOKINFO_TAI& shellHookInfo) {
  if (!lParam) {
    return;
  }

  g_minimizeAnimationLastCorrectedLParamTai = lParam;
  g_minimizeAnimationLastCorrectedHwndTai = shellHookInfo.hwnd;
  g_minimizeAnimationLastCorrectedRawTai = shellHookInfo.rc;
  g_minimizeAnimationLastCorrectedTickTai = GetTickCount();
}

static SHORT SignedLowWordTai(LONG value) {
  return static_cast<SHORT>(static_cast<WORD>(value & 0xFFFF));
}

static SHORT SignedHighWordTai(LONG value) {
  return static_cast<SHORT>(static_cast<WORD>((static_cast<DWORD>(value) >> 16) & 0xFFFF));
}

static LONG PackSignedPointTai(LONG x, LONG y) {
  return static_cast<LONG>((static_cast<DWORD>(static_cast<WORD>(y)) << 16) |
                           static_cast<DWORD>(static_cast<WORD>(x)));
}

static bool TryDecodePackedMinimizeRectTai(const SHELLHOOKINFO_TAI& shellHookInfo, RECT* decodedRect) {
  if (!decodedRect) {
    return false;
  }

  // On recent Windows 11 builds, the SHELLHOOKINFO rc for HSHELL_GETMINRECT can
  // arrive as two packed POINTS rather than a normal RECT:
  //   rc.left = MAKELONG(left, top)
  //   rc.top  = MAKELONG(right, bottom)
  //   rc.right/rc.bottom = 0
  // Example raw log: rc=(87304471,93399320,0,0)
  // Decoded: (10519,1332,10520,1425)
  if (shellHookInfo.rc.right != 0 || shellHookInfo.rc.bottom != 0) {
    return false;
  }

  RECT candidate{};
  candidate.left = SignedLowWordTai(shellHookInfo.rc.left);
  candidate.top = SignedHighWordTai(shellHookInfo.rc.left);
  candidate.right = SignedLowWordTai(shellHookInfo.rc.top);
  candidate.bottom = SignedHighWordTai(shellHookInfo.rc.top);

  if (candidate.right < candidate.left) {
    std::swap(candidate.left, candidate.right);
  }
  if (candidate.bottom < candidate.top) {
    std::swap(candidate.top, candidate.bottom);
  }

  if (candidate.left == 0 && candidate.top == 0 &&
      candidate.right == 0 && candidate.bottom == 0) {
    return false;
  }

  // The packed format is often point-like or very narrow, so do not reject
  // small widths. Only reject obviously unusable vertical geometry.
  if (candidate.bottom <= candidate.top) {
    return false;
  }

  *decodedRect = candidate;
  return true;
}

static RECT EncodePackedMinimizeRectTai(const RECT& rect) {
  RECT encoded{};
  encoded.left = PackSignedPointTai(rect.left, rect.top);
  encoded.top = PackSignedPointTai(rect.right, rect.bottom);
  encoded.right = 0;
  encoded.bottom = 0;
  return encoded;
}

static void CorrectMinimizeAnimationRectTai(HWND targetWindow, RECT* rect) {
  if (!targetWindow || !rect || IsRectEmpty(rect)) {
    return;
  }

  MinimizeAnimationCorrectionTai correction{};
  if (!GetMinimizeAnimationCorrectionForWindowTai(targetWindow, &correction)) {
    return;
  }

  RECT before = *rect;

  LONG transformedLeft = TransformMinimizeRectXTai(rect->left, correction);
  LONG transformedRight = TransformMinimizeRectXTai(rect->right, correction);
  if (transformedRight < transformedLeft) {
    std::swap(transformedLeft, transformedRight);
  }

  rect->left = transformedLeft;
  rect->right = transformedRight;

  int measuredMatchTier = 0;
  const bool usedMeasuredButtonRect =
      TryUseMeasuredTaskbarButtonRectTai(targetWindow, rect, correction, &measuredMatchTier);

  if (correction.hasClampXRect) {
    ClampRectCenterXToBoundsTai(rect, correction.clampXRect);
  }

  if (!EqualRect(&before, rect)) {
    Wh_Log(L"[MinRectFix] hwnd=%p before=(%ld,%ld,%ld,%ld) after=(%ld,%ld,%ld,%ld) mode=%s matchTier=%d offsetDip=%.2f scale=%.4f centerDip=%.2f dpiScale=%.3f measuredButtons=%zu",
           targetWindow,
           before.left,
           before.top,
           before.right,
           before.bottom,
           rect->left,
           rect->top,
           rect->right,
           rect->bottom,
           usedMeasuredButtonRect ? L"measured" : L"transform",
           measuredMatchTier,
           correction.layoutOffsetXDip,
           correction.visualScale,
           correction.scaleCenterXDip,
           correction.rasterizationScale,
           correction.measuredButtons.size());
  }
}

static bool TryCorrectShellHookMinRectMessageTai(UINT Msg, WPARAM wParam, LPARAM lParam) {
  if (!g_shellHookMessageTai || Msg != g_shellHookMessageTai ||
      wParam != HSHELL_GETMINRECT || !lParam ||
      g_minimizeAnimationCorrectionUninitializingTai.load(std::memory_order_acquire)) {
    return false;
  }

  SHELLHOOKINFO_TAI shellHookInfo{};
  if (!ReadShellHookInfoTai(lParam, &shellHookInfo)) {
    Wh_Log(L"[MinRectFix] skipped unreadable HSHELL_GETMINRECT payload");
    return true;
  }

  if (!shellHookInfo.hwnd || !IsWindow(shellHookInfo.hwnd)) {
    return true;
  }

  if (WasMinimizeAnimationPayloadAlreadyCorrectedTai(lParam, shellHookInfo)) {
    return true;
  }

  Wh_Log(L"[MinRectFix] HSHELL_GETMINRECT received target=%p raw=(%ld,%ld,%ld,%ld)",
         shellHookInfo.hwnd,
         shellHookInfo.rc.left,
         shellHookInfo.rc.top,
         shellHookInfo.rc.right,
         shellHookInfo.rc.bottom);

  if (!IsRectEmpty(&shellHookInfo.rc)) {
    RECT corrected = shellHookInfo.rc;
    CorrectMinimizeAnimationRectTai(shellHookInfo.hwnd, &corrected);
    if (!EqualRect(&shellHookInfo.rc, &corrected) && WriteShellHookRectTai(lParam, corrected)) {
      SHELLHOOKINFO_TAI remembered = shellHookInfo;
      remembered.rc = corrected;
      RememberMinimizeAnimationCorrectedPayloadTai(lParam, remembered);
    }
    return true;
  }

  RECT decodedPackedRect{};
  if (TryDecodePackedMinimizeRectTai(shellHookInfo, &decodedPackedRect)) {
    RECT before = decodedPackedRect;
    CorrectMinimizeAnimationRectTai(shellHookInfo.hwnd, &decodedPackedRect);

    if (!EqualRect(&before, &decodedPackedRect)) {
      RECT encoded = EncodePackedMinimizeRectTai(decodedPackedRect);
      if (WriteShellHookRectTai(lParam, encoded)) {
        SHELLHOOKINFO_TAI remembered = shellHookInfo;
        remembered.rc = encoded;
        RememberMinimizeAnimationCorrectedPayloadTai(lParam, remembered);
        Wh_Log(L"[MinRectFix] packed decoded before=(%ld,%ld,%ld,%ld) after=(%ld,%ld,%ld,%ld) encoded=(%ld,%ld,%ld,%ld)",
               before.left,
               before.top,
               before.right,
               before.bottom,
               decodedPackedRect.left,
               decodedPackedRect.top,
               decodedPackedRect.right,
               decodedPackedRect.bottom,
               encoded.left,
               encoded.top,
               encoded.right,
               encoded.bottom);
      }
    } else {
      Wh_Log(L"[MinRectFix] packed decoded no-op rect=(%ld,%ld,%ld,%ld)",
             decodedPackedRect.left,
             decodedPackedRect.top,
             decodedPackedRect.right,
             decodedPackedRect.bottom);
    }

    return true;
  }

  Wh_Log(L"[MinRectFix] skipped empty/unrecognized min rect payload");
  return true;
}

static void ClearMinimizeAnimationCorrectionForMonitorTai(const std::wstring& monitorName) {
  if (monitorName.empty()) {
    return;
  }

  std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
  g_minimizeAnimationCorrectionByMonitorNameTai.erase(monitorName);
}

static void SetMinimizeAnimationCorrectionForMonitorTai(
    const std::wstring& monitorName,
    double layoutOffsetXDip,
    double visualScale,
    double scaleCenterXDip,
    double rasterizationScale,
    const RECT* clampXRect,
    const std::vector<MinimizeAnimationMeasuredButtonTai>* measuredButtons) {
  if (monitorName.empty() ||
      g_minimizeAnimationCorrectionUninitializingTai.load(std::memory_order_acquire)) {
    return;
  }

  RECT monitorRect{};
  if (!GetMonitorRectByNameTai(monitorName, &monitorRect) || IsRectEmpty(&monitorRect)) {
    return;
  }

  if (!std::isfinite(layoutOffsetXDip) ||
      !std::isfinite(visualScale) ||
      !std::isfinite(scaleCenterXDip) ||
      !std::isfinite(rasterizationScale)) {
    return;
  }

  MinimizeAnimationCorrectionTai correction{};
  correction.layoutOffsetXDip = layoutOffsetXDip;
  correction.visualScale = std::clamp(visualScale, 0.01, 4.0);
  correction.scaleCenterXDip = scaleCenterXDip;
  correction.rasterizationScale = std::clamp(rasterizationScale, 0.25, 8.0);
  correction.monitorRect = monitorRect;

  if (clampXRect && !IsRectEmpty(clampXRect) &&
      clampXRect->right > clampXRect->left) {
    correction.clampXRect = *clampXRect;
    correction.hasClampXRect = true;
  }

  if (measuredButtons) {
    correction.measuredButtons.reserve(measuredButtons->size());
    for (const auto& button : *measuredButtons) {
      if (IsUsableMeasuredButtonRectTai(button.visibleRectPx)) {
        correction.measuredButtons.push_back(button);
      }
    }
  }

  bool shouldLog = true;
  {
    std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
    auto it = g_minimizeAnimationCorrectionByMonitorNameTai.find(monitorName);
    if (it != g_minimizeAnimationCorrectionByMonitorNameTai.end()) {
      const auto& previous = it->second;
      shouldLog =
          std::abs(previous.layoutOffsetXDip - correction.layoutOffsetXDip) > 0.01 ||
          std::abs(previous.visualScale - correction.visualScale) > 0.0001 ||
          std::abs(previous.scaleCenterXDip - correction.scaleCenterXDip) > 0.01 ||
          std::abs(previous.rasterizationScale - correction.rasterizationScale) > 0.0001 ||
          previous.hasClampXRect != correction.hasClampXRect ||
          previous.measuredButtons.size() != correction.measuredButtons.size() ||
          !EqualRect(&previous.monitorRect, &correction.monitorRect) ||
          (correction.hasClampXRect && !EqualRect(&previous.clampXRect, &correction.clampXRect));
    }
    g_minimizeAnimationCorrectionByMonitorNameTai[monitorName] = correction;
  }

  if (shouldLog) {
    Wh_Log(L"[MinRectFix] correction monitor=%s offsetDip=%.2f scale=%.4f centerDip=%.2f dpiScale=%.3f clamp=%d [%ld..%ld] measuredButtons=%zu",
           monitorName.c_str(),
           correction.layoutOffsetXDip,
           correction.visualScale,
           correction.scaleCenterXDip,
           correction.rasterizationScale,
           correction.hasClampXRect ? 1 : 0,
           correction.hasClampXRect ? correction.clampXRect.left : 0,
           correction.hasClampXRect ? correction.clampXRect.right : 0,
           correction.measuredButtons.size());
  }
}

static LRESULT WINAPI SendMessageW_HookTai(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  LRESULT result = SendMessageW_OriginalTai
      ? SendMessageW_OriginalTai(hWnd, Msg, wParam, lParam)
      : 0;

  TryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam);
  return result;
}

static void InitMinimizeAnimationCorrectionTai() {
  g_minimizeAnimationCorrectionUninitializingTai.store(false, std::memory_order_release);
  g_minimizeAnimationCorrectionReadyTai.store(false, std::memory_order_release);
  g_shellHookMessageTai = RegisterWindowMessageW(L"SHELLHOOK");

  HMODULE user32 = GetModuleHandleW(L"user32.dll");
  if (user32) {
    auto sendMessageW = reinterpret_cast<SendMessageW_t>(GetProcAddress(user32, "SendMessageW"));
    if (sendMessageW) {
      if (WindhawkUtils::Wh_SetFunctionHookT(sendMessageW,
                                             SendMessageW_HookTai,
                                             &SendMessageW_OriginalTai)) {
        Wh_Log(L"[MinRectFix] Successfully hooked SendMessageW");
      } else {
        Wh_Log(L"[MinRectFix] Failed to hook SendMessageW");
      }
    }
  }

  g_minimizeAnimationCorrectionReadyTai.store(g_shellHookMessageTai != 0, std::memory_order_release);
  Wh_Log(L"[MinRectFix] initialized without persistent window subclassing");
}

static void UninitMinimizeAnimationCorrectionTai() {
  g_minimizeAnimationCorrectionUninitializingTai.store(true, std::memory_order_release);
  g_minimizeAnimationCorrectionReadyTai.store(false, std::memory_order_release);

  {
    std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
    g_minimizeAnimationCorrectionByMonitorNameTai.clear();
  }

  g_shellHookMessageTai = 0;
}

std::wstring EscapeXmlAttribute(std::wstring_view data) {
  std::wstring buffer;
  buffer.reserve(data.size());
  for (wchar_t c : data) buffer.append((c == L'&') ? L"&amp;" : (c == L'\"') ? L"&quot;" : (c == L'<') ? L"&lt;" : (c == L'>') ? L"&gt;" : std::wstring(1, c));
  return buffer;
}

Style GetStyleFromXamlSetters(const std::wstring_view type, const std::wstring_view xamlStyleSetters, std::wstring& outXaml) {
  std::wstring xaml =
      LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";
  if (auto pos = type.rfind('.'); pos != type.npos) {
    auto typeNamespace = std::wstring_view(type).substr(0, pos);
    auto typeName = std::wstring_view(type).substr(pos + 1);
    xaml += L"\n    xmlns:windhawkstyler=\"using:";
    xaml += EscapeXmlAttribute(typeNamespace);
    xaml += L"\">\n    <Style TargetType=\"windhawkstyler:";
    xaml += EscapeXmlAttribute(typeName);
    xaml += L"\">\n";
  } else {
    xaml += L">\n    <Style TargetType=\"";
    xaml += EscapeXmlAttribute(type);
    xaml += L"\">\n";
  }
  xaml += xamlStyleSetters;
  xaml +=
      L"    </Style>\n"
      L"</ResourceDictionary>";
  outXaml = xaml;
  auto resourceDictionary = Markup::XamlReader::Load(xaml).as<ResourceDictionary>();
  auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
  return styleInspectable.as<Style>();
}

void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue, bool isXamlValue) {
  if(!obj) return;
  std::wstring outXamlResult;
  try {
    std::wstring xamlSetter = L"<Setter Property=\"";
    xamlSetter += EscapeXmlAttribute(propertyName);
    xamlSetter += L"\"";
    if (isXamlValue) {
      xamlSetter +=
          L">\n"
          L"    <Setter.Value>\n";
      xamlSetter += propertyValue;
      xamlSetter += L"\n    </Setter.Value>\n";
      xamlSetter += L"</Setter>";
    } else {
      xamlSetter += L" Value=\"";
      xamlSetter += EscapeXmlAttribute(propertyValue);
      xamlSetter += L"\"/>";
    }
    auto style = GetStyleFromXamlSetters(type, xamlSetter, outXamlResult);
    for (uint32_t i = 0; i < style.Setters().Size(); ++i) {
      auto setter = style.Setters().GetAt(i).as<Setter>();
      obj.SetValue(setter.Property(), setter.Value());
    }
  } catch (const std::exception& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error: %S. Xaml Result: %s", ex.what(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error: %S", ex.what());
    }
  } catch (const winrt::hresult_error& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error %08X: %s. Xaml Result: %s", ex.code(), ex.message().c_str(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
  } catch (...) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Unknown error occurred while setting property. Xaml Result: %s", outXamlResult.c_str());
    } else {
      Wh_Log(L"Unknown error occurred while setting property.");
    }
  }
}

void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue) { return SetElementPropertyFromString(obj, type, propertyName, propertyValue, false); }
