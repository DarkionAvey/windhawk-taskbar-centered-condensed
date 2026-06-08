SearchButtonBase_UpdateButtonPadding_Original(pThis);
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
    FindChildByName(toggleButtonElement, L"SearchBoxButtonRootPanel")
        .try_as<Controls::Grid>();
if (!panelElement) {
    return;
}
if (FindChildByName(panelElement, L"SearchBoxTextBlock")) {
    return;
}
const double targetWidth = GetEffectiveTaskbarButtonTargetWidth();
bool changed = EnsureElementTaskbarButtonWidth(toggleButtonElement, targetWidth, true);
changed = EnsureElementTaskbarButtonWidth(panelElement, targetWidth, true) || changed;
if (changed) {
    Wh_Log(L"Updating search button width to %f", targetWidth);
    panelElement.UpdateLayout();
}
