if (!WaitForConditionWithTimeout(
        [] { return g_hookCallCounter.load() <= 0; },
        kHookDrainTimeoutMs,
        kHookDrainPollIntervalMs)) {
    Wh_Log(L"Timed out waiting for taskbar icon size hooks to drain");
}
