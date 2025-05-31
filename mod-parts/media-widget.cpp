// Include necessary WinRT headers
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Playback.h>
#include <winrt/Windows.Media.h>

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::Windows::UI::Xaml::Shapes;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Playback;
using namespace winrt::Windows::UI;

// Creates a 300×80 media control panel with system media transport controls
FrameworkElement CreateMediaControlUI(MediaPlayer const& mediaPlayer)
{
    // Configure SystemMediaTransportControls
    auto smtc = mediaPlayer.SystemMediaTransportControls();
    smtc.IsEnabled(true);
    smtc.IsPlayEnabled(true);
    smtc.IsPauseEnabled(true);
    smtc.IsNextEnabled(true);
    smtc.IsPreviousEnabled(true);

    // Update SMTC display metadata (album art, title, etc.)
    auto updater = smtc.DisplayUpdater();
    updater.Type(MediaPlaybackType::Music);
    updater.Update();

    // Root grid container
    Grid grid;
    grid.Width(300);
    grid.Height(80);
    grid.Background(SolidColorBrush{ ColorHelper::FromArgb(0xFF, 0x1E, 0x1E, 0x1E) });

    // Album art placeholder
    Image albumArt;
    albumArt.Width(80);
    albumArt.Height(80);
    albumArt.Stretch(Stretch::UniformToFill);
    // TODO: Load updater.Thumbnail() stream into BitmapImage via SetSourceAsync

    // Controls panel on the right
    StackPanel controlsPanel;
    controlsPanel.Orientation(Orientation::Horizontal);
    controlsPanel.HorizontalAlignment(HorizontalAlignment::Right);
    controlsPanel.VerticalAlignment(VerticalAlignment::Center);
    controlsPanel.Margin(Thickness{0, 0, 10, 0});

    // Previous button
    Button prevButton;
    prevButton.Content(box_value(L"⏮"));
    // prevButton.Click(RoutedEventHandler([&](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
    //     smtc.PlaybackStatus(MediaPlaybackStatus::Changing);
    //     mediaPlayer.PlaybackSession().Position(std::chrono::seconds{0});
    //     smtc.PlaybackStatus(MediaPlaybackStatus::Playing);
    // }));
    controlsPanel.Children().Append(prevButton);

    // Play/Pause button
    Button playPauseButton;
    playPauseButton.Content(box_value(L"⏯"));
    // playPauseButton.Click(RoutedEventHandler([&](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
    //     auto session = mediaPlayer.PlaybackSession();
    //     if (session.PlaybackState() == MediaPlaybackState::Playing)
    //     {
    //         mediaPlayer.Pause();
    //         smtc.PlaybackStatus(MediaPlaybackStatus::Paused);
    //     }
    //     else
    //     {
    //         mediaPlayer.Play();
    //         smtc.PlaybackStatus(MediaPlaybackStatus::Playing);
    //     }
    // }));
    controlsPanel.Children().Append(playPauseButton);

    // Next button
    Button nextButton;
    nextButton.Content(box_value(L"⏭"));
    // nextButton.Click(RoutedEventHandler([&](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
    //     smtc.PlaybackStatus(MediaPlaybackStatus::Changing);
    //     smtc.PlayNext();
    // }));
    controlsPanel.Children().Append(nextButton);

    // Define two columns: fixed (80px) and star
    auto cols = grid.ColumnDefinitions();
    cols.Append(ColumnDefinition{});
    cols.GetAt(0).Width(GridLengthHelper::FromValueAndType(80, GridUnitType::Pixel));
    cols.Append(ColumnDefinition{});
    cols.GetAt(1).Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));

    // Place album art and controls in grid
    Grid::SetColumn(albumArt, 0);
    grid.Children().Append(albumArt);
    Grid::SetColumn(controlsPanel, 1);
    grid.Children().Append(controlsPanel);

    return grid;
}

// Usage example:
// MediaPlayer player{};
// player.Source(MediaSource::CreateFromUri(Uri(L"ms-appx:///Assets/audio.mp3")));
// auto panel = CreateMediaControlUI(player);
// this->Content(panel);


#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt;
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;

IAsyncAction MonitorSystemMedia()
{
    auto manager = co_await GlobalSystemMediaTransportControlsSessionManager::RequestAsync();

    manager.CurrentSessionChanged([=](auto const& sender, auto const&) {
        auto session = sender.GetCurrentSession();
        if (session)
        {
    Wh_Log(L"MonitorSystemMedia CurrentSessionChanged");

            session.MediaPropertiesChanged([=](auto const&, auto const&) {
                auto props = session.TryGetMediaPropertiesAsync().get();
                  Wh_Log(L"MonitorSystemMedia Now Playing");
            });

            session.PlaybackInfoChanged([=](auto const&, auto const&) {
                auto info = session.GetPlaybackInfo();
                  Wh_Log(L"MonitorSystemMedia PlaybackInfoChanged");

            });
        }
        else
        {
              Wh_Log(L"MonitorSystemMedia No active session");
        }
    });

    auto initialSession = manager.GetCurrentSession();
    if (initialSession)
    {
        auto props = co_await initialSession.TryGetMediaPropertiesAsync();
           Wh_Log(L"MonitorSystemMedia Initially Playing %s", props.Title().c_str() );
        // std::wcout << L"Initially Playing: " << props.Title().c_str() << L" by " << props.Artist().c_str() << L"\n";
    }
    else
    {
           Wh_Log(L"MonitorSystemMedia No media session initially");

    }
}

void AttachMediaControlIfNeeded(Grid const& rootGridTaskBar)
{

}
