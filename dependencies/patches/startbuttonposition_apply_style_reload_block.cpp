const auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
if (!xamlRootContent) {
    Wh_Log(L"XamlRoot content is null");
    return TRUE;
}

auto dispatcher = xamlRootContent.Dispatcher();
if (!dispatcher) {
    Wh_Log(L"XamlRoot content dispatcher is null");
    return TRUE;
}

std::wstring monitorName = GetMonitorName(hWnd);
auto applyOnDispatcher = [xamlRootContent, monitorName]() {
    if (!ApplyStyle(xamlRootContent, monitorName)) {
        Wh_Log(L"ApplyStyles failed");
    }
};

if (dispatcher.HasThreadAccess()) {
    applyOnDispatcher();
} else if (!g_unloading) {
    auto priority = winrt::Windows::UI::Core::CoreDispatcherPriority::Low;
    int highPriorityPasses = g_high_priority_dispatch_passes.load();
    while (highPriorityPasses > 0) {
        if (g_high_priority_dispatch_passes.compare_exchange_weak(
                highPriorityPasses, highPriorityPasses - 1)) {
            priority = winrt::Windows::UI::Core::CoreDispatcherPriority::High;
            break;
        }
    }
    dispatcher.TryRunAsync(priority, applyOnDispatcher);
}
