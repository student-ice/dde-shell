# SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_library(dock-clipboarditem SHARED
    clipboarditem.cpp
    clipboarditem.h
    ../dockiteminfo.cpp
    ../dockiteminfo.h
)

target_link_libraries(dock-clipboarditem PRIVATE
    dde-shell-frame
)

ds_install_package(PACKAGE org.deepin.ds.dock.clipboarditem TARGET dock-clipboarditem)
