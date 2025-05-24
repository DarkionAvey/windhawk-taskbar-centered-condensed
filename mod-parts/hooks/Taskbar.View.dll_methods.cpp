using TaskbarTelemetry_StartItemEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemEntranceAnimation_t orig_StartItemEntranceAnimation = nullptr;

using TaskbarTelemetry_StartItemPlateEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemPlateEntranceAnimation_t orig_StartItemPlateEntranceAnimation = nullptr;

void WINAPI Hook_StartItemEntranceAnimation_call(const bool& b) {
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemEntranceAnimation(%d)", b);
  orig_StartItemEntranceAnimation(b);
  ApplySettingsDebounced(50);
}

void WINAPI Hook_StartItemPlateEntranceAnimation_call(const bool& b) {
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemPlateEntranceAnimation(%d)", b);
  orig_StartItemPlateEntranceAnimation(b);
  ApplySettingsDebounced(50);
}

using TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_t = void(WINAPI*)(void* pThis);
TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_t TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original;
static void WINAPI TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Hook(void* pThis) {
  Wh_Log(L"Method called: TaskbarTelemetry_StartEntranceAnimationCompleted");
  TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original(pThis);
  ApplySettingsDebounced(300);
  return;
}

using TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_t = void(WINAPI*)(void* pThis);
TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_t TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original;
static void WINAPI TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Hook(void* pThis) {
  TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original(pThis);
  Wh_Log(L"Method called: TaskbarTelemetry_StartHideAnimationCompleted");
  ApplySettingsDebounced(300);
  return;
}

using winrt__impl__produce_get_IsMultiWindow1_WithArgs_t = int(WINAPI*)(void* pThis, bool* param1);
winrt__impl__produce_get_IsMultiWindow1_WithArgs_t winrt__impl__produce_get_IsMultiWindow1_WithArgs_Original;
int WINAPI winrt__impl__produce_get_IsMultiWindow1_WithArgs_Hook(void* pThis, bool* param1) {
  Wh_Log(L"Method called: winrt__impl__produce_get_IsMultiWindow1, param1 = %d", *param1);
  int result = winrt__impl__produce_get_IsMultiWindow1_WithArgs_Original(pThis, param1);
  *param1 = false;
  return result;
}

using winrt__impl__produce_get_IsMultiWindow2_WithArgs_t = int(WINAPI*)(void* pThis, bool* param1);
winrt__impl__produce_get_IsMultiWindow2_WithArgs_t winrt__impl__produce_get_IsMultiWindow2_WithArgs_Original;
int WINAPI winrt__impl__produce_get_IsMultiWindow2_WithArgs_Hook(void* pThis, bool* param1) {
  Wh_Log(L"Method called: winrt__impl__produce_get_IsMultiWindow2, param1 = %d", *param1);
  int result = winrt__impl__produce_get_IsMultiWindow2_WithArgs_Original(pThis, param1);
  *param1 = false;
  return result;
}

using winrt__impl__produce_get_IsMultiWindow3_WithArgs_t = int(WINAPI*)(void* pThis, bool* param1);
winrt__impl__produce_get_IsMultiWindow3_WithArgs_t winrt__impl__produce_get_IsMultiWindow3_WithArgs_Original;
int WINAPI winrt__impl__produce_get_IsMultiWindow3_WithArgs_Hook(void* pThis, bool* param1) {
  Wh_Log(L"Method called: winrt__impl__produce_get_IsMultiWindow3, param1 = %d", *param1);
  int result = winrt__impl__produce_get_IsMultiWindow3_WithArgs_Original(pThis, param1);
  *param1 = false;
  return result;
}
