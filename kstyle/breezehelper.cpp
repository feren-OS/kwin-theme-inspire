/*
 * Copyright 2009-2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright 2007 Casper Boemann <cbr@boemann.dk>
 * Copyright 2007 Fredrik H?glund <fredrik@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "breezehelper.h"

#include "breezemetrics.h"

#include <KColorUtils>
#include <QPainter>

namespace Breeze
{
    //____________________________________________________________________
    Helper::Helper( KSharedConfig::Ptr config ):
        _config( config )
    {}

    //____________________________________________________________________
    KSharedConfigPtr Helper::config() const
    { return _config; }

    //____________________________________________________________________
    void Helper::loadConfig()
    {
        _viewFocusBrush = KStatefulBrush( KColorScheme::View, KColorScheme::FocusColor, _config );
        _viewHoverBrush = KStatefulBrush( KColorScheme::View, KColorScheme::HoverColor, _config );
        _viewNegativeTextBrush = KStatefulBrush( KColorScheme::View, KColorScheme::NegativeText, _config );
    }

    //______________________________________________________________________________
    QPalette Helper::mergePalettes( const QPalette& source, qreal ratio ) const
    {

        QPalette out( source );
        out.setColor( QPalette::Background, KColorUtils::mix( source.color( QPalette::Active, QPalette::Background ), source.color( QPalette::Disabled, QPalette::Background ), 1.0-ratio ) );
        out.setColor( QPalette::Highlight, KColorUtils::mix( source.color( QPalette::Active, QPalette::Highlight ), source.color( QPalette::Disabled, QPalette::Highlight ), 1.0-ratio ) );
        out.setColor( QPalette::WindowText, KColorUtils::mix( source.color( QPalette::Active, QPalette::WindowText ), source.color( QPalette::Disabled, QPalette::WindowText ), 1.0-ratio ) );
        out.setColor( QPalette::ButtonText, KColorUtils::mix( source.color( QPalette::Active, QPalette::ButtonText ), source.color( QPalette::Disabled, QPalette::ButtonText ), 1.0-ratio ) );
        out.setColor( QPalette::Text, KColorUtils::mix( source.color( QPalette::Active, QPalette::Text ), source.color( QPalette::Disabled, QPalette::Text ), 1.0-ratio ) );
        out.setColor( QPalette::Button, KColorUtils::mix( source.color( QPalette::Active, QPalette::Button ), source.color( QPalette::Disabled, QPalette::Button ), 1.0-ratio ) );
        return out;
    }

    //____________________________________________________________________
    QColor Helper::frameOutlineColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        // colors
        const QColor focus( _viewFocusBrush.brush( palette ).color() );
        const QColor hover( _viewHoverBrush.brush( palette ).color() );
        const QColor defaultOutline( KColorUtils::mix( palette.color( QPalette::Window ), palette.color( QPalette::WindowText ), 0.25 ) );

        QColor outline;
        if( mode == AnimationFocus )
        {

            if( mouseOver ) outline = KColorUtils::mix( hover, focus, opacity );
            else outline = KColorUtils::mix( defaultOutline, focus, opacity );

        } else if( hasFocus ) {

            outline = focus;

        } else if( mode == AnimationHover ) {

            outline = KColorUtils::mix( defaultOutline, hover, opacity );

        } else if( mouseOver ) {

            outline = hover;

        } else outline = defaultOutline;

        return outline;

    }

    //____________________________________________________________________
    QColor Helper::buttonOutlineColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        const QColor defaultOutline( KColorUtils::mix( palette.color( QPalette::Button ), palette.color( QPalette::ButtonText ), 0.4 ) );

        if( mode == AnimationHover )
        {

            if( hasFocus ) return QColor();
            else return alphaColor( defaultOutline, 1-opacity );

        } else if( mouseOver ) {

            return QColor();

        } else if( mode == AnimationFocus ) {

            return alphaColor( defaultOutline, 1-opacity );

        } else if( hasFocus ) {

            return QColor();

        } else return defaultOutline;

    }

    //____________________________________________________________________
    QColor Helper::buttonPanelColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        const QColor normal( palette.color( QPalette::Button ) );
        const QColor focus( _viewFocusBrush.brush( palette ).color() );
        const QColor hover( _viewHoverBrush.brush( palette ).color() );

        if( mode == AnimationHover )
        {

            if( hasFocus ) return KColorUtils::mix( focus, hover, opacity );
            else return KColorUtils::mix( normal, hover, opacity );

        } else if( mouseOver ) {

            return hover;

        } else if( mode == AnimationFocus ) {

            return KColorUtils::mix( normal, focus, opacity );

        } else if( hasFocus ) {

            return focus;

        } else return normal;

    }

    //____________________________________________________________________
    QColor Helper::alphaColor( QColor color, qreal alpha )
    {
        if( alpha >= 0 && alpha < 1.0 )
        { color.setAlphaF( alpha*color.alphaF() ); }
        return color;
    }

    //______________________________________________________________________________
    void Helper::renderFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QColor& outline, bool focus ) const
    {

        painter->setRenderHint( QPainter::Antialiasing );

        const QRectF baseRect( rect );

        if( color.isValid() )
        {
            // content
            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 0.5 );

            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawRoundedRect( baseRect.adjusted( 1, 1, -1, -1 ), radius, radius );
        }

        if( outline.isValid() )
        {

            // outline
            if( focus )
            {

                const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 1.5 );

                painter->setPen( QPen( outline, 2 ) );
                painter->setBrush( Qt::NoBrush );
                painter->drawRoundedRect( baseRect.adjusted( 1, 1, -1, -1 ), radius, radius );

            } else {

                const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 1 );

                painter->setPen( QPen( outline, 1 ) );
                painter->setBrush( Qt::NoBrush );
                painter->drawRoundedRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ), radius, radius );

            }
        }

    }

    //______________________________________________________________________________
    void Helper::renderButtonFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QColor& outline, const QColor& shadow,
        bool focus, bool sunken ) const
    {
        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        // copy rect
        QRectF baseRect( rect );

        // shadow
        if( !sunken )
        {

            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 1 );

            painter->setPen( QPen( shadow, 2 ) );
            painter->setBrush( Qt::NoBrush );
            const QRectF shadowRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ).translated( 0, 0.5 ) );
            painter->drawRoundedRect( shadowRect, radius, radius );

        }

        // content
        {

            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 0.5 );

            painter->setPen( Qt::NoPen );

            const QRectF contentRect( baseRect.adjusted( 1, 1, -1, -1 ) );
            QLinearGradient gradient( contentRect.topLeft(), contentRect.bottomLeft() );
            gradient.setColorAt( 0, color.lighter( focus ? 103:101 ) );
            gradient.setColorAt( 1, color.darker( focus ? 110:103 ) );
            painter->setBrush( gradient );

            painter->drawRoundedRect( contentRect, radius, radius );

        }

        // outline
        if( outline.isValid() )
        {
            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 1 );
            painter->setPen( QPen( outline, 1 ) );
            painter->setBrush( Qt::NoBrush );
            const QRectF outlineRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ) );
            painter->drawRoundedRect( outlineRect, radius, radius );

        }

    }

    //______________________________________________________________________________
    void Helper::renderTabWidgetFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QColor& outline, Corners corners ) const
    {

        painter->setRenderHint( QPainter::Antialiasing );

        const QRectF baseRect( rect );

        if( color.isValid() )
        {
            // content
            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 0.5 );
            const QRectF contentRect( baseRect.adjusted( 1, 1, -1, -1 ) );
            QPainterPath path( roundedPath( contentRect, radius, corners ) );
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawPath( path );
        }

        if( outline.isValid() )
        {

            // outline
            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 1 );
            const QRectF outlineRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ) );
            QPainterPath path( roundedPath( outlineRect, radius, corners ) );

            painter->setPen( QPen( outline, 1 ) );
            painter->setBrush( Qt::NoBrush );
            painter->drawPath( path );

        }

    }

    //______________________________________________________________________________
    void Helper::renderCheckBox(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QColor& shadow,
        bool sunken, CheckBoxState state ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        // copy rect
        QRectF baseRect( rect );

        // shadow
        if( !sunken )
        {

            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 1 );
            painter->setPen( QPen( shadow, 2 ) );
            painter->setBrush( Qt::NoBrush );

            const QRectF shadowRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ).translated( 0, 0.5 ) );
            painter->drawRoundedRect( shadowRect, radius, radius );

        }

        // content
        {

            const qreal radius( qreal( Metrics::Frame_FrameRadius ) - 0.5 );
            painter->setPen( QPen( color, 2 ) );
            painter->setBrush( Qt::NoBrush );

            const QRectF contentRect( baseRect.adjusted( 2, 2, -2, -2 ) );
            painter->drawRoundedRect( contentRect, radius, radius );

        }

        // mark
        if( state == CheckOn )
        {

            painter->setBrush( color );
            painter->setPen( Qt::NoPen );

            const QRectF markerRect( baseRect.adjusted( 5, 5, -5, -5 ) );
            painter->drawRect( markerRect );

        } else if( state == CheckPartial ) {

            QPen pen( color, 2 );
            pen.setJoinStyle( Qt::MiterJoin );
            painter->setPen( pen );

            const QRectF markerRect( baseRect.adjusted( 6, 6, -6, -6 ) );
            painter->drawRect( markerRect );

            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->setRenderHint( QPainter::Antialiasing, false );

            QPainterPath path;
            path.moveTo( 5, 5 );
            path.lineTo( qreal( Metrics::CheckBox_Size ) -6, 5 );
            path.lineTo( 5, qreal( Metrics::CheckBox_Size ) - 6 );
            painter->drawPath( path );

        }

    }

    //______________________________________________________________________________
    void Helper::renderRadioButton(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QColor& shadow,
        bool sunken, bool checked ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        QRectF baseRect( rect );

        // shadow
        if( !sunken )
        {

            painter->setPen( QPen( shadow, 2 ) );
            painter->setBrush( Qt::NoBrush );

            const QRectF shadowRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ).translated( 0, 0.5 ) );
            painter->drawEllipse( shadowRect );

        }

        // content
        {

            painter->setPen( QPen( color, 2 ) );
            painter->setBrush( Qt::NoBrush );

            const QRectF contentRect( baseRect.adjusted( 2, 2, -2, -2 ) );
            painter->drawEllipse( contentRect );

        }

        // mark
        if( checked )
        {

            painter->setBrush( color );
            painter->setPen( Qt::NoPen );

            const QRectF markerRect( baseRect.adjusted( 5, 5, -5, -5 ) );
            painter->drawEllipse( markerRect );

        }

    }

    //______________________________________________________________________________
    void Helper::renderSliderGroove(
        QPainter* painter, const QRect& rect,
        const QColor& color ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        const qreal radius( 0.5*Metrics::Slider_Thickness );

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawRoundedRect( baseRect, radius, radius );
        }

        return;

    }

    //______________________________________________________________________________
    void Helper::renderDialGroove(
        QPainter* painter, const QRect& rect,
        const QColor& color ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );

        // content
        if( color.isValid() )
        {
            const qreal penWidth( Metrics::Slider_Thickness );
            const QRectF grooveRect( rect.adjusted( penWidth/2, penWidth/2, -penWidth/2, -penWidth/2 ) );

            painter->setPen( QPen( color, penWidth ) );
            painter->setBrush( Qt::NoBrush );
            painter->drawEllipse( grooveRect );
        }

        return;

    }

    //______________________________________________________________________________
    void Helper::renderDialContents(
        QPainter* painter, const QRect& rect,
        const QColor& color,
        qreal first, qreal second ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );

        // content
        if( color.isValid() )
        {

            // setup groove rect
            const qreal penWidth( Metrics::Slider_Thickness );
            const QRectF grooveRect( rect.adjusted( penWidth/2, penWidth/2, -penWidth/2, -penWidth/2 ) );

            // setup angles
            const int angleStart( first * 180 * 16 / M_PI );
            const int angleSpan( (second - first ) * 180 * 16 / M_PI );

            // setup pen
            if( angleSpan != 0 )
            {
                QPen pen( color, penWidth );
                pen.setCapStyle( Qt::RoundCap );
                painter->setPen( pen );
                painter->setBrush( Qt::NoBrush );
                painter->drawArc( grooveRect, angleStart, angleSpan );
            }

        }

        return;

    }

    //______________________________________________________________________________
    void Helper::renderSliderHandle(
        QPainter* painter, const QRect& rect,
        const QColor& color,
        const QColor& outline,
        const QColor& shadow,
        bool focus,
        bool sunken ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        QRectF baseRect( rect );

        // shadow
        if( !sunken )
        {

            painter->setPen( QPen( shadow, 2 ) );
            painter->setBrush( Qt::NoBrush );

            const QRectF shadowRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ).translated( 0, 0.5 ) );
            painter->drawEllipse( shadowRect );

        }

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );

            const QRectF contentRect( baseRect.adjusted( 1, 1, -1, -1 ) );
            painter->drawEllipse( contentRect );

        }

        // outline
        if( outline.isValid() )
        {
            painter->setBrush( Qt::NoBrush );
            QRectF outlineRect;
            if( focus )
            {
                painter->setPen( QPen( outline, 2 ) );
                outlineRect = baseRect.adjusted( 2, 2, -2, -2 );
            } else {
                painter->setPen( QPen( outline, 1 ) );
                outlineRect = baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 );
            }

            painter->drawEllipse( outlineRect );

        }

    }

    //______________________________________________________________________________
    void Helper::renderProgressBarGroove(
        QPainter* painter, const QRect& rect,
        const QColor& color ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        const qreal radius( 0.5*Metrics::ProgressBar_Thickness );

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawRoundedRect( baseRect, radius, radius );
        }

        return;

    }


    //______________________________________________________________________________
    void Helper::renderProgressBarBusyContents(
        QPainter* painter, const QRect& rect,
        const QColor& first,
        const QColor& second,
        bool horizontal,
        bool reverse,
        int progress
        ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        const qreal radius( 0.5*Metrics::ProgressBar_Thickness );

        // setup brush
        QPixmap pixmap( horizontal ? 2*Metrics::ProgressBar_BusyIndicatorSize : 1, horizontal ? 1:2*Metrics::ProgressBar_BusyIndicatorSize );
        pixmap.fill( second );
        if( horizontal )
        {

            QPainter painter( &pixmap );
            painter.setBrush( first );
            painter.setPen( Qt::NoPen );

            progress %= 2*Metrics::ProgressBar_BusyIndicatorSize;
            if( reverse ) progress = 2*Metrics::ProgressBar_BusyIndicatorSize - progress - 1;
            painter.drawRect( QRect( 0, 0, Metrics::ProgressBar_BusyIndicatorSize, 1 ).translated( progress, 0 ) );

            if( progress > Metrics::ProgressBar_BusyIndicatorSize )
            { painter.drawRect( QRect( 0, 0, Metrics::ProgressBar_BusyIndicatorSize, 1 ).translated( progress - 2*Metrics::ProgressBar_BusyIndicatorSize, 0 ) ); }

        } else {

            QPainter painter( &pixmap );
            painter.setBrush( first );
            painter.setPen( Qt::NoPen );

            progress %= 2*Metrics::ProgressBar_BusyIndicatorSize;
            progress = 2*Metrics::ProgressBar_BusyIndicatorSize - progress - 1;
            painter.drawRect( QRect( 0, 0, 1, Metrics::ProgressBar_BusyIndicatorSize ).translated( 0, progress ) );

            if( progress > Metrics::ProgressBar_BusyIndicatorSize )
            { painter.drawRect( QRect( 0, 0, 1, Metrics::ProgressBar_BusyIndicatorSize ).translated( 0, progress - 2*Metrics::ProgressBar_BusyIndicatorSize ) ); }

        }

        painter->setPen( Qt::NoPen );
        painter->setBrush( pixmap );
        painter->drawRoundedRect( baseRect, radius, radius );

        return;

    }

    //______________________________________________________________________________
    void Helper::renderScrollBarHandle(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QColor& outline ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        const qreal radius( 0.5*Metrics::ScrollBar_SliderWidth );

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawRoundedRect( baseRect, radius, radius );
        }


        // outline
        if( outline.isValid() )
        {
            painter->setPen( QPen( outline, 2 ) );
            painter->setBrush( Qt::NoBrush );

            const QRectF outlineRect( baseRect.adjusted( 1, 1, -1, -1 ) );
            painter->drawRoundedRect( outlineRect, radius - 0.5, radius - 0.5 );
        }

        return;

    }

    //______________________________________________________________________________
    void Helper::renderTabBarTab( QPainter* painter, const QRect& rect, const QColor& color, const QColor& outline, Corners corners ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );

            const qreal radius( qreal( Metrics::TabBar_TabRadius ) - 0.5 );
            QPainterPath path( roundedPath( baseRect, radius, corners ) );
            painter->drawPath( path );

        }

        // outline
        if( outline.isValid() )
        {
            painter->setPen( QPen( outline, 1 ) );
            painter->setBrush( Qt::NoBrush );

            const qreal radius( qreal( Metrics::TabBar_TabRadius ) - 1 );
            QPainterPath path( roundedPath( baseRect.adjusted( 0.5, 0.5, -0.5, -0.5 ), radius, corners ) );
            painter->drawPath( path );
        }

    }

    //______________________________________________________________________________
    QPainterPath Helper::roundedPath( const QRectF& rect, qreal radius, Corners corners ) const
    {

        QPainterPath path;

        const QSizeF cornerSize( 2*radius, 2*radius );

        // rotate counterclockwise
        // top left corner
        if( corners & CornerTopLeft )
        {

            path.moveTo( rect.topLeft() + QPointF( radius, 0 ) );
            path.arcTo( QRectF( rect.topLeft(), cornerSize ), 90, 90 );

        } else path.moveTo( rect.topLeft() );

        // bottom left corner
        if( corners & CornerBottomLeft )
        {

            path.lineTo( rect.bottomLeft() - QPointF( 0, radius ) );
            path.arcTo( QRectF( rect.bottomLeft() - QPointF( 0, 2*radius ), cornerSize ), 180, 90 );

        } else path.lineTo( rect.bottomLeft() );

        // bottom right corner
        if( corners & CornerBottomRight )
        {

            path.lineTo( rect.bottomRight() - QPointF( radius, 0 ) );
            path.arcTo( QRectF( rect.bottomRight() - QPointF( 2*radius, 2*radius ), cornerSize ), 270, 90 );

        } else path.lineTo( rect.bottomRight() );

        // top right corner
        if( corners & CornerTopRight )
        {

            path.lineTo( rect.topRight() + QPointF( 0, radius ) );
            path.arcTo( QRectF( rect.topRight() - QPointF( 2*radius, 0 ), cornerSize ), 0, 90 );

        } else path.lineTo( rect.topRight() );

        path.closeSubpath();
        return path;

    }

}