ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
if (g_hasDynamicIconScaling && g_unloading) {
    return;
}
FrameworkElement toggleButtonElement = nullptr;
((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                       winrt::put_abi(toggleButtonElement));
if (!toggleButtonElement) {
    return;
}
auto panelElement =
    FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
        .try_as<Controls::Grid>();
if (!panelElement) {
    return;
}
auto className = winrt::get_class_name(toggleButtonElement);
if (className != L"Taskbar.ExperienceToggleButton" &&
    className != L"Taskbar.SearchBoxButton") {
    return;
}
if (className == L"Taskbar.SearchBoxButton" && panelElement.Margin() != Thickness{}) {
    return;
}

const double targetWidth = GetEffectiveTaskbarButtonTargetWidth();
const bool allowHardWidth =
    className != L"Taskbar.SearchBoxButton" ||
    !FindChildByName(panelElement, L"SearchBoxTextBlock");
bool changed = EnsureElementTaskbarButtonWidth(toggleButtonElement, targetWidth, allowHardWidth);
changed = EnsureElementTaskbarButtonWidth(panelElement, targetWidth, allowHardWidth) || changed;
if (changed) {
    Wh_Log(L"Updating taskbar button width for %s to %f", className.c_str(), targetWidth);
    panelElement.UpdateLayout();
}
