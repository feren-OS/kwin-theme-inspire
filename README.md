# Inspire KWin Theme

## Overview

KWin Window Decorations for Feren OS's Inspire theme.

## Installation

This theme was designed to be compiled on KWin 5.24.3, *however compilation can be attempted on other versions of KWin, but this is not recommended*.

Open a terminal inside the source directory and do:
```sh
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
make
sudo make install
```
After the intallation, restart KWin by logging out and in or using ALT+F2 and typing `kwin --replace` in there. After that, 'Inspire' will be available for application in *System Settings &rarr; Appearance &rarr; Window Decorations* so that you can apply it.
