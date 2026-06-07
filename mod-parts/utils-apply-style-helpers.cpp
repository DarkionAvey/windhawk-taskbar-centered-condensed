bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return true;
  try {
    auto transform = element.TransformToVisual(nullptr);
    winrt::Windows::Foundation::Rect rect = transform.TransformBounds(
        winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
    return rect.Width <= 0.0 || rect.Height <= 0.0 || rect.X < -kLayoutToleranceDip || rect.Y < -kLayoutToleranceDip;
  } catch (...) {
    // Overflow/recycled taskbar elements can briefly be disconnected from the
    // visual tree while Explorer is rebuilding the task list. Treat them as
    // invalid instead of letting a transient XAML exception take down Explorer.
    return true;
  }
}
bool IsTaskbarWidgetsEnabled() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"TaskbarDa", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 1;
        }
        RegCloseKey(hKey);
    }
    return false;
}
static float SnapToPhysicalPixel(float value, float rasterizationScale = 1.0f) {
  if (rasterizationScale <= 0.0f) {
    rasterizationScale = 1.0f;
  }

  float scaledValue = value * rasterizationScale;
  float snappedScaledValue =
      (scaledValue >= 0.0f)
          ? static_cast<float>(static_cast<long>(scaledValue + 0.5f))
          : -static_cast<float>(static_cast<long>(-scaledValue + 0.5f));

  return snappedScaledValue / rasterizationScale;
}

static float GetRasterizationScale(FrameworkElement const& element) {
  if (element) {
    auto xamlRoot = element.XamlRoot();
    if (xamlRoot) {
      float rasterizationScale = static_cast<float>(xamlRoot.RasterizationScale());
      if (rasterizationScale > 0.0f) {
        return rasterizationScale;
      }
    }
  }

  return 1.0f;
}
