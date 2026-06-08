// ---- WindhawkBlur support ---------------------------------------------------
// Based on the WindhawkBlur/XamlBlurBrush approach used by Windows 11 Taskbar Styler.
// Yoinked from ramensoftware windows-11-taskbar-styler
// which is yoinked from https://github.com/TranslucentTB/TranslucentTB/blob/release/ExplorerTAP/XamlBlurBrush.cpp
#include <dwmapi.h>
#include <chrono>
#include <string>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <utility>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <string_view>
#include <vector>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <chrono>
#include <thread>
#include <windows.h>
#include <psapi.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <mutex>
#include <cmath>
#include <cwctype>
using namespace winrt::Windows::UI::Xaml;

#include <cmath>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <list>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace wf = winrt::Windows::Foundation;
namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {
typedef interface IGraphicsEffectSource IGraphicsEffectSource;
typedef interface IGraphicsEffectD2D1Interop IGraphicsEffectD2D1Interop;
typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING {
  GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
  GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
} GRAPHICS_EFFECT_PROPERTY_MAPPING;
#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown, "2FC57384-A068-44D7-A331-30982FCF7177") {
  STDMETHOD(GetEffectId)(_Out_ GUID* id) PURE;
  STDMETHOD(GetNamedPropertyMapping)(LPCWSTR name, _Out_ UINT* index, _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) PURE;
  STDMETHOD(GetPropertyCount)(_Out_ UINT* count) PURE;
  STDMETHOD(GetProperty)(UINT index, _Outptr_ winrt::impl::abi_t<wf::IPropertyValue>** value) PURE;
  STDMETHOD(GetSource)(UINT index, _Outptr_ IGraphicsEffectSource** source) PURE;
  STDMETHOD(GetSourceCount)(_Out_ UINT* count) PURE;
};
}  // namespace Effects
}  // namespace Graphics
}  // namespace Windows
#ifndef BUILD_WINDOWS
}  // namespace ABI
#endif

template <>
inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>{
    winrt::impl::guid_v<wge::IGraphicsEffectSource>};
template <>
inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

namespace awge = ABI::Windows::Graphics::Effects;

inline void CopyWindhawkBlurPropertyValueToAbi(wf::IPropertyValue const& propertyValue, winrt::impl::abi_t<wf::IPropertyValue>** value) {
  winrt::copy_to_abi(propertyValue, *reinterpret_cast<void**>(value));
}

typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION {
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff
} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;

struct CompositeEffect : winrt::implements<CompositeEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  std::vector<wge::IGraphicsEffectSource> Sources;
  D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
  winrt::hstring m_name = L"CompositeEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1Composite; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    if (std::wstring_view(name) == L"Mode") { *index = D2D1_COMPOSITE_PROP_MODE; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    if (index != D2D1_COMPOSITE_PROP_MODE) return E_BOUNDS;
    CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(static_cast<UINT32>(Mode)).as<wf::IPropertyValue>(), value);
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override try {
    if (!source) return E_INVALIDARG;
    winrt::copy_to_abi(Sources.at(index), *reinterpret_cast<void**>(source));
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = static_cast<UINT>(Sources.size()); return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};

struct FloodEffect : winrt::implements<FloodEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  winrt::Windows::UI::Color Color{};
  winrt::hstring m_name = L"FloodEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1Flood; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    if (std::wstring_view(name) == L"Color") { *index = D2D1_FLOOD_PROP_COLOR; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    if (index != D2D1_FLOOD_PROP_COLOR) return E_BOUNDS;
    float rgba[] = {Color.R / 255.0f, Color.G / 255.0f, Color.B / 255.0f, Color.A / 255.0f};
    CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateSingleArray(winrt::array_view<float const>(rgba, rgba + 4)).as<wf::IPropertyValue>(), value);
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT, awge::IGraphicsEffectSource** source) noexcept override { if (!source) return E_INVALIDARG; return E_BOUNDS; }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 0; return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};

struct GaussianBlurEffect : winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  wge::IGraphicsEffectSource Source{nullptr};
  float BlurAmount = 30.0f;
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION Optimization = MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
  D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;
  winrt::hstring m_name = L"GaussianBlurEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1GaussianBlur; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    const std::wstring_view n(name);
    if (n == L"BlurAmount") { *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"Optimization") { *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"BorderMode") { *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 3; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    switch (index) {
      case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateSingle(BlurAmount).as<wf::IPropertyValue>(), value); break;
      case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(static_cast<UINT32>(Optimization)).as<wf::IPropertyValue>(), value); break;
      case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(static_cast<UINT32>(BorderMode)).as<wf::IPropertyValue>(), value); break;
      default: return E_BOUNDS;
    }
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override {
    if (!source) return E_INVALIDARG;
    if (index == 0 && Source) { winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source)); return S_OK; }
    return E_BOUNDS;
  }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};

struct ColorMatrixEffect : winrt::implements<ColorMatrixEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  wge::IGraphicsEffectSource Source{nullptr};
  float Matrix[20] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0};
  uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
  bool ClampOutput = false;
  winrt::hstring m_name = L"ColorMatrixEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1ColorMatrix; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    const std::wstring_view n(name);
    if (n == L"ColorMatrix") { *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"AlphaMode") { *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"ClampOutput") { *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 3; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    switch (index) {
      case D2D1_COLORMATRIX_PROP_COLOR_MATRIX: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateSingleArray(winrt::array_view<float const>(Matrix, Matrix + 20)).as<wf::IPropertyValue>(), value); break;
      case D2D1_COLORMATRIX_PROP_ALPHA_MODE: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(AlphaMode).as<wf::IPropertyValue>(), value); break;
      case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateBoolean(ClampOutput).as<wf::IPropertyValue>(), value); break;
      default: return E_BOUNDS;
    }
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override {
    if (!source) return E_INVALIDARG;
    if (index == 0 && Source) { winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source)); return S_OK; }
    return E_BOUNDS;
  }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};


class XamlBlurBrush : public Media::XamlCompositionBrushBaseT<XamlBlurBrush> {
 public:
  XamlBlurBrush(UIElement element,
                float blurAmount,
                winrt::Windows::UI::Color tint,
                std::optional<uint8_t> tintOpacity,
                winrt::hstring tintThemeResourceKey,
                std::optional<float> tintLuminosityOpacity,
                std::optional<float> tintSaturation,
                std::optional<float> inversionAmount,
                std::optional<winrt::Windows::UI::Color> fallbackColor,
                winrt::hstring fallbackThemeResourceKey)
      : m_compositor(wuxh::ElementCompositionPreview::GetElementVisual(element).Compositor()),
        m_blurAmount(blurAmount),
        m_tint(tint),
        m_tintOpacity(tintOpacity),
        m_tintThemeResourceKey(std::move(tintThemeResourceKey)),
        m_tintLuminosityOpacity(tintLuminosityOpacity),
        m_tintSaturation(tintSaturation),
        m_inversionAmount(inversionAmount),
        m_fallbackColor(fallbackColor),
        m_fallbackThemeResourceKey(std::move(fallbackThemeResourceKey)) {
    auto fe = element.try_as<FrameworkElement>();
    auto createProxy = [&](winrt::hstring const& themeResourceKey) -> Media::SolidColorBrush {
      if (!fe || themeResourceKey.empty()) return nullptr;
      std::wstring xaml = LR"(<SolidColorBrush xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" Color="{ThemeResource )";
      xaml += std::wstring(themeResourceKey.c_str());
      xaml += LR"(}"/>)";
      try {
        return Markup::XamlReader::Load(winrt::hstring(xaml)).try_as<Media::SolidColorBrush>();
      } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create WindhawkBlur theme proxy brush %08X", ex.code());
        return nullptr;
      }
    };

    static std::atomic<unsigned int> s_proxyCounter{0};
    if (!m_tintThemeResourceKey.empty()) {
      if (auto proxyBrush = createProxy(m_tintThemeResourceKey)) {
        auto proxyKey = winrt::hstring(L"__WinDockWindhawkBlurTint_" + std::to_wstring(++s_proxyCounter));
        fe.Resources().Insert(winrt::box_value(proxyKey), proxyBrush);
        m_proxyBrush = proxyBrush;
        m_weakProxyElement = winrt::make_weak(fe);
        m_proxyKey = proxyKey;
        m_proxyBrushColorChangedToken = m_proxyBrush.RegisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            [weakThis = get_weak()](auto&&, auto&&) {
              if (g_unloading) return;
              if (auto self = weakThis.get()) self->RefreshBrush();
            });
      }
    }
    if (!m_fallbackThemeResourceKey.empty()) {
      if (auto proxyBrush = createProxy(m_fallbackThemeResourceKey)) {
        auto proxyKey = winrt::hstring(L"__WinDockWindhawkBlurFallback_" + std::to_wstring(++s_proxyCounter));
        fe.Resources().Insert(winrt::box_value(proxyKey), proxyBrush);
        m_fallbackProxyBrush = proxyBrush;
        if (!m_weakProxyElement.get()) m_weakProxyElement = winrt::make_weak(fe);
        m_fallbackProxyKey = proxyKey;
        m_fallbackProxyBrushColorChangedToken = m_fallbackProxyBrush.RegisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            [weakThis = get_weak()](auto&&, auto&&) {
              if (g_unloading) return;
              if (auto self = weakThis.get()) self->RefreshBrush();
            });
      }
    }

    if (m_fallbackColor || !m_fallbackThemeResourceKey.empty()) {
      m_dispatcher = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
      try {
        m_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
        auto dispatcher = m_dispatcher;
        m_advancedEffectsEnabledChangedToken = m_uiSettings.AdvancedEffectsEnabledChanged(
            [weakThis = get_weak(), dispatcher](auto&&, auto&&) {
              if (g_unloading || !dispatcher) return;
              dispatcher.TryEnqueue([weakThis] {
                if (g_unloading) return;
                if (auto self = weakThis.get()) self->RefreshBrush();
              });
            });
        m_energySaverStatusChangedToken =
            winrt::Windows::System::Power::PowerManager::EnergySaverStatusChanged(
                [weakThis = get_weak(), dispatcher](auto&&, auto&&) {
                  if (g_unloading || !dispatcher) return;
                  dispatcher.TryEnqueue([weakThis] {
                    if (g_unloading) return;
                    if (auto self = weakThis.get()) self->RefreshBrush();
                  });
                });
      } catch (...) {
        Wh_Log(L"Failed to register WindhawkBlur fallback listeners: %08X", winrt::to_hresult());
      }
      LONG regStatus = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Power", 0, KEY_NOTIFY, &m_powerKey);
      if (regStatus == ERROR_SUCCESS) {
        m_regNotifyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (m_regNotifyEvent) {
          regStatus = RegNotifyChangeKeyValue(m_powerKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, m_regNotifyEvent, TRUE);
          if (regStatus == ERROR_SUCCESS) {
            if (!RegisterWaitForSingleObject(&m_regWaitHandle, m_regNotifyEvent, OnEnergySaverRegistryChanged, this, INFINITE, WT_EXECUTEINWAITTHREAD)) {
              Wh_Log(L"RegisterWaitForSingleObject failed: %lu", GetLastError());
              m_regWaitHandle = nullptr;
            }
          }
        }
      }
    }
  }

  ~XamlBlurBrush() {
    if (m_regWaitHandle) {
      UnregisterWaitEx(m_regWaitHandle, INVALID_HANDLE_VALUE);
      m_regWaitHandle = nullptr;
    }
    if (m_regNotifyEvent) {
      CloseHandle(m_regNotifyEvent);
      m_regNotifyEvent = nullptr;
    }
    if (m_powerKey) {
      RegCloseKey(m_powerKey);
      m_powerKey = nullptr;
    }
    if (m_proxyBrush && m_proxyBrushColorChangedToken) {
      try {
        m_proxyBrush.UnregisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            m_proxyBrushColorChangedToken);
      } catch (...) {}
      m_proxyBrushColorChangedToken = 0;
    }
    if (m_fallbackProxyBrush && m_fallbackProxyBrushColorChangedToken) {
      try {
        m_fallbackProxyBrush.UnregisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            m_fallbackProxyBrushColorChangedToken);
      } catch (...) {}
      m_fallbackProxyBrushColorChangedToken = 0;
    }
    if (m_uiSettings && m_advancedEffectsEnabledChangedToken.value) {
      try { m_uiSettings.AdvancedEffectsEnabledChanged(m_advancedEffectsEnabledChangedToken); } catch (...) {}
      m_advancedEffectsEnabledChangedToken = {};
    }
    if (m_energySaverStatusChangedToken.value) {
      try { winrt::Windows::System::Power::PowerManager::EnergySaverStatusChanged(m_energySaverStatusChangedToken); } catch (...) {}
      m_energySaverStatusChangedToken = {};
    }
    if (auto element = m_weakProxyElement.get()) {
      try {
        if (!m_proxyKey.empty()) element.Resources().Remove(winrt::box_value(m_proxyKey));
        if (!m_fallbackProxyKey.empty()) element.Resources().Remove(winrt::box_value(m_fallbackProxyKey));
      } catch (...) {}
    }
    m_proxyBrush = nullptr;
    m_fallbackProxyBrush = nullptr;
    m_weakProxyElement = nullptr;
    m_dispatcher = nullptr;
  }


  void OnConnected() {
    if (!CompositionBrush()) {
      RefreshThemeTint();
      RefreshFallbackColor();
      CompositionBrush(ShouldUseFallback() ? CreateFallbackBrush() : CreateEffectBrush());
    }
  }

  void OnDisconnected() {
    if (const auto brush = CompositionBrush()) {
      brush.Close();
      CompositionBrush(nullptr);
    }
  }

 private:
  static void CALLBACK OnEnergySaverRegistryChanged(PVOID context, BOOLEAN) {
    auto* self = static_cast<XamlBlurBrush*>(context);
    if (g_unloading) return;
    if (self->m_powerKey && self->m_regNotifyEvent) {
      RegNotifyChangeKeyValue(self->m_powerKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, self->m_regNotifyEvent, TRUE);
    }
    if (self->m_dispatcher) {
      auto weakThis = self->get_weak();
      self->m_dispatcher.TryEnqueue([weakThis] {
        if (g_unloading) return;
        if (auto self = weakThis.get()) self->RefreshBrush();
      });
    }
  }

  void RefreshThemeTint() {
    if (!m_proxyBrush) return;
    m_tint = m_proxyBrush.Color();
    if (m_tintOpacity) m_tint.A = *m_tintOpacity;
  }

  void RefreshFallbackColor() {
    if (!m_fallbackProxyBrush) return;
    m_fallbackColor = m_fallbackProxyBrush.Color();
  }

  bool ShouldUseFallback() const {
    if (!m_fallbackColor && m_fallbackThemeResourceKey.empty()) return false;
    bool energySaverActive = false;
    HKEY key{};
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Power", 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
      DWORD value = 0, type = 0, size = sizeof(value);
      if (RegQueryValueExW(key, L"EnergySaverState", nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD) {
        energySaverActive = (value == 1);
      }
      RegCloseKey(key);
    }
    if (!energySaverActive) {
      SYSTEM_POWER_STATUS powerStatus{};
      if (GetSystemPowerStatus(&powerStatus) && powerStatus.SystemStatusFlag != 0) energySaverActive = true;
    }
    bool advancedEffectsOff = false;
    if (m_uiSettings) {
      try { advancedEffectsOff = !m_uiSettings.AdvancedEffectsEnabled(); } catch (...) {}
    }
    return energySaverActive || advancedEffectsOff;
  }

  void RefreshBrush() {
    if (const auto brush = CompositionBrush()) {
      brush.Close();
      CompositionBrush(nullptr);
      OnConnected();
    }
  }

  wuc::CompositionBrush CreateFallbackBrush() {
    return m_compositor.CreateColorBrush(m_fallbackColor.value_or(m_tint));
  }

  wuc::CompositionBrush CreateEffectBrush() {
    auto backdropBrush = m_compositor.CreateBackdropBrush();
    constexpr float kLumaR = 0.2126f;
    constexpr float kLumaG = 0.7152f;
    constexpr float kLumaB = 0.0722f;

    auto blurEffect = winrt::make_self<GaussianBlurEffect>();
    blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
    blurEffect->BlurAmount = m_blurAmount;
    blurEffect->Name(L"BlurEffect");
    wge::IGraphicsEffectSource topOfStack = *blurEffect;

    if (m_tintSaturation && *m_tintSaturation != 1.0f) {
      float s = std::max(*m_tintSaturation, 0.0f);
      float invS = 1.0f - s;
      auto satMatrix = winrt::make_self<ColorMatrixEffect>();
      satMatrix->Source = topOfStack;
      auto& m = satMatrix->Matrix;
      m[0] = invS * kLumaR + s; m[1] = invS * kLumaR;     m[2] = invS * kLumaR;     m[3] = 0.0f;
      m[4] = invS * kLumaG;     m[5] = invS * kLumaG + s; m[6] = invS * kLumaG;     m[7] = 0.0f;
      m[8] = invS * kLumaB;     m[9] = invS * kLumaB;     m[10] = invS * kLumaB + s; m[11] = 0.0f;
      m[12] = 0.0f;             m[13] = 0.0f;             m[14] = 0.0f;             m[15] = 1.0f;
      satMatrix->Name(L"SaturationEffect");
      topOfStack = *satMatrix;
    }

    if (m_tintLuminosityOpacity && *m_tintLuminosityOpacity > 0.0f) {
      float op = std::clamp(*m_tintLuminosityOpacity, 0.0f, 1.0f);
      float tintLum = (m_tint.R / 255.0f) * kLumaR + (m_tint.G / 255.0f) * kLumaG + (m_tint.B / 255.0f) * kLumaB;
      auto lumMatrix = winrt::make_self<ColorMatrixEffect>();
      lumMatrix->Source = topOfStack;
      auto& m = lumMatrix->Matrix;
      m[0] = 1.0f - (kLumaR * op); m[1] = -(kLumaR * op);       m[2] = -(kLumaR * op);       m[3] = 0.0f;
      m[4] = -(kLumaG * op);       m[5] = 1.0f - (kLumaG * op); m[6] = -(kLumaG * op);       m[7] = 0.0f;
      m[8] = -(kLumaB * op);       m[9] = -(kLumaB * op);       m[10] = 1.0f - (kLumaB * op); m[11] = 0.0f;
      m[12] = 0.0f;                m[13] = 0.0f;                m[14] = 0.0f;                m[15] = 1.0f;
      m[16] = tintLum * op;        m[17] = tintLum * op;        m[18] = tintLum * op;        m[19] = 0.0f;
      lumMatrix->Name(L"LuminosityBlend");
      topOfStack = *lumMatrix;
    }

    if (m_inversionAmount && *m_inversionAmount > 0.0f) {
      float inv = std::clamp(*m_inversionAmount, 0.0f, 1.0f);
      float scale = 1.0f - (2.0f * inv);
      auto inversionMatrix = winrt::make_self<ColorMatrixEffect>();
      inversionMatrix->Source = topOfStack;
      auto& m = inversionMatrix->Matrix;
      m[0] = scale; m[1] = 0.0f;  m[2] = 0.0f;  m[3] = 0.0f;
      m[4] = 0.0f;  m[5] = scale; m[6] = 0.0f;  m[7] = 0.0f;
      m[8] = 0.0f;  m[9] = 0.0f;  m[10] = scale; m[11] = 0.0f;
      m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
      m[16] = inv;  m[17] = inv;  m[18] = inv;  m[19] = 0.0f;
      inversionMatrix->ClampOutput = true;
      inversionMatrix->Name(L"InversionEffect");
      topOfStack = *inversionMatrix;
    }

    auto floodEffect = winrt::make_self<FloodEffect>();
    floodEffect->Color = m_tint;
    floodEffect->Name(L"FloodEffect");

    auto compositeEffect = winrt::make_self<CompositeEffect>();
    compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
    compositeEffect->Sources.push_back(topOfStack);
    compositeEffect->Sources.push_back(*floodEffect);

    auto factory = m_compositor.CreateEffectFactory(*compositeEffect);
    auto brush = factory.CreateBrush();
    brush.SetSourceParameter(L"backdrop", backdropBrush);
    return brush;
  }

  wuc::Compositor m_compositor{nullptr};
  float m_blurAmount = 30.0f;
  winrt::Windows::UI::Color m_tint{};
  std::optional<uint8_t> m_tintOpacity;
  winrt::hstring m_tintThemeResourceKey;
  std::optional<float> m_tintLuminosityOpacity;
  std::optional<float> m_tintSaturation;
  std::optional<float> m_inversionAmount;
  std::optional<winrt::Windows::UI::Color> m_fallbackColor;
  winrt::hstring m_fallbackThemeResourceKey;
  Media::SolidColorBrush m_proxyBrush{nullptr};
  Media::SolidColorBrush m_fallbackProxyBrush{nullptr};
  winrt::weak_ref<FrameworkElement> m_weakProxyElement;
  winrt::hstring m_proxyKey;
  winrt::hstring m_fallbackProxyKey;
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{nullptr};
  int64_t m_proxyBrushColorChangedToken{};
  int64_t m_fallbackProxyBrushColorChangedToken{};
  winrt::event_token m_advancedEffectsEnabledChangedToken{};
  winrt::event_token m_energySaverStatusChangedToken{};
  winrt::Windows::System::DispatcherQueue m_dispatcher{nullptr};
  HKEY m_powerKey{nullptr};
  HANDLE m_regNotifyEvent{nullptr};
  HANDLE m_regWaitHandle{nullptr};
};

std::wstring TrimWide(std::wstring value) {
  const auto start = value.find_first_not_of(L" \t\r\n");
  if (start == std::wstring::npos) return L"";
  const auto end = value.find_last_not_of(L" \t\r\n");
  return value.substr(start, end - start + 1);
}

struct ParsedWindhawkColorSetting {
  winrt::Windows::UI::Color color;
  std::wstring themeResourceKey;
};

ParsedWindhawkColorSetting ParseWindhawkColorSetting(std::wstring value, winrt::Windows::UI::Color fallbackColor) {
  value = TrimWide(std::move(value));
  constexpr std::wstring_view prefix = L"{ThemeResource ";
  if (value.size() > prefix.size() + 1 && value.rfind(prefix, 0) == 0 && value.back() == L'}') {
    auto key = TrimWide(value.substr(prefix.size(), value.size() - prefix.size() - 1));
    if (!key.empty()) return {fallbackColor, key};
  }

  if (!value.empty() && value[0] == L'#') value.erase(value.begin());
  if (value.size() == 6 || value.size() == 8) {
    try {
      const unsigned long parsed = std::stoul(value, nullptr, 16);
      if (value.size() == 8) {
        return {winrt::Windows::UI::Color{static_cast<uint8_t>((parsed >> 24) & 0xFF), static_cast<uint8_t>((parsed >> 16) & 0xFF), static_cast<uint8_t>((parsed >> 8) & 0xFF), static_cast<uint8_t>(parsed & 0xFF)}, L""};
      }
      return {winrt::Windows::UI::Color{255, static_cast<uint8_t>((parsed >> 16) & 0xFF), static_cast<uint8_t>((parsed >> 8) & 0xFF), static_cast<uint8_t>(parsed & 0xFF)}, L""};
    } catch (...) {
    }
  }

  return {fallbackColor, L""};
}
void ClearWindhawkBlurFromBackgroundFill(FrameworkElement const& backgroundFillChild) {
  if (!backgroundFillChild) return;

  auto rectangle = backgroundFillChild.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
  if (!rectangle) {
    return;
  }
  try {
    // Do not leave a mod-implemented XamlCompositionBrushBase attached to
    // Explorer's XAML tree while Windhawk unloads/reloads this DLL.
    // Otherwise Explorer can later call into the old brush object's vtable after
    // the module that implemented it has been unloaded.
    auto oldFill = rectangle.Fill();
    rectangle.Fill(Media::Brush{nullptr});
    rectangle.ClearValue(winrt::Windows::UI::Xaml::Shapes::Shape::FillProperty());
    rectangle.Opacity(1.0);
    oldFill = nullptr;
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"WindhawkBlur cleanup failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (...) {
    Wh_Log(L"WindhawkBlur cleanup failed: %08X", winrt::to_hresult());
  }
}

void ApplyWindhawkBlurToBackgroundFill(FrameworkElement const& backgroundFillChild) {
  if (!backgroundFillChild) return;
  auto rectangle = backgroundFillChild.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
  if (!rectangle) {
    Wh_Log(L"WindhawkBlur: BackgroundFill is not a Rectangle");
    return;
  }
  if (g_unloading) {
    ClearWindhawkBlurFromBackgroundFill(backgroundFillChild);
    return;
  }
  const float backgroundOpacity = std::clamp(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f, 0.0f, 1.0f);
  rectangle.Opacity(backgroundOpacity);

  auto tintSetting = ParseWindhawkColorSetting(
      g_settings.userDefinedTaskbarBackgroundTintColor,
      winrt::Windows::UI::Color{0, 255, 255, 255});
  auto fallbackSetting = ParseWindhawkColorSetting(
      g_settings.userDefinedTaskbarBackgroundFallbackColor,
      winrt::Windows::UI::Color{255, 32, 32, 32});

  const float tintOpacity = std::clamp(g_settings.userDefinedTaskbarBackgroundTint / 100.0f, 0.0f, 1.0f);
  const auto tintAlpha = static_cast<uint8_t>(std::round(tintOpacity * 255.0f));
  tintSetting.color.A = tintAlpha;

  const float luminosityOpacity = std::clamp(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f, 0.0f, 1.0f);
  const float saturation = std::clamp(g_settings.userDefinedTaskbarBackgroundTintSaturation / 100.0f, 0.0f, 5.0f);
  const float inversion = std::clamp(g_settings.userDefinedTaskbarBackgroundInversion / 100.0f, 0.0f, 1.0f);

  try {
    auto oldFill = rectangle.Fill();
    rectangle.Fill(Media::Brush{nullptr});
    oldFill = nullptr;

    auto blurBrush = winrt::make<XamlBlurBrush>(
        rectangle,
        static_cast<float>(g_settings.userDefinedTaskbarBackgroundBlurAmount),
        tintSetting.color,
        std::optional<uint8_t>(tintAlpha),
        winrt::hstring(tintSetting.themeResourceKey),
        std::optional<float>(luminosityOpacity),
        std::optional<float>(saturation),
        inversion > 0.0f ? std::optional<float>(inversion) : std::nullopt,
        fallbackSetting.themeResourceKey.empty() ? std::optional<winrt::Windows::UI::Color>(fallbackSetting.color) : std::nullopt,
        winrt::hstring(fallbackSetting.themeResourceKey));
    rectangle.Fill(blurBrush);
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"WindhawkBlur failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (...) {
    Wh_Log(L"WindhawkBlur failed: %08X", winrt::to_hresult());
  }
}
