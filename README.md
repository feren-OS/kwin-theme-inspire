# Feren OS KWin Theme

## Overview

The Feren OS KWin Theme is a fork of the official KDE Breeze KWin Theme which is adapted to look like the official Feren OS titlebar theme, while keeping most of the features of the KDE Breeze Theme.

## Credits

This theme was forked from the official KWin Breeze Theme Source Code, and you'll find credits for who made the theme in each code file.

Parts of 'Breeze10' by https://github.com/fauzie811 were also used and adapted into this theme, mainly around the code for the titlebar buttons.

You can see most of the code changes made in this fork by visiting this link: https://github.com/feren-OS/kwin-theme-feren/commit/fdd119e2405cceac1c4a54b81a989caf49d10b5f

## Installation

This theme was designed to be compiled on KWin 5.16.3, *however compilation can be attempted on other versions of KWin, but this is not recommended*.

Open a terminal inside the source directory and do:
```sh
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_LIBDIR=lib -DBUILD_TESTING=OFF -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
make
sudo make install
```
After the intallation, restart KWin by logging out and in or using ALT+F2 and typing `kwin --replace` in there. After that, 'Feren' will be available for application in *System Settings &rarr; Application Style &rarr; Window Decorations* so that you can apply it.

## Screenshot
![Feren OS KWin Theme in action](https://github.com/feren-OS/kwin-theme-feren/raw/5.17.0/screenshots/screenshot.png)
