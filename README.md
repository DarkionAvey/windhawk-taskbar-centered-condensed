![Screenshot](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/raw/main/screenshot.png)

# WinDock Windhawk Mod

Transform your Windows 11 taskbar into a smooth, MacOS-style dock — without losing any of the original functionality!

---

## 🚀 How to Install (Development Build)

⚠️ **Note:** Please disable any mods that affect taskbar height or taskbar icons—this mod already includes those
features.
1. [Install Windhawk](https://windhawk.net/) if you haven't already.
2. Copy the contents of [
   `assembled-mod.cpp`](https://raw.githubusercontent.com/DarkionAvey/windhawk-taskbar-centered-condensed/main/assembled-mod.cpp)
   to your clipboard.
3. Open **WindHawk** and navigate to: `Explore` → `Create a new mod`.
4. Press `Ctrl+A` to select all, then `Ctrl+V` to paste.
5. Click **Compile Mod** button on the top left corner.
6. Change the mod's settings to fit your preference.
7. If your explorer.exe is not responding, disable the mod from Windhawk, open task manager, restart explorer.exe, wait
   for the default taskbar to appear, then apply WinDock.

---

## 🛠 Source Code

The actual mod code is split into files under [
`mod-parts/`](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/blob/main/mod-parts/), which are later merged together using a Python script.

❗ **Do not edit `assembled-mod.cpp` manually**, as any changes will be overwritten in the next build cycle. Instead,
modify the source files in the `mod-parts` directory.

---

## 🛣️ To-do

1. Proper unloading
2. Fix overflowing

---

## 🙌 Credits

Huge thanks to these awesome developers who made this mod possible -- your contributions to modding Windows are truly appreciated!:

- [`Michael Maltsev (m417z)`](https://github.com/m417z)
- [`Valentin Radu (valinet)`](https://github.com/valinet)

---

## 🔥 Recommended Mods

- [Smart Auto Hide for Taskbar](https://windhawk.net/mods/taskbar-auto-hide-when-maximized)
- [Taskbar Auto-Hide Speed/Frame Rate](https://windhawk.net/mods/taskbar-auto-hide-speed)
- [Show All Tray Icons](https://windhawk.net/mods/taskbar-notification-icons-show-all)

---

# Options

| Property | Name | Description | Accepted values |
| --- | --- | --- | --- |
| `TrayTaskGap` | Tray task gap | Adjusts the space between the task area and the tray area (Default is 20) | Non-negative integer |
| `TaskbarBackgroundHorizontalPadding` | Taskbar background horizontal padding | Sets the horizontal padding on both sides of the taskbar background (Default is 6) | Non-negative integer |
| `TaskbarOffsetY` | Taskbar vertical offset | Moves the taskbar up or down. Padding of the same value is applied to the top (Default is 6) | Non-negative integer |
| `TaskbarHeight` | Taskbar height | Sets the height of the taskbar (Default is 78) | Non-negative integer |
| `TaskbarIconSize` | Taskbar icon size | Defines the width and height of taskbar icons (Default is 48) | Non-negative integer |
| `TaskbarButtonSize` | Taskbar button size | Sets the size of taskbar buttons, which surround the icons (Default is 74) | Non-negative integer |
| `TaskbarCornerRadius` | Taskbar corner radius | Controls how rounded the taskbar corners appear (Default is 24) | Non-negative integer |
| `TaskButtonCornerRadius` | Task button corner radius | Controls how rounded the corners of individual task buttons are (Default is 16) | Non-negative integer |
| `FlatTaskbarBottomCorners` | Flat bottom corners | If enabled, the bottom corners of the taskbar will be squared instead of rounded, and the taskbar will dock to the screen edge. This overrides the taskbar offset; this is always on with full-width taskbar background (Default is off) | Boolean (true/false) |
| `CustomizeTaskbarBackground` | Stylize the taskbar background | If enabled, the taskbar background will be changed to acrylic blur. Disable this if you are running other mods that change the taskbar background. You may need to restart explorer.exe to restore the default taskbar background (Default is on) | Boolean (true/false) |
| `TaskbarBackgroundOpacity` | Background opacity | Adjusts the opacity of the taskbar background. 0 = fully transparent, 100 = fully opaque (Default is 100) | Non-negative integer |
| `TaskbarBackgroundTint` | Background tint | Modifies the taskbar tint level, where higher values increase grayscale effect. Range 0-100 (Default is 0) | Non-negative integer |
| `TaskbarBackgroundLuminosity` | Background luminosity | Adjusts luminosity of the taskbar background. Higher values make it more opaque, lower values make it more glass-like. Range 0-100 (Default is 30) | Non-negative integer |
| `TaskbarBorderOpacity` | Border opacity | Controls the opacity of the taskbar border, as well as the dividers. Range 0-100 (Default is 20) | Non-negative integer |
| `TaskbarBorderColorHex` | Border color (HEX) | Adjusts the color of the taskbar border, as well as the dividers. Hex color as `#RRGGBB` (Default is `#ffffff`) | string hex color |
| `TaskbarBorderThickness` | Taskbar border thickness scale | Adjusts the scale of the taskbar border. Range 0-100 (Default is 8) | Non-negative integer |
| `DividedAppNames` | App names for divider placement | Specify portions of app names (supports regex) where you want a divider on their left side. Separate entries with `;` (e.g., Steam;Notepad\+\+;Settings). Case-insensitive. | string regex |
| `FullWidthTaskbarBackground` | Full-width taskbar background | If enabled, the taskbar background spans the entire width of the screen (Default is off) | Boolean (true/false) |
| `IgnoreShowDesktopButton` | Ignore "Show Desktop" button | When enabled, the "Show Desktop" button is ignored in width calculations (Default is off) | Boolean (true/false) |
| `TrayAreaDivider` | Tray area divider | If enabled, the tray area will be separated by a divider with the same color as the taskbar. Will also apply to widget element if available (Default is on) | Boolean (true/false) |
| `StyleTrayArea` | Modify the tray area appearance | If enabled, the options for tray icon size will take effect (Default is off) | Boolean (true/false) |
| `TrayIconSize` | Tray icon size | Defines the width and height of tray icons (Default is 30) | Non-negative integer |
| `TrayButtonSize` | Tray button size | Sets the size of tray buttons, which surround the icons (Default is 45) | Non-negative integer |
| `MoveFlyoutStartMenu` | Move Start Menu with Taskbar | Dynamically repositions the Start menu to align with taskbar size and location (Default is on). | Boolean (true/false) |
| `MoveFlyoutControlCenter` | Move Control Center with Taskbar | Dynamically repositions the Control Center to align with taskbar size and location (Default is on). | Boolean (true/false) |
| `MoveFlyoutNotificationCenter` | Move Notification Center with Taskbar | Dynamically repositions the Notification Center to align with taskbar size and location (Default is on). | Boolean (true/false) |
| `AlignFlyoutInner` | Align flyout windows to the inside of the taskbar | When on, the flyout windows will be aligned within the bounds of the taskbar. When off, they will be 50% inside the taskbar bounds (Default is on). | Boolean (true/false) |