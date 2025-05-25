bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return false;
  auto transform = element.TransformToVisual(nullptr);
  winrt::Windows::Foundation::Rect rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
  return rect.X < 0 || rect.Y < 0;
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