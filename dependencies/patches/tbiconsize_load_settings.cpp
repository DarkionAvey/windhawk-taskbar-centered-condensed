void LoadSettingsTBIconSize() {
    const int requestedHeight =
        ReadPositiveIntSettingOrDefault(L"TaskbarHeight", kDefaultTaskbarHeight);
    int taskbarHeight = ClampInt(abs(requestedHeight), kMinTaskbarHeight, kMaxTaskbarHeight);

    const int taskbarOffsetY =
        std::max(0, Wh_GetIntSetting(L"TaskbarOffsetY"));
    const int heightExpansion =
        ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") ||
          Wh_GetIntSetting(L"FullWidthTaskbarBackground"))
             ? 0
             : (taskbarOffsetY * 2));
    g_settings_tbiconsize.taskbarHeight = ClampInt(
        taskbarHeight + heightExpansion,
        kMinTaskbarHeight,
        kMaxTaskbarHeight + (kDefaultTaskbarOffsetY * 2));

    const int requestedButtonSize =
        ReadPositiveIntSettingOrDefault(L"TaskbarButtonSize", kDefaultTaskbarButtonSize);
    const int taskbarButtonSize =
        ClampInt(abs(requestedButtonSize), kMinTaskbarButtonSize, kMaxTaskbarButtonSize);
    g_settings_tbiconsize.taskbarButtonWidth = taskbarButtonSize;
    g_settings_tbiconsize.taskbarButtonWidthSmall = taskbarButtonSize;

    const int requestedIconSize =
        ReadPositiveIntSettingOrDefault(L"TaskbarIconSize", kDefaultTaskbarIconSize);
    const int maxIconSize =
        GetMaxTaskbarIconSizeForLayout(g_settings_tbiconsize.taskbarHeight, taskbarButtonSize);
    g_settings_tbiconsize.iconSize = ClampInt(abs(requestedIconSize), kMinTaskbarIconSize, maxIconSize);
    g_settings_tbiconsize.iconSizeSmall =
        std::min(g_settings_tbiconsize.iconSize, kSystemSmallTaskbarIconSize);
}
