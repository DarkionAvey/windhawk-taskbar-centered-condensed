
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/////      .___________.     ___       __        /////
/////      |           |    /   \     |  |       /////
/////      `---|  |----`   /  ^  \    |  |       /////
/////          |  |       /  /_\  \   |  |       /////
/////          |  |      /  _____  \  |  |       /////
/////          |__|     /__/     \__\ |__|       /////
/////                                            /////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#include <dwmapi.h>
#include <chrono>
#include <string>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <utility>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <string_view>
#include <vector>
#include <atomic>
#include <cmath>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <thread>
#include <windows.h>
#include <psapi.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <mutex>
#include <condition_variable>
using namespace winrt::Windows::UI::Xaml;

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

std::vector<std::wstring> SplitAndTrim(PCWSTR input) {
  std::vector<std::wstring> result;

  if (!input || *input == L'\0') {
    return result;
  }
  std::wstringstream ss(input);
  std::wstring item;
  while (std::getline(ss, item, L';')) {
    size_t start = item.find_first_not_of(L" \t");
    size_t end = item.find_last_not_of(L" \t");

    if (start != std::wstring::npos && end != std::wstring::npos) {
      std::wstring trimmed = item.substr(start, end - start + 1);
      if (!trimmed.empty()) {
        result.push_back(trimmed);
      }
    }
  }
  return result;
}

bool RegexMatchInsensitive(const std::wstring& haystack, const std::wstring& pattern) {
  try {
    std::wregex regexPattern(pattern, std::regex_constants::icase);
    return std::regex_search(haystack, regexPattern);
  } catch (const std::regex_error&) {
    return false;
  }
}


std::atomic<bool> g_scheduled_low_priority_update = false;
std::atomic<bool> g_delayed_apply_worker_running = false;
std::atomic<int64_t> g_delayed_apply_due_ms = 0;
std::atomic<unsigned long long> g_delayed_apply_generation = 0;
std::atomic<bool> g_initial_style_apply_completed = false;
std::atomic<bool> g_initial_taskbar_size_apply_done = false;
std::atomic<int64_t> g_initial_style_apply_not_before_ms = 0;
std::atomic<int> g_force_style_apply_passes = 0;
std::atomic<int> g_high_priority_dispatch_passes = 0;
std::atomic<int> g_reset_animation_target_passes = 0;
std::atomic<int64_t> g_last_geometry_critical_apply_ms = 0;
std::atomic<bool> g_animation_followup_worker_running = false;
std::atomic<int64_t> g_suppress_low_priority_apply_until_ms = 0;
std::atomic<bool> g_worker_threads_stopping = false;
std::mutex g_delayed_apply_worker_thread_mutex;
std::thread g_delayed_apply_worker_thread;
std::mutex g_delayed_apply_worker_wait_mutex;
std::condition_variable g_delayed_apply_worker_wake;
std::mutex g_animation_followup_worker_thread_mutex;
std::thread g_animation_followup_worker_thread;
constexpr int kDefaultStyleDebounceDelayMs = 150;
constexpr int kTaskbarIslandAnimationDurationMs = 250;
constexpr int kStartButtonAnchorStablePassesRequired = 2;
constexpr double kTaskbarRepeaterVirtualWidthMultiplier = 4.0;
constexpr int kTaskbarVirtualExtraButtonReserve = 128;
constexpr int kTaskbarVirtualOverflowRecoveryButtonReserve = 512;
constexpr int kLowPriorityStyleDelayMs =
    kDefaultStyleDebounceDelayMs + (kTaskbarIslandAnimationDurationMs * 3);
constexpr int kExplorerStartupSettleAnimationWindows = 6;
constexpr int kInitialExplorerStyleDelayMs =
    kLowPriorityStyleDelayMs + kDefaultStyleDebounceDelayMs +
    (kTaskbarIslandAnimationDurationMs * kExplorerStartupSettleAnimationWindows);
constexpr int kGeometryCriticalApplyMinIntervalMs = kDefaultStyleDebounceDelayMs / 2;
constexpr int kInitialExplorerStyleRetryDelayMs = kTaskbarIslandAnimationDurationMs * 2;
constexpr int kButtonSizeLowPrioritySuppressionMs = kTaskbarIslandAnimationDurationMs;
constexpr int kGeometryCriticalLowPrioritySuppressionMs =
    (kTaskbarIslandAnimationDurationMs * 2) + kDefaultStyleDebounceDelayMs;
constexpr int kScheduledLowPriorityFlagTtlMs =
    kLowPriorityStyleDelayMs + kDefaultStyleDebounceDelayMs + kTaskbarIslandAnimationDurationMs;
constexpr double kMinimumTrustedRefreshHz = 24.0;
constexpr double kMaximumTrustedRefreshHz = 1000.0;
constexpr int kDefaultFrameIntervalMs = 16;
constexpr int kMinimumFrameIntervalMs = 1;
constexpr int kMaximumFrameIntervalMs = 42;
constexpr int kAnimationFollowupGraceFrames = 2;
constexpr float kDisplayGeometryChangeToleranceDip = 0.5f;
constexpr float kDisplayRasterizationChangeTolerance = 0.001f;
template <typename TAnimation>
void ConfigureTaskbarIslandAnimation(TAnimation const& animation) {
  animation.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(kTaskbarIslandAnimationDurationMs)));
  animation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(0)));
}

winrt::Windows::UI::Composition::CompositionEasingFunction CreateTaskbarIslandEasingFunction(
    winrt::Windows::UI::Composition::Compositor const& compositor) {
  if (!compositor) {
    return nullptr;
  }

  // Keep every moving piece on the same curve. The explicit ease-out avoids
  // one visual catching up linearly while another decelerates, which reads as
  // rubber-banding when the taskbar island is resized, translated, and scaled
  // at the same time.
  return compositor.CreateCubicBezierEasingFunction(
      winrt::Windows::Foundation::Numerics::float2{0.16f, 1.0f},
      winrt::Windows::Foundation::Numerics::float2{0.30f, 1.0f});
}

template <typename TAnimation, typename TValue>
void InsertTaskbarIslandKeyFrame(TAnimation const& animation,
                                 float progress,
                                 TValue const& value) {
  if (auto compositor = animation.Compositor()) {
    if (auto easing = CreateTaskbarIslandEasingFunction(compositor)) {
      animation.InsertKeyFrame(progress, value, easing);
      return;
    }
  }

  animation.InsertKeyFrame(progress, value);
}

float ApplyTaskbarIslandEasingForEstimate(float progress) {
  progress = std::clamp(progress, 0.0f, 1.0f);
  // Approximation of the compositor ease-out used above. This is only for
  // calculating a safe start value when an in-flight background shape animation
  // is retargeted; all real animations use the compositor easing function.
  const float inv = 1.0f - progress;
  return 1.0f - (inv * inv * inv);
}

float LerpFloat(float from, float to, float progress) {
  return from + ((to - from) * progress);
}
float EstimateAnimationValue(float from, float to, int64_t startedMs, int64_t nowMs) {
  if (startedMs <= 0 || nowMs <= startedMs) {
    return from;
  }
  float progress = static_cast<float>(nowMs - startedMs) / static_cast<float>(kTaskbarIslandAnimationDurationMs);
  if (progress <= 0.0f) {
    return from;
  }
  if (progress >= 1.0f) {
    return to;
  }
   return LerpFloat(from, to, ApplyTaskbarIslandEasingForEstimate(progress));
}

static float SnapToPhysicalPixel(float value, float rasterizationScale);

float SnapScaleForPhysicalPixels(float scale,
                                 float unscaledWidth,
                                 float rasterizationScale) {
  if (unscaledWidth <= 0.0f || !std::isfinite(unscaledWidth) ||
      !std::isfinite(scale)) {
    return 1.0f;
  }

  const float snappedWidth =
      SnapToPhysicalPixel(unscaledWidth * scale, rasterizationScale);
  if (snappedWidth <= 0.0f || !std::isfinite(snappedWidth)) {
    return scale;
  }

  return snappedWidth / unscaledWidth;
}

float CalculateTaskbarIslandScale(float screenLeft,
                                  float screenRight,
                                  float screenWidth,
                                  float scaleCenterX,
                                  float rasterizationScale) {
  if (screenWidth <= 0.0f || screenRight <= screenLeft ||
      !std::isfinite(screenLeft) || !std::isfinite(screenRight) ||
      !std::isfinite(screenWidth) || !std::isfinite(scaleCenterX)) {
    return 1.0f;
  }

  const float unscaledWidth = screenRight - screenLeft;
  float targetScale = 1.0f;

  // Scale only as much as needed to keep the island inside the current screen.
  // This is intentionally not limited by a user setting: once the task area is
  // wider than the monitor, shrinking to any size is safer than allowing
  // Explorer's native overflow layout to appear and destabilize the taskbar.
  if (screenLeft < 0.0f && scaleCenterX > screenLeft) {
    targetScale = std::min(targetScale,
                           scaleCenterX / (scaleCenterX - screenLeft));
  }
  if (screenRight > screenWidth && screenRight > scaleCenterX) {
    targetScale = std::min(targetScale,
                           (screenWidth - scaleCenterX) /
                               (screenRight - scaleCenterX));
  }

  if (!std::isfinite(targetScale) || targetScale <= 0.0f) {
    targetScale = screenWidth / unscaledWidth;
  }
  if (!std::isfinite(targetScale) || targetScale <= 0.0f) {
    return 1.0f;
  }

  targetScale = std::min(targetScale, 1.0f);
  const float snappedScale = SnapScaleForPhysicalPixels(targetScale,
                                                        unscaledWidth,
                                                        rasterizationScale);
  if (std::isfinite(snappedScale) && snappedScale > 0.0f) {
    // Pixel snapping can round the scaled width up by a fraction of a pixel.
    // Never let snapping pick a larger scale than the geometric fit.
    targetScale = std::min(targetScale, snappedScale);
  }
  return std::min(targetScale, 1.0f);
}
float ApplyScaleToScreenX(float screenX, float scaleCenterX, float scale) {
  return scaleCenterX + ((screenX - scaleCenterX) * scale);
}

void SetVisualScaleCenterAndAnimate(
    winrt::Windows::UI::Composition::Visual const& visual,
    float targetScale,
    float localCenterX,
    float localCenterY,
    float visualOffsetTolerance,
    bool animate) {
  if (!visual) {
    return;
  }

  if (!std::isfinite(targetScale) || targetScale <= 0.0f) {
    targetScale = 1.0f;
  }
  if (!std::isfinite(localCenterX)) {
    localCenterX = 0.0f;
  }
  if (!std::isfinite(localCenterY)) {
    localCenterY = 0.0f;
  }

  visual.CenterPoint({localCenterX, localCenterY, visual.CenterPoint().z});

  const auto currentScale = visual.Scale();
  if (std::abs(currentScale.x - targetScale) <= visualOffsetTolerance &&
      std::abs(currentScale.y - targetScale) <= visualOffsetTolerance) {
    return;
  }

  if (animate) {
    if (auto compositor = visual.Compositor()) {
      auto scaleAnimation = compositor.CreateVector3KeyFrameAnimation();
      ConfigureTaskbarIslandAnimation(scaleAnimation);
      InsertTaskbarIslandKeyFrame(
          scaleAnimation,
          1.0f,
          winrt::Windows::Foundation::Numerics::float3{
              targetScale, targetScale, currentScale.z});
      visual.StartAnimation(L"Scale", scaleAnimation);
      return;
    }
  }

  visual.StopAnimation(L"Scale");
  visual.Scale({targetScale, targetScale, currentScale.z});
}
void ResetBackgroundVisualCache(TaskbarState& state) {
  state.lastBackgroundShapeTargetWidth = 0.0f;
  state.lastBackgroundShapeTargetOffsetX = 0.0f;
  state.lastBackgroundShapeTargetOffsetY = 0.0f;
  state.backgroundAnimationFromWidth = 0.0f;
  state.backgroundAnimationToWidth = 0.0f;
  state.backgroundAnimationFromOffsetX = 0.0f;
  state.backgroundAnimationToOffsetX = 0.0f;
  state.backgroundAnimationFromOffsetY = 0.0f;
  state.backgroundAnimationToOffsetY = 0.0f;
  state.backgroundAnimationStartMs = 0;
}
void ResetAnimationTargetCache(TaskbarState& state) {
  state.hasLastTargetTaskFrameOffsetX = false;
  state.hasLastTargetTaskbarIslandScale = false;
  state.lastTargetTaskbarIslandScale = 1.0f;
  state.lastTaskbarIslandScaleCenterX = 0.0f;
  state.hasLastTargetTrayOffsetX = false;
  state.hasLastTargetWidgetOffset = false;
  state.hasLastStartButtonAnchorRect = false;
  state.hasStableStartButtonAnchorRect = false;
  state.startButtonAnchorStablePasses = 0;
  ResetBackgroundVisualCache(state);
}
bool CheckAndUpdateDisplayGeometrySignature(TaskbarState& state,
                                            FrameworkElement const& xamlRootContent,
                                            FrameworkElement const& taskFrame,
                                            float rasterizationScale) {
  const float rootWidth = static_cast<float>(xamlRootContent.ActualWidth());
  const float rootHeight = static_cast<float>(xamlRootContent.ActualHeight());
  const float taskFrameWidth = taskFrame ? static_cast<float>(taskFrame.ActualWidth()) : 0.0f;
  const float taskFrameHeight = taskFrame ? static_cast<float>(taskFrame.ActualHeight()) : 0.0f;

  const bool validSignature =
      std::isfinite(rootWidth) && rootWidth > 0.0f &&
      std::isfinite(rootHeight) && rootHeight > 0.0f &&
      std::isfinite(rasterizationScale) && rasterizationScale > 0.0f;
  if (!validSignature) {
    return false;
  }

  const bool changed =
      !state.hasLastDisplayGeometrySignature ||
      std::abs(state.lastObservedRootWidth - rootWidth) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedRootHeight - rootHeight) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedRasterizationScale - rasterizationScale) > kDisplayRasterizationChangeTolerance ||
      std::abs(state.lastObservedTaskFrameWidth - taskFrameWidth) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedTaskFrameHeight - taskFrameHeight) > kDisplayGeometryChangeToleranceDip;

  state.lastObservedRootWidth = rootWidth;
  state.lastObservedRootHeight = rootHeight;
  state.lastObservedRasterizationScale = rasterizationScale;
  state.lastObservedTaskFrameWidth = taskFrameWidth;
  state.lastObservedTaskFrameHeight = taskFrameHeight;
  state.hasLastDisplayGeometrySignature = true;

  return changed;
}
void ApplySettings(HWND hTaskbarWnd);
int64_t DelayedApplyNowMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
int GetCompositionFrameIntervalMs(HWND hwnd) {
  DWM_TIMING_INFO timing{};
  timing.cbSize = sizeof(timing);
  if (SUCCEEDED(DwmGetCompositionTimingInfo(hwnd, &timing)) &&
      timing.rateRefresh.uiNumerator > 0 &&
      timing.rateRefresh.uiDenominator > 0) {
    const double detectedRefreshHz =
        static_cast<double>(timing.rateRefresh.uiNumerator) /
        static_cast<double>(timing.rateRefresh.uiDenominator);
    const double clampedRefreshHz = std::clamp(detectedRefreshHz,
                                               kMinimumTrustedRefreshHz,
                                               kMaximumTrustedRefreshHz);
    return ClampInt(static_cast<int>((1000.0 / clampedRefreshHz) + 0.5),
                    kMinimumFrameIntervalMs,
                    kMaximumFrameIntervalMs);
  }
  return kDefaultFrameIntervalMs;
}
int GetTaskbarIslandFollowupPassCount(HWND hwnd) {
  const int frameIntervalMs = GetCompositionFrameIntervalMs(hwnd);
  const int followupWindowMs =
      kTaskbarIslandAnimationDurationMs +
      (frameIntervalMs * kAnimationFollowupGraceFrames);
  return std::max(1, (followupWindowMs + frameIntervalMs - 1) / frameIntervalMs);
}
void ArmStyleApplyPasses(int passCount, bool resetAnimationTargets = false) {
  passCount = std::max(1, passCount);
  g_force_style_apply_passes.store(passCount);
  g_high_priority_dispatch_passes.store(passCount);
  if (resetAnimationTargets) {
    g_reset_animation_target_passes.store(passCount);
  }
}
void ArmStyleFollowupPasses(HWND hwnd, bool resetAnimationTargets = false) {
  ArmStyleApplyPasses(GetTaskbarIslandFollowupPassCount(hwnd), resetAnimationTargets);
}
void ArmSingleStyleApplyPass(bool resetAnimationTargets = false) {
  ArmStyleApplyPasses(1, resetAnimationTargets);
}
bool WaitForConditionWithTimeout(std::function<bool()> condition,
                                 int timeoutMs,
                                 int pollIntervalMs) {
  const int64_t startMs = DelayedApplyNowMs();
  while (!condition()) {
    if (g_unloading && timeoutMs > 0) {
      // During unload we still wait briefly for workers/hooks to exit, but never
      // spin forever inside code that may be unloaded by recompilation.
    }
    const int64_t elapsedMs = DelayedApplyNowMs() - startMs;
    if (elapsedMs >= timeoutMs) {
      return false;
    }
    Sleep(static_cast<DWORD>(std::max(1, pollIntervalMs)));
  }
  return true;
}
void QueueTaskbarAnimationFollowup(HWND hTaskbarWnd) {
  if (g_unloading || g_worker_threads_stopping.load() ||
      !hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
    return;
  }
  bool expected = false;
  if (!g_animation_followup_worker_running.compare_exchange_strong(expected, true)) {
    return;
  }

  std::lock_guard<std::mutex> lock(g_animation_followup_worker_thread_mutex);
  if (g_unloading || g_worker_threads_stopping.load()) {
    g_animation_followup_worker_running = false;
    return;
  }
  if (g_animation_followup_worker_thread.joinable()) {
    g_animation_followup_worker_thread.join();
  }

  try {
    g_animation_followup_worker_thread = std::thread([hTaskbarWnd]() {
      struct FollowupWorkerGuard {
        ~FollowupWorkerGuard() { g_animation_followup_worker_running = false; }
      } followupWorkerGuard;
      try {
        const int frameIntervalMs = GetCompositionFrameIntervalMs(hTaskbarWnd);
        const int followupWindowMs =
            kTaskbarIslandAnimationDurationMs +
            (frameIntervalMs * kAnimationFollowupGraceFrames);
        int elapsedMs = 0;
        while (elapsedMs < followupWindowMs) {
          Sleep(static_cast<DWORD>(frameIntervalMs));
          elapsedMs += frameIntervalMs;
          if (g_unloading || g_worker_threads_stopping.load() ||
              !hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
            break;
          }
          ArmSingleStyleApplyPass();
          ApplySettings(hTaskbarWnd);
        }
      } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Animation follow-up worker failed %08X: %s", ex.code(), ex.message().c_str());
      } catch (...) {
        Wh_Log(L"Animation follow-up worker failed: %08X", winrt::to_hresult());
      }
    });
  } catch (std::exception const& ex) {
    g_animation_followup_worker_running = false;
    Wh_Log(L"Failed to create animation follow-up worker: %S", ex.what());
  } catch (...) {
    g_animation_followup_worker_running = false;
    Wh_Log(L"Failed to create animation follow-up worker");
  }
}

void DelayedApplyWorker();
void EnsureDelayedApplyWorker() {
  if (g_unloading || g_worker_threads_stopping.load()) {
    return;
  }

  bool expected = false;
  if (g_delayed_apply_worker_running.compare_exchange_strong(expected, true)) {
    std::lock_guard<std::mutex> lock(g_delayed_apply_worker_thread_mutex);
    if (g_unloading || g_worker_threads_stopping.load()) {
      g_delayed_apply_worker_running = false;
      return;
    }
    if (g_delayed_apply_worker_thread.joinable()) {
      g_delayed_apply_worker_thread.join();
    }
    try {
      g_delayed_apply_worker_thread = std::thread(DelayedApplyWorker);
    } catch (std::exception const& ex) {
      g_delayed_apply_worker_running = false;
      Wh_Log(L"Failed to create delayed apply worker: %S", ex.what());
      return;
    } catch (...) {
      g_delayed_apply_worker_running = false;
      Wh_Log(L"Failed to create delayed apply worker");
      return;
    }
  }

  g_delayed_apply_worker_wake.notify_one();
}

void RequestTaskbarButtonSizeRelayout() {
  if (g_unloading) {
    return;
  }

  // Taskbar buttons are virtualized/recycled. Instead of forcing an arbitrary
  // number of relayout passes, mark button widths as customized and kick one
  // immediate style pass. Every realized/recycled button is validated in the
  // normal taskbar child loop and fixed only if its width is wrong.
  g_taskbarButtonWidthCustomized = true;
  ArmSingleStyleApplyPass(true);
  g_scheduled_low_priority_update = false;
  g_suppress_low_priority_apply_until_ms =
      DelayedApplyNowMs() + kButtonSizeLowPrioritySuppressionMs;

  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
    ApplySettings(hTaskbarWnd);
  }
}

void ArmInitialExplorerStyleApplyDelay() {
  g_initial_style_apply_completed = false;
  g_initial_taskbar_size_apply_done = false;
  g_initial_style_apply_not_before_ms =
      DelayedApplyNowMs() + kInitialExplorerStyleDelayMs;
  Wh_Log(L"Initial Explorer style apply armed");
}
bool InitializeDebounce() {
  // Kept as a compatibility shim for older call sites. The old DispatcherTimer
  // debounce was removed because timer creation/stop could race Explorer/XAML
  // initialization and crash. Scheduling is now handled by DelayedApplyWorker.
  g_worker_threads_stopping = false;
  g_already_requested_debounce_initializing = false;
  return true;
}

void CleanupDebounce() {
  g_worker_threads_stopping = true;
  g_already_requested_debounce_initializing = false;
  g_scheduled_low_priority_update = false;
  g_delayed_apply_due_ms = 0;
  g_delayed_apply_generation.fetch_add(1);
  g_delayed_apply_worker_wake.notify_all();

  std::thread animationFollowupWorker;
  {
    std::lock_guard<std::mutex> lock(g_animation_followup_worker_thread_mutex);
    animationFollowupWorker = std::move(g_animation_followup_worker_thread);
  }

  std::thread delayedApplyWorker;
  {
    std::lock_guard<std::mutex> lock(g_delayed_apply_worker_thread_mutex);
    delayedApplyWorker = std::move(g_delayed_apply_worker_thread);
  }

  if (animationFollowupWorker.joinable()) {
    animationFollowupWorker.join();
  }
  if (delayedApplyWorker.joinable()) {
    delayedApplyWorker.join();
  }

  g_animation_followup_worker_running = false;
  g_delayed_apply_worker_running = false;
}
void DelayedApplyWorker() {
  struct DelayedWorkerGuard {
    ~DelayedWorkerGuard() {
      g_delayed_apply_worker_running = false;
    }
  } delayedWorkerGuard;

  std::unique_lock<std::mutex> waitLock(g_delayed_apply_worker_wait_mutex);
  try {
    for (;;) {
      if (g_unloading || g_worker_threads_stopping.load()) {
        break;
      }

      int64_t dueMs = g_delayed_apply_due_ms.load();
      if (dueMs <= 0) {
        g_delayed_apply_worker_wake.wait(waitLock, [] {
          return g_unloading || g_worker_threads_stopping.load() ||
                 g_delayed_apply_due_ms.load() > 0;
        });
        continue;
      }

      int64_t nowMs = DelayedApplyNowMs();
      if (nowMs < dueMs) {
        g_delayed_apply_worker_wake.wait_for(
            waitLock,
            std::chrono::milliseconds(std::max<int64_t>(1, dueMs - nowMs)),
            [dueMs] {
              return g_unloading || g_worker_threads_stopping.load() ||
                     g_delayed_apply_due_ms.load() != dueMs;
            });
        continue;
      }

      unsigned long long generation = g_delayed_apply_generation.load();
      waitLock.unlock();

      HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
      if (!hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
        Wh_Log(L"Delayed apply postponed: taskbar window is not ready");
        g_delayed_apply_generation.fetch_add(1);
        g_delayed_apply_due_ms = DelayedApplyNowMs() + kInitialExplorerStyleRetryDelayMs;
        waitLock.lock();
        continue;
      }

      g_scheduled_low_priority_update = false;
      Wh_Log(L"Delayed apply triggered");
      const bool initialStyleApply = !g_initial_style_apply_completed.load();
      if (initialStyleApply) {
        ArmStyleFollowupPasses(hTaskbarWnd, true);
      }
      if (!g_initial_style_apply_completed.load() &&
          !g_initial_taskbar_size_apply_done.exchange(true)) {
        ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
      }
      ApplySettings(hTaskbarWnd);
      if (!g_initial_style_apply_completed.load() && !g_unloading) {
        Wh_Log(L"Initial ApplyStyle did not complete; retrying delayed apply");
        g_delayed_apply_generation.fetch_add(1);
        g_delayed_apply_due_ms = DelayedApplyNowMs() + kInitialExplorerStyleRetryDelayMs;
        waitLock.lock();
        continue;
      }

      if (g_delayed_apply_generation.load() == generation) {
        int64_t expectedDueMs = dueMs;
        g_delayed_apply_due_ms.compare_exchange_strong(expectedDueMs, 0);
      }
      waitLock.lock();
    }
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"Delayed apply worker failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (...) {
    Wh_Log(L"Delayed apply worker failed: %08X", winrt::to_hresult());
  }
}
void ApplySettingsDebounced(int delayMs) {
  if (g_unloading) {
    return;
  }
  if (delayMs <= 0) {
    delayMs = kLowPriorityStyleDelayMs;
  }
  if (delayMs < 1) {
    delayMs = kDefaultStyleDebounceDelayMs;
  }
  int64_t nowMs = DelayedApplyNowMs();
  int64_t dueMs = nowMs + delayMs;
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > dueMs) {
    dueMs = initialNotBeforeMs;
  }
  g_delayed_apply_generation.fetch_add(1);
  g_delayed_apply_due_ms = dueMs;
  Wh_Log(L"Scheduled delayed apply in %lld ms", static_cast<long long>(std::max<int64_t>(0, dueMs - nowMs)));
  EnsureDelayedApplyWorker();
}
void ApplySettingsDebounced() {
  ApplySettingsDebounced(kDefaultStyleDebounceDelayMs);
}
void ApplySettingsFromTaskbarThreadImmediately() {
  if (g_unloading) {
    return;
  }
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > DelayedApplyNowMs()) {
    ApplySettingsDebounced(1);
    return;
  }
  g_scheduled_low_priority_update = false;
  g_delayed_apply_due_ms = 0;
  g_delayed_apply_generation.fetch_add(1);
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  ArmStyleFollowupPasses(hTaskbarWnd);
  g_suppress_low_priority_apply_until_ms =
      DelayedApplyNowMs() + kGeometryCriticalLowPrioritySuppressionMs;
  if (!hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
    Wh_Log(L"Immediate apply skipped: taskbar window is not ready");
    return;
  }
  Wh_Log(L"Immediate taskbar animation apply");
  ApplySettings(hTaskbarWnd);
  QueueTaskbarAnimationFollowup(hTaskbarWnd);
}
void ApplySettingsFromTaskbarThreadGeometryChanged() {
  if (g_unloading) {
    return;
  }
  int64_t nowMs = DelayedApplyNowMs();
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > nowMs) {
    ApplySettingsDebounced(1);
    return;
  }

  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  int64_t lastMs = g_last_geometry_critical_apply_ms.load();
  while (nowMs - lastMs >= kGeometryCriticalApplyMinIntervalMs) {
    if (g_last_geometry_critical_apply_ms.compare_exchange_weak(lastMs, nowMs)) {
      g_scheduled_low_priority_update = false;
      ArmStyleFollowupPasses(hTaskbarWnd);
      g_suppress_low_priority_apply_until_ms =
          nowMs + kGeometryCriticalLowPrioritySuppressionMs;
      ApplySettingsDebounced(1);
      if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        QueueTaskbarAnimationFollowup(hTaskbarWnd);
      }
      return;
    }
  }

  ArmStyleFollowupPasses(hTaskbarWnd);
}
void ScheduleInitialExplorerStyleApply() {
  ApplySettingsDebounced(kInitialExplorerStyleDelayMs);
}

bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return true;
  try {
    auto transform = element.TransformToVisual(nullptr);
    winrt::Windows::Foundation::Rect rect = transform.TransformBounds(
        winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
    return rect.Width <= 0.0 || rect.Height <= 0.0 || rect.X < -kLayoutToleranceDip || rect.Y < -kLayoutToleranceDip;
  } catch (...) {
    // Overflow/recycled taskbar elements can briefly be disconnected from the
    // visual tree while Explorer is rebuilding the task list. Treat them as
    // invalid instead of letting a transient XAML exception take down Explorer.
    return true;
  }
}
bool IsTaskbarWidgetsEnabled() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"TaskbarDa", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 1;
        }
        RegCloseKey(hKey);
    }
    return false;
}
static float SnapToPhysicalPixel(float value, float rasterizationScale = 1.0f) {
  if (rasterizationScale <= 0.0f) {
    rasterizationScale = 1.0f;
  }

  float scaledValue = value * rasterizationScale;
  float snappedScaledValue =
      (scaledValue >= 0.0f)
          ? static_cast<float>(static_cast<long>(scaledValue + 0.5f))
          : -static_cast<float>(static_cast<long>(-scaledValue + 0.5f));

  return snappedScaledValue / rasterizationScale;
}

static float GetRasterizationScale(FrameworkElement const& element) {
  if (element) {
    auto xamlRoot = element.XamlRoot();
    if (xamlRoot) {
      float rasterizationScale = static_cast<float>(xamlRoot.RasterizationScale());
      if (rasterizationScale > 0.0f) {
        return rasterizationScale;
      }
    }
  }

  return 1.0f;
}

using StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t = void(WINAPI*)(void* pThis, winrt::Windows::Foundation::Size param1);
StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original;
void WINAPI StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook(void* pThis, winrt::Windows::Foundation::Size param1) {
 if (StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original) {
    StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original(pThis, param1);
  }
    Wh_Log(L"Method called: StartDocked__StartSizingFrame_UpdateWindowRegion (Width: %.2f, Height: %.2f)", param1.Width, param1.Height);
  const int measuredStartMenuWidth = static_cast<int>(param1.Width + 0.5f);
  if (measuredStartMenuWidth > 0 && g_lastRecordedStartMenuWidth != measuredStartMenuWidth) {
    g_lastRecordedStartMenuWidth = measuredStartMenuWidth;
    Wh_SetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
  }
}

std::atomic<int64_t> g_update_flag_set_time_ms = 0;
int64_t NowMs() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
void ResetFlagAfterDelay() {
  std::this_thread::sleep_for(std::chrono::milliseconds(kScheduledLowPriorityFlagTtlMs));
  int64_t now = NowMs();
  int64_t set_time = g_update_flag_set_time_ms.load();
  if (g_scheduled_low_priority_update && (now - set_time >= kScheduledLowPriorityFlagTtlMs)) {
    g_scheduled_low_priority_update = false;
  }
}

void ApplySettingsFromTaskbarThreadIfRequired() {
  if (!g_unloading && DelayedApplyNowMs() < g_suppress_low_priority_apply_until_ms.load()) {
    return;
  }
  if (!g_scheduled_low_priority_update.exchange(true)) {
    g_update_flag_set_time_ms = NowMs();
    Wh_Log(L"Scheduled low priority update");
    ApplySettingsDebounced(-1);
  }
}

void SetDividerForElement(FrameworkElement const& element, float const& panelHeight, bool dividerVisible, bool dividerShouldBeOnLeft = g_settings.userDefinedDividerLeftAligned) {
  if (!element) return;

  if (panelHeight <= 0.0f) return;

  auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
  if (!visual) return;
  auto compositor = visual.Compositor();
  if (!compositor) return;

  auto shapeVisual = compositor.CreateShapeVisual();
  if (!shapeVisual) return;
  dividerVisible = dividerVisible && !g_unloading;
  if (dividerVisible) {
    auto lineGeometry = compositor.CreateLineGeometry();
    if (!lineGeometry) return;
    auto lineShape = compositor.CreateSpriteShape(lineGeometry);
    if (!lineShape) return;

    float borderThicknessFloat = static_cast<float>(g_settings.userDefinedAppsDividerThickness) * 2.0f;
    float scaledHeight = panelHeight * g_settings.userDefinedAppsDividerVerticalScale;
    float yOffset = (panelHeight - scaledHeight) * 0.5f;
    auto size = visual.Size();
    float xOffset = (dividerShouldBeOnLeft) ? 0.0f : (size.x - borderThicknessFloat / 2.0f);
    shapeVisual.Size({borderThicknessFloat, scaledHeight});
    shapeVisual.Offset({xOffset, yOffset, 0.0f});

    lineGeometry.Start({0.0f, 0.0f});
    lineGeometry.End({0.0f, scaledHeight});

    winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
    auto strokeBrush = compositor.CreateColorBrush(borderColor);
    if (!strokeBrush) return;

    lineShape.StrokeBrush(strokeBrush);
    lineShape.StrokeThickness(borderThicknessFloat);
    shapeVisual.Shapes().Append(lineShape);
  }
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, shapeVisual);
}
void ChangeControlCenterIconSize(FrameworkElement const& systemTrayFrameGrid) {
  if (!g_settings.userDefinedStyleTrayArea) return;

  if (auto ControlCenterButton = FindChildByName(systemTrayFrameGrid, L"ControlCenterButton")) {
    if (auto innerGrid = FindChildByClassName(ControlCenterButton, L"Windows.UI.Xaml.Controls.Grid")) {
      if (auto ContentPresenter = FindChildByName(innerGrid, L"ContentPresenter")) {
        if (auto innerItemPresenter = FindChildByClassName(ContentPresenter, L"Windows.UI.Xaml.Controls.ItemsPresenter")) {
          if (auto innerStackPanel = FindChildByClassName(innerItemPresenter, L"Windows.UI.Xaml.Controls.StackPanel")) {
            auto userDefinedTrayIconSizeStr = std::to_wstring(g_settings.userDefinedTrayIconSize);

            int childCount = Media::VisualTreeHelper::GetChildrenCount(innerStackPanel);
            for (int i = 0; i < childCount; ++i) {
              auto child = Media::VisualTreeHelper::GetChild(innerStackPanel, i).try_as<FrameworkElement>();
              if (!child) continue;
              auto SystemTrayIcon = FindChildByName(child, L"SystemTrayIcon");
              if (!SystemTrayIcon) continue;
              auto ContainerGrid = FindChildByName(SystemTrayIcon, L"ContainerGrid");
              if (!ContainerGrid) continue;
              auto ContentGrid = FindChildByName(ContainerGrid, L"ContentGrid");
              if (!ContentGrid) continue;
              auto TextIconContent = FindChildByClassName(ContentGrid, L"SystemTray.TextIconContent");
              if (!TextIconContent) continue;
              auto ContainerGridInner = FindChildByName(TextIconContent, L"ContainerGrid");
              if (!ContainerGridInner) continue;

              if (auto Layer = FindChildByName(ContainerGridInner, L"Underlay")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }

              if (auto Layer = FindChildByName(ContainerGridInner, L"Base")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }

              if (auto Layer = FindChildByName(ContainerGridInner, L"AccentOverlay")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }
            }
          }
        }
      }
    }
  }
}
void ProcessStackPanelChildren(FrameworkElement const& stackPanel, float const& panelHeight) {
  if (!g_settings.userDefinedStyleTrayArea) return;

  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  int childCount = Media::VisualTreeHelper::GetChildrenCount(stackPanel);
  for (int i = 0; i < childCount; ++i) {
    auto child = Media::VisualTreeHelper::GetChild(stackPanel, i).try_as<FrameworkElement>();
    if (!child) continue;

    auto notifyItemIcon = FindChildByName(child, L"NotifyItemIcon");
    if (!notifyItemIcon) continue;

    auto containerGrid = FindChildByName(notifyItemIcon, L"ContainerGrid");
    if (!containerGrid) continue;

    auto innerContentPresenter = FindChildByName(containerGrid, L"ContentPresenter");
    if (!innerContentPresenter) continue;

    auto contentGrid = FindChildByName(innerContentPresenter, L"ContentGrid");
    if (!contentGrid) continue;

    auto imageIconContent = FindChildByClassName(contentGrid, L"SystemTray.ImageIconContent");
    if (!imageIconContent) continue;

    auto innerContainerGrid = FindChildByName(imageIconContent, L"ContainerGrid");
    if (!innerContainerGrid) continue;

    auto image = FindChildByClassName(innerContainerGrid, L"Windows.UI.Xaml.Controls.Image");

    if (!image) continue;

    auto imageCtrl = image.try_as<winrt::Windows::UI::Xaml::Controls::Image>();

    if (!imageCtrl) continue;

    if (g_settings.userDefinedStyleTrayArea) {
      child.Width(g_settings.userDefinedTrayButtonSize);
      child.Height(g_settings.userDefinedTaskbarHeight);

      SetElementPropertyFromString(containerGrid, L"Windows.UI.Xaml.Controls.Grid", L"CornerRadius", userDefinedTaskButtonCornerRadius);

      image.Width(g_settings.userDefinedTrayIconSize);
      image.Height(g_settings.userDefinedTrayIconSize);
    }
  }
}
void StyleNativeDividerElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return;
  using namespace winrt::Windows::UI::Xaml::Hosting;
  using namespace winrt::Windows::Foundation::Numerics;

  element.Opacity(g_unloading ? 1.0f : std::min(1.0f, static_cast<float>(g_settings.userDefinedTaskbarBorderOpacity / 255.0f)));
  element.Width(std::max(0.0, g_settings.userDefinedAppsDividerThickness * 0.99));

  if (auto visual = ElementCompositionPreview::GetElementVisual(element)) {
    if (auto compositor = visual.Compositor()) {
      visual.CenterPoint({0.0f, static_cast<float>(element.ActualHeight()) / 2.0f, 0.0f});
      visual.Scale({1.0f, g_unloading ? 1.0f : g_settings.userDefinedAppsDividerVerticalScale, 1.0f});
    }
  }

  PCWSTR hex = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  PCWSTR originalHex = hex;
  if (!hex || *hex == L'\0') {
    hex = L"#ffffff";
    originalHex = nullptr;
  }

  if (*hex == L'#') ++hex;

  std::wstring fillBrush = L"<SolidColorBrush Color=\"#" + std::wstring(hex) + L"\"/>";
  SetElementPropertyFromString(element, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill", fillBrush.c_str(), true);

  if (originalHex) {
    Wh_FreeStringSetting(originalHex);
  }
}
void InvalidateTaskbarButtonLayoutElement(FrameworkElement const& element) {
  if (!element) {
    return;
  }
  try {
    element.InvalidateMeasure();
    element.InvalidateArrange();
  } catch (...) {
  }
}

double GetEffectiveTaskbarButtonTargetWidth() {
  const double defaultWidth = g_smallIconSize
                                  ? kSystemSmallTaskbarButtonSize
                                  : kSystemMediumTaskbarButtonSize;
  return g_unloading
             ? defaultWidth
             : static_cast<double>(g_smallIconSize
                                       ? g_settings_tbiconsize.taskbarButtonWidthSmall
                                       : g_settings_tbiconsize.taskbarButtonWidth);
}

bool IsExpectedTaskbarButtonDimension(double value, double expected) {
  if (!std::isfinite(value) || value <= 0.0) {
    return false;
  }
  return std::abs(value - expected) <= kLayoutToleranceDip;
}

bool EnsureElementTaskbarButtonWidth(FrameworkElement const& element,
                                     double targetWidth,
                                     bool allowHardWidth) {
  if (!element) {
    return false;
  }

  bool changed = false;
  try {
    if (!IsExpectedTaskbarButtonDimension(element.MinWidth(), targetWidth)) {
      element.MinWidth(targetWidth);
      changed = true;
    }

    if (allowHardWidth) {
      if (!IsExpectedTaskbarButtonDimension(element.MaxWidth(), targetWidth)) {
        element.MaxWidth(targetWidth);
        changed = true;
      }

      const double currentWidth = element.Width();
      const double actualWidth = element.ActualWidth();
      const bool explicitWidthWrong =
          !std::isfinite(currentWidth) ||
          !IsExpectedTaskbarButtonDimension(currentWidth, targetWidth);
      const bool actualWidthWrong =
          actualWidth > 0.0 &&
          !IsExpectedTaskbarButtonDimension(actualWidth, targetWidth);

      if (explicitWidthWrong || actualWidthWrong) {
        element.Width(targetWidth);
        changed = true;
      }
    }

    if (changed) {
      InvalidateTaskbarButtonLayoutElement(element);
    }
  } catch (...) {
  }

  return changed;
}
bool IsStartButtonElement(FrameworkElement const& child,
                          winrt::hstring const& className) {
  if (!child || className != L"Taskbar.ExperienceToggleButton") {
    return false;
  }
  try {
    return Automation::AutomationProperties::GetAutomationId(child) == L"StartButton";
  } catch (...) {
    return false;
  }
}

bool ApplyTaskbarButtonSizeToElement(FrameworkElement const& child,
                                     winrt::hstring const& className) {
  if (!child) {
    return false;
  }

  const bool isTaskbarButton =
      className == L"Taskbar.TaskListButton" ||
      className == L"Taskbar.ExperienceToggleButton" ||
      className == L"Taskbar.OverflowToggleButton" ||
      className == L"Taskbar.SearchBoxButton";
  if (!isTaskbarButton) {
    return false;
  }

  const double targetWidth = static_cast<double>(g_settings.userDefinedTaskbarButtonSize);
  const bool allowHardWidth =
      className != L"Taskbar.SearchBoxButton" ||
      !FindChildByName(child, L"SearchBoxTextBlock");
  bool changed = EnsureElementTaskbarButtonWidth(child, targetWidth, allowHardWidth);

  FrameworkElement innerElementChild = nullptr;
  if (className == L"Taskbar.SearchBoxButton") {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
    if (!innerElementChild) {
      innerElementChild = FindChildByName(child, L"SearchBoxButtonRootPanel");
    }
  } else if (className == L"Taskbar.TaskListButton") {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
    if (!innerElementChild) {
      innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
    }
  } else if (className == L"Taskbar.ExperienceToggleButton") {
    innerElementChild = FindChildByName(child, L"ExperienceToggleButtonRootPanel");
  } else if (className == L"Taskbar.OverflowToggleButton") {
    innerElementChild = FindChildByName(child, L"OverflowToggleButtonRootPanel");
  } else {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
  }

  if (innerElementChild) {
    changed = EnsureElementTaskbarButtonWidth(innerElementChild, targetWidth, allowHardWidth) || changed;
  }

  return changed;
}

double CalculateValidChildrenWidth(FrameworkElement element,
                                   int& childrenCount,
                                   TaskbarState& state,
                                   double* leftMostEdge = nullptr,
                                   double* rightMostEdge = nullptr,
                                   FrameworkElement boundsRelativeTo = nullptr,
                                   double* startButtonLeft = nullptr,
                                   double* startButtonTop = nullptr,
                                   double* startButtonWidth = nullptr,
                                   double* startButtonHeight = nullptr) {
  if (!element) {
    if (leftMostEdge) *leftMostEdge = 0.0;
    if (rightMostEdge) *rightMostEdge = 0.0;
       if (startButtonLeft) *startButtonLeft = 0.0;
    if (startButtonTop) *startButtonTop = 0.0;
    if (startButtonWidth) *startButtonWidth = 0.0;
    if (startButtonHeight) *startButtonHeight = 0.0;
    return 0.0;
  }
  const float tbHeightFloat = static_cast<float>(g_settings.userDefinedTaskbarHeight);
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  auto userDefinedTaskbarIconSize = std::to_wstring(g_settings.userDefinedTaskbarIconSize);
  double totalWidth = 0.0;
  double minEdge = std::numeric_limits<double>::infinity();
  double maxEdge = -std::numeric_limits<double>::infinity();
    double startLeft = 0.0;
  double startTop = 0.0;
  double startWidth = 0.0;
  double startHeight = 0.0;
  bool foundStartButton = false;
  auto transformTarget = boundsRelativeTo ? boundsRelativeTo : element;

  childrenCount = 0;
  int childrenCountTentative = Media::VisualTreeHelper::GetChildrenCount(element);
  for (int i = 0; i < childrenCountTentative; i++) {
    auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
    if (!child) {
      Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCountTentative);
      continue;
    }
    auto className = winrt::get_class_name(child);
    const bool taskbarButtonSizeChanged = ApplyTaskbarButtonSizeToElement(child, className);
    winrt::Windows::Foundation::Rect rect{};
    try {
      if (taskbarButtonSizeChanged) {
        child.UpdateLayout();
      }
     auto transform = child.TransformToVisual(transformTarget);
      rect = transform.TransformBounds(
          winrt::Windows::Foundation::Rect(0, 0, child.ActualWidth(), child.ActualHeight()));
    } catch (...) {
      // A taskbar child can be present in the tree while being recycled during
      // overflow transitions. Skip the transient child for this pass.
      continue;
    }
    // Exclude recycled/transient rectangles, but don't assume that the valid
    // task button group is centered inside TaskbarFrameRepeater. Near overflow,
    // Explorer intentionally lays it out off-center before the overflow button
    // appears, so we measure the real bounds below and translate from them.
    if (rect.Width <= 0.0 || rect.Height <= 0.0 || rect.X < -kLayoutToleranceDip || rect.Y < -kLayoutToleranceDip) {
      continue;
    }
    SetElementPropertyFromString(child, className.c_str(), L"CornerRadius", userDefinedTaskButtonCornerRadius);
    if (className == L"Taskbar.SearchBoxButton") {
      // Search only needs the common sizing helper above.
    } else if (className == L"Taskbar.ExperienceToggleButton") {
       if (IsStartButtonElement(child, className)) {
        startLeft = rect.X;
        startTop = rect.Y;
        startWidth = rect.Width;
        startHeight = rect.Height;
        foundStartButton = true;
        state.lastStartButtonXActual = static_cast<float>(rect.X);
      }
    } else if (className == L"Taskbar.TaskListButton") {
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
      if (innerElementChild) {
        auto iconElementChild = FindChildByName(innerElementChild, L"Icon");
        if (iconElementChild) {
          iconElementChild.Width(g_settings.userDefinedTaskbarIconSize);
          iconElementChild.Height(g_settings.userDefinedTaskbarIconSize);
          auto currentIconAppName = child.GetValue(winrt::Windows::UI::Xaml::Automation::AutomationProperties::NameProperty());
          const std::wstring currentIconAppNameStr = winrt::unbox_value<winrt::hstring>(currentIconAppName).c_str();
          // Wh_Log(L"bbwi: %s", currentIconAppNameStr);
          SetDividerForElement(child, tbHeightFloat, false);
          if (!currentIconAppNameStr.empty()) {
            for (const auto& pat : g_settings.userDefinedDividedAppNames) {
              if (RegexMatchInsensitive(currentIconAppNameStr, pat)) {
                SetDividerForElement(child, tbHeightFloat, true);
                break;
              }
            }
          }
        }
      }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {  // widget element
      child.Margin(Thickness{0, 0, 0, 0});
      auto ExperienceToggleButtonRootPanelElement = FindChildByName(child, L"ExperienceToggleButtonRootPanel");
      if (ExperienceToggleButtonRootPanelElement) {
        ExperienceToggleButtonRootPanelElement.Margin(Thickness{0, 0, 0, 0});
      }
      continue;
    } else if (className == L"Taskbar.OverflowToggleButton") {  // overflow button
      if (auto OverflowToggleButtonRootPanel = FindChildByName(child, L"OverflowToggleButtonRootPanel")) {
        if (auto RightOverflowButtonDivider = FindChildByName(OverflowToggleButtonRootPanel, L"RightOverflowButtonDivider")) {
          if (g_settings.userDefinedTrayAreaDivider) {
            RightOverflowButtonDivider.Opacity(0);
          } else {
            StyleNativeDividerElement(RightOverflowButtonDivider);
          }
        }
      }
    }
    if (auto iconPanelElement = FindChildByName(child, L"IconPanel")) {
      if (auto mostRecentlyUsedDivider = FindChildByName(iconPanelElement, L"MostRecentlyUsedDivider")) {
        StyleNativeDividerElement(mostRecentlyUsedDivider);
      }
      if (auto progressIndicator = FindChildByName(iconPanelElement, L"ProgressIndicator")) {
        if (auto layoutRoot = FindChildByName(progressIndicator, L"LayoutRoot")) {
          if (auto progressBarRoot = FindChildByName(layoutRoot, L"ProgressBarRoot")) {
            if (auto border = FindChildByClassName(progressBarRoot, L"Windows.UI.Xaml.Controls.Border")) {
              if (auto grid = FindChildByClassName(border, L"Windows.UI.Xaml.Controls.Grid")) {
                grid.Height(3.8);
                if (auto progressBarTrack = FindChildByName(grid, L"ProgressBarTrack")) {
                  progressBarTrack.Opacity(0.5);
                }
              }
            }
          }
        }
      } else if (auto runningIndicator = FindChildByName(iconPanelElement, L"RunningIndicator")) {
        runningIndicator.Height(3.5);
        runningIndicator.Opacity(1);
      }
    }
    totalWidth += rect.Width;
     minEdge = std::min(static_cast<double>(minEdge),static_cast<double>( rect.X));
    maxEdge = std::max(static_cast<double>(maxEdge), static_cast<double>(rect.X + rect.Width));

    childrenCount++;
  }
  if (childrenCount > 0 && minEdge != std::numeric_limits<double>::infinity()) {
    if (leftMostEdge) *leftMostEdge = minEdge;
    if (rightMostEdge) *rightMostEdge = maxEdge;
  } else {
    if (leftMostEdge) *leftMostEdge = 0.0;
    if (rightMostEdge) *rightMostEdge = 0.0;
    }
  if (foundStartButton && startWidth > 0.0 && startHeight > 0.0) {
    if (startButtonLeft) *startButtonLeft = startLeft;
    if (startButtonTop) *startButtonTop = startTop;
    if (startButtonWidth) *startButtonWidth = startWidth;
    if (startButtonHeight) *startButtonHeight = startHeight;
  } else {
    if (startButtonLeft) *startButtonLeft = 0.0;
    if (startButtonTop) *startButtonTop = 0.0;
    if (startButtonWidth) *startButtonWidth = 0.0;
    if (startButtonHeight) *startButtonHeight = 0.0;
  }
  return totalWidth;
}

bool TryGetStartButtonRectRelativeTo(FrameworkElement const& taskbarFrameRepeater,
                                     FrameworkElement const& relativeTo,
                                     winrt::Windows::Foundation::Rect& rect) {
  if (!taskbarFrameRepeater || !relativeTo) {
    return false;
  }
  int childrenCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrameRepeater);
  for (int i = 0; i < childrenCount; ++i) {
    auto child = Media::VisualTreeHelper::GetChild(taskbarFrameRepeater, i).try_as<FrameworkElement>();
    if (!child) {
      continue;
    }
    auto className = winrt::get_class_name(child);
    if (!IsStartButtonElement(child, className)) {
      continue;
    }
    try {
      auto transform = child.TransformToVisual(relativeTo);
      rect = transform.TransformBounds(
          winrt::Windows::Foundation::Rect(0, 0, child.ActualWidth(), child.ActualHeight()));
      return rect.Width > 0.0 &&
             rect.Height > 0.0 &&
             std::isfinite(rect.X) &&
             std::isfinite(rect.Y) &&
             rect.X > -kLayoutToleranceDip &&
             rect.Y > -kLayoutToleranceDip;
    } catch (...) {
      return false;
    }
  }
  return false;
}


void DisableElementClip(FrameworkElement const& element) {
  if (!element) {
    return;
  }
  try {
    element.Clip(nullptr);
  } catch (...) {
  }
}

void DisableClipForAncestorChain(FrameworkElement element, int maxDepth = 8) {
  for (int i = 0; element && i < maxDepth; ++i) {
    DisableElementClip(element);
    try {
      element = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    } catch (...) {
      break;
    }
  }
}

bool SetVirtualLayoutWidth(FrameworkElement const& element, double width) {
  if (!element || !std::isfinite(width) || width <= 0.0) {
    return false;
  }

  bool changed = false;
  try {
    if (!IsExpectedTaskbarButtonDimension(element.Width(), width)) {
      element.Width(width);
      changed = true;
    }
    if (!IsExpectedTaskbarButtonDimension(element.MinWidth(), width)) {
      element.MinWidth(width);
      changed = true;
    }
    if (!IsExpectedTaskbarButtonDimension(element.MaxWidth(), width)) {
      element.MaxWidth(width);
      changed = true;
    }
    element.HorizontalAlignment(HorizontalAlignment::Left);
    DisableElementClip(element);
    if (changed) {
      element.InvalidateMeasure();
      element.InvalidateArrange();
    }
  } catch (...) {
  }
  return changed;
}
double CalculateDynamicTaskbarVirtualSurfaceWidth(FrameworkElement const& taskbarFrameRepeater,
                                                 double rootWidth,
                                                 bool isOverflowing) {
  if (g_unloading || !std::isfinite(rootWidth) || rootWidth <= 0.0) {
    return rootWidth;
  }

  int visualChildCount = 0;
  try {
    visualChildCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrameRepeater);
  } catch (...) {
    visualChildCount = 0;
  }

  const double buttonSize = std::max<double>(
      1.0, static_cast<double>(g_settings.userDefinedTaskbarButtonSize));
  const int reserveButtons = isOverflowing
      ? kTaskbarVirtualOverflowRecoveryButtonReserve
      : kTaskbarVirtualExtraButtonReserve;
  const double buttonReserveWidth =
      static_cast<double>(std::max(visualChildCount, 1) + reserveButtons) *
      buttonSize;
  const double screenReserveWidth =
      rootWidth * kTaskbarRepeaterVirtualWidthMultiplier;
  const double virtualWidth = std::max({rootWidth,
                                        screenReserveWidth,
                                        buttonReserveWidth});
  return std::isfinite(virtualWidth) && virtualWidth > rootWidth
      ? virtualWidth
      : rootWidth;
}
void ApplyVirtualTaskbarLayoutSurface(FrameworkElement const& xamlRootContent,
                                      FrameworkElement const& taskFrame,
                                      FrameworkElement const& rootGridTaskBar,
                                      FrameworkElement const& taskbarFrameRepeater,
                                      FrameworkElement const& taskbarBackground,
                                      FrameworkElement const& backgroundFillParent,
                                      FrameworkElement const& backgroundFillChild,
                                      double virtualWidth) {
  if (!std::isfinite(virtualWidth) || virtualWidth <= 0.0) {
    return;
  }

  // Keep the physical taskbar frame at the monitor width. Only the taskbar
  // content surface is virtualized. This gives ItemsRepeater more measure space
  // without making the whole taskbar window logically wider.
  DisableElementClip(xamlRootContent);
  DisableElementClip(taskFrame);
  DisableClipForAncestorChain(rootGridTaskBar);
  SetVirtualLayoutWidth(rootGridTaskBar, virtualWidth);
  SetVirtualLayoutWidth(taskbarFrameRepeater, virtualWidth);
  SetVirtualLayoutWidth(taskbarBackground, virtualWidth);
  SetVirtualLayoutWidth(backgroundFillParent, virtualWidth);
  SetVirtualLayoutWidth(backgroundFillChild, virtualWidth);
}

void SetTaskbarOverflowButtonSuppressed(FrameworkElement const& overflowButton, bool suppress) {
  if (!overflowButton) {
    return;
  }

  try {
    if (suppress) {
      overflowButton.Opacity(0.0);
      overflowButton.IsHitTestVisible(false);
      overflowButton.MinWidth(0.0);
      overflowButton.MaxWidth(0.0);
      overflowButton.Width(0.0);
      overflowButton.Clip(nullptr);
      overflowButton.InvalidateMeasure();
      overflowButton.InvalidateArrange();
      overflowButton.UpdateLayout();
    } else {
      overflowButton.Opacity(1.0);
      overflowButton.IsHitTestVisible(true);
      overflowButton.ClearValue(FrameworkElement::WidthProperty());
      overflowButton.ClearValue(FrameworkElement::MinWidthProperty());
      overflowButton.ClearValue(FrameworkElement::MaxWidthProperty());
    }
  } catch (...) {
  }
}


// ---- Temporary taskbar geometry diagnostics -------------------------------
// Set to false after collecting logs. The throttle keeps ApplyStyle from
// flooding Windhawk logs during insertion/removal animations.
constexpr bool kDebugTaskbarGeometry = true;
constexpr int64_t kDebugTaskbarGeometryMinIntervalMs = 250;
std::atomic<int64_t> g_lastTaskbarGeometryDebugLogMs = 0;

FrameworkElement FindStartButtonElement(FrameworkElement const& taskbarFrameRepeater) {
  if (!taskbarFrameRepeater) {
    return nullptr;
  }
  int childrenCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrameRepeater);
  for (int i = 0; i < childrenCount; ++i) {
    auto child = Media::VisualTreeHelper::GetChild(taskbarFrameRepeater, i).try_as<FrameworkElement>();
    if (!child) {
      continue;
    }
    auto className = winrt::get_class_name(child);
    if (IsStartButtonElement(child, className)) {
      return child;
    }
  }
  return nullptr;
}

bool ShouldLogTaskbarGeometry(bool interesting) {
  if constexpr (!kDebugTaskbarGeometry) {
    return false;
  }
  if (!interesting) {
    return false;
  }
  const int64_t nowMs = DelayedApplyNowMs();
  int64_t lastMs = g_lastTaskbarGeometryDebugLogMs.load();
  while (nowMs - lastMs >= kDebugTaskbarGeometryMinIntervalMs) {
    if (g_lastTaskbarGeometryDebugLogMs.compare_exchange_weak(lastMs, nowMs)) {
      return true;
    }
  }
  return false;
}

bool TryGetDebugBoundsRelativeTo(FrameworkElement const& element,
                                 winrt::Windows::UI::Xaml::UIElement const& relativeTo,
                                 winrt::Windows::Foundation::Rect& rect) {
  if (!element) {
    return false;
  }
  try {
    auto transform = element.TransformToVisual(relativeTo);
    rect = transform.TransformBounds(
        winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
    return std::isfinite(rect.X) &&
           std::isfinite(rect.Y) &&
           std::isfinite(rect.Width) &&
           std::isfinite(rect.Height);
  } catch (...) {
    return false;
  }
}
static bool BuildMeasuredMinimizeAnimationButtonsTai(
    FrameworkElement const& taskbarFrameRepeater,
    FrameworkElement const& rootGridTaskBar,
    const RECT& monitorRect,
    double targetTaskRootOffsetXDip,
    double targetTaskbarIslandScale,
    double targetScaleCenterScreenXDip,
    double rasterizationScale,
    std::vector<MinimizeAnimationMeasuredButtonTai>* measuredButtons) {
  if (!taskbarFrameRepeater || !rootGridTaskBar || !measuredButtons ||
      IsRectEmpty(&monitorRect) ||
      !std::isfinite(targetTaskRootOffsetXDip) ||
      !std::isfinite(targetTaskbarIslandScale) ||
      !std::isfinite(targetScaleCenterScreenXDip) ||
      !std::isfinite(rasterizationScale) ||
      rasterizationScale <= 0.0) {
    return false;
  }

  measuredButtons->clear();

  int childrenCount = 0;
  try {
    childrenCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrameRepeater);
  } catch (...) {
    return false;
  }

  measuredButtons->reserve(static_cast<size_t>(std::max(0, childrenCount)));

  for (int i = 0; i < childrenCount; ++i) {
    FrameworkElement child = nullptr;
    try {
      child = Media::VisualTreeHelper::GetChild(taskbarFrameRepeater, i).try_as<FrameworkElement>();
    } catch (...) {
      child = nullptr;
    }

    if (!child) {
      continue;
    }

    winrt::hstring className;
    try {
      className = winrt::get_class_name(child);
    } catch (...) {
      continue;
    }

    if (className != L"Taskbar.TaskListButton") {
      continue;
    }

    winrt::Windows::Foundation::Rect layoutRect{};
    if (!TryGetDebugBoundsRelativeTo(child, rootGridTaskBar, layoutRect) ||
        layoutRect.Width <= 0.0 ||
        layoutRect.Height <= 0.0 ||
        layoutRect.X < -kLayoutToleranceDip ||
        layoutRect.Y < -kLayoutToleranceDip) {
      continue;
    }

    const double unscaledLeftDip = layoutRect.X + targetTaskRootOffsetXDip;
    const double unscaledRightDip = layoutRect.X + layoutRect.Width + targetTaskRootOffsetXDip;
    const double visibleLeftDip = ApplyScaleToScreenX(
        static_cast<float>(unscaledLeftDip),
        static_cast<float>(targetScaleCenterScreenXDip),
        static_cast<float>(targetTaskbarIslandScale));
    const double visibleRightDip = ApplyScaleToScreenX(
        static_cast<float>(unscaledRightDip),
        static_cast<float>(targetScaleCenterScreenXDip),
        static_cast<float>(targetTaskbarIslandScale));

    if (!std::isfinite(visibleLeftDip) || !std::isfinite(visibleRightDip)) {
      continue;
    }

    RECT visibleRectPx{};
    visibleRectPx.left = monitorRect.left + static_cast<LONG>(std::lround(std::min(visibleLeftDip, visibleRightDip) * rasterizationScale));
    visibleRectPx.right = monitorRect.left + static_cast<LONG>(std::lround(std::max(visibleLeftDip, visibleRightDip) * rasterizationScale));
    // Preserve only the measured X interval. The shell min-rect already carries
    // the correct bottom-taskbar Y coordinates; root-relative XAML Y values are
    // not in the same coordinate space on every monitor/taskbar edge.
    visibleRectPx.top = 1;
    visibleRectPx.bottom = 2;

    if (!IsUsableMeasuredButtonRectTai(visibleRectPx)) {
      continue;
    }

    std::wstring automationName;
    try {
      auto value = child.GetValue(Automation::AutomationProperties::NameProperty());
      automationName = winrt::unbox_value_or<winrt::hstring>(value, L"").c_str();
    } catch (...) {
      automationName.clear();
    }

    measuredButtons->push_back(MinimizeAnimationMeasuredButtonTai{visibleRectPx, TrimTai(automationName)});
  }

  return !measuredButtons->empty();
}

void LogDebugRect(PCWSTR label, bool ok, winrt::Windows::Foundation::Rect const& rect) {
  if (!ok) {
    Wh_Log(L"    %-18s unavailable", label);
    return;
  }
  Wh_Log(L"    %-18s X=%8.2f Y=%8.2f W=%8.2f H=%8.2f R=%8.2f B=%8.2f",
         label,
         rect.X,
         rect.Y,
         rect.Width,
         rect.Height,
         rect.X + rect.Width,
         rect.Y + rect.Height);
}

void LogElementGeometry(PCWSTR tag,
                        FrameworkElement const& element,
                        FrameworkElement const& rootGridTaskBar,
                        FrameworkElement const& taskbarFrameRepeater,
                        FrameworkElement const& taskFrame) {
  if (!element) {
    Wh_Log(L"[TBGEOM] %s: <null>", tag);
    return;
  }

  std::wstring className;
  std::wstring name;
  std::wstring automationId;
  try { className = std::wstring(winrt::get_class_name(element).c_str()); } catch (...) { className = L"<class?>"; }
  try { name = std::wstring(element.Name().c_str()); } catch (...) { name = L""; }
  try { automationId = std::wstring(Automation::AutomationProperties::GetAutomationId(element).c_str()); } catch (...) { automationId = L""; }

  auto parent = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
  std::wstring parentClass;
  std::wstring parentName;
  if (parent) {
    try { parentClass = std::wstring(winrt::get_class_name(parent).c_str()); } catch (...) { parentClass = L"<class?>"; }
    try { parentName = std::wstring(parent.Name().c_str()); } catch (...) { parentName = L""; }
  } else {
    parentClass = L"<null>";
  }

  Thickness margin{};
  winrt::Windows::Foundation::Numerics::float3 actualOffset{};
  try { margin = element.Margin(); } catch (...) {}
  try { actualOffset = element.ActualOffset(); } catch (...) {}

  winrt::Windows::Foundation::Numerics::float3 visualOffset{};
  winrt::Windows::Foundation::Numerics::float2 visualSize{};
  if (auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element)) {
    visualOffset = visual.Offset();
    visualSize = visual.Size();
  }

  Wh_Log(L"[TBGEOM] %s class=%s name=%s aid=%s parent=%s/%s",
         tag,
         className.c_str(),
         name.c_str(),
         automationId.c_str(),
         parentClass.c_str(),
         parentName.c_str());
  Wh_Log(L"    size Actual=%8.2fx%-8.2f W=%8.2f H=%8.2f MinW=%8.2f MaxW=%8.2f Margin=(%.2f,%.2f,%.2f,%.2f)",
         element.ActualWidth(),
         element.ActualHeight(),
         element.Width(),
         element.Height(),
         element.MinWidth(),
         element.MaxWidth(),
         margin.Left,
         margin.Top,
         margin.Right,
         margin.Bottom);
  Wh_Log(L"    ActualOffset=(%.2f,%.2f,%.2f) VisualOffset=(%.2f,%.2f,%.2f) VisualSize=(%.2f,%.2f)",
         actualOffset.x,
         actualOffset.y,
         actualOffset.z,
         visualOffset.x,
         visualOffset.y,
         visualOffset.z,
         visualSize.x,
         visualSize.y);

  winrt::Windows::Foundation::Rect rect{};
  LogDebugRect(L"to-parent", parent ? TryGetDebugBoundsRelativeTo(element, parent, rect) : false, rect);
  LogDebugRect(L"to-repeater", taskbarFrameRepeater ? TryGetDebugBoundsRelativeTo(element, taskbarFrameRepeater, rect) : false, rect);
  LogDebugRect(L"to-rootGrid", rootGridTaskBar ? TryGetDebugBoundsRelativeTo(element, rootGridTaskBar, rect) : false, rect);
  LogDebugRect(L"to-taskFrame", taskFrame ? TryGetDebugBoundsRelativeTo(element, taskFrame, rect) : false, rect);
  LogDebugRect(L"to-xamlRoot", TryGetDebugBoundsRelativeTo(element, winrt::Windows::UI::Xaml::UIElement{nullptr}, rect), rect);
}

void LogAncestorGeometryChain(PCWSTR tag,
                              FrameworkElement element,
                              FrameworkElement const& rootGridTaskBar,
                              int maxDepth = 8) {
  Wh_Log(L"[TBGEOM] ancestor-chain for %s", tag);
  for (int depth = 0; element && depth < maxDepth; ++depth) {
    std::wstring className;
    std::wstring name;
    try { className = std::wstring(winrt::get_class_name(element).c_str()); } catch (...) { className = L"<class?>"; }
    try { name = std::wstring(element.Name().c_str()); } catch (...) { name = L""; }

    Thickness margin{};
    winrt::Windows::Foundation::Numerics::float3 actualOffset{};
    winrt::Windows::Foundation::Numerics::float3 visualOffset{};
    try { margin = element.Margin(); } catch (...) {}
    try { actualOffset = element.ActualOffset(); } catch (...) {}
    if (auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element)) {
      visualOffset = visual.Offset();
    }

    winrt::Windows::Foundation::Rect rect{};
    const bool okRoot = rootGridTaskBar && TryGetDebugBoundsRelativeTo(element, rootGridTaskBar, rect);
    Wh_Log(L"    #%d class=%s name=%s Actual=%6.2fx%-6.2f ActualOffset=(%.2f,%.2f,%.2f) VisualOffset=(%.2f,%.2f,%.2f) Margin=(%.2f,%.2f,%.2f,%.2f) rootRect=%s X=%.2f Y=%.2f W=%.2f H=%.2f",
           depth,
           className.c_str(),
           name.c_str(),
           element.ActualWidth(),
           element.ActualHeight(),
           actualOffset.x,
           actualOffset.y,
           actualOffset.z,
           visualOffset.x,
           visualOffset.y,
           visualOffset.z,
           margin.Left,
           margin.Top,
           margin.Right,
           margin.Bottom,
           okRoot ? L"ok" : L"--",
           okRoot ? rect.X : 0.0,
           okRoot ? rect.Y : 0.0,
           okRoot ? rect.Width : 0.0,
           okRoot ? rect.Height : 0.0);

    element = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
  }
}

void LogTaskbarGeometryProbe(PCWSTR reason,
                             std::wstring const& monitorName,
                             FrameworkElement const& xamlRootContent,
                             FrameworkElement const& taskFrame,
                             FrameworkElement const& rootGridTaskBar,
                             FrameworkElement const& taskbarFrameRepeater,
                             FrameworkElement const& trayFrame,
                             FrameworkElement const& systemTrayFrameGrid,
                             FrameworkElement const& taskbarBackground,
                             FrameworkElement const& backgroundFillChild,
                             FrameworkElement const& startButton,
                             double rootWidth,
                             double childrenWidthTaskbarDbl,
                             double taskbarLeftEdge,
                             double taskbarRightEdge,
                             double startButtonLeft,
                             double startButtonTop,
                             double startButtonWidth,
                             double startButtonHeight,
                             double predictedCenteredTaskbarLeft,
                             bool isOverflowing,
                             bool taskbarLayoutIsEdgeClamped,
                             bool useStableStartButtonAnchor,
                             float targetContentLeft,
                             float targetContentWidth,
                             float targetTaskRootOffsetX,
                             float targetOffsetXTray,
                             float targetBackgroundLeftScreen,
                             float targetBackgroundRightScreen,
                             TaskbarState const& state) {
  Wh_Log(L"[TBGEOM] ===== %s monitor=%s overflow=%d edgeClamp=%d useStableStart=%d =====",
         reason,
         monitorName.c_str(),
         isOverflowing ? 1 : 0,
         taskbarLayoutIsEdgeClamped ? 1 : 0,
         useStableStartButtonAnchor ? 1 : 0);
  Wh_Log(L"[TBGEOM] scalars rootWidth=%.2f taskChildren=%.2f measuredEdges=[%.2f..%.2f] predictedLeft=%.2f",
         rootWidth,
         childrenWidthTaskbarDbl,
         taskbarLeftEdge,
         taskbarRightEdge,
         predictedCenteredTaskbarLeft);
  Wh_Log(L"[TBGEOM] startSample X=%.2f Y=%.2f W=%.2f H=%.2f stable=%d stableRect X=%.2f Y=%.2f W=%.2f H=%.2f stablePasses=%d",
         startButtonLeft,
         startButtonTop,
         startButtonWidth,
         startButtonHeight,
         state.hasStableStartButtonAnchorRect ? 1 : 0,
         state.stableStartButtonAnchorLeft,
         state.stableStartButtonAnchorTop,
         state.stableStartButtonAnchorWidth,
         state.stableStartButtonAnchorHeight,
         state.startButtonAnchorStablePasses);
  Wh_Log(L"[TBGEOM] targets contentLeft=%.2f contentWidth=%.2f rootOffsetX=%.2f trayOffsetX=%.2f bgScreen=[%.2f..%.2f]",
         targetContentLeft,
         targetContentWidth,
         targetTaskRootOffsetX,
         targetOffsetXTray,
         targetBackgroundLeftScreen,
         targetBackgroundRightScreen);

  LogElementGeometry(L"xamlRootContent", xamlRootContent, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"taskFrame", taskFrame, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"rootGridTaskBar", rootGridTaskBar, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"repeater", taskbarFrameRepeater, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"startButton", startButton, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"trayFrame", trayFrame, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"trayGrid", systemTrayFrameGrid, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"taskbarBackground", taskbarBackground, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"backgroundFill", backgroundFillChild, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogAncestorGeometryChain(L"startButton", startButton, rootGridTaskBar);
  LogAncestorGeometryChain(L"repeater", taskbarFrameRepeater, rootGridTaskBar);
  Wh_Log(L"[TBGEOM] ===== end =====");
}
void UpdateGlobalSettings() {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  auto getInt = [&](PCWSTR key) { return Wh_GetIntSetting(key); };
  auto clamp = [](int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; };
  // Booleans
  g_settings.userDefinedFlatTaskbarBottomCorners = (getInt(L"FlatTaskbarBottomCorners") != 0);
  g_settings.userDefinedFullWidthTaskbarBackground = (getInt(L"FullWidthTaskbarBackground") != 0) || g_unloading;
  if (g_settings.userDefinedFullWidthTaskbarBackground) g_settings.userDefinedFlatTaskbarBottomCorners = true;
  g_settings.userDefinedIgnoreShowDesktopButton = (getInt(L"IgnoreShowDesktopButton") != 0);
  g_settings.userDefinedTrayAreaDivider = (getInt(L"TrayAreaDivider") != 0) && !g_unloading;
  g_settings.userDefinedStyleTrayArea = (getInt(L"StyleTrayArea") != 0);
  g_settings.userDefinedAlignFlyoutInner = (getInt(L"AlignFlyoutInner") != 0);
  g_settings.userDefinedCustomizeTaskbarBackground = (getInt(L"CustomizeTaskbarBackground") != 0);
  g_settings.userDefinedDisableCustomBlurBackground = (getInt(L"DisableCustomBlurBackground") != 0);
  PCWSTR appsDividerAlignment = Wh_GetStringSetting(L"AppsDividerAlignment");
  g_settings.userDefinedDividerLeftAligned =
      appsDividerAlignment && _wcsicmp(appsDividerAlignment, L"left") == 0;
  if (appsDividerAlignment) {
    Wh_FreeStringSetting(appsDividerAlignment);
  }
  // Gaps & Padding (non-negative)
  g_settings.userDefinedTrayTaskGap = g_unloading ? 0 : std::max(0, getInt(L"TrayTaskGap"));
  g_settings.userDefinedTaskbarBackgroundHorizontalPadding = g_unloading ? 0 : std::max(0, getInt(L"TaskbarBackgroundHorizontalPadding"));
  // Offset Y (negative up, zero if flat or unloading)
  int offsetY = abs(getInt(L"TaskbarOffsetY"));
  g_settings.userDefinedTaskbarOffsetY = (g_unloading || g_settings.userDefinedFlatTaskbarBottomCorners) ? 0 : -offsetY;
  // Height & Sizes
  int h = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarHeight", kDefaultTaskbarHeight)),
                   kMinTaskbarHeight,
                   kMaxTaskbarHeight);
  g_settings.userDefinedTaskbarHeight = g_unloading ? kSystemMediumTaskbarButtonSize : h;
  int taskbarButtonSize = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarButtonSize", kDefaultTaskbarButtonSize)),
                                   kMinTaskbarButtonSize,
                                   kMaxTaskbarButtonSize);
  g_settings.userDefinedTaskbarButtonSize = g_unloading ? kSystemMediumTaskbarButtonSize : taskbarButtonSize;
  const int maxTaskbarIconSize =
      GetMaxTaskbarIconSizeForLayout(static_cast<int>(g_settings.userDefinedTaskbarHeight),
                                     static_cast<int>(g_settings.userDefinedTaskbarButtonSize));
  int taskbarIconSize = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarIconSize", kDefaultTaskbarIconSize)),
                                 kMinTaskbarIconSize,
                                 maxTaskbarIconSize);
  g_settings.userDefinedTaskbarIconSize = g_unloading ? kSystemMediumTaskbarIconSize : taskbarIconSize;
  g_settings.userDefinedTrayIconSize = std::max(kMinTrayIconSize, getInt(L"TrayIconSize"));
  g_settings.userDefinedTrayButtonSize = std::max(kMinTrayButtonSize, getInt(L"TrayButtonSize"));
  // Corner radii
  float tcr = float(fmax(0.0f, getInt(L"TaskbarCornerRadius")));
  tcr = fmin(tcr, g_settings.userDefinedTaskbarHeight / 2.0f);
  g_settings.userDefinedTaskbarCornerRadius = g_unloading ? 0.0f : tcr;
  int btnCr = clamp(abs(getInt(L"TaskButtonCornerRadius")), 0, g_settings.userDefinedTaskbarHeight / 2);
  g_settings.userDefinedTaskButtonCornerRadius = g_unloading ? 0 : btnCr;
  // Opacities & tints (0–100)
  int bgOp = clamp(abs(getInt(L"TaskbarBackgroundOpacity")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundOpacity = bgOp;
  g_settings.userDefinedTaskbarBackgroundTint = clamp(abs(getInt(L"TaskbarBackgroundTint")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundLuminosity = clamp(abs(getInt(L"TaskbarBackgroundLuminosity")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundBlurAmount = clamp(abs(getInt(L"TaskbarBackgroundBlurAmount")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundTintSaturation = clamp(abs(getInt(L"TaskbarBackgroundTintSaturation")), 0, 500);
  g_settings.userDefinedTaskbarBackgroundInversion = clamp(abs(getInt(L"TaskbarBackgroundInversion")), 0, 100);
  PCWSTR bgTintColor = Wh_GetStringSetting(L"TaskbarBackgroundTintColor");
  g_settings.userDefinedTaskbarBackgroundTintColor = (bgTintColor && *bgTintColor) ? bgTintColor : L"{ThemeResource CardStrokeColorDefaultSolid}";
  if (bgTintColor) {
    Wh_FreeStringSetting(bgTintColor);
  }
  PCWSTR bgFallbackColor = Wh_GetStringSetting(L"TaskbarBackgroundFallbackColor");
  g_settings.userDefinedTaskbarBackgroundFallbackColor = (bgFallbackColor && *bgFallbackColor) ? bgFallbackColor : L"{ThemeResource CardStrokeColorDefaultSolid}";
  if (bgFallbackColor) {
    Wh_FreeStringSetting(bgFallbackColor);
  }
  // Border opacity: 0–255
  int bOp = clamp(abs(getInt(L"TaskbarBorderOpacity")), 0, 100);
  g_settings.userDefinedTaskbarBorderOpacity = uint8_t(round(bOp * 2.55f));
  // Border thickness: 0.0–10.0 (10% of [0–100])
  g_settings.userDefinedTaskbarBorderThickness = g_unloading ? 0.0f : (clamp(abs(getInt(L"TaskbarBorderThickness")), 0, 100) * 0.1f);
  g_settings.userDefinedAppsDividerThickness = g_unloading ? 0.0f : (clamp(abs(getInt(L"AppsDividerThickness")), 0, 100) * 0.1f);
  g_settings.userDefinedAppsDividerVerticalScale = g_unloading ? 0.0f : (clamp(abs(getInt(L"AppsDividerVerticalScale")), 0, 100) / 100.0f);
  // Border color
  PCWSTR hex = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  PCWSTR originalHex = hex;
  if (!hex || *hex == L'\0') {
    hex = L"#ffffff";
    originalHex = nullptr;
  }
  if (*hex == L'#') ++hex;
  unsigned int r = 255, g = 255, b = 255;
  if (swscanf_s(hex, L"%02x%02x%02x", &r, &g, &b) != 3) {
    r = g = b = 255;
  }
  g_settings.borderColorR = r;
  g_settings.borderColorG = g;
  g_settings.borderColorB = b;
  if (originalHex) {
    Wh_FreeStringSetting(originalHex);
  }
  // String list
  PCWSTR dividerAppNames = Wh_GetStringSetting(L"DividedAppNames");
  g_settings.userDefinedDividedAppNames = SplitAndTrim(dividerAppNames);
  if (dividerAppNames) {
    Wh_FreeStringSetting(dividerAppNames);
  }
}
bool HasInvalidSettings() {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  if (g_settings.userDefinedTrayTaskGap < 0) return true;
  if (g_settings.userDefinedTaskbarBackgroundHorizontalPadding < 0) return true;
  if ((int)g_settings.userDefinedTaskbarOffsetY < 0 && !g_settings.userDefinedFlatTaskbarBottomCorners) return true;
  if (g_settings.userDefinedTaskbarHeight < kMinTaskbarHeight || g_settings.userDefinedTaskbarHeight > kMaxTaskbarHeight) return true;
  if (g_settings.userDefinedTaskbarIconSize <= 0) return true;
  if (g_settings.userDefinedTrayIconSize <= 0) return true;
  if (g_settings.userDefinedTaskbarButtonSize <= 0) return true;
  if (g_settings.userDefinedTrayButtonSize <= 0) return true;
  if (g_settings.userDefinedTaskbarCornerRadius < 0.0f || g_settings.userDefinedTaskbarCornerRadius > (g_settings.userDefinedTaskbarHeight / 2.0f)) return true;
  if (g_settings.userDefinedTaskButtonCornerRadius < 0 || g_settings.userDefinedTaskButtonCornerRadius > (g_settings.userDefinedTaskbarHeight / 2)) return true;
  if (g_settings.userDefinedTaskbarBackgroundOpacity > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundTint > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundLuminosity > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundBlurAmount > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundTintSaturation > 500) return true;
  if (g_settings.userDefinedTaskbarBackgroundInversion > 100) return true;
  if (g_settings.userDefinedTaskbarBorderOpacity > 255) return true;
  if (g_settings.userDefinedTaskbarBorderThickness < 0.0 || g_settings.userDefinedTaskbarBorderThickness > 10.0) return true;
  return false;
}
void LogAllSettings() {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayTaskGap, L"userDefinedTrayTaskGap");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundHorizontalPadding, L"userDefinedTaskbarBackgroundHorizontalPadding");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarOffsetY, L"userDefinedTaskbarOffsetY");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarHeight, L"userDefinedTaskbarHeight");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarIconSize, L"userDefinedTaskbarIconSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayIconSize, L"userDefinedTrayIconSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarButtonSize, L"userDefinedTaskbarButtonSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayButtonSize, L"userDefinedTrayButtonSize");
  Wh_Log(L"setting %d %s", (int)g_settings.userDefinedTaskbarCornerRadius, L"userDefinedTaskbarCornerRadius");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskButtonCornerRadius, L"userDefinedTaskButtonCornerRadius");
  Wh_Log(L"setting %d %s", g_settings.userDefinedFlatTaskbarBottomCorners ? 1 : 0, L"userDefinedFlatTaskbarBottomCorners");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundOpacity, L"userDefinedTaskbarBackgroundOpacity");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundTint, L"userDefinedTaskbarBackgroundTint");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundLuminosity, L"userDefinedTaskbarBackgroundLuminosity");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundBlurAmount, L"userDefinedTaskbarBackgroundBlurAmount");
  Wh_Log(L"setting %s %s", g_settings.userDefinedTaskbarBackgroundTintColor.c_str(), L"userDefinedTaskbarBackgroundTintColor");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundTintSaturation, L"userDefinedTaskbarBackgroundTintSaturation");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundInversion, L"userDefinedTaskbarBackgroundInversion");
  Wh_Log(L"setting %s %s", g_settings.userDefinedTaskbarBackgroundFallbackColor.c_str(), L"userDefinedTaskbarBackgroundFallbackColor");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBorderOpacity, L"userDefinedTaskbarBorderOpacity");
  Wh_Log(L"setting %d %s", (int)(g_settings.userDefinedTaskbarBorderThickness * 100.0 / 10.0), L"userDefinedTaskbarBorderThickness (scaled)");
  Wh_Log(L"setting %d %s", g_settings.userDefinedFullWidthTaskbarBackground ? 1 : 0, L"userDefinedFullWidthTaskbarBackground");
  Wh_Log(L"setting %d %s", g_settings.userDefinedIgnoreShowDesktopButton ? 1 : 0, L"userDefinedIgnoreShowDesktopButton");
  Wh_Log(L"setting %d %s", g_settings.userDefinedStyleTrayArea ? 1 : 0, L"userDefinedStyleTrayArea");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayAreaDivider ? 1 : 0, L"userDefinedTrayAreaDivider");
  Wh_Log(L"setting %d %s", g_settings.borderColorR, L"borderColorR");
  Wh_Log(L"setting %d %s", g_settings.borderColorG, L"borderColorG");
  Wh_Log(L"setting %d %s", g_settings.borderColorB, L"borderColorB");
  Wh_Log(L"setting %d %s", g_settings.userDefinedCustomizeTaskbarBackground ? 1 : 0, L"userDefinedCustomizeTaskbarBackground");
  Wh_Log(L"setting %d %s", g_settings.userDefinedDisableCustomBlurBackground ? 1 : 0, L"userDefinedDisableCustomBlurBackground");
}
bool ApplyStyle(FrameworkElement const& xamlRootContent, std::wstring monitorName) {
  if (!xamlRootContent) {
    Wh_Log(L"xamlRootContent is null");
    return false;
  }
  try {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  auto stateHandle = GetOrCreateTaskbarState(monitorName);
  if (!stateHandle) {
    Wh_Log(L"Failed to get taskbar state for monitor: %s", monitorName.c_str());
    return false;
  }
  std::lock_guard<std::recursive_mutex> stateLock(stateHandle->mutex);
  auto& state = *stateHandle;
  Wh_Log(L"ApplyStyle for monitor: %s", monitorName.c_str());
  g_scheduled_low_priority_update = false;
  bool forceStyleApply = false;
  int forceStyleApplyPasses = g_force_style_apply_passes.load();
  while (forceStyleApplyPasses > 0) {
    if (g_force_style_apply_passes.compare_exchange_weak(forceStyleApplyPasses, forceStyleApplyPasses - 1)) {
      forceStyleApply = true;
      break;
    }
  }
  bool resetAnimationTargetsThisPass = false;
  int resetAnimationTargetPasses = g_reset_animation_target_passes.load();
  while (resetAnimationTargetPasses > 0) {
    if (g_reset_animation_target_passes.compare_exchange_weak(resetAnimationTargetPasses, resetAnimationTargetPasses - 1)) {
      resetAnimationTargetsThisPass = true;
      forceStyleApply = true;
      break;
    }
  }
  if (resetAnimationTargetsThisPass) {
    ResetAnimationTargetCache(state);
  }
  const float rasterizationScale = GetRasterizationScale(xamlRootContent);
  auto snapPx = [rasterizationScale](float value) -> float {
    return SnapToPhysicalPixel(value, rasterizationScale);
  };
  auto taskFrame = FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  if (!taskFrame) {
    Wh_Log(L"Failed to find Taskbar.TaskbarFrame");
    return false;
  }
   const bool displayGeometryChanged = CheckAndUpdateDisplayGeometrySignature(
      state, xamlRootContent, taskFrame, rasterizationScale);
  if (displayGeometryChanged && !g_unloading) {
    Wh_Log(L"Taskbar display geometry changed; invalidating cached layout targets");
    ResetAnimationTargetCache(state);
    forceStyleApply = true;
    RequestTaskbarDimensionInvalidation();
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
      ArmStyleFollowupPasses(hTaskbarWnd, true);
    }
  }
  auto now = std::chrono::steady_clock::now();
  if (!forceStyleApply && !g_unloading && now - state.lastApplyStyleTime < std::chrono::milliseconds(200)) {
    return true;
  }
  state.lastApplyStyleTime = now;
  auto rootGridTaskBar = FindChildByName(taskFrame, L"RootGrid");
  if (!rootGridTaskBar) {
    Wh_Log(L"Failed to find RootGrid in taskFrame");
    return false;
  }
  auto taskbarFrameRepeater = FindChildByName(rootGridTaskBar, L"TaskbarFrameRepeater");
  if (!taskbarFrameRepeater) {
    Wh_Log(L"Failed to find TaskbarFrameRepeater in rootGridTaskBar");
    return false;
  }
  auto trayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
  if (!trayFrame) {
    Wh_Log(L"Failed to find SystemTray.SystemTrayFrame");
    return false;
  }
  auto systemTrayFrameGrid = FindChildByName(trayFrame, L"SystemTrayFrameGrid");
  if (!systemTrayFrameGrid) {
    Wh_Log(L"Failed to find SystemTrayFrameGrid in trayFrame");
    return false;
  }
  auto showDesktopButton = FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
  if (!showDesktopButton) {
    Wh_Log(L"Failed to find ShowDesktopStack in systemTrayFrameGrid");
    return false;
  }
  auto taskbarBackground = FindChildByClassName(rootGridTaskBar, L"Taskbar.TaskbarBackground");
  if (!taskbarBackground) {
    Wh_Log(L"Failed to find Taskbar.TaskbarBackground in rootGridTaskBar");
    return false;
  }
  auto backgroundFillParent = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (!backgroundFillParent) {
    Wh_Log(L"Failed to find backgroundFillParent in taskbarBackground");
    return false;
  }
  auto backgroundFillChild = FindChildByName(backgroundFillParent, L"BackgroundFill");
  if (!backgroundFillChild) {
    Wh_Log(L"Failed to find BackgroundFill in backgroundFillParent");
    return false;
  }
  auto notificationAreaIcons = FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
  if (!notificationAreaIcons) {
    Wh_Log(L"Failed to find NotificationAreaIcons in systemTrayFrameGrid");
    return false;
  }
  auto itemsPresenter = FindChildByClassName(notificationAreaIcons, L"Windows.UI.Xaml.Controls.ItemsPresenter");
  if (!itemsPresenter) {
    Wh_Log(L"Failed to find ItemsPresenter in notificationAreaIcons");
    return false;
  }
  auto stackPanel = FindChildByClassName(itemsPresenter, L"Windows.UI.Xaml.Controls.StackPanel");
  if (!stackPanel) {
    Wh_Log(L"Failed to find StackPanel in itemsPresenter");
    return false;
  }
  bool widgetPresent = IsTaskbarWidgetsEnabled();
  auto widgetElement = widgetPresent ? FindChildByClassName(taskbarFrameRepeater, L"Taskbar.AugmentedEntryPointButton") : nullptr;
  auto widgetMainView = widgetElement ? FindChildByName(widgetElement, L"ExperienceToggleButtonRootPanel") : widgetElement;
  widgetPresent = widgetPresent && widgetMainView != nullptr;
  auto widgetElementWidth = widgetPresent && widgetMainView ? widgetMainView.ActualWidth() : 0;
  if (widgetPresent && widgetElementWidth <= 0) {
    Wh_Log(L"Error: widgetPresent && widgetElementWidth<=0");
    return false;
  }
  auto widgetElementInnerChild = widgetPresent ? FindChildByClassName(widgetElement, L"Taskbar.TaskListButtonPanel") : nullptr;
  auto widgetElementVisibleWidth = widgetElementInnerChild ? widgetElementInnerChild.ActualWidth() : 0;
  auto widgetElementVisibleHeight = widgetElementInnerChild ? widgetElementInnerChild.ActualHeight() : 0;
  if (widgetElementInnerChild && widgetElementVisibleWidth <= 0) {
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleWidth<=0");
    return false;
  }
  if (widgetElementInnerChild && widgetElementVisibleHeight <= 0) {
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleHeight<=0");
    return false;
  }
  auto overflowButton = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.OverflowToggleButton");
  bool isOverflowing = overflowButton != nullptr && !IsWeirdFrameworkElement(overflowButton);
  double rootWidth = xamlRootContent.ActualWidth();
  state.lastRootWidth = static_cast<float>(rootWidth);
  const double minimumRootWidth =
      std::max<double>(1.0, static_cast<double>(g_settings.userDefinedTaskbarButtonSize));
  if (!g_unloading && rootWidth < minimumRootWidth) {
    Wh_Log(L"root width is too small");
    return false;
  }
   const double taskbarVirtualSurfaceWidth = g_unloading
      ? rootWidth
      : CalculateDynamicTaskbarVirtualSurfaceWidth(taskbarFrameRepeater,
                                                  rootWidth,
                                                  isOverflowing);
  const bool useVirtualTaskbarSurface =
      !g_unloading &&
      std::isfinite(taskbarVirtualSurfaceWidth) &&
      taskbarVirtualSurfaceWidth > rootWidth + kLayoutToleranceDip;
  if (useVirtualTaskbarSurface) {
    ApplyVirtualTaskbarLayoutSurface(xamlRootContent,
                                     taskFrame,
                                     rootGridTaskBar,
                                     taskbarFrameRepeater,
                                     taskbarBackground,
                                     backgroundFillParent,
                                     backgroundFillChild,
                                     taskbarVirtualSurfaceWidth);
    SetTaskbarOverflowButtonSuppressed(overflowButton, true);
    isOverflowing = false;
  } else {
    SetTaskbarOverflowButtonSuppressed(overflowButton, false);
  }
  const double taskbarLayoutSurfaceWidth = useVirtualTaskbarSurface
      ? std::max(rootWidth, taskbarFrameRepeater.ActualWidth())
      : rootWidth;
  int childrenCountTaskbar = 0;
  double taskbarLeftEdge = 0.0;
  double taskbarRightEdge = 0.0;
  double startButtonLeft = 0.0;
  double startButtonTop = 0.0;
  double startButtonWidth = 0.0;
  double startButtonHeight = 0.0;
  const double childrenWidthTaskbarDbl = CalculateValidChildrenWidth(
      taskbarFrameRepeater, childrenCountTaskbar, state, &taskbarLeftEdge, &taskbarRightEdge);
  winrt::Windows::Foundation::Rect startButtonAnchorRect{};
  auto startButtonElement = FindStartButtonElement(taskbarFrameRepeater);
  if (TryGetStartButtonRectRelativeTo(taskbarFrameRepeater, rootGridTaskBar, startButtonAnchorRect)) {
    startButtonLeft = startButtonAnchorRect.X;
    startButtonTop = startButtonAnchorRect.Y;
    startButtonWidth = startButtonAnchorRect.Width;
    startButtonHeight = startButtonAnchorRect.Height;
  }
  if (!g_unloading && childrenWidthTaskbarDbl <= 0) {
    Wh_Log(L"Error: childrenWidthTaskbarDbl <= 0");
    return false;
  }
  double actualTaskbarBoundsWidthDbl = taskbarRightEdge - taskbarLeftEdge;
  if (actualTaskbarBoundsWidthDbl <= 0.0) {
    actualTaskbarBoundsWidthDbl = childrenWidthTaskbarDbl;
    taskbarLeftEdge = (rootWidth - actualTaskbarBoundsWidthDbl) / 2.0;
    taskbarRightEdge = taskbarLeftEdge + actualTaskbarBoundsWidthDbl;
  }
  // Use the sum of valid child widths as the logical task width. The measured
  // min/max bounds are still useful as an anchor near overflow, but during
  // insert/remove animations they can include temporary gaps from recycled
  // views and make the island width overshoot.
  unsigned int childrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbarDbl + 0.5);
  signed int rightMostEdgeTaskbar = static_cast<signed int>(taskbarRightEdge + 0.5);
  if (!g_unloading && childrenCountTaskbar < 1) {
    Wh_Log(L"Error: childrenCountTaskbar < 1");
    return false;
  }
  const unsigned int minimumTaskbarChildrenWidth =
      static_cast<unsigned int>(std::max<double>(1.0, g_settings.userDefinedTaskbarButtonSize * 0.25));
  if (!g_unloading && childrenWidthTaskbar <= minimumTaskbarChildrenWidth) {
    Wh_Log(L"Error: childrenWidthTaskbar is too small");
    return false;
  }
  if (!g_unloading && rightMostEdgeTaskbar < 0) {
    Wh_Log(L"Error: rightMostEdgeTaskbar < 0");
    return false;
  }
  bool rightMostEdgeChangedTaskbar = (state.lastTaskbarData.rightMostEdge != rightMostEdgeTaskbar);
  if (isOverflowing != state.wasOverflowing) {
    ResetAnimationTargetCache(state);
    forceStyleApply = true;
  }
  if (rightMostEdgeChangedTaskbar || state.lastTaskbarData.rightMostEdge == 0.0 || isOverflowing) {
    state.lastTaskbarData.childrenCount = childrenCountTaskbar;
    state.lastTaskbarData.rightMostEdge = rightMostEdgeTaskbar;
    state.lastTaskbarData.childrenWidth = childrenWidthTaskbar;
  }
  trayFrame.Clip(nullptr);
    DisableElementClip(systemTrayFrameGrid);
  DisableClipForAncestorChain(systemTrayFrameGrid);
  auto trayHorizontalAlignmentRef = trayFrame
      .GetValue(FrameworkElement::HorizontalAlignmentProperty())
      .try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>();
  if (trayHorizontalAlignmentRef && trayHorizontalAlignmentRef.Value() == HorizontalAlignment::Center) {
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Right));
  }
  int childrenCountTray = 0;
  double trayFrameWidthDbl = CalculateValidChildrenWidth(systemTrayFrameGrid, childrenCountTray, state);
  if (!g_unloading && trayFrameWidthDbl <= 0) {
    Wh_Log(L"Error: trayFrameWidthDbl <= 0");
    return false;
  }
  if (!g_unloading && childrenCountTray <= 0) {
    Wh_Log(L"Error: childrenCountTray <= 0");
    return false;
  }
  const double showDesktopButtonActualWidth = showDesktopButton ? showDesktopButton.ActualWidth() : 0.0;
  if (g_settings.userDefinedIgnoreShowDesktopButton && showDesktopButtonActualWidth > 0.0) {
    trayFrameWidthDbl = std::max(0.0, trayFrameWidthDbl - showDesktopButtonActualWidth);
  }
  int trayGapPlusExtras = g_settings.userDefinedTrayTaskGap + widgetElementVisibleWidth + (widgetPresent ? -6 + g_settings.userDefinedTrayTaskGap : 0);
  const double trayFrameWidthSafeDbl = std::max(0.0, trayFrameWidthDbl + static_cast<double>(trayGapPlusExtras));
  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthSafeDbl + 0.5);
  if (!g_unloading && childrenCountTray == 0) {
    Wh_Log(L"Error: childrenCountTray == 0");
    return false;
  }
  if (!g_unloading && trayFrameWidth <= 1) {
    Wh_Log(L"Error: trayFrameWidth <= 1");
    return false;
  }
  const float targetContentWidth = static_cast<float>(childrenWidthTaskbar + trayFrameWidth);
  const float targetContentLeft = snapPx(static_cast<float>((rootWidth - targetContentWidth) / 2.0f));
  const float targetTrayLogicalLeft = snapPx(targetContentLeft + static_cast<float>(childrenWidthTaskbar));
  float targetOffsetXTray = snapPx(targetTrayLogicalLeft - static_cast<float>(rootWidth - trayFrameWidth));
  // tray animations
  auto systemTrayFrameGridVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(systemTrayFrameGrid);
  if (!systemTrayFrameGridVisual) {
    Wh_Log(L"Error: !SystemTrayFrameGridVisual");
    return false;
  }
  auto originalOffset = systemTrayFrameGridVisual.Offset();
  if (state.initOffsetX == -1) {
    state.initOffsetX = originalOffset.x;
  }
  auto rootGridTaskBarVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(rootGridTaskBar);
  if (!rootGridTaskBarVisual) {
    Wh_Log(L"Error: !rootGridTaskBarVisual");
    return false;
  }
  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  if (!taskbarFrameRepeaterVisual) {
    Wh_Log(L"Error: !taskbarFrameRepeaterVisual");
    return false;
  }
  const float visualOffsetTolerance =
      std::max<float>(static_cast<float>(kLayoutToleranceDip),
                      2.0f / GetRasterizationScale(xamlRootContent));
  // Move the stable RootGrid instead of TaskbarFrameRepeater. The repeater is
  // virtualized and can expose transient/recycled child offsets during insert
  // and remove animations, which makes compositor Offset animations overshoot.
  // RootGrid remains the authoritative visual transform; the repeater itself is
  // kept horizontally neutral below.
  //
  // The Start button rect is sampled in RootGrid coordinates, but it is not used
  // as a live target. During task insertion/removal, Explorer can animate the
  // repeater's internal layout; chasing that transient Start rect causes a
  // feedback loop where RootGrid keeps catching up to Windows' own animation.
  // Only promote the sampled rect to an anchor after it has been stable for a
  // couple of ApplyStyle passes. Until then, keep using the last stable anchor
  // or the edge-clamp fallback from the root-grid version.
  const bool hasStartButtonAnchorSample =
      startButtonWidth > 0.0 &&
      startButtonHeight > 0.0 &&
      std::isfinite(startButtonLeft) &&
      std::isfinite(startButtonTop);
  const bool startButtonSampleChanged =
      hasStartButtonAnchorSample &&
      (!state.hasLastStartButtonAnchorRect ||
       std::abs(state.lastStartButtonAnchorLeft - static_cast<float>(startButtonLeft)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorTop - static_cast<float>(startButtonTop)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorWidth - static_cast<float>(startButtonWidth)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorHeight - static_cast<float>(startButtonHeight)) > visualOffsetTolerance);
  if (hasStartButtonAnchorSample) {
    if (startButtonSampleChanged) {
      state.startButtonAnchorStablePasses = 0;
    } else if (state.startButtonAnchorStablePasses < kStartButtonAnchorStablePassesRequired) {
      state.startButtonAnchorStablePasses++;
    }
    state.lastStartButtonAnchorLeft = static_cast<float>(startButtonLeft);
    state.lastStartButtonAnchorTop = static_cast<float>(startButtonTop);
    state.lastStartButtonAnchorWidth = static_cast<float>(startButtonWidth);
    state.lastStartButtonAnchorHeight = static_cast<float>(startButtonHeight);
    state.hasLastStartButtonAnchorRect = true;
    if (state.startButtonAnchorStablePasses >= kStartButtonAnchorStablePassesRequired) {
      state.stableStartButtonAnchorLeft = static_cast<float>(startButtonLeft);
      state.stableStartButtonAnchorTop = static_cast<float>(startButtonTop);
      state.stableStartButtonAnchorWidth = static_cast<float>(startButtonWidth);
      state.stableStartButtonAnchorHeight = static_cast<float>(startButtonHeight);
      state.hasStableStartButtonAnchorRect = true;
    }
  } else {
    state.hasLastStartButtonAnchorRect = false;
    state.hasStableStartButtonAnchorRect = false;
    state.startButtonAnchorStablePasses = 0;
  }
  const double predictedCenteredTaskbarLeft =
      (taskbarLayoutSurfaceWidth - static_cast<double>(childrenWidthTaskbar)) / 2.0;
  const double taskbarEdgeClampTolerance =
      std::max<double>(visualOffsetTolerance,
                       static_cast<double>(g_settings.userDefinedTaskbarButtonSize) * 0.25);
  const bool taskbarLayoutIsEdgeClamped =
      isOverflowing ||
      taskbarLeftEdge <= taskbarEdgeClampTolerance ||
      taskbarRightEdge >= taskbarLayoutSurfaceWidth - taskbarEdgeClampTolerance ||
      predictedCenteredTaskbarLeft <= taskbarEdgeClampTolerance;
  // Windows can shift the taskbar item group left before the official overflow
  // button appears, especially when the centered item group would get too close
  // to the tray/suffixed area. Do not turn that into a second live anchor state
  // machine: just broaden the original stable Start-button anchor path for this
  // specific tray-collision condition.
  const double trayFrameLeft = rootWidth - static_cast<double>(trayFrameWidth);
  const double predictedTaskbarRightOnScreen =
      targetContentLeft + static_cast<double>(childrenWidthTaskbar);
  const double predictedGapToTray =
      trayFrameLeft - predictedTaskbarRightOnScreen;
  const double minimumTaskbarToTrayGap =
      std::max<double>(static_cast<double>(visualOffsetTolerance) * 2.0,
                       static_cast<double>(g_settings.userDefinedTaskbarButtonSize) * 0.75);
  const bool taskbarLayoutIsTrayConstrained =
      !useVirtualTaskbarSurface &&
      trayFrameWidth > 0 &&
      taskbarRightEdge > taskbarLeftEdge &&
      predictedGapToTray < minimumTaskbarToTrayGap &&
      std::abs(taskbarLeftEdge - predictedCenteredTaskbarLeft) >
          visualOffsetTolerance;
  const bool stableStartAnchorMatchesCurrentSample =
      state.hasStableStartButtonAnchorRect &&
      hasStartButtonAnchorSample &&
      std::abs(state.stableStartButtonAnchorLeft -
               static_cast<float>(startButtonLeft)) <= visualOffsetTolerance &&
      std::abs(state.stableStartButtonAnchorWidth -
               static_cast<float>(startButtonWidth)) <= visualOffsetTolerance;
  const double fallbackTaskbarAnchorLeft =
      taskbarLayoutIsEdgeClamped ? taskbarLeftEdge : predictedCenteredTaskbarLeft;
  const bool useStableStartButtonAnchor =
      state.hasStableStartButtonAnchorRect &&
      (taskbarLayoutIsEdgeClamped ||
       isOverflowing ||
       (taskbarLayoutIsTrayConstrained && stableStartAnchorMatchesCurrentSample));
  const double taskbarAnchorLeft = useStableStartButtonAnchor
      ? static_cast<double>(state.stableStartButtonAnchorLeft)
      : fallbackTaskbarAnchorLeft;
  float targetTaskRootOffsetX = snapPx(targetContentLeft - static_cast<float>(taskbarAnchorLeft));
  float centeredTaskbarRightEdge = targetTrayLogicalLeft;
  const float taskbarBackgroundPadding =
      static_cast<float>(g_settings.userDefinedTaskbarBackgroundHorizontalPadding);
  float targetWidth = g_unloading
      ? static_cast<float>(rootWidth)
      : (static_cast<float>(childrenWidthTaskbar + trayFrameWidth) +
         (taskbarBackgroundPadding * 2.0f));
  if (targetWidth < 1) {
    Wh_Log(L"Error: targetWidth<1");
    return false;
  }
  const float targetBackgroundLeftScreen =
      snapPx(targetContentLeft - taskbarBackgroundPadding);
  const float targetBackgroundRightScreen =
      snapPx(targetBackgroundLeftScreen + targetWidth);
  const float targetScaleCenterScreenX =
      snapPx((targetBackgroundLeftScreen + targetBackgroundRightScreen) * 0.5f);
  const float targetTaskbarIslandScale = g_unloading
      ? 1.0f
      : CalculateTaskbarIslandScale(targetBackgroundLeftScreen,
                                    targetBackgroundRightScreen,
                                    static_cast<float>(rootWidth),
                                    targetScaleCenterScreenX,
                                    rasterizationScale);
  const float scaledBackgroundLeftScreen = snapPx(
      ApplyScaleToScreenX(targetBackgroundLeftScreen,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  const float scaledBackgroundRightScreen = snapPx(
      ApplyScaleToScreenX(targetBackgroundRightScreen,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  const bool taskbarIslandScaleTargetChanged =
      !state.hasLastTargetTaskbarIslandScale ||
      std::abs(state.lastTargetTaskbarIslandScale - targetTaskbarIslandScale) > 0.001f ||
      std::abs(state.lastTaskbarIslandScaleCenterX - targetScaleCenterScreenX) > visualOffsetTolerance;
  const uint64_t dimensionInvalidationGeneration =
      g_dimensionInvalidationGeneration.load(std::memory_order_acquire);
  const bool invalidateDimensionsThisPass =
      state.lastDimensionInvalidationGeneration != dimensionInvalidationGeneration;
  if (!forceStyleApply && !invalidateDimensionsThisPass && !g_unloading &&
      std::abs(targetOffsetXTray - systemTrayFrameGridVisual.Offset().x) <= visualOffsetTolerance &&
      childrenWidthTaskbar == state.lastChildrenWidthTaskbar &&
      trayFrameWidth == state.lastTrayFrameWidth &&
      std::abs(targetTaskRootOffsetX - rootGridTaskBarVisual.Offset().x) <= visualOffsetTolerance &&
      std::abs(taskbarFrameRepeaterVisual.Offset().x) <= visualOffsetTolerance &&
      !taskbarIslandScaleTargetChanged) {
    Wh_Log(L"taskbar root/tray offsets/scale are within tolerance %f; widths didn't change: %d, %d", visualOffsetTolerance, childrenWidthTaskbar, state.lastTrayFrameWidth);
    return true;
  }
  if (childrenWidthTaskbar < 1) {
    state.lastChildrenWidthTaskbar = 1;
  } else {
    state.lastChildrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbar);
  }
  if (trayFrameWidth < 1) {
    state.lastTrayFrameWidth = 1;
  } else {
    state.lastTrayFrameWidth = static_cast<unsigned int>(trayFrameWidth);
  }
  signed int userDefinedTaskbarOffsetY = (g_settings.userDefinedFlatTaskbarBottomCorners || g_settings.userDefinedFullWidthTaskbarBackground) ? 0 : g_settings.userDefinedTaskbarOffsetY;
  if (ShouldLogTaskbarGeometry(isOverflowing || taskbarLayoutIsEdgeClamped || taskbarLayoutIsTrayConstrained || useStableStartButtonAnchor || targetTaskbarIslandScale < 0.999f || forceStyleApply)) {
    Wh_Log(L"[TBGEOM] virtualSurface=%d virtualWidth=%.2f actualRepeaterWidth=%.2f layoutSurfaceWidth=%.2f overflowSuppressed=%d",
           useVirtualTaskbarSurface ? 1 : 0,
           taskbarVirtualSurfaceWidth,
           taskbarFrameRepeater.ActualWidth(),
           taskbarLayoutSurfaceWidth,
           (overflowButton && useVirtualTaskbarSurface) ? 1 : 0);
    Wh_Log(L"[TBGEOM] scale target=%.4f centerX=%.2f rawBg=[%.2f..%.2f] scaledBg=[%.2f..%.2f]",
           targetTaskbarIslandScale,
           targetScaleCenterScreenX,
           targetBackgroundLeftScreen,
           targetBackgroundRightScreen,
           scaledBackgroundLeftScreen,
           scaledBackgroundRightScreen);
    Wh_Log(L"[TBGEOM] trayConstraint=%d predictedGapToTray=%.2f minGap=%.2f stableStartMatches=%d",
           taskbarLayoutIsTrayConstrained ? 1 : 0,
           predictedGapToTray,
           minimumTaskbarToTrayGap,
           stableStartAnchorMatchesCurrentSample ? 1 : 0);
    LogTaskbarGeometryProbe(
        L"ApplyStyle",
        monitorName,
        xamlRootContent,
        taskFrame,
        rootGridTaskBar,
        taskbarFrameRepeater,
        trayFrame,
        systemTrayFrameGrid,
        taskbarBackground,
        backgroundFillChild,
        startButtonElement,
        rootWidth,
        childrenWidthTaskbarDbl,
        taskbarLeftEdge,
        taskbarRightEdge,
        startButtonLeft,
        startButtonTop,
        startButtonWidth,
        startButtonHeight,
        predictedCenteredTaskbarLeft,
        isOverflowing,
        taskbarLayoutIsEdgeClamped,
        useStableStartButtonAnchor,
        targetContentLeft,
        targetContentWidth,
        targetTaskRootOffsetX,
        targetOffsetXTray,
        targetBackgroundLeftScreen,
        targetBackgroundRightScreen,
        state);
  }
  const float unscaledStartButtonScreenX = useStableStartButtonAnchor
      ? snapPx(state.stableStartButtonAnchorLeft + targetTaskRootOffsetX)
      : targetBackgroundLeftScreen;
  state.lastStartButtonXCalculated = snapPx(
      ApplyScaleToScreenX(unscaledStartButtonScreenX,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));
  if (heightValue < g_settings.userDefinedTaskbarHeight / 2) {
    Wh_Log(L"Error: heightValue<g_settings.userDefinedTaskbarHeight/2");
    return false;
  }
  if (invalidateDimensionsThisPass) {
    if (g_settings.userDefinedTaskbarHeight <= 0) {
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    if (heightValue <= 0) {
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    trayFrame.Height(g_settings.userDefinedTaskbarHeight);
    trayFrame.MaxHeight(g_settings.userDefinedTaskbarHeight);
    taskFrame.Height(heightValue);
    taskFrame.MaxHeight(heightValue);
    taskbarFrameRepeater.Height(g_settings.userDefinedTaskbarHeight);
    taskbarFrameRepeater.MaxHeight(g_settings.userDefinedTaskbarHeight);
    ApplyVirtualTaskbarLayoutSurface(xamlRootContent,
                                     taskFrame,
                                     rootGridTaskBar,
                                     taskbarFrameRepeater,
                                     taskbarBackground,
                                     backgroundFillParent,
                                     backgroundFillChild,
                                     taskbarVirtualSurfaceWidth);
    state.lastDimensionInvalidationGeneration = dimensionInvalidationGeneration;
  }
  // Any previous version of the mod may have left a horizontal Offset
  // animation on the repeater. Clear only X so RootGrid owns task-area X
  // motion, but preserve the repeater's native Y offset. Windows/XAML can use
  // that Y component to vertically align the task buttons with the tray.
  if (std::abs(taskbarFrameRepeaterVisual.Offset().x) > visualOffsetTolerance) {
    auto repeaterOffset = taskbarFrameRepeaterVisual.Offset();
    taskbarFrameRepeaterVisual.StopAnimation(L"Offset");
    taskbarFrameRepeaterVisual.Offset({0.0f, repeaterOffset.y, repeaterOffset.z});
  }
  const float rootGridScaleCenterLocalX =
      snapPx(targetScaleCenterScreenX - targetTaskRootOffsetX);
  const float rootGridScaleCenterLocalY =
      snapPx(static_cast<float>(rootGridTaskBar.ActualHeight()) * 0.5f);
  const float trayScaleCenterLocalX =
      snapPx(targetScaleCenterScreenX - targetTrayLogicalLeft);
  const float trayScaleCenterLocalY =
      snapPx(static_cast<float>(systemTrayFrameGrid.ActualHeight()) * 0.5f);
  SetVisualScaleCenterAndAnimate(rootGridTaskBarVisual,
                                 targetTaskbarIslandScale,
                                 rootGridScaleCenterLocalX,
                                 rootGridScaleCenterLocalY,
                                 0.001f,
                                 !g_unloading);
  SetVisualScaleCenterAndAnimate(systemTrayFrameGridVisual,
                                 targetTaskbarIslandScale,
                                 trayScaleCenterLocalX,
                                 trayScaleCenterLocalY,
                                 0.001f,
                                 !g_unloading);
  state.lastTargetTaskbarIslandScale = targetTaskbarIslandScale;
  state.lastTaskbarIslandScaleCenterX = targetScaleCenterScreenX;
  state.hasLastTargetTaskbarIslandScale = true;
  if (auto taskRootVisualCompositor = rootGridTaskBarVisual.Compositor()) {
    if (!g_unloading) {
      const bool taskRootOffsetChanged =
          invalidateDimensionsThisPass ||
          !state.hasLastTargetTaskFrameOffsetX ||
          std::abs(state.lastTargetTaskFrameOffsetX - targetTaskRootOffsetX) > visualOffsetTolerance;
      if (taskRootOffsetChanged) {
        auto taskRootOffsetAnimation = taskRootVisualCompositor.CreateVector3KeyFrameAnimation();
        ConfigureTaskbarIslandAnimation(taskRootOffsetAnimation);
        InsertTaskbarIslandKeyFrame(taskRootOffsetAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetTaskRootOffsetX, rootGridTaskBarVisual.Offset().y, rootGridTaskBarVisual.Offset().z});
        rootGridTaskBarVisual.StartAnimation(L"Offset", taskRootOffsetAnimation);
        state.lastTargetTaskFrameOffsetX = targetTaskRootOffsetX;
        state.hasLastTargetTaskFrameOffsetX = true;
      }
    } else {
      rootGridTaskBarVisual.StopAnimation(L"Offset");
      rootGridTaskBarVisual.Offset({0.0f, 0.0f, rootGridTaskBarVisual.Offset().z});
      taskbarFrameRepeaterVisual.StopAnimation(L"Offset");
      taskbarFrameRepeaterVisual.Offset({0.0f, 0.0f, taskbarFrameRepeaterVisual.Offset().z});
      state.hasLastTargetTaskFrameOffsetX = false;
    }
  }
  auto taskbarVisual = rootGridTaskBarVisual;
  auto trayVisualCompositor = systemTrayFrameGridVisual.Compositor();
    if (trayVisualCompositor) {
    if (!g_unloading) {
      const bool trayOffsetChanged =
          invalidateDimensionsThisPass ||
          !state.hasLastTargetTrayOffsetX ||
          std::abs(state.lastTargetTrayOffsetX - targetOffsetXTray) > visualOffsetTolerance;
      if (trayOffsetChanged) {
        auto trayAnimation = trayVisualCompositor.CreateVector3KeyFrameAnimation();
        ConfigureTaskbarIslandAnimation(trayAnimation);
        InsertTaskbarIslandKeyFrame(trayAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, systemTrayFrameGridVisual.Offset().y, systemTrayFrameGridVisual.Offset().z});
        systemTrayFrameGridVisual.StartAnimation(L"Offset", trayAnimation);
        state.lastTargetTrayOffsetX = targetOffsetXTray;
        state.hasLastTargetTrayOffsetX = true;
      }
    } else {
      systemTrayFrameGridVisual.Offset({0.0f, 0.0f, 0.0f});
      state.hasLastTargetTrayOffsetX = false;
    }
  }
  if (widgetPresent && widgetMainView) {
    if (widgetElement) {
      auto widgetVisualParent = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetElement);
      if (widgetVisualParent && widgetVisualParent.Offset().x != 0.0f) {
        widgetVisualParent.Offset({0.0f, widgetVisualParent.Offset().y, widgetVisualParent.Offset().z});
      }
    }
    auto widgetVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetMainView);
     if (widgetVisual) {
      if (!g_unloading) {
        auto compositorWidget = widgetVisual.Compositor();
        if (compositorWidget) {
          // Widget visual is inside the RootGrid-shifted subtree, so its local
          // target must exclude the RootGrid compositor offset. Its screen
          // position still lands after the centered task buttons.
          float targetOffsetXWidget = centeredTaskbarRightEdge - targetTaskRootOffsetX - 8.0f + g_settings.userDefinedTrayTaskGap;
          targetOffsetXWidget = snapPx(targetOffsetXWidget);
          float targetOffsetYWidget = snapPx(static_cast<float>(abs(g_settings.userDefinedTaskbarHeight - widgetElementVisibleHeight)));
          const bool widgetOffsetChanged =
              invalidateDimensionsThisPass ||
              !state.hasLastTargetWidgetOffset ||
              std::abs(state.lastTargetWidgetOffsetX - targetOffsetXWidget) > visualOffsetTolerance ||
              std::abs(state.lastTargetWidgetOffsetY - targetOffsetYWidget) > visualOffsetTolerance;
          if (widgetOffsetChanged) {
            auto widgetOffsetAnimation = compositorWidget.CreateVector3KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(widgetOffsetAnimation);
            InsertTaskbarIslandKeyFrame(widgetOffsetAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXWidget, targetOffsetYWidget, taskbarVisual.Offset().z});
            widgetVisual.StartAnimation(L"Offset", widgetOffsetAnimation);
            state.lastTargetWidgetOffsetX = targetOffsetXWidget;
            state.lastTargetWidgetOffsetY = targetOffsetYWidget;
            state.hasLastTargetWidgetOffset = true;
          }
        }
      } else {
        widgetVisual.Offset({0.0f, 0.0f, 0.0f});
        state.hasLastTargetWidgetOffset = false;
      }
    }
  }
  if (state.lastTargetWidth <= static_cast<float>(minimumTaskbarChildrenWidth)) {
    state.lastTargetWidth = static_cast<float>(rootWidth);
    if (!g_unloading && state.lastTargetWidth <= 0) {
      Wh_Log(L"Error: g_unloading && state.lastTargetWidth <= 0");
      return false;
    }
  }
  const float targetWidthRect = !g_settings.userDefinedFullWidthTaskbarBackground ? targetWidth : static_cast<float>(rootWidth);
  if (!g_unloading && targetWidthRect <= 0) {
    Wh_Log(L"Error: targetWidthRect<=0");
    return false;
  }
  int rightMostEdgeTray = static_cast<int>(scaledBackgroundRightScreen + 0.5f);
  if (state.lastRightMostEdgeTray != rightMostEdgeTray) {
    state.lastRightMostEdgeTray = rightMostEdgeTray;
    Wh_SetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), rightMostEdgeTray);
  }
  float leftMostEdgeTray = scaledBackgroundRightScreen - (static_cast<float>(trayFrameWidth) * targetTaskbarIslandScale);
  if (leftMostEdgeTray != state.lastLeftMostEdgeTray) {
    state.lastLeftMostEdgeTray = leftMostEdgeTray;
    Wh_SetIntValue((L"lastLeftMostEdgeTray_" + monitorName).c_str(), static_cast<int>(leftMostEdgeTray));
  }
  const auto targetHeightPrelim = (!g_settings.userDefinedFullWidthTaskbarBackground ? g_settings.userDefinedTaskbarHeight : xamlRootContent.ActualHeight());
  if (!g_unloading && targetHeightPrelim <= 0) {
    Wh_Log(L"Error: targetHeightPrelim<=0");
    return false;
  }
   if (!g_unloading && (useVirtualTaskbarSurface || targetTaskbarIslandScale < 0.999f)) {
    RECT monitorRectForMinRectFix{};
    RECT taskbarClampRectForMinRectFix{};
    const bool haveMonitorRectForMinRectFix = GetMonitorRectByNameTai(monitorName, &monitorRectForMinRectFix);
    if (haveMonitorRectForMinRectFix) {
      taskbarClampRectForMinRectFix.left = monitorRectForMinRectFix.left +
          static_cast<LONG>(std::lround(static_cast<double>(scaledBackgroundLeftScreen) * rasterizationScale));
      taskbarClampRectForMinRectFix.right = monitorRectForMinRectFix.left +
          static_cast<LONG>(std::lround(static_cast<double>(scaledBackgroundRightScreen) * rasterizationScale));
      taskbarClampRectForMinRectFix.top = monitorRectForMinRectFix.top;
      taskbarClampRectForMinRectFix.bottom = monitorRectForMinRectFix.bottom;
      if (taskbarClampRectForMinRectFix.right <= taskbarClampRectForMinRectFix.left) {
        taskbarClampRectForMinRectFix = monitorRectForMinRectFix;
      }
    }

    std::vector<MinimizeAnimationMeasuredButtonTai> measuredMinRectButtons;
    if (haveMonitorRectForMinRectFix) {
      BuildMeasuredMinimizeAnimationButtonsTai(
          taskbarFrameRepeater,
          rootGridTaskBar,
          monitorRectForMinRectFix,
          static_cast<double>(targetTaskRootOffsetX),
          static_cast<double>(targetTaskbarIslandScale),
          static_cast<double>(targetScaleCenterScreenX),
          static_cast<double>(rasterizationScale),
          &measuredMinRectButtons);
    }

    // targetTaskRootOffsetX is kept as a fallback transform only. The preferred
    // path above uses freshly measured TaskListButton XAML rectangles, converted
    // through the actual RootGrid offset/scale for this ApplyStyle pass, and
    // matches them to the target HWND by dynamic window/button metadata. This
    // avoids guessing with constants when Explorer returns a packed min-rect
    // whose X coordinate has collapsed or drifted inside the fake-wide surface.
    SetMinimizeAnimationCorrectionForMonitorTai(
        monitorName,
        static_cast<double>(targetTaskRootOffsetX),
        static_cast<double>(targetTaskbarIslandScale),
        static_cast<double>(targetScaleCenterScreenX),
        static_cast<double>(rasterizationScale),
        haveMonitorRectForMinRectFix ? &taskbarClampRectForMinRectFix : nullptr,
        &measuredMinRectButtons);
  } else {
    ClearMinimizeAnimationCorrectionForMonitorTai(monitorName);
  }
  const auto clipHeight = static_cast<float>(targetHeightPrelim + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? (targetHeightPrelim - g_settings.userDefinedTaskbarCornerRadius) : 0.0f));
  if (!g_unloading && clipHeight <= 0) {
    Wh_Log(L"Error: clipHeight<=0");
    return false;
  }
  ProcessStackPanelChildren(stackPanel, clipHeight);
  ChangeControlCenterIconSize(systemTrayFrameGrid);
  auto trayOverflowArrowNotifyIconStack = FindChildByName(systemTrayFrameGrid, L"NotifyIconStack");
  if (trayOverflowArrowNotifyIconStack) {
    SetDividerForElement(trayOverflowArrowNotifyIconStack, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  } else {
    SetDividerForElement(stackPanel, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  }
  //  if (widgetPresent && widgetElementInnerChild) {
  //    SetDividerForElement(widgetElementInnerChild, clipHeight, widgetPresent && g_settings.userDefinedTrayAreaDivider, true);
  //  }
  const bool shouldApplyCustomTaskbarBackground = !g_unloading && g_settings.userDefinedCustomizeTaskbarBackground;
  const bool shouldClearCustomTaskbarBackground = g_unloading || (!shouldApplyCustomTaskbarBackground && state.hasCustomTaskbarBackgroundVisuals);
  auto taskbarStroke = FindChildByName(backgroundFillParent, L"BackgroundStroke");
  auto screenEdgeStroke = FindChildByName(rootGridTaskBar, L"ScreenEdgeStroke");
  // you can also try SystemAccentColor
  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  auto compositorTaskBackground = backgroundFillVisual.Compositor();
  if (shouldApplyCustomTaskbarBackground) {
    if (taskbarStroke) {
      taskbarStroke.Opacity(0.0);
    }
    if (screenEdgeStroke) {
      screenEdgeStroke.Opacity(0.0);
    }
    ApplyWindhawkBlurToBackgroundFill(backgroundFillChild);
    state.hasCustomTaskbarBackgroundVisuals = true;
//    For custom brush
//    auto compositor = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild).Compositor();
//    float blurAmount = float(g_settings.userDefinedTaskbarBackgroundLuminosity);
//    winrt::Windows::Foundation::Numerics::float4 tint = {0,0,0,0};
//    auto blurBrush = winrt::make<XamlBlurBrush>(compositor, blurAmount, tint);
//    auto rectangle = backgroundFillChild.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
//    if (rectangle){
//    rectangle.Fill(blurBrush);
//    }
  } else if (shouldClearCustomTaskbarBackground) {
    if (taskbarStroke) {
      taskbarStroke.ClearValue(UIElement::OpacityProperty());
    }
    if (screenEdgeStroke) {
      screenEdgeStroke.ClearValue(UIElement::OpacityProperty());
    }
    ClearWindhawkBlurFromBackgroundFill(backgroundFillChild);
    if (backgroundFillVisual) {
      backgroundFillVisual.Clip(nullptr);
    }
    if (compositorTaskBackground) {
      winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, compositorTaskBackground.CreateShapeVisual());
    }
    ResetBackgroundVisualCache(state);
    state.hasCustomTaskbarBackgroundVisuals = false;
  }
  // borders and corners
  if (shouldApplyCustomTaskbarBackground) {
    if (backgroundFillVisual) {
      if (compositorTaskBackground) {
        const float userDefinedTaskbarBorderThicknessFloat = static_cast<float>(g_settings.userDefinedTaskbarBorderThickness);
        // BackgroundFill belongs to RootGrid, and RootGrid is now translated.
        // Convert the desired screen-space island left into RootGrid-local
        // coordinates so the island background and task/tray content stay in
        // the same coordinate system.
        const float offsetXRect = snapPx(targetBackgroundLeftScreen - targetTaskRootOffsetX);
        const float newOffsetYRect = snapPx(userDefinedTaskbarOffsetY <= 0 ? static_cast<float>(abs(userDefinedTaskbarOffsetY)) : 0.0f);
        const bool backgroundShapeTargetChanged =
            invalidateDimensionsThisPass ||
            std::abs(state.lastBackgroundShapeTargetWidth - targetWidthRect) > visualOffsetTolerance ||
            std::abs(state.lastBackgroundShapeTargetOffsetX - offsetXRect) > visualOffsetTolerance ||
            std::abs(state.lastBackgroundShapeTargetOffsetY - newOffsetYRect) > visualOffsetTolerance;
        if (backgroundShapeTargetChanged || g_settings.userDefinedFullWidthTaskbarBackground || state.lastBackgroundShapeTargetWidth <= 0.0f) {
          auto roundedRect = compositorTaskBackground.CreateRoundedRectangleGeometry();
          roundedRect.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});
          auto borderGeometry = compositorTaskBackground.CreateRoundedRectangleGeometry();
          borderGeometry.CornerRadius({g_settings.userDefinedTaskbarCornerRadius - userDefinedTaskbarBorderThicknessFloat / 2.0f, g_settings.userDefinedTaskbarCornerRadius - userDefinedTaskbarBorderThicknessFloat / 2.0f});
          borderGeometry.Offset({userDefinedTaskbarBorderThicknessFloat / 2.0f, userDefinedTaskbarBorderThicknessFloat / 2.0f});
          float animationStartWidth = targetWidthRect;
          float animationStartOffsetX = offsetXRect;
          float animationStartOffsetY = newOffsetYRect;
          const int64_t animationNowMs = DelayedApplyNowMs();
          const bool canContinueRunningBackgroundAnimation =
              !invalidateDimensionsThisPass &&
              state.backgroundAnimationStartMs > 0 &&
              animationNowMs - state.backgroundAnimationStartMs < kTaskbarIslandAnimationDurationMs;
          if (!g_settings.userDefinedFullWidthTaskbarBackground) {
            if (canContinueRunningBackgroundAnimation) {
              animationStartWidth = EstimateAnimationValue(state.backgroundAnimationFromWidth, state.backgroundAnimationToWidth, state.backgroundAnimationStartMs, animationNowMs);
              animationStartOffsetX = EstimateAnimationValue(state.backgroundAnimationFromOffsetX, state.backgroundAnimationToOffsetX, state.backgroundAnimationStartMs, animationNowMs);
              animationStartOffsetY = EstimateAnimationValue(state.backgroundAnimationFromOffsetY, state.backgroundAnimationToOffsetY, state.backgroundAnimationStartMs, animationNowMs);
            } else if (state.lastTargetWidth > static_cast<float>(minimumTaskbarChildrenWidth) && std::abs(state.lastTargetWidth - rootWidth) > visualOffsetTolerance) {
              animationStartWidth = state.lastTargetWidth;
              animationStartOffsetX = state.lastTargetOffsetX == 0.0f ? offsetXRect : snapPx(state.lastTargetOffsetX);
              animationStartOffsetY = snapPx(state.lastTargetOffsetY);
            }
          }
          roundedRect.Size({animationStartWidth, clipHeight});
          auto shapeVisualBorderControl = compositorTaskBackground.CreateShapeVisual();
          shapeVisualBorderControl.Size({animationStartWidth, clipHeight});
          auto geometricClip = compositorTaskBackground.CreateGeometricClip(roundedRect);
          auto borderShape = compositorTaskBackground.CreateSpriteShape(borderGeometry);
          winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
          borderShape.StrokeBrush(compositorTaskBackground.CreateColorBrush(borderColor));
          borderShape.StrokeThickness(g_settings.userDefinedTaskbarBorderThickness);
          borderShape.FillBrush(nullptr);
          borderGeometry.Size({static_cast<float>(animationStartWidth - userDefinedTaskbarBorderThicknessFloat), static_cast<float>(clipHeight - userDefinedTaskbarBorderThicknessFloat)});
          backgroundFillVisual.Clip(geometricClip);
          shapeVisualBorderControl.Shapes().Append(borderShape);
          winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, shapeVisualBorderControl);
          if (!g_settings.userDefinedFullWidthTaskbarBackground) {
            auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(sizeAnimationRect);
            sizeAnimationRect.InsertKeyFrame(0.0f, {animationStartWidth, clipHeight});
            InsertTaskbarIslandKeyFrame(sizeAnimationRect, 1.0f, winrt::Windows::Foundation::Numerics::float2{targetWidthRect, clipHeight});
            auto sizeAnimationBorderGeometry = compositorTaskBackground.CreateVector2KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(sizeAnimationBorderGeometry);
            sizeAnimationBorderGeometry.InsertKeyFrame(0.0f, {animationStartWidth - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
            InsertTaskbarIslandKeyFrame(sizeAnimationBorderGeometry, 1.0f, winrt::Windows::Foundation::Numerics::float2{targetWidthRect - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
            roundedRect.StartAnimation(L"Size", sizeAnimationRect);
            shapeVisualBorderControl.StartAnimation(L"Size", sizeAnimationRect);
            borderGeometry.StartAnimation(L"Size", sizeAnimationBorderGeometry);
            roundedRect.Offset({animationStartOffsetX, animationStartOffsetY});
            shapeVisualBorderControl.Offset({animationStartOffsetX, animationStartOffsetY, 0.0f});
            auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(offsetAnimationRect);
            offsetAnimationRect.InsertKeyFrame(0.0f, {animationStartOffsetX, animationStartOffsetY});
            InsertTaskbarIslandKeyFrame(offsetAnimationRect, 1.0f, winrt::Windows::Foundation::Numerics::float2{offsetXRect, newOffsetYRect});
            auto offsetAnimationRect3V = compositorTaskBackground.CreateVector3KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(offsetAnimationRect3V);
            offsetAnimationRect3V.InsertKeyFrame(0.0f, {animationStartOffsetX, animationStartOffsetY, 0.0f});
            InsertTaskbarIslandKeyFrame(offsetAnimationRect3V, 1.0f, winrt::Windows::Foundation::Numerics::float3{offsetXRect, newOffsetYRect, 0.0f});
            roundedRect.StartAnimation(L"Offset", offsetAnimationRect);
            shapeVisualBorderControl.StartAnimation(L"Offset", offsetAnimationRect3V);
            state.backgroundAnimationFromWidth = animationStartWidth;
            state.backgroundAnimationToWidth = targetWidthRect;
            state.backgroundAnimationFromOffsetX = animationStartOffsetX;
            state.backgroundAnimationToOffsetX = offsetXRect;
            state.backgroundAnimationFromOffsetY = animationStartOffsetY;
            state.backgroundAnimationToOffsetY = newOffsetYRect;
            state.backgroundAnimationStartMs = animationNowMs;
            state.lastTargetOffsetX = offsetXRect;
            state.lastTargetOffsetY = newOffsetYRect;
          } else {
            state.lastTargetOffsetX = 0;
            state.lastTargetOffsetY = 0;
            state.backgroundAnimationStartMs = 0;
            roundedRect.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY});
            shapeVisualBorderControl.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY, 0.0f});
          }
          state.lastBackgroundShapeTargetWidth = targetWidthRect;
          state.lastBackgroundShapeTargetOffsetX = offsetXRect;
          state.lastBackgroundShapeTargetOffsetY = newOffsetYRect;
        }
      }
    }
  }
  state.wasOverflowing = isOverflowing;
  state.lastTargetWidth = targetWidth;
  g_initial_style_apply_completed = true;
  g_initial_style_apply_not_before_ms = 0;
  return true;
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"ApplyStyle failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (std::exception const& ex) {
    Wh_Log(L"ApplyStyle failed: %S", ex.what());
  } catch (...) {
    Wh_Log(L"ApplyStyle failed: %08X", winrt::to_hresult());
  }
  return false;
}
void ApplySettings(HWND hTaskbarWnd) {
  if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
    RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
  }
}
void RefreshSettings() {
  unsigned int oldTaskbarButtonSize;
  {
    std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
    oldTaskbarButtonSize = g_settings.userDefinedTaskbarButtonSize;
  }
  Wh_ModSettingsChangedTBIconSize();
  Wh_ModSettingsChangedStartButtonPosition();
  UpdateGlobalSettings();
  RequestTaskbarDimensionInvalidation();
  unsigned int newTaskbarButtonSize;
  {
    std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
    newTaskbarButtonSize = g_settings.userDefinedTaskbarButtonSize;
  }
  if (!g_unloading && oldTaskbarButtonSize > 0 &&
      oldTaskbarButtonSize != newTaskbarButtonSize) {
    RequestTaskbarButtonSizeRelayout();
  }
}
void ResetGlobalVars() {
  for (const auto& stateHandle : GetTaskbarStatesSnapshot()) {
    std::lock_guard<std::recursive_mutex> stateLock(stateHandle->mutex);
    auto& state = *stateHandle;
    state.lastTaskbarData.childrenCount = 0;
    state.lastTaskbarData.rightMostEdge = 0;
    // state.lastTaskbarData.childrenWidth = 0;
    state.lastChildrenWidthTaskbar = 0;
    // state.lastTrayFrameWidth = 0;
    state.wasOverflowing = false;
  }
  RequestTaskbarDimensionInvalidation();
}
bool g_PartialMode = false;
void Wh_ModSettingsChanged() {
  if (g_PartialMode) {
    return;
  }
  Wh_Log(L"Settings Changed");
  ResetGlobalVars();
  RefreshSettings();
  ApplySettings(FindCurrentProcessTaskbarWnd());
}
bool IsExplorer() {
  wchar_t processPath[MAX_PATH];
  if (GetModuleFileName(NULL, processPath, MAX_PATH)) {
    const wchar_t* processName = wcsrchr(processPath, L'\\');
    if (processName && _wcsicmp(processName + 1, L"explorer.exe") == 0) {
      return true;
    }
  }
  return false;
}
using SetWindowPos_t = BOOL(WINAPI*)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
SetWindowPos_t SetWindowPos_Original = nullptr;
std::wstring GetProcessExeName(DWORD processId) {
  std::wstring result = L"<unknown>";
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
  if (hProcess) {
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
      std::wstring fullPath = path;
      size_t pos = fullPath.find_last_of(L"\\/");
      result = (pos != std::wstring::npos) ? fullPath.substr(pos + 1) : fullPath;
    }
    CloseHandle(hProcess);
  }
  return result;
}
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
  DWORD processId = 0;
  const bool userDefinedMoveFlyoutControlCenter =
      Wh_GetIntSetting(L"MoveFlyoutControlCenter") != 0;
  auto callOriginal = [&]() -> BOOL {
    return SetWindowPos_Original
        ? SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags)
        : FALSE;
  };
  if (!hWnd || !GetWindowThreadProcessId(hWnd, &processId)) {
    return callOriginal();
  }
  WCHAR className[256] = L"<unknown>";
  GetClassNameW(hWnd, className, ARRAYSIZE(className));
  const std::wstring windowClassName = className;
  const std::wstring processFileName = GetProcessExeName(processId);
  Wh_Log(L"[SetWindowPos] PID: %lu | EXE: %s | Class: %s | HWND: 0x%p | Pos: (%d,%d) Size: %dx%d Flags: 0x%08X",
         processId,
         processFileName.c_str(),
         windowClassName.c_str(),
         hWnd,
         X,
         Y,
         cx,
         cy,
         uFlags);
  if (!g_unloading && userDefinedMoveFlyoutControlCenter && _wcsicmp(processFileName.c_str(), L"ShellHost.exe") == 0 && _wcsicmp(windowClassName.c_str(), L"ControlCenterWindow") == 0) {
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
      return callOriginal();
    }

    auto monitorName = GetMonitorName(monitor);
    if (monitorName.empty()) {
      Wh_Log(L"[SetWindowPos] Failed to resolve monitor name");
      return callOriginal();
    }

    int lastRecordedTrayRightMostEdgeForMonitor = Wh_GetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), -1);
    if (lastRecordedTrayRightMostEdgeForMonitor > 0) {
      UINT monitorDpiX = 96;
      UINT monitorDpiY = 96;
      if (FAILED(GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY)) ||
          monitorDpiX == 0) {
        monitorDpiX = 96;
        monitorDpiY = 96;
      }
      float dpiScale = static_cast<float>(monitorDpiX) / 96.0f;
      const int flyoutInnerPaddingPx = GetFlyoutInnerPaddingPx(dpiScale);
      X = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale + flyoutInnerPaddingPx - (Wh_GetIntSetting(L"AlignFlyoutInner") ? cx : (cx / 2.0f)));
      Wh_Log(L"[SetWindowPos] New X %d", X);
    } else {
      Wh_Log(L"[SetWindowPos] No reference state for monitor %s", monitorName.c_str());
    }
  }
  return callOriginal();
}
BOOL Wh_ModInit() {
  Wh_Log(L"======================================================");
  HMODULE moduleUser32 = GetModuleHandleW(L"user32.dll");
  if (moduleUser32) {
    auto pSetWindowPos = (SetWindowPos_t)GetProcAddress(moduleUser32, "SetWindowPos");
    if (pSetWindowPos) {
      if (WindhawkUtils::Wh_SetFunctionHookT(pSetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original)) {
        Wh_Log(L"Successfully hooked SetWindowPos");
      } else {
        Wh_Log(L"Failed to hook SetWindowPos");
      }
    } else {
      Wh_Log(L"Failed to get address of SetWindowPos");
    }
  } else {
    Wh_Log(L"Failed to load user32.dll");
  }
  if (!IsExplorer()) {
    g_PartialMode = true;
    Wh_Log(L"Not explorer.exe; setting g_PartialMode to true");
    HMODULE moduleStartDocked = GetModuleHandle(L"StartDocked.dll");
    if (moduleStartDocked) {
      WindhawkUtils::SYMBOL_HOOK hook[] = {{{LR"(private: void __cdecl StartDocked::StartSizingFrame::UpdateWindowRegion(class Windows::Foundation::Size))"}, &StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original, StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook}};
      return WindhawkUtils::HookSymbols(moduleStartDocked, hook, ARRAYSIZE(hook));
    }
    return true;
  }
  g_unloading = false;
  g_worker_threads_stopping = false;
  InitMinimizeAnimationCorrectionTai();
  ArmInitialExplorerStyleApplyDelay();
  if (!Wh_ModInitTBIconSize()) {
    Wh_Log(L"Wh_ModInitTBIconSize failed");
    UninitMinimizeAnimationCorrectionTai();
    return FALSE;
  }
  if (!Wh_ModInitStartButtonPosition()) {
    Wh_Log(L"Wh_ModInitStartButtonPosition failed");
    UninitMinimizeAnimationCorrectionTai();
    return FALSE;
  }
  return TRUE;
}
void Wh_ModAfterInit() {
  if (g_PartialMode) {
    g_lastRecordedStartMenuWidth = Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
    return;
  }
  Wh_ModAfterInitTBIconSize();
  ResetGlobalVars();
  LoadSettingsTBIconSize();
  LoadSettingsStartButtonPosition();
  UpdateGlobalSettings();
  ScheduleInitialExplorerStyleApply();
}
void Wh_ModBeforeUninit() {
  if (g_PartialMode) {
    return;
  }
  g_unloading = true;
  CleanupDebounce();
  UninitMinimizeAnimationCorrectionTai();
  Wh_ModBeforeUninitTBIconSize();
  Wh_ModBeforeUninitStartButtonPosition();
  RefreshSettings();
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}
void Wh_ModUninit() {
  if (g_PartialMode) {
    return;
  }
  UninitMinimizeAnimationCorrectionTai();
  CleanupDebounce();
  Wh_ModUninitTBIconSize();
  ResetGlobalVars();
  ClearTaskbarStates();
  {
    std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
    g_settings.userDefinedDividedAppNames.clear();
  }
  Wh_Log(L"... detached");
}
