{{LR"(public: static void __cdecl TaskbarTelemetry::StartItemEntranceAnimation<bool const &>(bool const &))"}, &orig_StartItemEntranceAnimation, Hook_StartItemEntranceAnimation_call},

    {{LR"(public: static void __cdecl TaskbarTelemetry::StartItemPlateEntranceAnimation<bool const &>(bool const &))"}, &orig_StartItemPlateEntranceAnimation, Hook_StartItemPlateEntranceAnimation_call},

    {{LR"(public: static void __cdecl TaskbarTelemetry::StartHideAnimationCompleted(void))"}, &TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original, TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Hook},

    {{LR"(public: static void __cdecl TaskbarTelemetry::StartEntranceAnimationCompleted(void))"}, &TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original, TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Hook},

    {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsMultiWindow(bool *))"}, &winrt__impl__produce_get_IsMultiWindow1_WithArgs_Original, winrt__impl__produce_get_IsMultiWindow1_WithArgs_Hook},

    {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_IsMultiWindow(bool *))"},
     &winrt__impl__produce_get_IsMultiWindow2_WithArgs_Original,
     winrt__impl__produce_get_IsMultiWindow2_WithArgs_Hook},

    {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_IsMultiWindow(bool *))"},
     &winrt__impl__produce_get_IsMultiWindow3_WithArgs_Original,
     winrt__impl__produce_get_IsMultiWindow3_WithArgs_Hook},
