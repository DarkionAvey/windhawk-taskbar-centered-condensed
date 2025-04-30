using TaskbarTelemetry_StartItemEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemEntranceAnimation_t orig_StartItemEntranceAnimation = nullptr;

using TaskbarTelemetry_StartItemPlateEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemPlateEntranceAnimation_t orig_StartItemPlateEntranceAnimation = nullptr;

void WINAPI Hook_StartItemEntranceAnimation_call(const bool& b) {
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemEntranceAnimation(%d)", b);
  orig_StartItemEntranceAnimation(b);
  ApplySettingsDebounced();
}

void WINAPI Hook_StartItemPlateEntranceAnimation_call(const bool& b) {
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemPlateEntranceAnimation(%d)", b);
  orig_StartItemPlateEntranceAnimation(b);
  ApplySettingsDebounced();
}
