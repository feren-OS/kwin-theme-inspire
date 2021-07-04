/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2019 fauzie811 <https://github.com/fauzie811/Breeze10>
 * SPDX-FileCopyrightText: 2019-2021 Dominic Hayes <ferenosdev@outlook.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */
#include "breezebutton.h"

#include <KDecoration2/DecoratedClient>
#include <KColorUtils>
#include <KIconLoader>
#include <KWindowSystem>

#include <QPainter>
#include <QVariantAnimation>
#include <QPainterPath>

namespace Breeze
{

    using KDecoration2::ColorRole;
    using KDecoration2::ColorGroup;
    using KDecoration2::DecorationButtonType;


    //__________________________________________________________________
    Button::Button(DecorationButtonType type, Decoration* decoration, QObject* parent)
        : DecorationButton(type, decoration, parent)
        , m_animation( new QVariantAnimation( this ) )
    {

        // setup animation
        // It is important start and end value are of the same type, hence 0.0 and not just 0
        m_animation->setStartValue( 0.0 );
        m_animation->setEndValue( 1.0 );
        m_animation->setEasingCurve( QEasingCurve::InOutQuad );
        connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            setOpacity(value.toReal());
        });

        // setup default geometry
        const int height = decoration->buttonHeight();
        const int width = height * (type == DecorationButtonType::Menu ? 1.0 : 1.2);
        setGeometry(QRect(0, 0, height, height));
        setIconSize(QSize( height, height ));

        // connections
        connect(decoration->client().data(), SIGNAL(iconChanged(QIcon)), this, SLOT(update()));
        connect(decoration->settings().data(), &KDecoration2::DecorationSettings::reconfigured, this, &Button::reconfigure);
        connect( this, &KDecoration2::DecorationButton::hoveredChanged, this, &Button::updateAnimationState );

        reconfigure();

    }

    //__________________________________________________________________
    Button::Button(QObject *parent, const QVariantList &args)
        : Button(args.at(0).value<DecorationButtonType>(), args.at(1).value<Decoration*>(), parent)
    {
        m_flag = FlagStandalone;
        //! icon size must return to !valid because it was altered from the default constructor,
        //! in Standalone mode the button is not using the decoration metrics but its geometry
        m_iconSize = QSize(-1, -1);
    }
            
    //__________________________________________________________________
    Button *Button::create(DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent)
    {
        if (auto d = qobject_cast<Decoration*>(decoration))
        {
            Button *b = new Button(type, d, parent);
            switch( type )
            {

                case DecorationButtonType::Close:
                b->setVisible( d->client().data()->isCloseable() );
                QObject::connect(d->client().data(), &KDecoration2::DecoratedClient::closeableChanged, b, &Breeze::Button::setVisible );
                break;

                case DecorationButtonType::Maximize:
                b->setVisible( d->client().data()->isMaximizeable() );
                QObject::connect(d->client().data(), &KDecoration2::DecoratedClient::maximizeableChanged, b, &Breeze::Button::setVisible );
                break;

                case DecorationButtonType::Minimize:
                b->setVisible( d->client().data()->isMinimizeable() );
                QObject::connect(d->client().data(), &KDecoration2::DecoratedClient::minimizeableChanged, b, &Breeze::Button::setVisible );
                break;

                case DecorationButtonType::ContextHelp:
                b->setVisible( d->client().data()->providesContextHelp() );
                QObject::connect(d->client().data(), &KDecoration2::DecoratedClient::providesContextHelpChanged, b, &Breeze::Button::setVisible );
                break;

                case DecorationButtonType::Shade:
                b->setVisible( d->client().data()->isShadeable() );
                QObject::connect(d->client().data(), &KDecoration2::DecoratedClient::shadeableChanged, b, &Breeze::Button::setVisible );
                break;

                case DecorationButtonType::Menu:
                QObject::connect(d->client().data(), &KDecoration2::DecoratedClient::iconChanged, b, [b]() { b->update(); });
                break;

                default: break;

            }

            return b;
        }

        return nullptr;

    }

    //__________________________________________________________________
    void Button::paint(QPainter *painter, const QRect &repaintRegion)
    {
        Q_UNUSED(repaintRegion)

        if (!decoration()) return;

        painter->save();

        // translate from offset
        if( m_flag == FlagFirstInList ) painter->translate( m_offset );
        else painter->translate( 0, m_offset.y() );

        if( !m_iconSize.isValid() ) m_iconSize = geometry().size().toSize();

        // menu button
        if (type() == DecorationButtonType::Menu)
        {

            // Welcome to abusing the fact that the normal icon size in this scenario is pretty much equal to the titlebar height to be able to find out what the titlebar height is - I couldn't get 'height' reading to work :v
            // Set the icon size for the application icon
            int iconpixelsize = (m_iconSize.height()*0.56);
            // Set the Y location for the icon...
            int iconlocation = ( (m_iconSize.height() / 2) - (iconpixelsize / 2) );
            // Set the X location for the icon... if Y is used instead the icon will be permanently stuck to the left
            int iconlocationx = (geometry().topLeft().x() + iconlocation);
            const QRectF iconRect( iconlocationx, iconlocation, iconpixelsize, iconpixelsize );
            if (auto deco =  qobject_cast<Decoration*>(decoration())) {
                const QPalette activePalette = KIconLoader::global()->customPalette();
                QPalette palette = decoration()->client().data()->palette();
                palette.setColor(QPalette::Foreground, deco->fontColor());
                KIconLoader::global()->setCustomPalette(palette);
                decoration()->client().data()->icon().paint(painter, iconRect.toRect());
                if (activePalette == QPalette()) {
                    KIconLoader::global()->resetPalette();
                }    else {
                    KIconLoader::global()->setCustomPalette(palette);
                }
            } else {
                decoration()->client().data()->icon().paint(painter, iconRect.toRect());
            }

        } else {

            drawIcon( painter );

        }

        painter->restore();

    }

    //__________________________________________________________________
    void Button::drawIcon( QPainter *painter ) const
    {

        painter->setRenderHints( QPainter::Antialiasing );

        /*
        scale painter so that its window matches QRect( 0, 0, 36, 30 )
        this makes all further rendering and scaling simpler
        all further rendering is preformed inside QRect( 0, 0, 36, 30 )
        */
        painter->translate( geometry().topLeft() );

        const qreal height( m_iconSize.height() );
        const qreal width( m_iconSize.width() );
        if ( height != 30 )
            painter->scale( width/36, height/32 );

        // render background
        const QColor backgroundColor( this->backgroundColor() );

        // render mark
        const QColor foregroundColor( this->foregroundColor() );
        if( foregroundColor.isValid() )
        {

            // setup painter
            QPen pen( foregroundColor );
            pen.setCapStyle( Qt::RoundCap );
            pen.setJoinStyle( Qt::MiterJoin );
            //pen.setWidthF( PenWidth::Symbol*qMax((qreal)1.0, 20/width ) );
            pen.setWidthF( 2.0 );
            
            const qreal buttonradius(KWindowSystem::compositingActive() ? Metrics::Frame_FrameRadius - 1.0 : 0.0);

            painter->setPen( pen );
            painter->setBrush( Qt::NoBrush );

            switch( type() )
            {

                case DecorationButtonType::Close:
                {
                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }
                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );

                    painter->drawLine( QPointF( 13, 10 ), QPointF( 23, 20 ) );
                    painter->drawLine( QPointF( 13, 20 ), QPointF( 23, 10 ) );
                    break;
                }

                case DecorationButtonType::Maximize:
                {
                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }

                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );

                    if (isChecked())
                    {
                        //For the thinner part of the symbol
                        QPen thinpen = pen;
                        thinpen.setWidthF( 1.0 );
                        thinpen.setCapStyle( Qt::SquareCap );

                        painter->drawRect(QRectF(12, 11, 10.0, 10.0));
                        painter->setPen( thinpen );

                        painter->drawLine( QPointF( 15, 8.5 ), QPointF( 24.5, 8.5 ) );
                        painter->drawLine( QPointF( 24.5, 8.5 ), QPointF( 24.5, 19 ) );
                    }
                    else {
                        painter->drawRect(QRectF(13, 10, 10.0, 10.0));
                    }
                    break;
                }

                case DecorationButtonType::Minimize:
                {
                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }

                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );

                    painter->drawLine( QPointF( 13, 15 ), QPointF( 23, 15 ) );
                    break;
                }

                case DecorationButtonType::OnAllDesktops:
                {
                    painter->setPen( Qt::NoPen );
                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }
                    painter->setBrush( foregroundColor );

                    if( isChecked())
                    {

                        // outer ring
                        painter->drawRect( QRectF( 13, 10, 11, 11 ) );

                        // center dot
//                         QColor backgroundColor( this->backgroundColor() );
//                         auto d = qobject_cast<Decoration*>( decoration() );
//                         if( !backgroundColor.isValid() && d ) backgroundColor = d->titleBarColor();
// 
//                         if( backgroundColor.isValid() )
//                         {
//                             painter->setBrush( backgroundColor );
//                             painter->drawEllipse( QRectF( 8, 8, 2, 2 ) );
//                         }

                    } else {

                        pen.setWidthF( 1.5 );
                        painter->drawPolygon( QPolygonF()
                            << QPointF( 15.5, 15.5 )
                            << QPointF( 21, 10 )
                            << QPointF( 24, 13 )
                            << QPointF( 18.5, 18.5 ) );

                        painter->setPen( pen );
                        painter->drawLine( QPointF( 14.5, 14.5 ), QPointF( 19.5, 19.5 ) );
                        painter->drawLine( QPointF( 21, 13 ), QPointF( 13.5, 20.5 ) );
                        pen.setWidthF( 2.0 );
                    }
                    break;
                }

                case DecorationButtonType::Shade:
                {

                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }
                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );

                    painter->drawLine( 14, 13, 22, 13 );
                    if (isChecked()) {
                        painter->drawPolyline( QPolygonF()
                            << QPointF( 14, 16 )
                            << QPointF( 18, 20 )
                            << QPointF( 22, 16 ) );

                    } else {
                        painter->drawPolyline( QPolygonF()
                            << QPointF( 14, 20 )
                            << QPointF( 18, 16 )
                            << QPointF( 22, 20 ) );
                    }

                    break;

                }

                case DecorationButtonType::KeepBelow:
                {

                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }
                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 14, 12 )
                        << QPointF( 18, 16 )
                        << QPointF( 22, 12 ) );

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 14, 16 )
                        << QPointF( 18, 20 )
                        << QPointF( 22, 16 ) );
                    break;

                }

                case DecorationButtonType::KeepAbove:
                {
                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }
                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 14, 16 )
                        << QPointF( 18, 12 )
                        << QPointF( 22, 16 ) );

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 14, 20 )
                        << QPointF( 18, 16 )
                        << QPointF( 22, 20 ) );
                    break;
                }


                case DecorationButtonType::ApplicationMenu:
                {
                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }
                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );

                    painter->drawLine( QPointF( 13, 12 ), QPointF( 24, 12 ) );
                    painter->drawLine( QPointF( 13, 16 ), QPointF( 24, 16 ) );
                    painter->drawLine( QPointF( 13, 20 ), QPointF( 24, 20 ) );
                    break;
                }

                case DecorationButtonType::ContextHelp:
                {
                    if( backgroundColor.isValid() )
                    {
                        painter->setPen( Qt::NoPen );
                        
                        //Hitbox
                        painter->setBrush( Qt::transparent );
                        painter->drawRect( QRectF( 0, 0, 34, 32 ) );
                        
                        //Visual
                        painter->setBrush( backgroundColor );
                        painter->drawRoundedRect(QRectF( 2, 2, 32, 26 ), buttonradius, buttonradius);
                    }
                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );
                    
                    QPainterPath path;
                    path.moveTo( 14, 12 );
                    path.arcTo( QRectF( 14, 9.5, 8, 5 ), 180, -180 );
                    path.cubicTo( QPointF(22.5, 15.5), QPointF( 18, 13.5 ), QPointF( 18, 17.5 ) );
                    painter->drawPath( path );

                    painter->drawPoint( 18, 21 );

                    break;
                }

                default: break;

            }

        }

    }

    //__________________________________________________________________
    QColor Button::foregroundColor() const
    {
        auto d = qobject_cast<Decoration*>( decoration() );
        if( !d ) {

            return QColor();

        } else if( isPressed() ) {
            
            if( type() == DecorationButtonType::Close ) {
                QColor color(255,255,255);
                return color;
            } else {
                return d->fontColor();
            }

//         } else if( type() == DecorationButtonType::Close && d->internalSettings()->outlineCloseButton() ) {
// 
//             return d->titleBarColor();

        } else if( ( type() == DecorationButtonType::KeepBelow || type() == DecorationButtonType::KeepAbove || type() == DecorationButtonType::Shade ) && isChecked() ) {

            return d->fontColor();

        } else if( m_animation->state() == QAbstractAnimation::Running ) {

            if( type() == DecorationButtonType::Close ) {
                QColor color(255,255,255);
                return color;
            } else {
                return d->fontColor();
            }

        } else if( isHovered() ) {

            if( type() == DecorationButtonType::Close ) {
                QColor color(255,255,255);
                return color;
            } else {
                return d->fontColor();
            }

        } else {

            return d->fontColor();

        }

    }

    //__________________________________________________________________
    QColor Button::backgroundColor() const
    {
        auto d = qobject_cast<Decoration*>( decoration() );
        if( !d ) {

            return QColor();

        }

        auto c = d->client().data();
        QColor redColor( c->color( ColorGroup::Warning, ColorRole::Foreground ) );

        if( isPressed() ) {
            
            if( type() == DecorationButtonType::Close ) {
                QColor color( 200,55,55 );
                return color;
            } else {
                QColor color( d->fontColor() );
                color.setAlpha( color.alpha()*m_opacity*0.2 );
                return KColorUtils::mix( d->titleBarColor(), color, 1 );
            }

        } else if( ( type() == DecorationButtonType::KeepBelow || type() == DecorationButtonType::KeepAbove || type() == DecorationButtonType::Shade ) && isChecked() ) {

            QColor color( d->fontColor() );
            color.setAlpha( color.alpha()*m_opacity*0.2 );
            return KColorUtils::mix( d->titleBarColor(), color, 1 );

        } else if( m_animation->state() == QAbstractAnimation::Running ) {

            if( type() == DecorationButtonType::Close )
            {
                QColor color( 255,85,85 );
                color.setAlpha( color.alpha()*m_opacity );
                return color;
                
            } else {

                QColor color( d->fontColor() );
                color.setAlpha( color.alpha()*m_opacity*0.1 );
                return KColorUtils::mix( d->titleBarColor(), color, 1 );

            }

        } else if( isHovered() ) {

            if( type() == DecorationButtonType::Close ) {
                QColor color( 255,85,85 );
                return color;

            } else {

                QColor color( d->fontColor() );
                color.setAlpha( color.alpha()*m_opacity*0.1 );
                return KColorUtils::mix( d->titleBarColor(), color, 1 );

            }

        } else {

            return QColor();

        }

    }

    //________________________________________________________________
    void Button::reconfigure()
    {

        // animation
        auto d = qobject_cast<Decoration*>(decoration());
        if( d )  m_animation->setDuration( d->animationsDuration() );

    }

    //__________________________________________________________________
    void Button::updateAnimationState( bool hovered )
    {

        auto d = qobject_cast<Decoration*>(decoration());
        if( !(d && d->animationsDuration() > 0 ) ) return;

        m_animation->setDirection( hovered ? QAbstractAnimation::Forward : QAbstractAnimation::Backward );
        if( m_animation->state() != QAbstractAnimation::Running ) m_animation->start();

    }

} // namespace
