{{LR"(public: virtual void __cdecl CTaskBand::RemoveIcon(struct ITaskItem *))"}, &CTaskBand_RemoveIcon_WithArgs_Original, CTaskBand_RemoveIcon_WithArgs_Hook},

    {{LR"(protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup *,struct ITaskItem *))"}, &CTaskBand__UpdateItemIcon_WithArgs_Original, CTaskBand__UpdateItemIcon_WithArgs_Hook},

    {
        {LR"(protected: static __int64 __cdecl CImpWndProc::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CImpWndProc__WndProc_Original,
        CImpWndProc__WndProc_Hook,
    },
    {
        {LR"(protected: static __int64 __cdecl CTraySearchControl::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTraySearchControl__WndProc_Original,
        CTraySearchControl__WndProc_Hook,
    },
    {
        {LR"(private: virtual __int64 __cdecl CSecondaryTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CSecondaryTaskBand__WndProc_Original,
        CSecondaryTaskBand__WndProc_Hook,
    },
    {
        {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTaskBand__WndProc_Original,
        CTaskBand__WndProc_Hook,
    },

    {
        {LR"(protected: virtual __int64 __cdecl CTaskListWnd::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTaskListWnd__WndProc_Original,
        CTaskListWnd__WndProc_Hook,
    },

    /////////////////////////////////////
    {
        {LR"(protected: long __cdecl CTaskBand::_InsertItem(struct HWND__ *,struct ITaskItem * *,struct HWND__ *,struct HWND__ *))"},
        &CTaskBand__InsertItem_Original,
        CTaskBand__InsertItem_Hook,
    },

    {
        {LR"(protected: void __cdecl CTaskBand::_UpdateAllIcons(void))"},
        &CTaskBand__UpdateAllIcons_Original,
        CTaskBand__UpdateAllIcons_Hook,
    },

    {
        {LR"(public: virtual void __cdecl CTaskBand::TaskOrderChanged(struct ITaskGroup *,int))"},
        &CTaskBand__TaskOrderChanged_Original,
        CTaskBand__TaskOrderChanged_Hook,
    },

    {
        {LR"(protected: void __cdecl CTaskBand::_ProcessWindowDestroyed(struct HWND__ *))"},
        &CTaskBand__ProcessWindowDestroyed_Original,
        CTaskBand__ProcessWindowDestroyed_Hook,
    },

    {
        {LR"(public: void __cdecl TrayUI::_Hide(void))"},
        &TrayUI__Hide_Original,
        TrayUI__Hide_Hook,
    },
    {
        {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
        &CSecondaryTray__AutoHide_Original,
        CSecondaryTray__AutoHide_Hook,
    },

    {
        {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
        &TrayUI_WndProc_Original,
        TrayUI_WndProc_Hook,
    },
    {
        {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CSecondaryTray_v_WndProc_Original,
        CSecondaryTray_v_WndProc_Hook,
    },
    {
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::get_Alignment(int *))"},
        &ITaskbarSettings_get_Alignment_Original,
        ITaskbarSettings_get_Alignment_Hook,
    },