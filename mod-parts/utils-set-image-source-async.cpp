//
// #include <winrt/Windows.Storage.Streams.h>
// #include <winrt/Windows.Storage.h>
// #include <winrt/Windows.UI.Xaml.Media.Imaging.h>
//
// using namespace winrt;
// using namespace winrt::Windows::UI::Xaml::Controls;
// using namespace winrt::Windows::UI::Xaml::Media::Imaging;
// using namespace winrt::Windows::Storage;
// using namespace winrt::Windows::Storage::Streams;
//
// #include <winrt/Windows.UI.Xaml.h>
// #include <winrt/Windows.UI.Xaml.Controls.h>
// #include <winrt/Windows.UI.Xaml.Media.Imaging.h>
// #include <winrt/Windows.Storage.h>
// #include <winrt/Windows.Storage.Streams.h>
// #include <winrt/Windows.Foundation.h>
// #include <winrt/Windows.UI.Core.h>
//
//
// winrt::Windows::Foundation::IAsyncAction SetImageAsync(FrameworkElement const& obj, std::wstring const& imagePath)
//{
//    if (auto imageControl = obj.try_as<Image>())
//    {
//        StorageFile file{ nullptr };
//        try
//        {
//            file = co_await StorageFile::GetFileFromPathAsync(imagePath);
//        }
//        catch (hresult_error const& ex)
//        {
//            Wh_Log(L"Failed to get file from path");
//            co_return;
//        }
//
//        IRandomAccessStream stream;
//        try
//        {
//            stream = co_await file.OpenAsync(FileAccessMode::Read);
//        }
//        catch (hresult_error const& ex)
//        {
//            Wh_Log(L"Failed to open file stream: HRESULT= %d" ,ex.code());
//            co_return;
//        }
//
//        // marshal back to UI thread
//        co_await resume_foreground(obj.Dispatcher());
//
//        try
//        {
//            BitmapImage bitmap{};
//            bitmap.SetSource(stream);
//            imageControl.Source(bitmap);
//            Wh_Log(L"Image successfully set from path");
//        }
//        catch (hresult_error const& ex)
//        {
//            Wh_Log(L"Failed to set BitmapImage source:");
//        }
//    }
//    else
//    {
//        Wh_Log(L"Can't cast object to Image");
//    }
//}