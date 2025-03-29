<p align="center">
  <img alt="Logo" src="https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/raw/main/logo-raster.png" height="200"/>
</p>

![Screenshot](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/raw/main/screenshot.png)

# Docklike Windhawk Mod ⚒️ Under development
This mod turns Windows 11 taskbar into MacOS-like dock with proper smooth animations without sacrificing the funcitonality of the default taskbar as when using custom apps. It is currently under development. Pull requests are welcome.

# How to install
Copy [`assembled-mod.cpp`](https://raw.githubusercontent.com/DarkionAvey/windhawk-taskbar-centered-condensed/refs/heads/main/assembled-mod.cpp) contents to clipboard, then go to WindHawk > Explore > Create a new mod > Compile Mod

# Source code
You will find the actual code for the mod under [`mod-parts/docklike-mod.cpp`](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/blob/main/mod-parts/docklike-mod.cpp). The rest of the lines are auto generated using the python code. Please don't edit the `assembled-mod.cpp`, the edits will be overwritten during the next build cycle. Instead, edit the source files found in `mod-parts`

# Credits
This mod wouldn't have been possible without `m417z`'s mods, so many thanks to him!

# Options

| Property | Name | Description | Accepted values |
| --- | --- | --- | --- |
| `TrayTaskGap` | Tray task gap | Adjusts the space between task area and tray area. | Integer (whole number) |
| `TaskbarBackgroundHorizontalPadding` | Taskbar background horizontal padding | Adjusts the horizontal padding on both sides of the taskbar background. | Integer (whole number) |
| `TaskbarOffsetY` | Taskbar vertical offset | Adjusts the vertical offset of the entire taskbar; Padding with the same value will be applied to the top. | Non-negative integer |
| `TaskbarHeight` | Taskbar height | Adjusts the height of the entire taskbar. | Non-negative integer |
| `TaskbarIconSize` | Taskbar icon size | Adjusts the width and height of the taskbar icons. | Non-negative integer |
| `TaskbarButtonSize` | Taskbar button size | Adjusts the width and height of taskbar button (surrounding the icons). | Non-negative integer |
| `TaskbarCornerRadius` | Corner radius of task bar | Defines the corner rounding of the taskbar. | Non-negative integer |
| `TaskButtonCornerRadius` | Corner radius of the task buttons | Defines the corner rounding of individual task buttons. | Non-negative integer |
| `FlatTaskbarBottomCorners` | Flat bottom corners | If enabled, the bottom corners of the taskbar will be squared instead of rounded and the taskbar will dock to the edge of the screen. Overrides the taskbar offset setting. | Boolean (true/false) |
| `TaskbarBackgroundOpacity` | Background opacity percentage |  | Non-negative integer |
| `TaskbarBackgroundTint` | Background tint percentage (high = greyscale) |  | Non-negative integer |
| `TaskbarBackgroundLuminosity` | Background luminosity percentage  (high = opaque, low = glassy) |  | Non-negative integer |
| `TaskbarBorderOpacity` | The opacity of the border surrounding the taskbar |  | Non-negative integer |