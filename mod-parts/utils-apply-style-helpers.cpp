bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return false;
  auto transform = element.TransformToVisual(nullptr);
  winrt::Windows::Foundation::Rect rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
  return rect.X < 0 || rect.Y < 0;
}

void UpdateIcons(FrameworkElement& obj, winrt::hstring& currentIconAppNameStr) {
  //
  // auto taskbarMap = g_icon_scanner.getTaskbarAppsIconsMap();
  // auto trayMap = g_icon_scanner.getTrayAppsIconsMap();
  //
  //// helper to strip extension from running app name
  // auto stripExt = [&](std::wstring s) {
  //   fs::path p{s};
  //   return p.stem().wstring();
  // };
  //
  // for (auto const& [key, imgPath] : taskbarMap) {
  //   std::wstring iconKey = stripExt(key);
  //   Wh_Log(L"UpdateIcons: iconKey %s currentIconAppNameStr: %s", iconKey.c_str(), currentIconAppNameStr.c_str());
  //
  //// && IsImageTagOutdatedOrNull(obj, imgPath.c_str())
  //  if (ContainsCaseInsensitive(currentIconAppNameStr.c_str(), iconKey)) {
  //    obj.Tag(winrt::box_value(imgPath));
  //    SetImageAsync(obj, imgPath);
  //  }
  //}
  //   for (auto const& [key, imgPath] : trayMap) {
  //     std::wstring appNameStr = /* get your app name string here */;
  //     std::wstring baseName = stripExt(appNameStr);

  //     if (baseName == key && IsImageTagOutdatedOrNull(iconElementChild, imgPath)) {
  //       iconElementChild.Tag(winrt::box_value(tag));
  //       Wh_Log(L"Tray: %s", baseName.c_str());
  //       SetImageAsync(iconElementChild, imgPath);
  //     }
  //   }
}
