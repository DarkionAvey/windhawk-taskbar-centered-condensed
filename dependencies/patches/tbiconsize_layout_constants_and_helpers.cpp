constexpr int kDefaultTaskbarHeight = 74;
constexpr int kDefaultTaskbarIconSize = 42;
constexpr int kDefaultTaskbarButtonSize = 74;
constexpr int kDefaultTaskbarOffsetY = 6;
constexpr int kDefaultTrayIconSize = 15;
constexpr int kDefaultTrayButtonSize = 30;
constexpr int kSystemSmallTaskbarIconSize = 16;
constexpr int kSystemMediumTaskbarIconSize = 24;
constexpr int kSystemSmallTaskbarButtonSize = 32;
constexpr int kSystemMediumTaskbarButtonSize = 44;
constexpr int kMinTaskbarHeight = kSystemMediumTaskbarButtonSize;
constexpr int kMaxTaskbarHeight = 200;
constexpr int kMinTaskbarButtonSize = kSystemMediumTaskbarButtonSize;
constexpr int kMaxTaskbarButtonSize = 300;
constexpr int kMinTaskbarIconSize = 8;
constexpr int kMaxTaskbarIconSize = 300;
constexpr int kMinTrayIconSize = 15;
constexpr int kMinTrayButtonSize = 20;
constexpr double kLayoutToleranceDip = 0.5;
constexpr int kWorkerShutdownPollMs = 10;
constexpr int kDelayedApplyWorkerShutdownTimeoutMs = 5000;
constexpr int kAnimationFollowupWorkerShutdownTimeoutMs = 2000;
constexpr int kTaskbarMeasurePollIntervalMs = 100;
constexpr int kTaskbarMeasureOverrideTimeoutMs = 10000;
constexpr int kHookDrainPollIntervalMs = 100;
constexpr int kHookDrainTimeoutMs = 10000;

bool WaitForConditionWithTimeout(std::function<bool()> condition,
                                 int timeoutMs,
                                 int pollIntervalMs);

FrameworkElement TryQueryFrameworkElement(IUnknown* unknown,
                                          PCWSTR context = L"FrameworkElement") {
    if (!unknown) {
        return nullptr;
    }

    FrameworkElement element{nullptr};
    try {
        HRESULT hr = unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                             winrt::put_abi(element));
        if (FAILED(hr) || !element) {
            if (context) {
                Wh_Log(L"%s QueryInterface failed: %08X", context, hr);
            }
            return nullptr;
        }
    } catch (winrt::hresult_error const& ex) {
        if (context) {
            Wh_Log(L"%s QueryInterface threw %08X: %s", context, ex.code(), ex.message().c_str());
        }
        return nullptr;
    } catch (...) {
        if (context) {
            Wh_Log(L"%s QueryInterface threw: %08X", context, winrt::to_hresult());
        }
        return nullptr;
    }

    return element;
}

int ClampInt(int value, int minValue, int maxValue) {
    return value < minValue ? minValue : (value > maxValue ? maxValue : value);
}

int ReadPositiveIntSettingOrDefault(const wchar_t* key, int defaultValue) {
    int value = Wh_GetIntSetting(key);
    return value > 0 ? value : defaultValue;
}

int GetMaxTaskbarIconSizeForLayout(int taskbarHeight, int taskbarButtonSize) {
    int maxIconSize = std::min(kMaxTaskbarIconSize, std::min(taskbarHeight, taskbarButtonSize));
    return std::max(kMinTaskbarIconSize, maxIconSize);
}
