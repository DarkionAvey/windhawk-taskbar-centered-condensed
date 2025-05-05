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
