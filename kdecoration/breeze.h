#ifndef breeze_h
#define breeze_h

/*
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "breezesettings.h"

#include <QSharedPointer>
#include <QList>

namespace Breeze
{
    //* convenience typedefs
    using InternalSettingsPtr = QSharedPointer<InternalSettings>;
    using InternalSettingsList = QList<InternalSettingsPtr>;
    using InternalSettingsListIterator = QListIterator<InternalSettingsPtr>;

    //* metrics
    enum Metrics
    {

        //* corner radius (pixels)
        Frame_FrameRadius = 3,

        //* titlebar metrics, in units of small spacing
        TitleBar_TopMargin = 2,
        TitleBar_BottomMargin = 2,
        TitleBar_SideMargin = 2,
        TitleBar_ButtonSpacing = 2,

        // shadow dimensions (pixels)
        Shadow_Overlap = 3,

    };

    //* standard pen widths
    namespace PenWidth
    {
        /* Using 1 instead of slightly more than 1 causes symbols drawn with
         * pen strokes to look skewed. The exact amount added does not matter
         * as long as it isn't too visible.
         */
        // The standard pen stroke width for symbols.
        static constexpr qreal Symbol = 1.01;
    }

    //* exception
    enum ExceptionMask
    {
        None = 0,
        BorderSize = 1<<4
    };
}

#endif
