//- CustomTaskbarIconsDir: ""
//  $name: (Experimental, not active) Folder to custom taskbar icons
//  $description: Specify a valid path to a folder containing .png files of app names for taskbar
//  $type: string dir path
//
//- CustomTrayIconsDir: ""
//  $name: (Experimental, not active) Folder to custom tray icons
//  $description: Specify a valid path to a folder containing .png files of app names for tray
//  $type: string dir path


static winrt::Windows::Storage::Search::StorageFileQueryResult g_taskbarQuery{
    nullptr};
static winrt::Windows::Storage::Search::StorageFileQueryResult g_trayQuery{
    nullptr};
static winrt::event_token g_taskbarToken{};
static winrt::event_token g_trayToken{};


// strip extension from wide-string buffer
static std::wstring StripExtension(const wchar_t* name) {
  std::wstring_view view(name);
  auto pos = view.rfind(L'.');
  return (pos == std::wstring_view::npos) ? std::wstring(view)
                                          : std::wstring(view.substr(0, pos));
}

static void UpdateList(
    winrt::Windows::Storage::Search::StorageFileQueryResult const& query,
    std::set<std::pair<std::wstring, std::wstring>>& container,
    const wchar_t* logContext) {
  g_invalidateCustomIcons = true;
  try {
    auto files = query.GetFilesAsync().get();
    container.clear();

    for (auto const& file : files) {
      if (!file) continue;
      auto ext = file.FileType().c_str();
      if (_wcsicmp(ext, L".png") != 0) continue;

      std::wstring fname = StripExtension(file.Name().c_str());
      std::wstring full = file.Path().c_str();

      Wh_Log(L"adding new file to list:: name: %s, path %s ", fname.c_str(),
             full.c_str());
      container.emplace(std::move(fname), std::move(full));
    }

    if (container.empty()) {
      Wh_Log(L"no png files found");
    }
  } catch (...) {
    Wh_Log(L"failed to update list");
  }
}

// Handlers must match TypedEventHandler<IStorageQueryResultBase, IInspectable>
static void OnTaskbarChanged(
    winrt::Windows::Storage::Search::IStorageQueryResultBase const& sender,
    winrt::Windows::Foundation::IInspectable const&) {
  auto query =
      sender.as<winrt::Windows::Storage::Search::StorageFileQueryResult>();
  UpdateList(query, g_taskbarIcons, L"taskbar");
}

static void OnTrayChanged(
    winrt::Windows::Storage::Search::IStorageQueryResultBase const& sender,
    winrt::Windows::Foundation::IInspectable const&) {
  auto query =
      sender.as<winrt::Windows::Storage::Search::StorageFileQueryResult>();
  UpdateList(query, g_trayIcons, L"tray");
}

/// Call this once at startup
void InitializeWatchers(const std::wstring& taskbarDir,
                        const std::wstring& trayDir) {
  if (taskbarDir.empty() && trayDir.empty()) {
    return;
  }

  try {
    winrt::Windows::Storage::Search::QueryOptions options{};
    options.FolderDepth(winrt::Windows::Storage::Search::FolderDepth::Deep);
    options.FileTypeFilter().Clear();
    options.FileTypeFilter().Append(L".png");
    if (!taskbarDir.empty()) {
      auto taskbarFolder =
          winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(
              taskbarDir)
              .get();
      if (!taskbarFolder) {
        Wh_Log(L"taskbar folder not found");
      } else {
        g_taskbarQuery = taskbarFolder.CreateFileQueryWithOptions(options);
        UpdateList(g_taskbarQuery, g_taskbarIcons, L"taskbar");
        g_taskbarToken = g_taskbarQuery.ContentsChanged({&OnTaskbarChanged});
      }
    }
    if (!trayDir.empty()) {
      auto trayFolder =
          winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(
              trayDir)
              .get();
      if (!trayFolder) {
        Wh_Log(L"tray folder not found");
      } else {
        g_trayQuery = trayFolder.CreateFileQueryWithOptions(options);
        UpdateList(g_trayQuery, g_trayIcons, L"tray");
        g_trayToken = g_trayQuery.ContentsChanged({&OnTrayChanged});
      }
    }
  } catch (...) {
    Wh_Log(L"load already in progress");
  }
}

/// Call this once at shutdown
void CleanupWatchers() {
  try {
    if (g_taskbarQuery) {
      g_taskbarQuery.ContentsChanged(g_taskbarToken);
      g_taskbarQuery = nullptr;
    }
    if (g_trayQuery) {
      g_trayQuery.ContentsChanged(g_trayToken);
      g_trayQuery = nullptr;
    }
    g_taskbarIcons.clear();
    g_trayIcons.clear();
  } catch (...) {
    Wh_Log(L"cleanup error");
  }
}

winrt::fire_and_forget SetImageFromFileAsync(
    winrt::Windows::UI::Xaml::Controls::Image const& imageControlRef,
    std::wstring filePath) {
    if(!imageControlRef)co_return;
    winrt::hstring hFilePath{filePath};

if(!g_invalidateCustomIcons){
     auto currentTag = imageControlRef.Tag();
  if (currentTag)
  {
 if (auto pathRef = currentTag.try_as<winrt::Windows::Foundation::IReference<winrt::hstring>>())
    {
      if (pathRef.Value() ==hFilePath)
      {
        Wh_Log(L"same image already set; skipping %s ",filePath.c_str());
        co_return;
      }
    }
  }
}

  try {
    auto weakImage = winrt::make_weak(imageControlRef);

    auto dispatcher = imageControlRef.Dispatcher();
    if (!dispatcher) {
      Wh_Log(L"dispatcher is null; abort");
      co_return;
    }

    co_await winrt::resume_background();

    winrt::Windows::Storage::StorageFile file{nullptr};
    try {
      file =
          co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(
              hFilePath);
    } catch (winrt::hresult_error const& ex) {
      Wh_Log(L"GetFileFromPathAsync failed: %s (0x%08X)", ex.message().c_str(),
             ex.code());
      co_return;
    }

    winrt::Windows::Storage::Streams::IRandomAccessStream stream{nullptr};
    try {
      stream = co_await file.OpenAsync(
          winrt::Windows::Storage::FileAccessMode::Read);
    } catch (...) {
      Wh_Log(L"OpenAsync failed");
      co_return;
    }

    const uint32_t target =
        static_cast<uint32_t>(g_settings.userDefinedTaskbarButtonSize * 1.5);

    uint32_t originalWidth = 0;
    bool doDecode = false;
    try {
      auto decoder = co_await winrt::Windows::Graphics::Imaging::BitmapDecoder::
          CreateAsync(stream);
      originalWidth = decoder.PixelWidth();
      if (originalWidth > target) {
        doDecode = true;
      }
    } catch (...) {
      Wh_Log(
          L"BitmapDecoder metadata read failed; will skip down‑sample check");
    }

    try {
      stream.Seek(0);
    } catch (winrt::hresult_error const& ex) {
      Wh_Log(L"Stream.Seek(0) failed: %s (0x%08X)", ex.message().c_str(),
             ex.code());
    }

    co_await winrt::resume_foreground(dispatcher);

    auto imageControl = weakImage.get();
    if (!imageControl) {
      Wh_Log(L"imageControl was destroyed before UI resume; abort");
      co_return;
    }


    winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage bitmapImage;
    if (doDecode) {
      bitmapImage.DecodePixelWidth(target);
    }

    try {
      co_await bitmapImage.SetSourceAsync(stream);
    } catch (...) {
      Wh_Log(L"[SetSourceAsync] failed");
      co_return;
    }

    Wh_Log(L"setting Image.Source");
    try {
      imageControl.Source(bitmapImage);
            imageControl.Tag(winrt::box_value(hFilePath));

    } catch (...) {
      Wh_Log(L"Image.Source threw");
    }
  } catch (...) {
    Wh_Log(L"Unhandled exception in SetImageFromFileAsync");
  }
}
