#include <mutex>

struct ModSettings {
  int userDefinedTrayTaskGap;
  int userDefinedTaskbarBackgroundHorizontalPadding;
  unsigned int userDefinedTaskbarOffsetY;
  unsigned int userDefinedTaskbarHeight;
  unsigned int userDefinedTaskbarIconSize;
  unsigned int userDefinedTrayIconSize;
  unsigned int userDefinedTaskbarButtonSize;
  unsigned int userDefinedTrayButtonSize;
  float userDefinedTaskbarCornerRadius;
  unsigned int userDefinedTaskButtonCornerRadius;
  bool userDefinedFlatTaskbarBottomCorners;
  unsigned int userDefinedTaskbarBackgroundOpacity;
  unsigned int userDefinedTaskbarBackgroundTint;
  unsigned int userDefinedTaskbarBackgroundLuminosity;
  unsigned int userDefinedTaskbarBackgroundBlurAmount;
  std::wstring userDefinedTaskbarBackgroundTintColor;
  unsigned int userDefinedTaskbarBackgroundTintSaturation;
  unsigned int userDefinedTaskbarBackgroundInversion;
  std::wstring userDefinedTaskbarBackgroundFallbackColor;
  uint8_t userDefinedTaskbarBorderOpacity;
  double userDefinedTaskbarBorderThickness;
  bool userDefinedFullWidthTaskbarBackground;
  bool userDefinedIgnoreShowDesktopButton;
  bool userDefinedStyleTrayArea;
  bool userDefinedTrayAreaDivider;
  unsigned int borderColorR, borderColorG, borderColorB;
  std::vector<std::wstring> userDefinedDividedAppNames;
  bool userDefinedAlignFlyoutInner;
  bool userDefinedCustomizeTaskbarBackground;
  bool userDefinedDisableCustomBlurBackground;
  double userDefinedAppsDividerThickness;
  float userDefinedAppsDividerVerticalScale{0.7};
  bool userDefinedDividerLeftAligned=false;
};

ModSettings g_settings;
std::recursive_mutex g_settingsMutex;

bool GetUserDefinedAlignFlyoutInner() {
  std::lock_guard<std::recursive_mutex> lock(g_settingsMutex);
  return g_settings.userDefinedAlignFlyoutInner;
}
