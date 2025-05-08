
// ==WindhawkMod==
// @id      hooks-spy-mod
// @name    Spy for function calls
// @description     Print the names of functions being called
// @version 0.1
// @author  DarkionAvey
// @include explorer.exe
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lcomctl32 -Wl,--export-all-symbols
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

// Auto-generated hook definitions and functions

using ShellIconLoader_LoadShellIconCacheIcon_WithArgs_t = bool(WINAPI*)(void* pThis, int param1);
ShellIconLoader_LoadShellIconCacheIcon_WithArgs_t ShellIconLoader_LoadShellIconCacheIcon_WithArgs_Original;
bool WINAPI ShellIconLoader_LoadShellIconCacheIcon_WithArgs_Hook(void* pThis, int param1) {
                Wh_Log(L"Method called: ShellIconLoader_LoadShellIconCacheIcon");
                return ShellIconLoader_LoadShellIconCacheIcon_WithArgs_Original(pThis, param1);
            }

using ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs_t = long(WINAPI*)(void* pThis);
ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs_t ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs_Original;
long WINAPI ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: ShellIconLoader__CLoadIconTask_InternalResumeRT");
                return ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs_Original(pThis);
            }

using ShellIconLoader_s_SetIconAsync_WithArgs_t = void(WINAPI*)( void * param1, void * param2, int param3, int param4);
ShellIconLoader_s_SetIconAsync_WithArgs_t ShellIconLoader_s_SetIconAsync_WithArgs_Original;
static void WINAPI ShellIconLoader_s_SetIconAsync_WithArgs_Hook( void * param1, void * param2, int param3, int param4) {
                Wh_Log(L"Method called: ShellIconLoader_s_SetIconAsync");
                return ShellIconLoader_s_SetIconAsync_WithArgs_Original( param1, param2, param3, param4);
            }

using ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs_t = void(WINAPI*)(void* pThis, int param1, int param2);
ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs_t ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs_Original;
void WINAPI ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs_Hook(void* pThis, int param1, int param2) {
                Wh_Log(L"Method called: ShellIconLoader_TriggerShellIconCacheBackgroundLoad");
                ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs_Original(pThis, param1, param2);
            }

using ShellIconLoader_s_ForceImagePresent_WithArgs_t = long(WINAPI*)( int param1, int param2, HICON__ * * param3);
ShellIconLoader_s_ForceImagePresent_WithArgs_t ShellIconLoader_s_ForceImagePresent_WithArgs_Original;
static long WINAPI ShellIconLoader_s_ForceImagePresent_WithArgs_Hook( int param1, int param2, HICON__ * * param3) {
                Wh_Log(L"Method called: ShellIconLoader_s_ForceImagePresent");
                return ShellIconLoader_s_ForceImagePresent_WithArgs_Original( param1, param2, param3);
            }

using ShellIconLoader_s_GetShellImageListSizeId_WithArgs_t = int(WINAPI*)( int param1);
ShellIconLoader_s_GetShellImageListSizeId_WithArgs_t ShellIconLoader_s_GetShellImageListSizeId_WithArgs_Original;
static int WINAPI ShellIconLoader_s_GetShellImageListSizeId_WithArgs_Hook( int param1) {
                Wh_Log(L"Method called: ShellIconLoader_s_GetShellImageListSizeId");
                return ShellIconLoader_s_GetShellImageListSizeId_WithArgs_Original( param1);
            }

using ShellIconLoader_s_GetShellIconSize_WithArgs_t = int(WINAPI*)( int param1);
ShellIconLoader_s_GetShellIconSize_WithArgs_t ShellIconLoader_s_GetShellIconSize_WithArgs_Original;
static int WINAPI ShellIconLoader_s_GetShellIconSize_WithArgs_Hook( int param1) {
                Wh_Log(L"Method called: ShellIconLoader_s_GetShellIconSize");
                return ShellIconLoader_s_GetShellIconSize_WithArgs_Original( param1);
            }

using ShellIconLoaderV2_s_GetShellIconSize_WithArgs_t =  int(WINAPI*)( int param1);
ShellIconLoaderV2_s_GetShellIconSize_WithArgs_t ShellIconLoaderV2_s_GetShellIconSize_WithArgs_Original;
static int WINAPI ShellIconLoaderV2_s_GetShellIconSize_WithArgs_Hook( int param1) {
    auto x=ShellIconLoaderV2_s_GetShellIconSize_WithArgs_Original( param1);
                Wh_Log(L"Method called: ShellIconLoaderV2_s_GetShellIconSize param1 %d %d",param1,x);
                return x;
            }

using ShellIconLoader_s_SendMessageCallback_WithArgs_t =  void(WINAPI*)( HWND__ * param1, unsigned int param2, unsigned __int64 param3, __int64 param4);
ShellIconLoader_s_SendMessageCallback_WithArgs_t ShellIconLoader_s_SendMessageCallback_WithArgs_Original;
static void WINAPI ShellIconLoader_s_SendMessageCallback_WithArgs_Hook( HWND__ * param1, unsigned int param2, unsigned __int64 param3, __int64 param4) {
                Wh_Log(L"Method called: ShellIconLoader_s_SendMessageCallback");
                return ShellIconLoader_s_SendMessageCallback_WithArgs_Original( param1, param2, param3, param4);
            }
namespace ShellIconLoader{
    struct SendMessageCallbackInfo;
};

using ShellIconLoader_SendMessageCallbackW_WithArgs_t = void(WINAPI*)(void* pThis, ShellIconLoader::SendMessageCallbackInfo * param1, HICON__ * param2);
ShellIconLoader_SendMessageCallbackW_WithArgs_t ShellIconLoader_SendMessageCallbackW_WithArgs_Original;
void WINAPI ShellIconLoader_SendMessageCallbackW_WithArgs_Hook(void* pThis, ShellIconLoader::SendMessageCallbackInfo * param1, HICON__ * param2) {
                Wh_Log(L"Method called: ShellIconLoader_SendMessageCallbackW");
                ShellIconLoader_SendMessageCallbackW_WithArgs_Original(pThis, param1, param2);
            }

using ShellIconLoader_LoadAsyncIcon_WithArgs_t = bool(WINAPI*)(void* pThis, int param1);
ShellIconLoader_LoadAsyncIcon_WithArgs_t ShellIconLoader_LoadAsyncIcon_WithArgs_Original;
bool WINAPI ShellIconLoader_LoadAsyncIcon_WithArgs_Hook(void* pThis, int param1) {
                Wh_Log(L"Method called: ShellIconLoader_LoadAsyncIcon");
                return ShellIconLoader_LoadAsyncIcon_WithArgs_Original(pThis, param1);
            }

using ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs_t = void(WINAPI*)(void* pThis);
ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs_t ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs_Original;
void WINAPI ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: ShellIconLoader_TryLoadIconFromResourceIfNeeded");
                ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs_Original(pThis);
            }

using ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs_t = bool(WINAPI*)(_ICONINFOEXW & param1, int param2);
ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs_t ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs_Original;
static bool WINAPI ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs_Hook( _ICONINFOEXW & param1, int param2) {
                Wh_Log(L"Method called: ShellIconLoader_s_ShouldLoadBetterIcon");
                return ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs_Original( param1, param2);
            }
interface IRunnableTask;
using ShellIconLoaderTaskScheduler_AddTask_WithArgs_t = long(WINAPI*)(void* pThis, IRunnableTask * param1);
ShellIconLoaderTaskScheduler_AddTask_WithArgs_t ShellIconLoaderTaskScheduler_AddTask_WithArgs_Original;
long WINAPI ShellIconLoaderTaskScheduler_AddTask_WithArgs_Hook(void* pThis, IRunnableTask * param1) {
                Wh_Log(L"Method called: ShellIconLoaderTaskScheduler_AddTask");
                return ShellIconLoaderTaskScheduler_AddTask_WithArgs_Original(pThis, param1);
            }
interface ITrayComponentHost; interface IShellIconLoadData;interface IIconLoaderNotifications2;interface IIconLoadingFunctions;
using ShellIconLoader_RuntimeClassInitialize_WithArgs_t = long(WINAPI*)(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4);
ShellIconLoader_RuntimeClassInitialize_WithArgs_t ShellIconLoader_RuntimeClassInitialize_WithArgs_Original;
long WINAPI ShellIconLoader_RuntimeClassInitialize_WithArgs_Hook(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4) {
                Wh_Log(L"Method called: ShellIconLoader_RuntimeClassInitialize");
                return ShellIconLoader_RuntimeClassInitialize_WithArgs_Original(pThis, param1, param2, param3, param4);
            }

using ShellIconLoader_LoadSyncWindowIcon_WithArgs_t = bool(WINAPI*)(void* pThis, int param1, HICON__ * * param2);
ShellIconLoader_LoadSyncWindowIcon_WithArgs_t ShellIconLoader_LoadSyncWindowIcon_WithArgs_Original;
bool WINAPI ShellIconLoader_LoadSyncWindowIcon_WithArgs_Hook(void* pThis, int param1, HICON__ * * param2) {
                Wh_Log(L"Method called: ShellIconLoader_LoadSyncWindowIcon");
                return ShellIconLoader_LoadSyncWindowIcon_WithArgs_Original(pThis, param1, param2);
            }

using LoadWindowIconOnly_WithArgs_t = long(WINAPI*)(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4);
LoadWindowIconOnly_WithArgs_t LoadWindowIconOnly_WithArgs_Original;
long WINAPI LoadWindowIconOnly_WithArgs_Hook(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4) {
                Wh_Log(L"Method called: LoadWindowIconOnly");
                return LoadWindowIconOnly_WithArgs_Original(pThis, param1, param2, param3, param4);
            }

using LoadShellIconCacheIconOnly_WithArgs_t = long(WINAPI*)(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4);
LoadShellIconCacheIconOnly_WithArgs_t LoadShellIconCacheIconOnly_WithArgs_Original;
long WINAPI LoadShellIconCacheIconOnly_WithArgs_Hook(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4) {
                Wh_Log(L"Method called: LoadShellIconCacheIconOnly");
                return LoadShellIconCacheIconOnly_WithArgs_Original(pThis, param1, param2, param3, param4);
            }

using ShellIconLoaderV2_s_ForceImagePresent_WithArgs_t = long(WINAPI*)(int param1, int param2, HICON__ * * param3);
ShellIconLoaderV2_s_ForceImagePresent_WithArgs_t ShellIconLoaderV2_s_ForceImagePresent_WithArgs_Original;
static long WINAPI ShellIconLoaderV2_s_ForceImagePresent_WithArgs_Hook(int param1, int param2, HICON__ * * param3) {
                Wh_Log(L"Method called: ShellIconLoaderV2_s_ForceImagePresent");
                return ShellIconLoaderV2_s_ForceImagePresent_WithArgs_Original( param1, param2, param3);
            }

using LoadWindowAndShellIcon_WithArgs_t = long(WINAPI*)(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4);
LoadWindowAndShellIcon_WithArgs_t LoadWindowAndShellIcon_WithArgs_Original;
long WINAPI LoadWindowAndShellIcon_WithArgs_Hook(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4) {
                Wh_Log(L"Method called: LoadWindowAndShellIcon");
                return LoadWindowAndShellIcon_WithArgs_Original(pThis, param1, param2, param3, param4);
            }

using ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs_t = bool(WINAPI*)(void* pThis, int param1, HICON__ * * param2);
ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs_t ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs_Original;
bool WINAPI ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs_Hook(void* pThis, int param1, HICON__ * * param2) {
                Wh_Log(L"Method called: ShellIconLoaderV2_LoadSyncWindowIcon %d",param1);
                return ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs_Original(pThis, param1, param2);
            }

using ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs_t = void(WINAPI*)(void* pThis);
ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs_t ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs_Original;
void WINAPI ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded");
                ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs_Original(pThis);
            }

using ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs_t = int(WINAPI*)(int param1);
ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs_t ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs_Original;
static int WINAPI ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs_Hook(int param1) {
                Wh_Log(L"Method called: ShellIconLoaderV2_s_GetShellImageListSizeId");
                return ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs_Original( param1);
            }

using CWinRTAppIconLoader_s_LoadIconSync_WithArgs_t = long(WINAPI*)( unsigned long param1, unsigned short const * param2, tagSIZE const & param3, tagSIZE const & param4, tagSIZE const & param5, unsigned int param6, HICON__ * * param7);
CWinRTAppIconLoader_s_LoadIconSync_WithArgs_t CWinRTAppIconLoader_s_LoadIconSync_WithArgs_Original;
static long WINAPI CWinRTAppIconLoader_s_LoadIconSync_WithArgs_Hook( unsigned long param1, unsigned short const * param2, tagSIZE const & param3, tagSIZE const & param4, tagSIZE const & param5, unsigned int param6, HICON__ * * param7) {
                Wh_Log(L"Method called: CWinRTAppIconLoader_s_LoadIconSync %d",param6);
                return CWinRTAppIconLoader_s_LoadIconSync_WithArgs_Original( param1, param2, param3, param4, param5, param6, param7);
            }


using ShellIconLoaderTaskScheduler_CountTasks_WithArgs_t = unsigned int(WINAPI*)(void* pThis, _GUID const & param1);
ShellIconLoaderTaskScheduler_CountTasks_WithArgs_t ShellIconLoaderTaskScheduler_CountTasks_WithArgs_Original;
unsigned int WINAPI ShellIconLoaderTaskScheduler_CountTasks_WithArgs_Hook(void* pThis, _GUID const & param1) {
                Wh_Log(L"Method called: ShellIconLoaderTaskScheduler_CountTasks");
                return ShellIconLoaderTaskScheduler_CountTasks_WithArgs_Original(pThis, param1);
            }

using ShellIconLoaderTaskScheduler_QueryInterface_WithArgs_t = long(WINAPI*)(void* pThis, _GUID const & param1, void * * param2);
ShellIconLoaderTaskScheduler_QueryInterface_WithArgs_t ShellIconLoaderTaskScheduler_QueryInterface_WithArgs_Original;
long WINAPI ShellIconLoaderTaskScheduler_QueryInterface_WithArgs_Hook(void* pThis, _GUID const & param1, void * * param2) {
                Wh_Log(L"Method called: ShellIconLoaderTaskScheduler_QueryInterface");
                return ShellIconLoaderTaskScheduler_QueryInterface_WithArgs_Original(pThis, param1, param2);
            }

using ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs_t = long(WINAPI*)(void* pThis, _GUID const & param1, unsigned __int64 param2, int param3);
ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs_t ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs_Original;
long WINAPI ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs_Hook(void* pThis, _GUID const & param1, unsigned __int64 param2, int param3) {
                Wh_Log(L"Method called: ShellIconLoaderTaskScheduler_RemoveTasks");
                return ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs_Original(pThis, param1, param2, param3);
            }


using ShellIconLoaderTaskScheduler_Status_WithArgs_t = long(WINAPI*)(void* pThis, unsigned long param1, unsigned long param2);
ShellIconLoaderTaskScheduler_Status_WithArgs_t ShellIconLoaderTaskScheduler_Status_WithArgs_Original;
long WINAPI ShellIconLoaderTaskScheduler_Status_WithArgs_Hook(void* pThis, unsigned long param1, unsigned long param2) {
                Wh_Log(L"Method called: ShellIconLoaderTaskScheduler_Status");
                return ShellIconLoaderTaskScheduler_Status_WithArgs_Original(pThis, param1, param2);
            }

using ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs_t = long(WINAPI*)( unsigned short const * param1, unsigned short param2, int param3, HICON__ * * param4);
ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs_t ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs_Original;
static long WINAPI ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs_Hook( unsigned short const * param1, unsigned short param2, int param3, HICON__ * * param4) {
                Wh_Log(L"Method called: ShellIconLoader_s_LoadIconsFromResourceSync");
                return ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs_Original( param1, param2, param3, param4);
            }
#include <winrt/base.h>
using ShellIconLoaderV2_ForceImagePresentAsync_WithArgs_t = struct winrt::fire_and_forget(WINAPI*)(void* pThis, int param1, int param2);
ShellIconLoaderV2_ForceImagePresentAsync_WithArgs_t ShellIconLoaderV2_ForceImagePresentAsync_WithArgs_Original;
struct winrt::fire_and_forget WINAPI ShellIconLoaderV2_ForceImagePresentAsync_WithArgs_Hook(void* pThis, int param1, int param2) {
                Wh_Log(L"Method called: ShellIconLoaderV2_ForceImagePresentAsync");
                return ShellIconLoaderV2_ForceImagePresentAsync_WithArgs_Original(pThis, param1, param2);
            }

using ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs_t = long(WINAPI*)(void* pThis);
ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs_t ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs_Original;
long WINAPI ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2");
                return ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs_Original(pThis);
            }

using ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs_t = long(WINAPI*)(void* pThis);
ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs_t ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs_Original;
long WINAPI ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2");
                return ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs_Original(pThis);
            }

using ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs_t = long(WINAPI*)(void* pThis);
ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs_t ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs_Original;
long WINAPI ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: ShellIconLoaderV2_InitializeWindowAndShellIconV2");
                return ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs_Original(pThis);
            }

using ShellIconLoaderV2_LoadAsyncIcon_WithArgs_t = struct winrt::fire_and_forget(WINAPI*)(void* pThis, int param1);
ShellIconLoaderV2_LoadAsyncIcon_WithArgs_t ShellIconLoaderV2_LoadAsyncIcon_WithArgs_Original;
struct winrt::fire_and_forget WINAPI ShellIconLoaderV2_LoadAsyncIcon_WithArgs_Hook(void* pThis, int param1) {
                Wh_Log(L"Method called: ShellIconLoaderV2_LoadAsyncIcon %d",param1);
                return ShellIconLoaderV2_LoadAsyncIcon_WithArgs_Original(pThis, param1);
            }

using ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs_t = bool(WINAPI*)(void* pThis, int param1);
ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs_t ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs_Original;
bool WINAPI ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs_Hook(void* pThis, int param1) {
                Wh_Log(L"Method called: ShellIconLoaderV2_LoadShellIconCacheIcon");
                return ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs_Original(pThis, param1);
            }

using LoadShellIconCacheIconOnlyV2_WithArgs_t = long(WINAPI*)(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4);
LoadShellIconCacheIconOnlyV2_WithArgs_t LoadShellIconCacheIconOnlyV2_WithArgs_Original;
long WINAPI LoadShellIconCacheIconOnlyV2_WithArgs_Hook(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4) {
                Wh_Log(L"Method called: LoadShellIconCacheIconOnlyV2");
                return LoadShellIconCacheIconOnlyV2_WithArgs_Original(pThis, param1, param2, param3, param4);
            }

using LoadWindowAndShellIconV2_WithArgs_t = long(WINAPI*)(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4);
LoadWindowAndShellIconV2_WithArgs_t LoadWindowAndShellIconV2_WithArgs_Original;
long WINAPI LoadWindowAndShellIconV2_WithArgs_Hook(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4) {
                Wh_Log(L"Method called: LoadWindowAndShellIconV2");
                return LoadWindowAndShellIconV2_WithArgs_Original(pThis, param1, param2, param3, param4);
            }

using LoadWindowIconOnlyV2_WithArgs_t = long(WINAPI*)(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4);
LoadWindowIconOnlyV2_WithArgs_t LoadWindowIconOnlyV2_WithArgs_Original;
long WINAPI LoadWindowIconOnlyV2_WithArgs_Hook(void* pThis, ITrayComponentHost * param1, IShellIconLoadData * param2, IIconLoaderNotifications2 * param3, IIconLoadingFunctions * param4) {
                Wh_Log(L"Method called: LoadWindowIconOnlyV2");
                return LoadWindowIconOnlyV2_WithArgs_Original(pThis, param1, param2, param3, param4);
            }

using ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs_t = bool(WINAPI*)(void* pThis, int param1);
ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs_t ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs_Original;
bool WINAPI ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs_Hook(void* pThis, int param1) {
                Wh_Log(L"Method called: ShellIconLoaderV2_TryLoadAsyncIcon %d",param1);
                ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs_Original(pThis, param1);
                return false;
            }

using ShellIconLoaderV2_final_release_WithArgs_t = struct winrt::fire_and_forget(WINAPI*)( void* param2);
ShellIconLoaderV2_final_release_WithArgs_t ShellIconLoaderV2_final_release_WithArgs_Original;
static struct winrt::fire_and_forget WINAPI ShellIconLoaderV2_final_release_WithArgs_Hook( void* param2) {
                Wh_Log(L"Method called: ShellIconLoaderV2_final_release");
                return ShellIconLoaderV2_final_release_WithArgs_Original(  param2);
            }

using ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs_t = long(WINAPI*)( unsigned short const * param1, unsigned short param2, int param3, HICON__ * * param4);
ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs_t ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs_Original;
static long WINAPI ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs_Hook( unsigned short const * param1, unsigned short param2, int param3, HICON__ * * param4) {
    // param3=500;
    Wh_Log(L"Method called: ShellIconLoaderV2_s_LoadIconsFromResourceSync - param1: %ls, param2: %u, param3: %d, param4: 0x%p",
           param1, param2, param3, param4);
    return ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs_Original( param1, param2, 4, param4);
}


using ShellIconLoaderV2_s_SetIconCallback_WithArgs_t =  void(WINAPI*)( void * param1, void * param2, int param3, int param4);
ShellIconLoaderV2_s_SetIconCallback_WithArgs_t ShellIconLoaderV2_s_SetIconCallback_WithArgs_Original;
static void WINAPI ShellIconLoaderV2_s_SetIconCallback_WithArgs_Hook( void * param1, void * param2, int param3, int param4) {
                Wh_Log(L"Method called: ShellIconLoaderV2_s_SetIconCallback");
                return ShellIconLoaderV2_s_SetIconCallback_WithArgs_Original( param1, param2, param3, param4);
            }

using ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs_t = bool(WINAPI*)( _ICONINFOEXW const & param1, int param2);
ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs_t ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs_Original;
static bool WINAPI ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs_Hook( _ICONINFOEXW const & param1, int param2) {
                Wh_Log(L"Method called: ShellIconLoaderV2_s_ShouldLoadBetterIcon");
                ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs_Original(param1, param2);
                return true;
            }


bool HookFunctionShellIconLoader_LoadShellIconCacheIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_LoadShellIconCacheIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: bool __cdecl ShellIconLoader::LoadShellIconCacheIcon(int))"},
                                         &ShellIconLoader_LoadShellIconCacheIcon_WithArgs_Original,
                                         ShellIconLoader_LoadShellIconCacheIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: virtual long __cdecl ShellIconLoader::CLoadIconTask::InternalResumeRT(void))"},
                                         &ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs_Original,
                                         ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_s_SetIconAsync_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_s_SetIconAsync_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static void __cdecl ShellIconLoader::s_SetIconAsync(void *,void *,int,int))"},
                                         &ShellIconLoader_s_SetIconAsync_WithArgs_Original,
                                         ShellIconLoader_s_SetIconAsync_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: void __cdecl ShellIconLoader::TriggerShellIconCacheBackgroundLoad(int,int))"},
                                         &ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs_Original,
                                         ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_s_ForceImagePresent_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_s_ForceImagePresent_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static long __cdecl ShellIconLoader::s_ForceImagePresent(int,int,struct HICON__ * *))"},
                                         &ShellIconLoader_s_ForceImagePresent_WithArgs_Original,
                                         ShellIconLoader_s_ForceImagePresent_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_s_GetShellImageListSizeId_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_s_GetShellImageListSizeId_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static int __cdecl ShellIconLoader::s_GetShellImageListSizeId(int))"},
                                         &ShellIconLoader_s_GetShellImageListSizeId_WithArgs_Original,
                                         ShellIconLoader_s_GetShellImageListSizeId_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_s_GetShellIconSize_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_s_GetShellIconSize_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static int __cdecl ShellIconLoader::s_GetShellIconSize(int))"},
                                         &ShellIconLoader_s_GetShellIconSize_WithArgs_Original,
                                         ShellIconLoader_s_GetShellIconSize_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_s_GetShellIconSize_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_s_GetShellIconSize_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static int __cdecl ShellIconLoaderV2::s_GetShellIconSize(int))"},
                                         &ShellIconLoaderV2_s_GetShellIconSize_WithArgs_Original,
                                         ShellIconLoaderV2_s_GetShellIconSize_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_s_SendMessageCallback_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_s_SendMessageCallback_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static void __cdecl ShellIconLoader::s_SendMessageCallback(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
                                         &ShellIconLoader_s_SendMessageCallback_WithArgs_Original,
                                         ShellIconLoader_s_SendMessageCallback_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_SendMessageCallbackW_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_SendMessageCallbackW_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: void __cdecl ShellIconLoader::SendMessageCallbackW(struct ShellIconLoader::SendMessageCallbackInfo *,struct HICON__ *))"},
                                         &ShellIconLoader_SendMessageCallbackW_WithArgs_Original,
                                         ShellIconLoader_SendMessageCallbackW_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_LoadAsyncIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_LoadAsyncIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: bool __cdecl ShellIconLoader::LoadAsyncIcon(int))"},
                                         &ShellIconLoader_LoadAsyncIcon_WithArgs_Original,
                                         ShellIconLoader_LoadAsyncIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: void __cdecl ShellIconLoader::TryLoadIconFromResourceIfNeeded(void))"},
                                         &ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs_Original,
                                         ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_s_ShouldLoadBetterIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static bool __cdecl ShellIconLoader::s_ShouldLoadBetterIcon(struct _ICONINFOEXW &,int))"},
                                         &ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs_Original,
                                         ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderTaskScheduler_AddTask_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderTaskScheduler_AddTask_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: long __cdecl ShellIconLoaderTaskScheduler::AddTask(struct IRunnableTask *))"},
                                         &ShellIconLoaderTaskScheduler_AddTask_WithArgs_Original,
                                         ShellIconLoaderTaskScheduler_AddTask_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_RuntimeClassInitialize_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_RuntimeClassInitialize_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: long __cdecl ShellIconLoader::RuntimeClassInitialize(struct ITrayComponentHost *,struct IShellIconLoadData *,struct IIconLoaderNotifications2 *,struct IIconLoadingFunctions *))"},
                                         &ShellIconLoader_RuntimeClassInitialize_WithArgs_Original,
                                         ShellIconLoader_RuntimeClassInitialize_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_LoadSyncWindowIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_LoadSyncWindowIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: bool __cdecl ShellIconLoader::LoadSyncWindowIcon(int,struct HICON__ * *))"},
                                         &ShellIconLoader_LoadSyncWindowIcon_WithArgs_Original,
                                         ShellIconLoader_LoadSyncWindowIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionLoadWindowIconOnly_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for LoadWindowIconOnly_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(long __cdecl LoadWindowIconOnly(struct ITrayComponentHost *,struct IShellIconLoadData *,struct IIconLoaderNotifications2 *,struct IIconLoadingFunctions *))"},
                                         &LoadWindowIconOnly_WithArgs_Original,
                                         LoadWindowIconOnly_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionLoadShellIconCacheIconOnly_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for LoadShellIconCacheIconOnly_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(long __cdecl LoadShellIconCacheIconOnly(struct ITrayComponentHost *,struct IShellIconLoadData *,struct IIconLoaderNotifications2 *,struct IIconLoadingFunctions *))"},
                                         &LoadShellIconCacheIconOnly_WithArgs_Original,
                                         LoadShellIconCacheIconOnly_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_s_ForceImagePresent_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_s_ForceImagePresent_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static long __cdecl ShellIconLoaderV2::s_ForceImagePresent(int,int,struct HICON__ * *))"},
                                         &ShellIconLoaderV2_s_ForceImagePresent_WithArgs_Original,
                                         ShellIconLoaderV2_s_ForceImagePresent_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionLoadWindowAndShellIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for LoadWindowAndShellIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(long __cdecl LoadWindowAndShellIcon(struct ITrayComponentHost *,struct IShellIconLoadData *,struct IIconLoaderNotifications2 *,struct IIconLoadingFunctions *))"},
                                         &LoadWindowAndShellIcon_WithArgs_Original,
                                         LoadWindowAndShellIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_LoadSyncWindowIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: bool __cdecl ShellIconLoaderV2::LoadSyncWindowIcon(int,struct HICON__ * *))"},
                                         &ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs_Original,
                                         ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: void __cdecl ShellIconLoaderV2::TryLoadIconFromResourceIfNeeded(void))"},
                                         &ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs_Original,
                                         ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static int __cdecl ShellIconLoaderV2::s_GetShellImageListSizeId(int))"},
                                         &ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs_Original,
                                         ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionCWinRTAppIconLoader_s_LoadIconSync_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for CWinRTAppIconLoader_s_LoadIconSync_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static long __cdecl CWinRTAppIconLoader::s_LoadIconSync(unsigned long,unsigned short const *,struct tagSIZE const &,struct tagSIZE const &,struct tagSIZE const &,unsigned int,struct HICON__ * *))"},
                                         &CWinRTAppIconLoader_s_LoadIconSync_WithArgs_Original,
                                         CWinRTAppIconLoader_s_LoadIconSync_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderTaskScheduler_CountTasks_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderTaskScheduler_CountTasks_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: virtual unsigned int __cdecl ShellIconLoaderTaskScheduler::CountTasks(struct _GUID const &))"},
                                         &ShellIconLoaderTaskScheduler_CountTasks_WithArgs_Original,
                                         ShellIconLoaderTaskScheduler_CountTasks_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderTaskScheduler_QueryInterface_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderTaskScheduler_QueryInterface_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: virtual long __cdecl ShellIconLoaderTaskScheduler::QueryInterface(struct _GUID const &,void * *))"},
                                         &ShellIconLoaderTaskScheduler_QueryInterface_WithArgs_Original,
                                         ShellIconLoaderTaskScheduler_QueryInterface_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderTaskScheduler_RemoveTasks_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: virtual long __cdecl ShellIconLoaderTaskScheduler::RemoveTasks(struct _GUID const &,unsigned __int64,int))"},
                                         &ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs_Original,
                                         ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}




bool HookFunctionShellIconLoaderTaskScheduler_Status_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderTaskScheduler_Status_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: virtual long __cdecl ShellIconLoaderTaskScheduler::Status(unsigned long,unsigned long))"},
                                         &ShellIconLoaderTaskScheduler_Status_WithArgs_Original,
                                         ShellIconLoaderTaskScheduler_Status_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoader_s_LoadIconsFromResourceSync_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static long __cdecl ShellIconLoader::s_LoadIconsFromResourceSync(unsigned short const *,unsigned short,int,struct HICON__ * *))"},
                                         &ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs_Original,
                                         ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_ForceImagePresentAsync_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_ForceImagePresentAsync_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: struct winrt::fire_and_forget __cdecl ShellIconLoaderV2::ForceImagePresentAsync(int,int))"},
                                         &ShellIconLoaderV2_ForceImagePresentAsync_WithArgs_Original,
                                         ShellIconLoaderV2_ForceImagePresentAsync_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: long __cdecl ShellIconLoaderV2::InitializeLoadWindowIconOnlyV2(void))"},
                                         &ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs_Original,
                                         ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: long __cdecl ShellIconLoaderV2::InitializeShellIconCacheIconOnlyV2(void))"},
                                         &ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs_Original,
                                         ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: long __cdecl ShellIconLoaderV2::InitializeWindowAndShellIconV2(void))"},
                                         &ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs_Original,
                                         ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_LoadAsyncIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_LoadAsyncIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: struct winrt::fire_and_forget __cdecl ShellIconLoaderV2::LoadAsyncIcon(int))"},
                                         &ShellIconLoaderV2_LoadAsyncIcon_WithArgs_Original,
                                         ShellIconLoaderV2_LoadAsyncIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: bool __cdecl ShellIconLoaderV2::LoadShellIconCacheIcon(int))"},
                                         &ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs_Original,
                                         ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionLoadShellIconCacheIconOnlyV2_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for LoadShellIconCacheIconOnlyV2_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(long __cdecl LoadShellIconCacheIconOnlyV2(struct ITrayComponentHost *,struct IShellIconLoadData *,struct IIconLoaderNotifications2 *,struct IIconLoadingFunctions *))"},
                                         &LoadShellIconCacheIconOnlyV2_WithArgs_Original,
                                         LoadShellIconCacheIconOnlyV2_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionLoadWindowAndShellIconV2_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for LoadWindowAndShellIconV2_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(long __cdecl LoadWindowAndShellIconV2(struct ITrayComponentHost *,struct IShellIconLoadData *,struct IIconLoaderNotifications2 *,struct IIconLoadingFunctions *))"},
                                         &LoadWindowAndShellIconV2_WithArgs_Original,
                                         LoadWindowAndShellIconV2_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionLoadWindowIconOnlyV2_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for LoadWindowIconOnlyV2_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(long __cdecl LoadWindowIconOnlyV2(struct ITrayComponentHost *,struct IShellIconLoadData *,struct IIconLoaderNotifications2 *,struct IIconLoadingFunctions *))"},
                                         &LoadWindowIconOnlyV2_WithArgs_Original,
                                         LoadWindowIconOnlyV2_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_TryLoadAsyncIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: bool __cdecl ShellIconLoaderV2::TryLoadAsyncIcon(int))"},
                                         &ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs_Original,
                                         ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_final_release_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_final_release_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static struct winrt::fire_and_forget __cdecl ShellIconLoaderV2::final_release(class std::unique_ptr<class ShellIconLoaderV2,struct std::default_delete<class ShellIconLoaderV2> >))"},
                                         &ShellIconLoaderV2_final_release_WithArgs_Original,
                                         ShellIconLoaderV2_final_release_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static long __cdecl ShellIconLoaderV2::s_LoadIconsFromResourceSync(unsigned short const *,unsigned short,int,struct HICON__ * *))"},
                                         &ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs_Original,
                                         ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_s_SetIconCallback_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_s_SetIconCallback_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static void __cdecl ShellIconLoaderV2::s_SetIconCallback(void *,void *,int,int))"},
                                         &ShellIconLoaderV2_s_SetIconCallback_WithArgs_Original,
                                         ShellIconLoaderV2_s_SetIconCallback_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll for ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: static bool __cdecl ShellIconLoaderV2::s_ShouldLoadBetterIcon(struct _ICONINFOEXW const &,int))"},
                                         &ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs_Original,
                                         ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


BOOL Wh_ModInit() {
    bool anyHooked = false;
    if (!HookFunctionShellIconLoader_LoadShellIconCacheIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_LoadShellIconCacheIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader__CLoadIconTask_InternalResumeRT_WithoutArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_s_SetIconAsync_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_s_SetIconAsync_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_TriggerShellIconCacheBackgroundLoad_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_s_ForceImagePresent_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_s_ForceImagePresent_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_s_GetShellImageListSizeId_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_s_GetShellImageListSizeId_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_s_GetShellIconSize_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_s_GetShellIconSize_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_s_GetShellIconSize_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_s_GetShellIconSize_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_s_SendMessageCallback_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_s_SendMessageCallback_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_SendMessageCallbackW_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_SendMessageCallbackW_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_LoadAsyncIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_LoadAsyncIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_TryLoadIconFromResourceIfNeeded_WithoutArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_s_ShouldLoadBetterIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_s_ShouldLoadBetterIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderTaskScheduler_AddTask_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderTaskScheduler_AddTask_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_RuntimeClassInitialize_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_RuntimeClassInitialize_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_LoadSyncWindowIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_LoadSyncWindowIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionLoadWindowIconOnly_WithArgs()) {
        Wh_Log(L"Method hooking failed: LoadWindowIconOnly_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionLoadShellIconCacheIconOnly_WithArgs()) {
        Wh_Log(L"Method hooking failed: LoadShellIconCacheIconOnly_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_s_ForceImagePresent_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_s_ForceImagePresent_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionLoadWindowAndShellIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: LoadWindowAndShellIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_LoadSyncWindowIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_LoadSyncWindowIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_TryLoadIconFromResourceIfNeeded_WithoutArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_s_GetShellImageListSizeId_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionCWinRTAppIconLoader_s_LoadIconSync_WithArgs()) {
        Wh_Log(L"Method hooking failed: CWinRTAppIconLoader_s_LoadIconSync_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderTaskScheduler_AddTask_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderTaskScheduler_AddTask_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderTaskScheduler_CountTasks_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderTaskScheduler_CountTasks_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderTaskScheduler_QueryInterface_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderTaskScheduler_QueryInterface_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderTaskScheduler_RemoveTasks_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderTaskScheduler_RemoveTasks_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_RuntimeClassInitialize_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_RuntimeClassInitialize_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderTaskScheduler_Status_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderTaskScheduler_Status_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoader_s_LoadIconsFromResourceSync_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoader_s_LoadIconsFromResourceSync_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_ForceImagePresentAsync_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_ForceImagePresentAsync_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_InitializeLoadWindowIconOnlyV2_WithoutArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_InitializeShellIconCacheIconOnlyV2_WithoutArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_InitializeWindowAndShellIconV2_WithoutArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_LoadAsyncIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_LoadAsyncIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_LoadShellIconCacheIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionLoadShellIconCacheIconOnlyV2_WithArgs()) {
        Wh_Log(L"Method hooking failed: LoadShellIconCacheIconOnlyV2_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionLoadWindowAndShellIconV2_WithArgs()) {
        Wh_Log(L"Method hooking failed: LoadWindowAndShellIconV2_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionLoadWindowIconOnlyV2_WithArgs()) {
        Wh_Log(L"Method hooking failed: LoadWindowIconOnlyV2_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_TryLoadAsyncIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_TryLoadAsyncIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_final_release_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_final_release_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_s_LoadIconsFromResourceSync_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_s_SetIconCallback_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_s_SetIconCallback_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs()) {
        Wh_Log(L"Method hooking failed: ShellIconLoaderV2_s_ShouldLoadBetterIcon_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!anyHooked) {         Wh_Log(L"Nothing was hooked");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {}
void Wh_ModSettingsChanged() {}