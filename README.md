# capture-card-viewer
Simple program to view a capture card video feed.
I made it because I was too lazy to open OBS and then open the windowed projector.

## To use
- Double click: toggle fullscreen
- Right click: fit window to preview size
- Hover over the window, click the settings button in the top-left, pick the capture card to be shown
- Program remembers last device opened

## To compile
- Have Qt6 installed
- Find a copy of OBS source
- Put the include files in `./include/obs`
- Put `obs.lib` in `./lib/`
- Hope to god it compiles properly
- Copy into the `debug` or `release` folder
  - `data`
  - `obs-plugins`
  - `platforms`
  - `styles`
  - `libobs-d3d11.dll`
  - `obs.dll`
  - `Qt6Core.dll`
  - `Qt6Gui.dll`
  - `Qt6Widgets.dll`
  - `swscale-8.dll`
  - `zlib.dll`