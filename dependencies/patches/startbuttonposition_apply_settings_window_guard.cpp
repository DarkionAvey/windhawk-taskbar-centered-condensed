   if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        RunFromWindowThread(
            hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
    }
