bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return false;

bool scrollAnchorProperty=  winrt::unbox_value<bool>(element.GetValue(UIElement::CanBeScrollAnchorProperty()));
  if(!scrollAnchorProperty)return true;
  auto transform = element.TransformToVisual(nullptr);
  winrt::Windows::Foundation::Rect rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
  return rect.X < 0 || rect.Y < 0;
}
