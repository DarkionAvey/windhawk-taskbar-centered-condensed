// #include <winrt/Windows.Foundation.h>
// #include <winrt/Windows.UI.Xaml.Media.Imaging.h>
//
// #include <algorithm>
// #include <condition_variable>
// #include <filesystem>
// #include <map>
// #include <mutex>
// #include <shared_mutex>
// #include <string>
// #include <thread>
//
// namespace fs = std::filesystem;
//
// class IconScanner {
//  public:
//   using AppIconMap = std::map<std::wstring, std::wstring>;
//
//   IconScanner() : m_stopFlag(false), m_newSettings(false) {
//     m_worker = std::thread([this] { this->workerThread(); });
//   }
//
//   ~IconScanner() {
//     {
//       std::unique_lock lk(m_mutex);
//       m_stopFlag = true;
//       m_cv.notify_all();
//     }
//     if (m_worker.joinable()) m_worker.join();
//   }
//
//   void onSettingsChanged(std::wstring const& taskbarDir, std::wstring const& trayDir) {
//     {
//       std::unique_lock lk(m_mutex);
//       m_taskbarDir = taskbarDir;
//       m_trayDir = trayDir;
//       m_newSettings = true;
//     }
//     m_cv.notify_one();
//   }
//
//   AppIconMap getTaskbarAppsIconsMap() {
//     std::shared_lock lk(m_mapMutex);
//     return m_taskbarMap;
//   }
//
//   AppIconMap getTrayAppsIconsMap() {
//     std::shared_lock lk(m_mapMutex);
//     return m_trayMap;
//   }
//
//  private:
//   void workerThread() {
//     std::unique_lock lk(m_mutex);
//     while (!m_stopFlag) {
//       m_cv.wait(lk, [&] { return m_stopFlag || m_newSettings; });
//       if (m_stopFlag) break;
//
//       auto taskbarDir = m_taskbarDir;
//       auto trayDir = m_trayDir;
//       m_newSettings = false;
//       lk.unlock();
//
//       AppIconMap newTaskbar, newTray;
//
//       if (validateDirectory(taskbarDir)) {
//         for (auto const& e : fs::directory_iterator(taskbarDir)) {
//           if (!e.is_regular_file() || !IsEqual(e.path().extension().wstring(), L".png")) continue;
//           // key = stem (filename without extension)
//           newTaskbar.emplace(e.path().stem().wstring(), e.path().wstring());
//         }
//       }
//       if (validateDirectory(trayDir)) {
//         for (auto const& e : fs::directory_iterator(trayDir)) {
//           if (!e.is_regular_file() || !IsEqual(e.path().extension().wstring(), L".png")) continue;
//           newTray.emplace(e.path().stem().wstring(), e.path().wstring());
//         }
//       }
//
//       {
//         std::unique_lock mapLk(m_mapMutex);
//         m_taskbarMap = std::move(newTaskbar);
//         m_trayMap = std::move(newTray);
//       }
//
//       lk.lock();
//     }
//   }
//
//   bool validateDirectory(std::wstring const& d) {
//     std::error_code ec;
//     if (d.empty()) return false;
//     fs::path p{d};
//     return fs::exists(p, ec) && fs::is_directory(p, ec);
//   }
//
//   static bool IsEqual(std::wstring a, std::wstring b) {
//     std::transform(a.begin(), a.end(), a.begin(), towlower);
//     std::transform(b.begin(), b.end(), b.begin(), towlower);
//     return a == b;
//   }
//
//   std::thread m_worker;
//   std::mutex m_mutex;
//   std::condition_variable m_cv;
//   bool m_stopFlag;
//   bool m_newSettings;
//   std::wstring m_taskbarDir;
//   std::wstring m_trayDir;
//
//   std::shared_mutex m_mapMutex;
//   AppIconMap m_taskbarMap;
//   AppIconMap m_trayMap;
// };
//
// bool IsImageTagOutdatedOrNull(FrameworkElement const& element, wchar_t const* imagePath)
//{
//     if (auto tagObj = element.Tag()) {
//         hstring tag = winrt::unbox_value<hstring>(tagObj);
//         if (std::wcscmp(tag.c_str(), imagePath) != 0) {
//             return true;  // Outdated tag
//         }
//     }
//     else {
//         return true;  // Tag is null
//     }
//
//     return false;  // Tag is valid and up-to-date
// }
//
bool ContainsCaseInsensitive(std::wstring const& haystack, std::wstring const& needle) {
  std::wstring h = haystack, n = needle;
  std::transform(h.begin(), h.end(), h.begin(), towlower);
  std::transform(n.begin(), n.end(), n.begin(), towlower);
  return h.find(n) != std::wstring::npos;
}
//
// IconScanner g_icon_scanner;
