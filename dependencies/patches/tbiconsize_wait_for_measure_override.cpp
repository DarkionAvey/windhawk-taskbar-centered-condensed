WaitForConditionWithTimeout(
        [] { return !g_pendingMeasureOverride.load(); },
        kTaskbarMeasureOverrideTimeoutMs,
        kTaskbarMeasurePollIntervalMs);
