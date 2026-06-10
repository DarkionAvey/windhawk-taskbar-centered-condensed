static bool TextEqualsOrdinalIgnoreCaseFlyoutTai(
    std::wstring_view left,
    std::wstring_view right) {
    if (left.size() != right.size() ||
        left.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
        return false;
    }

    return CompareStringOrdinal(
               left.data(),
               static_cast<int>(left.size()),
               right.data(),
               static_cast<int>(right.size()),
               TRUE) == CSTR_EQUAL;
}

static std::wstring GetWindowTitleForFlyoutTai(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return L"";
    }

    int length = GetWindowTextLengthW(hwnd);
    if (length <= 0 || length > 32767) {
        return L"";
    }

    std::wstring title(static_cast<size_t>(length) + 1, L'\0');
    int copied = GetWindowTextW(hwnd, title.data(), length + 1);
    if (copied <= 0) {
        return L"";
    }

    title.resize(static_cast<size_t>(copied));
    return title;
}

static bool IsNotificationCenterTitleForFlyoutTai(std::wstring_view title) {
    if (title.empty()) {
        return false;
    }

    // Keep this intentionally conservative. ShellExperienceHost hosts other
    // CoreWindow surfaces such as Share, and those must be left untouched.
    // If a locale is missing, the window is ignored instead of being moved.
    static constexpr PCWSTR kNotificationCenterTitles[] = {
        L"Notification Center",
        L"Notification Centre",
        L"Action Center",
        L"Action Centre",
        L"Centro de notificaciones",
        L"Centre de notifications",
        L"Centro notifiche",
        L"Centro de notificações",
        L"Benachrichtigungscenter",
        L"Benachrichtigungszentrum",
        L"Meldingscentrum",
        L"Centrum powiadomień",
        L"Centrum oznámení",
        L"Centrum upozornění",
        L"Centrum upozornení",
        L"Értesítési központ",
        L"Centru de notificări",
        L"Centar za obavijesti",
        L"Središče za obvestila",
        L"Centar za obaveštenja",
        L"Notifikationscenter",
        L"Meddelelsescenter",
        L"Varslingssenter",
        L"Ilmoituskeskus",
        L"Aviseringscenter",
        L"Teavituskeskus",
        L"Paziņojumu centrs",
        L"Pranešimų centras",
        L"Κέντρο ειδοποιήσεων",
        L"Центр уведомлений",
        L"Центр сповіщень",
        L"Център за известия",
        L"Bildirim merkezi",
        L"Pusat pemberitahuan",
        L"Trung tâm thông báo",
        L"ศูนย์การแจ้งเตือน",
        L"מרכז ההודעות",
        L"מרכז ההתראות",
        L"مركز الإشعارات",
        L"通知中心",
        L"通知センター",
        L"알림 센터",
    };

    for (PCWSTR knownTitle : kNotificationCenterTitles) {
        if (TextEqualsOrdinalIgnoreCaseFlyoutTai(title, knownTitle)) {
            return true;
        }
    }

    return false;
}

static bool IsNotificationCenterShellExperienceHostWindowTai(HWND hwnd) {
    std::wstring title = GetWindowTitleForFlyoutTai(hwnd);
    bool isNotificationCenter = IsNotificationCenterTitleForFlyoutTai(title);

    if (!isNotificationCenter) {
        Wh_Log(
            L"[Flyout] Ignoring ShellExperienceHost CoreWindow hwnd=%08X title='%s'",
            (DWORD)(DWORD_PTR)hwnd,
            title.c_str());
    }

    return isNotificationCenter;
}
