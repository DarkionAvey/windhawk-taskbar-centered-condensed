import re

# This file generates a Windhawk mod to monitor method calls.
# Simply paste the methods of interest inside the input_block block,
# and it will generate the mod. Useful for tracking method execution
# and identifying potential hooks.

input_block = r"""

Search "tagRECT" (188 hits in 1 file of 1 searched) [RegEx.]
  new 1 (188 hits)
	Line  1394: [0001DE84] private: bool __cdecl CTaskListThumbnailWnd::TryGetThumbShareIconAndTextRectsFromIndex(struct tagRECT,struct tagRECT *,struct tagRECT *)const 
	Line  1396: [0001E0A8] private: void __cdecl CTaskListThumbnailWnd::PaintShareOrStopButton(struct HDC__ *,int,struct tagRECT,bool,bool,enum winrt::Windows::UI::Shell::ShareWindowCommand)
	Line  1540: [00024C80] void __cdecl ImmersiveTray::NotifyPearlRectChanged(struct ITrayComponentHost *,struct HMONITOR__ *,struct tagRECT)
	Line  1543: [00024ED8] void __cdecl ImmersiveTray::NotifyPearlRectChangedHelper(struct IServiceProvider *,struct HMONITOR__ *,struct tagRECT)
	Line  1906: [00034064] private: int __cdecl CTaskListThumbnailWnd::_GetThumbRectFromIndexHelper(class CDPA<struct ITaskThumbnail,class CTContainer_PolicyUnOwned<struct ITaskThumbnail> > const *,int,struct tagSIZE const *,int,int,unsigned long,struct tagRECT *,struct tagRECT *,struct tagRECT *)const 
	Line  1916: [00034EF0] public: virtual int __cdecl CTaskListThumbnailWnd::GetThumbRectFromIndex(int,unsigned long,struct tagRECT *,struct tagRECT *)const 
	Line  2177: [0003E904] private: void __cdecl CTaskListThumbnailWnd::_ScaleRectWithAspect(struct tagRECT *,struct tagRECT const *,int)const 
	Line  2178: [0003EA54] private: void __cdecl CTaskListThumbnailWnd::_GetWorkAreaRect(struct tagRECT *)const 
	Line  2184: [0003F6A8] private: long __cdecl CTaskListThumbnailWnd::_ComputeThumbnailSize(struct ITaskThumbnail *,struct tagRECT const *,struct tagRECT *)const 
	Line  2192: [0003FEC8] private: float __cdecl CTaskListThumbnailWnd::_CalcThumbnailScale(class CDPA<struct ITaskThumbnail,class CTContainer_PolicyUnOwned<struct ITaskThumbnail> > const *,struct tagRECT const *,int,int,unsigned char,int *,int *,struct tagSIZE *)const 
	Line  2198: [00040A20] public: virtual int __cdecl CTaskListThumbnailWnd::TryGetThumbShareRegionRectFromIndex(int,struct tagRECT *)const 
	Line  2199: [00040AD0] public: virtual int __cdecl CTaskListThumbnailWnd::GetThumbCloseButtonRectFromIndex(int,struct tagRECT *)const 
	Line  2207: [000411DC] private: void __cdecl CTaskListThumbnailWnd::_DrawText(struct HDC__ *,int,struct tagRECT const *)
	Line  2226: [00042390] long __cdecl TrayDPIHelpers::GetThemeMargins(void *,struct HDC__ *,int,int,int,struct tagRECT const *,struct _MARGINS *,unsigned int)
	Line  2275: [00045020] public: virtual long __cdecl CTaskListWnd::GetMinRect(struct ITaskGroup *,struct ITaskItem *,struct tagRECT *)
	Line  2287: [000462E4] public: void __cdecl TrayUI::_UpdateStuckRectsForMonitorRect(struct tagRECT const *,enum TrayTelemetry::TrayStuckUpdateType)
	Line  2290: [00046710] public: int __cdecl TrayUI::_HandleSizing(unsigned __int64,struct tagRECT *,unsigned int,bool)
	Line  2294: [00046B94] public: void __cdecl TrayTelemetry::TrayStuckRectChanged_(struct tagRECT const *,unsigned int,struct tagSIZE,struct HMONITOR__ *,bool)
	Line  2296: [00046FE8] public: void __cdecl TrayTelemetry::UpdateTrayRects_(enum TrayTelemetry::TrayStuckUpdateType,struct tagRECT const *)const 
	Line  2319: [00047918] public: void __cdecl AutoHideTelemetry::TaskbarAutoHideTelemetry::AutoHideOnTaskbarHide::AutoHideStopMouseHover(struct HMONITOR__ *,struct tagPOINT *,struct tagRECT *,bool)
	Line  2338: [0004885C] private: int __cdecl CSecondaryTray::_GetDisplayRect(struct tagRECT *)
	Line  2339: [00048908] public: static void __cdecl TrayTelemetry::TrayStuckRectChanged<struct tagRECT (&)[4],unsigned long &,struct tagSIZE &,struct HMONITOR__ * &,bool>(struct tagRECT (&)[4],unsigned long &,struct tagSIZE &,struct HMONITOR__ * &,bool &&)
	Line  2629: [0004FB40] static  CCLambdaAdapter<int (__cdecl*)(HMONITOR__ *,HDC__ *,tagRECT *,__int64),4>::Adapt<<lambda_0eeb38154fbc7034fc32453b49299838> >()
	Line  2639: [00050080] public: virtual void __cdecl CTaskBtnGroup::GetLocation(int,struct tagRECT *)
	Line  2640: [0005013C] struct tagRECT __cdecl ToRawPixels(struct winrt::Windows::Foundation::Rect const &,unsigned int)
	Line  2641: [00050198] struct tagRECT __cdecl ToRawPixelsInDesktopCoordinates(struct winrt::Windows::Foundation::Rect const &,unsigned int,struct HWND__ *)
	Line  2642: [00050220] void __cdecl TrayDPIHelpers::ScaleByDPI(struct tagRECT *,unsigned int,enum TrayDPIHelpers::ScaleModifier)
	Line  2646: [000505D8] public: long __cdecl CPearl::SetBounds(struct tagRECT const &)
	Line  2724: [000542A0] void * __cdecl DeferWindowPosUsingRECT(void *,struct HWND__ *,struct HWND__ *,struct tagRECT *,unsigned int)
	Line  2731: [00054880] public: virtual void __cdecl TrayUI::MakeStuckRect(struct tagRECT *,struct tagRECT const *,struct tagSIZE,unsigned int)
	Line  2809: [00055750] public: virtual void __cdecl TrayUI::GetDisplayRectFromRect(struct HWND__ *,struct tagRECT const *,struct tagRECT *)
	Line  2848: [00057890] public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool)
	Line  2849: [00057C20] public: void __cdecl TrayUI::_SlideStep(struct HWND__ *,struct tagRECT const *,struct tagRECT const *,struct tagRECT const *)
	Line  2851: [00057E28] public: void __cdecl TrayUI::_ClipInternal(struct HWND__ *,struct tagRECT const *)
	Line  2853: [00058080] public: void __cdecl TrayUI::_GetStuckDisplayRect(struct tagRECT *)
	Line  2854: [00058128] public: void __cdecl TrayUI::_CalcClipCoords(struct tagRECT *,struct tagRECT const *,struct tagRECT const *)
	Line  2855: [000581DC] public: void __cdecl TrayUI::_GetDisplayRectFromPoint(struct HWND__ *,struct tagPOINT,struct tagRECT *)
	Line  2977: [0005B800] void __cdecl FillRectARGB(struct HDC__ *,struct tagRECT const *,unsigned char,unsigned long)
	Line  2979: [0005B9A0] public: virtual long __cdecl CTaskItem::GetThumbnailClip(struct tagRECT *)
	Line  3083: [0005F5D0] public: virtual unsigned int __cdecl TrayUI::GetDockedRect(struct tagRECT *,int)
	Line  3084: [0005F654] public: void __cdecl TrayUI::_ComputeHiddenRect(struct tagRECT *,unsigned int)
	Line  3088: [0005F87C] public: void __cdecl Taskbar::ExtendedUI::Graphics::GDIPlusHelper::DrawRoundRectPushButtonWithOverlay(struct HDC__ *,enum Taskbar::ExtendedUI::Graphics::GDIPlusHelper::ButtonState,enum Taskbar::ExtendedUI::Graphics::GDIPlusHelper::CategoryIndex,struct tagRECT const *)
	Line  3093: [0005FCA0] protected: static int __cdecl CTaskListWndMulti::s_MonitorEnumProc(struct HMONITOR__ *,struct HDC__ *,struct tagRECT *,__int64)
	Line  3097: [00060540] public: virtual long __cdecl CTaskListWndMulti::GetStuckRectFromMonitor(struct HMONITOR__ *,struct tagRECT *)
	Line  3122: [00060D50] public: virtual long __cdecl CTaskBar::OnPosRectChangeDB(struct tagRECT *)
	Line  3313: [00066A0C] public: struct HDC__ * __cdecl CPaintBuffer::Begin(struct HDC__ *,struct tagRECT,unsigned long)
	Line  3450: [000675C0] int __cdecl GetMonitorRects(struct HMONITOR__ *,struct tagRECT *,int)
	Line  3535: [00069060] private: void __cdecl CTaskListThumbnailWnd::_CalcDrawTextRect(struct HDC__ *,unsigned short *,unsigned long,struct tagRECT *)
	Line  3749: [0006D7D0] public: virtual void __cdecl TrayUI::SetRebarBandPosition(struct tagRECT const &)
	Line  3908: [0006E190] protected: long __cdecl CTaskListWnd::_GetImmersiveLivePreviewWindow(struct ITaskItem *,struct HWND__ * *,struct tagRECT *)
	Line  4021: [00070450] public: virtual long __cdecl CTaskListWndMulti::GetMinRect(struct ITaskGroup *,struct ITaskItem *,struct tagRECT *)
	Line  4360: [00077B20] public: virtual void __cdecl CTaskBtnGroup::GetLocation(struct ITaskItem *,struct tagRECT *)
	Line  4465: [0007A484] protected: void __cdecl CTaskThumbnail::_GetThumbBarRect(struct tagRECT *)
	Line  4633: [0007DCA0] public: virtual long __cdecl CTaskThumbnail::UpdateRect(struct tagRECT const *,struct tagRECT const *,int,int,unsigned char)
	Line  5024: [00082780] public: virtual void __cdecl TrayUI::ContextMenuInvoke(int,struct tagRECT const *)
	Line  5134: [00083D6C] void __cdecl GDIHelpers::DrawRect(struct HDC__ *,struct tagRECT const &,unsigned char,unsigned long,int,enum GDIHelpers::DrawRectFlags)
	Line  5336: [00086260] void __cdecl GDIHelpers::FillRectARGB(struct HDC__ *,struct tagRECT const *,unsigned char,unsigned long,bool)
	Line  5450: [00088460] public: virtual bool __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *,struct tagRECT *)
	Line  5452: [000885E0] public: virtual bool __cdecl TrayUI::GetVisualRect(struct HMONITOR__ *,struct tagRECT *,struct tagRECT *)
	Line  5459: [0008A4E8] public: void __cdecl TrayUI::_GetWindowSizes(unsigned int,struct tagRECT const *,struct tagRECT *,struct tagRECT *)
	Line  5492: [00090EB0] private: void __cdecl CTaskListThumbnailWnd::_HandlePaint(struct HDC__ *,struct tagRECT)
	Line  5512: [000937F0] public: virtual long __cdecl CTaskListWnd::SetTaskbarIconRect(struct ITaskGroup *,struct tagRECT const &)
	Line  6497: [000A6F88] protected: struct ITaskListUI * __cdecl CTaskListWndMulti::_GetSecondaryTaskListWnd(struct HMONITOR__ *,struct tagRECT const *,unsigned short const *)
	Line  6556: [000A7B30] public: virtual int __cdecl CTaskListThumbnailWnd::TryGetThumbShareButtonRectFromRegion(struct tagRECT,struct tagRECT *)const 
	Line  6597: [000A8DA8] private: void __cdecl CSecondaryTray::_ComputeHiddenRect(struct tagRECT *)
	Line  6598: [000A8ED0] private: void __cdecl CSecondaryTray::_OnSizing(struct tagRECT *,int)
	Line  6806: [000ACDC8] private: void __cdecl CTaskListThumbnailWnd::_GetScrollingControlRect(int,struct tagRECT *)const 
	Line  7151: [000AED10] public: virtual int __cdecl CTaskListThumbnailWnd::GetToolTipRect(struct ITaskItem *,struct tagRECT *)
	Line  7748: [000B6270] public: virtual long __cdecl CSimpleOleWindow::OnPosRectChangeDB(struct tagRECT *)
	Line  7754: [000B6270] public: virtual long __cdecl CTaskBand::ResizeBorderDW(struct tagRECT const *,struct IUnknown *,int)
	Line  7770: [000B6270] public: virtual long __cdecl CTaskListWndMulti::GetTrayMonitorRect(struct tagRECT *)
	Line  8563: [000CB264] static  <lambda_1b57783c6c0b6a8ec07efb0ede096262>::operator int (__cdecl*)(HMONITOR__ *,HDC__ *,tagRECT *,__int64)()
	Line  8806: [000D6450] public: virtual long __cdecl TrayUI::GetPearlRectForMonitor(struct HMONITOR__ *,struct tagRECT *,unsigned long *)
	Line  8823: [000D6B10] public: virtual void __cdecl TrayUI::GetStuckInfo(struct tagRECT *,unsigned int *)
	Line  8825: [000D6B70] public: virtual void __cdecl TrayUI::GetStuckRect(unsigned int,struct tagRECT *)
	Line  9087: [000E17E0] public: virtual void __cdecl TrayUI::ShowTrayProperties(struct tagRECT const *)
	Line  9151: [000E45C0] public: void __cdecl TrayUI::_HandleMoving(unsigned __int64,struct tagRECT *)
	Line  9162: [000E5488] public: unsigned int __cdecl TrayUI::_RecalcStuckPos(struct tagRECT *)
	Line 10665: [00103700] public: virtual struct tagRECT __cdecl CTaskBand::GetBoundsInRawPixelsRelativeToDesktop(void)
	Line 10961: [001147E8] protected: __int64 __cdecl CTaskBand::_HandleGetTaskbarIconRect(struct HWND__ *,struct tagRECT *)
	Line 12234: [0012ECB0] public: virtual int __cdecl CApi::AdjustWindowRect(struct tagRECT *,unsigned long,int)
	Line 12235: [0012ECD0] public: virtual int __cdecl CApi::AdjustWindowRectEx(struct tagRECT *,unsigned long,int,unsigned long)
	Line 12236: [0012ED00] public: virtual void * __cdecl CApi::BeginBufferedPaint(struct HDC__ *,struct tagRECT const *,enum _BP_BUFFERFORMAT,struct _BP_PAINTPARAMS *,struct HDC__ * *)
	Line 12245: [0012EDD0] public: virtual int __cdecl CApi::CopyRect(struct tagRECT *,struct tagRECT const *)
	Line 12262: [0012F0E0] public: virtual int __cdecl CApi::DrawTextExW(struct HDC__ *,unsigned short *,int,struct tagRECT *,unsigned int,struct tagDRAWTEXTPARAMS *)
	Line 12263: [0012F120] public: virtual long __cdecl CApi::DrawThemeBackground(void *,struct HDC__ *,int,int,struct tagRECT const *,struct tagRECT const *)
	Line 12264: [0012F160] public: virtual long __cdecl CApi::DrawThemeParentBackground(struct HWND__ *,struct HDC__ *,struct tagRECT const *)
	Line 12265: [0012F180] public: virtual long __cdecl CApi::DrawThemeTextEx(void *,struct HDC__ *,int,int,unsigned short const *,int,unsigned long,struct tagRECT *,struct _DTTOPTS const *)
	Line 12266: [0012F1F0] public: virtual long __cdecl CApi::DwmpActivateLivePreview(int,struct HWND__ *,struct HWND__ *,enum LIVEPREVIEW_TRIGGER,struct tagRECT *)
	Line 12270: [0012F280] public: virtual int __cdecl CApi::EnumDisplayMonitors(struct HDC__ *,struct tagRECT const *,int (__cdecl*)(struct HMONITOR__ *,struct HDC__ *,struct tagRECT *,__int64),__int64)
	Line 12271: [0012F2B0] public: virtual int __cdecl CApi::EqualRect(struct tagRECT const *,struct tagRECT const *)
	Line 12273: [0012F300] public: virtual int __cdecl CApi::FillRect(struct HDC__ *,struct tagRECT const *,struct HBRUSH__ *)
	Line 12274: [0012F320] public: virtual void __cdecl CApi::FillRectARGB(struct HDC__ *,struct tagRECT const *,unsigned char,unsigned long)
	Line 12283: [0012F450] public: virtual int __cdecl CApi::GetClientRect(struct HWND__ *,struct tagRECT *)
	Line 12284: [0012F470] public: virtual int __cdecl CApi::GetClipBox(struct HDC__ *,struct tagRECT *)
	Line 12304: [0012F680] public: virtual int __cdecl CApi::GetMonitorRects(struct HMONITOR__ *,struct tagRECT *,int)
	Line 12321: [0012F8D0] public: virtual long __cdecl CApi::GetThemeMargins(void *,struct HDC__ *,int,int,int,struct tagRECT const *,struct _MARGINS *)
	Line 12323: [0012F960] public: virtual long __cdecl CApi::GetThemePartSize(void *,struct HDC__ *,int,int,struct tagRECT const *,enum THEMESIZE,struct tagSIZE *)
	Line 12330: [0012FA50] public: virtual int __cdecl CApi::GetWindowRect(struct HWND__ *,struct tagRECT *)
	Line 12333: [0012FAA0] public: virtual int __cdecl CApi::InflateRect(struct tagRECT *,int,int)
	Line 12335: [0012FAE0] public: virtual int __cdecl CApi::InvalidateRect(struct HWND__ *,struct tagRECT const *,int)
	Line 12339: [0012FB30] public: virtual int __cdecl CApi::IsRectEmpty(struct tagRECT const *)
	Line 12351: [0012FC80] public: virtual struct HMONITOR__ * __cdecl CApi::MonitorFromRect(struct tagRECT const *,unsigned long)
	Line 12353: [0012FCC0] public: virtual int __cdecl CApi::OffsetRect(struct tagRECT *,int,int)
	Line 12359: [0012FDC0] public: virtual int __cdecl CApi::PtInRect(struct tagRECT const *,struct tagPOINT)
	Line 12362: [0012FE30] public: virtual int __cdecl CApi::RedrawWindow(struct HWND__ *,struct tagRECT const *,struct HRGN__ *,unsigned int)
	Line 12401: [00130170] public: virtual int __cdecl CApi::SetRect(struct tagRECT *,int,int,int,int)
	Line 12402: [001301A0] public: virtual int __cdecl CApi::SetRectEmpty(struct tagRECT *)
	Line 12418: [001303A0] public: virtual int __cdecl CApi::UnionRect(struct tagRECT *,struct tagRECT const *,struct tagRECT const *)
	Line 12422: [00130400] public: virtual long __cdecl CObjectFactory::CreateLivePreviewWindow(struct IImmersiveApplication *,bool,struct tagRECT const &,struct tagRECT const &,struct HINSTANCE__ *,struct ILivePreviewWindow * *)
	Line 12562: [00132D10] public: virtual long __cdecl CTaskItem::SetThumbnailClip(struct tagRECT *)
	Line 12633: [00135450] public: virtual long __cdecl CTaskListWndMulti::GetTaskbarIconRect(struct ITaskGroup *,struct ITaskItem *,struct tagRECT *)
	Line 12687: [001364B0] public: virtual long __cdecl CTaskListWndMulti::SetTaskbarIconRect(struct ITaskGroup *,struct tagRECT const &)
	Line 13234: [00144900] private: void __cdecl CTaskListThumbnailWnd::AssignAndPositionShareTooltip(struct HWND__ *,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct tagRECT)
	Line 13236: [00144B54] private: void __cdecl CTaskListThumbnailWnd::CalculateButtonInfo(struct HDC__ *,struct tagRECT,bool,bool,bool,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,bool,enum ShareButton,struct ShareButtonInfo *)
	Line 13243: [00144F78] private: void __cdecl CTaskListThumbnailWnd::DrawRoundedRectWithGDIPlus(struct HDC__ *,bool,bool,struct ShareButtonThemeSetting const &,struct tagRECT)
	Line 13244: [0014500C] private: void __cdecl CTaskListThumbnailWnd::DrawShareRegionText(struct HDC__ *,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > const &,struct ShareButtonThemeSetting const &,struct tagRECT)
	Line 13257: [00145690] public: virtual int __cdecl CTaskListThumbnailWnd::GetMargins(struct tagRECT *)const 
	Line 13262: [00145A30] public: virtual int __cdecl CTaskListThumbnailWnd::GetThumbBarRectFromIndex(int,struct tagRECT *)const 
	Line 13266: [00145B70] public: virtual int __cdecl CTaskListThumbnailWnd::GetWindowLocation(struct tagRECT *)const 
	Line 13269: [00145CD0] private: void __cdecl CTaskListThumbnailWnd::HandleShareRegionTooltip(bool,int,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > const &,struct tagRECT)
	Line 13277: [00145FF0] private: void __cdecl CTaskListThumbnailWnd::PaintShareConfirmAndCancelButtons(struct HDC__ *,int,struct tagRECT,bool,bool)
	Line 13303: [00146C90] private: bool __cdecl CTaskListThumbnailWnd::TryGetContentRectForShareButtons(struct tagRECT,int,struct tagRECT *)const 
	Line 13304: [00146D30] public: virtual int __cdecl CTaskListThumbnailWnd::TryGetThumbShareConfirmationButtonRectsFromRegion(struct tagRECT,struct tagRECT *,struct tagRECT *)const 
	Line 13361: [00149620] public: virtual void __cdecl CSecondaryTray::GetStuckRect(struct tagRECT *)
	Line 13395: [0014A55C] private: void __cdecl CSecondaryTray::_AdjustStuckWidths(struct tagRECT const &,struct tagSIZE *)
	Line 13405: [0014B0B8] private: void __cdecl CSecondaryTray::_OnMoving(unsigned __int64,struct tagRECT *)
	Line 13455: [0014C578] public: static long __cdecl CLivePreviewWindow::CreateInstance(struct IImmersiveApplication *,bool,struct tagRECT const &,struct tagRECT const &,struct HINSTANCE__ *,struct ILivePreviewWindow * *)
	Line 13457: [0014C6C0] public: virtual struct HWND__ * __cdecl CLivePreviewWindow::GetLivePreviewHWND(struct tagRECT *)
	Line 13461: [0014C7B0] private: void __cdecl CLivePreviewWindow::_InitThumbnailWindow(struct tagRECT const &,struct tagRECT const &,struct HINSTANCE__ *)
	Line 13462: [0014C8A4] private: long __cdecl CLivePreviewWindow::_Initialize(struct tagRECT const &,struct tagRECT const &,struct HINSTANCE__ *)
	Line 13609: [00151900] public: virtual bool __cdecl CTaskListWnd::GetBtnGroupRectFromIndex(int,struct tagRECT *)
	Line 13620: [00152360] public: virtual long __cdecl CTaskListWnd::GetGroupLocation(struct ITaskGroup *,struct ITaskItem *,int,struct tagRECT *)
	Line 13632: [001528F0] public: virtual long __cdecl CTaskListWnd::GetTaskbarIconRect(struct ITaskGroup *,struct ITaskItem *,struct tagRECT *)
	Line 13634: [00152A20] public: virtual long __cdecl CTaskListWnd::GetTrayMonitorRect(struct tagRECT *)
	Line 14753: [00174570] public: virtual void __cdecl CTaskThumbnail::GetDestRect(struct tagRECT *)
	Line 14798: [00175740] public: virtual struct tagRECT __cdecl CSecondaryTaskBand::GetBoundsInRawPixelsRelativeToDesktop(void)
	Line 14803: [00175810] public: virtual void __cdecl CSecondaryTaskBand::GetStuckRect(struct tagRECT *)
	Line 15066: [0017D7A8] void __cdecl DrawFocusRect(struct ITrayComponentHost *,struct HWND__ *,struct HDC__ *,struct tagRECT &)
	Line 15119: [0017EC98] static  <lambda_c1df40190820d2e0f30210f8b3b6189b>::operator int (__cdecl*)(HMONITOR__ *,HDC__ *,tagRECT *,__int64)()
	Line 15139: [0017F62C] long __cdecl ImmersiveTray::GetTransitionRects(struct HWND__ *,unsigned int,bool,struct tagRECT *,struct tagRECT *,struct tagRECT *,struct tagRECT *)
	Line 15730: [001A3560] public: static struct Geometry::CRect __cdecl Geometry::CRect::MulDiv(struct tagRECT const &,int,int)
	Line 15756: [001A5F20] static  CCLambdaAdapter<int (__cdecl*)(HDC__ *,unsigned short *,int,tagRECT *,unsigned int,__int64),6>::Adapt<<lambda_2332646ee2da4bd34a0bd6e9819a3d8e> >()
	Line 15759: [001A61E8] long __cdecl DPIToPPIHelpers::DrawThemeTextExPPI(struct HWND__ *,enum DPIToPPIHelpers::ScaleType,unsigned int *,void *,struct HDC__ *,int,int,unsigned short const *,int,unsigned long,struct tagRECT *,struct _DTTOPTS *)
	Line 15760: [001A631C] long __cdecl DPIToPPIHelpers::GetThemeTextExtentPPI(struct HWND__ *,enum DPIToPPIHelpers::ScaleType,unsigned int *,void *,struct HDC__ *,int,int,unsigned short const *,int,unsigned long,struct _DTTOPTS *,struct tagRECT *,struct tagRECT *)
	Line 20800: [001BB7A8] __imp_?DrawTextWithGlow@@YAJPEAUHDC__@@PEBGIPEAUtagRECT@@KKKIIHP6AH0PEAGH2I_J@Z4@Z
	Line 25622: [0020B8E4] struct `public: void __cdecl AutoHideTelemetry::TaskbarAutoHideTelemetry::AutoHideOnTaskbarHide::AutoHideStopMouseHover(struct HMONITOR__ *,struct tagPOINT *,struct tagRECT *,bool)'::`9'::<unnamed-type-_tlgEvent> const `public: void __cdecl AutoHideTelemetry::TaskbarAutoHideTelemetry::AutoHideOnTaskbarHide::AutoHideStopMouseHover(struct HMONITOR__ *,struct tagPOINT *,struct tagRECT *,bool)'::`9'::_tlgEvent
	Line 25635: [0020C796] struct `public: void __cdecl TrayTelemetry::TrayStuckRectChanged_(struct tagRECT const *,unsigned int,struct tagSIZE,struct HMONITOR__ *,bool)'::`4'::<unnamed-type-_tlgEvent> const `public: void __cdecl TrayTelemetry::TrayStuckRectChanged_(struct tagRECT const *,unsigned int,struct tagSIZE,struct HMONITOR__ *,bool)'::`4'::_tlgEvent
	Line 25638: [0020C9DD] struct `public: void __cdecl TrayTelemetry::UpdateTrayRects_(enum TrayTelemetry::TrayStuckUpdateType,struct tagRECT const *)const '::`7'::<unnamed-type-_tlgEvent> const `public: void __cdecl TrayTelemetry::UpdateTrayRects_(enum TrayTelemetry::TrayStuckUpdateType,struct tagRECT const *)const '::`7'::_tlgEvent

		
"""


def generate_hook_code(input_block, output_filename="generated_hooks.cpp"):
    input_block = re.sub(string=input_block, pattern=r"\s+Line\s+\d+\:\s", repl="\n", flags=re.MULTILINE | re.DOTALL)
    pattern = re.compile(
        r'\[\w+\]\s+'
        r'(?P<access>public:|private:|protected:)\s+'
        r'(?P<virtual>virtual\s+)?'
        r'(?P<return_type>[\w\s\*\d\_\:]+?)\s+'
        r'__cdecl\s+'
        r'(?P<full_method>[\w:]+)'
        r'::(?P<method>\w+)'
        r'\((?P<params>.*)\)'
    )
    hook_prefix = "HookFunction"
    hook_defs = []
    hook_array_entries = []
    hook_methods = []
    hook_names = []

    for line in input_block.strip().splitlines():
        line = line.strip()
        if not line:
            continue

        m = pattern.match(line)
        if not m:
            print(f"\033[91mUnable to parse line: {line}\033[0m")
            continue

        access = m.group('access').strip()
        virtual_keyword = m.group('virtual') or ""
        ret_type_raw = m.group('return_type').strip()
        full_method = m.group('full_method').strip()
        method_name = m.group('method').strip()
        params_raw = m.group('params').strip()

        class_name = full_method

        if ret_type_raw.lower() in ["__int64", "int64", "unsigned __int64"]:
            ret_type = "LRESULT"
        else:
            ret_type = ret_type_raw

        param_list = ["void* pThis"]
        params_clean = params_raw.strip()
        if params_clean and params_clean != "void":
            parts = [p.strip() for p in params_clean.split(",") if p.strip()]
            for idx, p in enumerate(parts, start=1):
                p_clean = p.replace("struct ", "")
                param_list.append(f"{p_clean} param{idx}")

        suffix = "_WithArgs" if len(param_list) > 1 else "_WithoutArgs"

        base_name = f"{class_name}_{method_name}"
        base_name = base_name.replace("::", "__")
        hook_base = base_name + suffix

        typedef_line = f"using {hook_base}_t = {ret_type}(WINAPI*)({', '.join(param_list)});"
        original_decl = f"{hook_base}_t {hook_base}_Original;"

        params_str = ", ".join(param_list)
        log_name = base_name

        if ret_type == "void":
            hook_func = (
                f"""void WINAPI {hook_base}_Hook({params_str}) {{
                Wh_Log(L"Method called: {log_name}");
                {hook_base}_Original({', '.join([p.split()[-1] for p in param_list])});
            }}"""
            )
        else:
            hook_func = (
                f"""{ret_type} WINAPI {hook_base}_Hook({params_str}) {{
                Wh_Log(L"Method called: {log_name}");
                return {hook_base}_Original({', '.join([p.split()[-1] for p in param_list])});
            }}"""
            )

        hook_entry = (
            f'    {{ {{LR"({line[line.find(access):].strip()}{virtual_keyword})"}},\n'
            f'      &{hook_base}_Original,\n'
            f'      {hook_base}_Hook,\n'
            f'    }}'
        )

        hook_defs.append(typedef_line)
        hook_defs.append(original_decl)
        hook_defs.append(hook_func)
        hook_defs.append("")
        hook_array_entries.append(hook_entry)

        method_name_str = hook_base
        hook_names.append(method_name_str)
        hook_method = (
            f"bool {hook_prefix}{method_name_str}() {{\n"
            f"    HMODULE module = LoadLibrary(L\"taskbar.dll\");\n"
            f"    if (!module) {{\n"
            f"        Wh_Log(L\"Failed to load taskbar.dll for {method_name_str}\");\n"
            f"        return false;\n"
            f"    }}\n\n"
            f"    WindhawkUtils::SYMBOL_HOOK hook[] = {{ {{ {{LR\"({line[line.find(access):].strip()}{virtual_keyword})\"}},\n"
            f"                                         &{hook_base}_Original,\n"
            f"                                         {hook_base}_Hook }} }};\n\n"
            f"    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));\n"
            f"}}\n"
        )
        hook_methods.append(hook_method)

    output_lines = []
    output_lines.append(
        r"""
// ==WindhawkMod==
// @id      hooks-spy-mod
// @name    Spy for function calls
// @description     Print the names of functions being called
// @version 0.1
// @author  DarkionAvey
// @include explorer.exe
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod prints the names of functions being called.
*/
// ==/WindhawkModReadme==


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////   __    __    ______     ______    __  ___      _______.             _______..______   ____    ____   ////
////  |  |  |  |  /  __  \   /  __  \  |  |/  /     /       |            /       ||   _  \  \   \  /   /   ////
////  |  |__|  | |  |  |  | |  |  |  | |  '  /     |   (----` ______    |   (----`|  |_)  |  \   \/   /    ////
////  |   __   | |  |  |  | |  |  |  | |    <       \   \    |______|    \   \    |   ___/    \_    _/     ////
////  |  |  |  | |  `--'  | |  `--'  | |  .  \  .----)   |           .----)   |   |  |          |  |       ////
////  |__|  |__|  \______/   \______/  |__|\__\ |_______/            |_______/    | _|          |__|       ////
////                                                                                                       ////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include <windhawk_utils.h>
#include <windhawk_api.h>
#include <string>
#include <atomic>
#include <functional>
#include <limits>
#include <optional>
#include <regex>
#include <dwmapi.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <sstream>
#include <string_view>
#include <chrono>
""")
    output_lines.append("// Auto-generated hook definitions and functions")
    output_lines.append("")
    output_lines.extend(hook_defs)
    output_lines.append("")
    for hm in hook_methods:
        output_lines.append(hm)
        output_lines.append("")
    init_function = ["BOOL Wh_ModInit() {", "    bool anyHooked = false;"]
    for idx, name in enumerate(hook_names, start=1):
        init_function.append(f"    if (!{hook_prefix}{name}()) {{")
        init_function.append(f"        Wh_Log(L\"Method hooking failed: {name}\");")
        init_function.append("    } else {")
        init_function.append("        anyHooked = true;")
        init_function.append("    }")
        init_function.append("")
    init_function.append("    if (!anyHooked) {")
    init_function.append("        return FALSE;")
    init_function.append("    }")
    init_function.append("    return TRUE;")
    init_function.append("}")
    init_function.append("")
    init_function.append("void Wh_ModUninit() {}")
    init_function.append("void Wh_ModSettingsChanged() {}")
    output_lines.extend(init_function)

    with open(output_filename, "w", encoding="utf-8") as f:
        f.write("\n".join(output_lines))


if __name__ == '__main__':
    generate_hook_code(input_block)
