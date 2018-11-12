/*
   This file is part of the KDE games lskat program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// General includes
// #include <typeinfo>

// Own include
#include "thememanager.h"

// Qt includes
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QRectF>

// KDE includes
#include <KConfigGroup>

// Local includes
#include "lskat_debug.h"
#include "lskatglobal.h"
#include "deck.h"
#include "fromlibkdegames/cardcache.h"

// SVG id for accessing the backside of a card
#define CARDBACK_SVGID "back"

//convenience methods for converting to KCardInfo enums
KCardInfo convertToKCardInfo(CardDeck::Suite suite, CardDeck::CardType card)
{
    KCardInfo::Suit s;
    KCardInfo::Card c;
    switch(card)
    {
        case CardDeck::Ace:
            c = KCardInfo::Ace;
            break;
        case CardDeck::Eight:
            c = KCardInfo::Eight;
            break;
        case CardDeck::King:
            c = KCardInfo::King;
            break;
        case CardDeck::Nine:
            c = KCardInfo::Nine;
            break;
        case CardDeck::Ten:
            c = KCardInfo::Ten;
            break;
        case CardDeck::Jack:
            c = KCardInfo::Jack;
            break;
        case CardDeck::Seven:
            c = KCardInfo::Seven;
            break;
        case CardDeck::Queen:
            c = KCardInfo::Queen;
            break;
    }
    switch(suite)
    {
        case CardDeck::Heart:
            s = KCardInfo::Heart;
            break;
        case CardDeck::Club:
            s = KCardInfo::Club;
            break;
        case CardDeck::Diamond:
            s = KCardInfo::Diamond;
            break;
        case CardDeck::Spade:
            s = KCardInfo::Spade;
            break;
        default:
            s = KCardInfo::None;
            break;
    }
    return KCardInfo(s, c);
}

// Constructor for the theme manager
ThemeManager::ThemeManager(const QString &cardTheme,
                           const QString &themefile, QObject *parent, int initialSize)
    : QObject(parent), mRenderer(0), mConfig(0), mCardTheme(cardTheme),
    mScale(initialSize), mAspectRatio(1.0), mCardAspectRatio(1.0)
{
    mCardCache = new KCardCache();

    // updateTheme(themefile);
    updateCardTheme(themefile, cardTheme);
}

ThemeManager::~ThemeManager()
{
    delete mConfig;
    delete mCardCache;
}

// Register an object with the manager
void ThemeManager::registerTheme(Themable *ob)
{
    mObjects[ob] = 1;
}

// Unregister an object from the manager
void ThemeManager::unregisterTheme(Themable *ob)
{
    mObjects.remove(ob);
}

// Check whether the theme is alright
int ThemeManager::checkTheme()
{
    // Check theme
    if (mRenderer == 0) return 1;
    return 0; // OK
}

// Force an refresh of the theme object given
void ThemeManager::updateTheme(Themable *ob)
{
    ob->changeTheme();
}

// Update card deck and card set
void ThemeManager::updateCardTheme(const QString &cardTheme)
{
    updateCardTheme(mThemeFile, cardTheme);
}

// Update card deck and card set
void ThemeManager::updateCardTheme(const QString &themefile, const QString &cardTheme)
{
    if (global_debug > 1)
    {
      qCDebug(LSKAT_LOG) << "ThemeManager Pixmap cards: ";
      qCDebug(LSKAT_LOG) << "  Card theme =" << cardTheme;
    }

    // Cards
    mCardTheme = cardTheme;
    mCardCache->setDeckName(mCardTheme);

    mCardCache->setSize(QSize());

    updateTheme(themefile);
}

// Update the theme file and refresh all registered objects. Used
// to really change the theme.
void ThemeManager::updateTheme(const QString &themefile)
{
    // Empty cache
    mPixmapCache.clear();
    mThemeFile = themefile;

    // Process dirs
    QString rcfile = QStandardPaths::locate(QStandardPaths::DataLocation, "grafix/" + themefile);
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "ThemeManager LOAD with theme " << rcfile;

    // Read config and SVG file for theme
    delete mConfig;
    mConfig = new KConfig(rcfile, KConfig::NoGlobals);
    QString svgfile = config(QLatin1String("general")).readEntry("svgfile");
    svgfile = QStandardPaths::locate(QStandardPaths::DataLocation, "grafix/" + svgfile);
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Reading SVG master file  =" << svgfile;

    mAspectRatio     =  config(QLatin1String("general")).readEntry("aspect-ratio", 1.0);
    mCardAspectRatio =  config(QLatin1String("general")).readEntry("card-aspect-ratio", 1.0);
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Aspect ration =" << mAspectRatio << "Cards aspect=" << mCardAspectRatio;

    delete mRenderer;
    mRenderer = new QSvgRenderer(this);
    bool result = mRenderer->load(svgfile);
    if (!result)
    {
        mRenderer = 0;
        qCCritical(LSKAT_LOG) << "Cannot open file" << svgfile;
    }

    // Notify all theme objects of a change
    QHashIterator<Themable *, int> it(mObjects);
    while (it.hasNext())
    {
        it.next();
        Themable *ob = it.key();
        ob->changeTheme();
    }
}

// Rescale the theme. Call all registered objects so that they can refresh.
void ThemeManager::rescale(int scale, QPoint offset)
{
    if (global_debug > 1)
    {
        if (scale == mScale)
            qCDebug(LSKAT_LOG) << "No scale change to" << scale << "If this happens too often it is BAD";
    }
    //if (scale == mScale) return;
    mScale = scale;
    mOffset = offset;
    if (global_debug > 1) qCDebug(LSKAT_LOG) << "THEMEMANAGER:: Rescale to " << scale << " offset to " << offset;

    QHashIterator<Themable *, int> it(mObjects);
    while (it.hasNext())
    {
        it.next();
        Themable *ob = it.key();
        ob->changeTheme();
    }
}

// Retrieve the theme's scale
double ThemeManager::getScale()
{
    return (double)mScale;
}

// Retrieve the theme offset
QPoint ThemeManager::getOffset()
{
    return mOffset;
}

// Retrieve the current theme configuration file.
KConfigGroup ThemeManager::config(const QString &id)
{
    KConfigGroup grp = mConfig->group(id);
    return grp;
}

// Get the pixmap for a card , given suite, type and the desired card width in pixel
const QPixmap ThemeManager::getCard(int suite, int cardtype, double width)
{
    KCardInfo info = convertToKCardInfo(CardDeck::Suite(suite), CardDeck::CardType(cardtype));
    QSize s(int(width), int(width / mCardAspectRatio));

    if (s != mCardCache->size())
    {
        mCardCache->setSize(s);
        QMetaObject::invokeMethod(this, "loadCardsInBackground", Qt::QueuedConnection);
    }

    QPixmap pix = mCardCache->frontside(info);
    return pix;
}

// Get the pixmap for a card back, given the desired card width in pixel
const QPixmap ThemeManager::getCardback(double width)
{
    QSize s(int(width), int(width / mCardAspectRatio));
    if (s != mCardCache->size())
    {
        mCardCache->setSize(s);
        QMetaObject::invokeMethod(this, "loadCardsInBackground", Qt::QueuedConnection);
    }

    QPixmap pix = mCardCache->backside();
    return pix;
}

// Get a pixmap when its size is given (this can distort the image)
const QPixmap ThemeManager::getPixmap(QSvgRenderer *renderer, const QString &svgid, const QSize &size)
{
    QPixmap pixmap;
    if (size.width() < 1 || size.height() < 1)
    {
        if (global_debug > 1)
        qCDebug(LSKAT_LOG) << "ThemeManager::getPixmap Cannot create svgid ID " << svgid << " with zero size" << size;
        return pixmap;
    }

    // Cached pixmap?
    if (mPixmapCache.contains(svgid))
    {
        pixmap = mPixmapCache[svgid];
        if (pixmap.size() == size)
        {
            return pixmap;
        }
    }

    // Create new image
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter p(&image);
    renderer->render(&p, svgid);
    p.end();
    pixmap = QPixmap::fromImage(image);
    if (pixmap.isNull())
        qCCritical(LSKAT_LOG) << "ThemeManager::getPixmap Cannot load svgid ID " << svgid;

    // Cache image
    mPixmapCache[svgid] = pixmap;

    return pixmap;
}

// Get a pixmap when its size is given (this can distort the image)
const QPixmap ThemeManager::getPixmap(const QString &svgid, const QSize &size)
{
    return getPixmap(mRenderer, svgid, size);
}

// Get a pixmap when only width is given (this keeps the aspect ratio)
const QPixmap ThemeManager::getPixmap(const QString &svgid, double width)
{
    QRectF rect   = mRenderer->boundsOnElement(svgid);
    double factor = width / rect.width();
    QSize size    = QSize(int(width), int(rect.height() * factor));
    return getPixmap(svgid, size);
}

// Get a pixmap with original properties and a scale factor given with respect to
// another SVG item.
const QPixmap ThemeManager::getPixmap(const QString &svgid, const QString &svgref, double refwidth)
{
    QRectF refrect    = mRenderer->boundsOnElement(svgref);
    QRectF rect       = mRenderer->boundsOnElement(svgid);
    double factor     = refwidth / refrect.width();
    QSize size        = QSize(int(rect.width() * factor), int(rect.height() * factor));
    return getPixmap(svgid, size);
}

void ThemeManager::loadCardsInBackground()
{
    mCardCache->loadTheme(KCardCache::LoadFrontSide|KCardCache::Load32Cards);
}

// ========================== Themable interface ===============================

// Constructs a themable interface
Themable::Themable()
{
    mScale        = 1.0;
    mThemeManager = 0;
}

// Constructs a themable interface given its id and the master theme manager.
// This automatically registers the object with the manager.
Themable::Themable(const QString &id, ThemeManager *thememanager)
{
    mScale        = 1.0;
    mId           = id;
    mThemeManager = thememanager;
    if (!thememanager) return;
    thememanager->registerTheme(this);
}

// Destructs the themable object
Themable::~Themable()
{
    if (mThemeManager) mThemeManager->unregisterTheme(this);
}
