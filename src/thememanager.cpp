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

// Qt includes
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QRectF>

// KDE includes
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "thememanager.h"
#include "deck.h"

// Constructor for the theme manager
ThemeManager::ThemeManager(QString cards, QString deck, bool svg, QString themefile, QObject* parent, int initialSize)
    : QObject(parent)
{
  mScale = initialSize;
  mAspectRatio = 1.0;
  mCardAspectRatio = 1.0;
  mSVGCards = svg;
  mCardFile = cards;
  mDeckFile = deck;
  mRenderer = 0;
  mCardRenderer = 0;

  // updateTheme(themefile);
  updateCardTheme(themefile, cards, deck);
}


// Register an object with the manager
void ThemeManager::registerTheme(Themable* ob)
{
  mObjects[ob] = 1;
}


// Unregister an object from the manager
void ThemeManager::unregisterTheme(Themable* ob)
{
  mObjects.remove(ob);
}


// Force an refresh of the theme object given
void ThemeManager::updateTheme(Themable* ob)
{
  ob->changeTheme();
}


// Update a pixmap card deck 
void ThemeManager::updateCardTheme(QString cards, QString deck)
{
  updateCardTheme(mThemeFile, cards, deck);
}
void ThemeManager::updateCardTheme(QString themefile, QString cards, QString deck)
{
  kDebug() << "ThemeManager Pixmap cards: "<< endl;
  kDebug() << "  Cards = " << cards << endl;
  kDebug() << "  Deck  = " << deck << endl;

  KConfig cardInfo( cards+"/index.desktop", KConfig::OnlyLocal);
  KConfigGroup group(&cardInfo, "KDE Backdeck");
  QPointF cardSize   = group.readEntry("BackSize", QPointF(1.0,1.0));
  QString svg        = group.readEntry("SVG", QString());
  kDebug() << "cardSize = " << cardSize << endl;

  mSVGCards = !svg.isNull();
  kDebug() << "SVG = " << svg << " is " << mSVGCards << endl;
  mCardFile = cards;
  mDeckFile = deck;

  QString svgfile = cards+"/"+svg;

  mCardRenderer = 0;
  if (mSVGCards)
  {
    mCardRenderer = new KSvgRenderer(this);
    bool result   = mCardRenderer->load(svgfile);
    if (!result) 
    {
      kFatal() << "Cannot open file " << svgfile << endl;
    }
    kDebug() << "Rendering " << svgfile << endl;
  }

  updateTheme(themefile);
}


// Update the theme file and refresh all registered objects. Used 
// to really change the theme.
void ThemeManager::updateTheme(QString themefile)
{
  // Empty cache
  mPixmapCache.clear();
  mThemeFile = themefile;

  // Process dirs
  QString rcfile = KStandardDirs::locate("data", themefile);
  kDebug() << "ThemeManager LOAD with theme "<<rcfile << endl;

  // Read config and SVG file for theme
  mConfig = new KConfig(rcfile, KConfig::NoGlobals);
  QString svgfile = config("general").readEntry("svgfile");
  svgfile = KStandardDirs::locate("data", svgfile);
  kDebug() << "Reading SVG master file  = " << svgfile << endl;

  mAspectRatio     =  config("general").readEntry("aspect-ratio", 1.0);
  mCardAspectRatio =  config("general").readEntry("card-aspect-ratio", 1.0);
  kDebug() << "Aspect ration = " << mAspectRatio << " Cards aspect=" << mCardAspectRatio<< endl;



  mRenderer = new KSvgRenderer(this);
  bool result = mRenderer->load(svgfile);
  if (!result) 
  {
    kFatal() << "Cannot open file " << svgfile << endl;
  }

  // Notify all theme objects of a change
  QHashIterator<Themable*, int> it(mObjects);
  while (it.hasNext())
  {
      it.next();
      Themable* ob = it.key();
      ob->changeTheme();
  }
}


// Rescale the theme. Call all registed objects so that they can refresh.
void ThemeManager::rescale(int scale)
{
  if (scale==mScale) return;
  mScale = scale;
  kDebug() << "Rescale to " << scale<<endl;

  QHashIterator<Themable*, int> it(mObjects);
  while (it.hasNext())
  {
      it.next();
      Themable* ob = it.key();
      ob->changeTheme();
  }
}


// Retreive the theme's scale
double ThemeManager::getScale()
{
  return (double)mScale;
}  


// Retreive the current theme configuration file.
KConfigGroup ThemeManager::config(QString id)
{
   KConfigGroup grp = mConfig->group(id); 
   return grp;
}


QString ThemeManager::calcCardSVGId(int no)
{
    const static char *ids[] = { "back",
                               "1_club", "1_spade", "1_heart", "1_diamond",
                               "king_club", "king_spade", "king_heart", "king_diamond",
                               "queen_club", "queen_spade", "queen_heart", "queen_diamond",
                               "jack_club", "jack_spade", "jack_heart", "jack_diamond",
                               "10_club", "10_spade", "10_heart", "10_diamond",
                               "9_club", "9_spade", "9_heart", "9_diamond",
                               "8_club", "8_spade", "8_heart", "8_diamond",
                               "7_club", "7_spade", "7_heart", "7_diamond",
                               "6_club", "6_spade", "6_heart", "6_diamond",
                               "5_club", "5_spade", "5_heart", "5_diamond",
                               "4_club", "4_spade", "4_heart", "4_diamond",
                               "3_club", "3_spade", "3_heart", "3_diamond",
                               "2_club", "2_spade", "2_heart", "2_diamond",
                               0 };
  return QString(ids[no]);
}

const QPixmap ThemeManager::getCard(int suite, int cardtype, double width)
{
  // Card no
  int no = 4*cardtype+suite+1;

  QPixmap pm;
  // SVG cards
  if (mSVGCards)
  {
    QString svgid = calcCardSVGId(no);
    QSize size = QSize(int(width), int(width/mCardAspectRatio) );
    // kDebug() << "Card " << Deck::name((Suite)suite, (CardType)cardtype) << " => "<< svgid << "S="<<size<< endl;
    pm = getPixmap(mCardRenderer, svgid, size);
  }
  // Pixmap cards
  else
  {
    QString dir = mCardFile;
    QString file = QString("%1.png").arg(no);
    if (!pm.load(dir+"/"+file))
    {
      kError() << "Cannot load card file " << dir+file << endl;
    }
  }

  // Scale to card aspect ratio if severly wrong
  double aspect = double(pm.width())/double(pm.height());
  if (aspect/mCardAspectRatio >1.05 || aspect/mCardAspectRatio < 0.95)
  {
    //kWarning() << "Wrong card aspect ratio " << aspect << " vs " << mCardAspectRatio << endl;
    return pm.scaled(int(width), int(width/mCardAspectRatio), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }
  else
  {
    return pm.scaledToWidth(int(width), Qt::SmoothTransformation);
  }
}

const QPixmap ThemeManager::getCardback(double width)
{
  QString file = mDeckFile;
  QPixmap pm;
  if (!pm.load(file))
  {
    kError() << "Cannot load deck file " << file << endl;
  }

  // Scale to card aspect ratio if severly wrong
  double aspect = double(pm.width())/double(pm.height());
  if (aspect/mCardAspectRatio >1.05 || aspect/mCardAspectRatio < 0.95)
  {
    //kWarning() << "Wrong deck aspect ratio " << aspect << " vs " << mCardAspectRatio << endl;
    return pm.scaled(int(width), int(width/mCardAspectRatio), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }
  else
  {
    return pm.scaledToWidth(int(width), Qt::SmoothTransformation);
  }
}


// Get a pixmap when its size is given (this can distort the image)
const QPixmap ThemeManager::getPixmap(KSvgRenderer* renderer, QString svgid, QSize size)
{
  if (size.width() < 1 || size.height() < 1) 
    kFatal() << "ThemeManager::getPixmap Cannot create svgid ID " << svgid << " with zero size " << size << endl;
  
  QPixmap pixmap;

  //  Cached pixmap?
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
  pixmap = QPixmap::fromImage(image);
  if (pixmap.isNull())
    kFatal() << "ThemeManager::getPixmap Cannot load svgid ID " << svgid << endl;

  // Cache image
  mPixmapCache[svgid] = pixmap;

  return pixmap;
}

// Get a pixmap when its size is given (this can distort the image)
const QPixmap ThemeManager::getPixmap(QString svgid, QSize size)
{
  return getPixmap(mRenderer, svgid, size);
}

// Get a pixmap when only width is given (this keeps the aspect ratio)
const QPixmap ThemeManager::getPixmap(QString svgid, double width)
{
  QRectF rect   = mRenderer->boundsOnElement(svgid);
  double factor = width/rect.width();
  QSize size    = QSize(int(width),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}


// Get a pixmap with original properties and a scale factor given with respect to
// another SVG item.
const QPixmap ThemeManager::getPixmap(QString svgid, QString svgref, double refwidth)
{
  QRectF refrect    = mRenderer->boundsOnElement(svgref);
  QRectF rect       = mRenderer->boundsOnElement(svgid);
  double factor     = refwidth/refrect.width();
  QSize size        = QSize(int(rect.width()*factor),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}


// ========================== Themable interface ===============================

// Constructs a themable interface
Themable::Themable()
{
  mScale        = 1.0;
  mThemeManager = 0;
}


// Constructs a themeable interface given its id and the master theme manager. 
// This automatically registeres the object with the manager.
Themable::Themable(QString id, ThemeManager* thememanager)
{
  mScale        = 1.0;
  mId           = id;
  mThemeManager = thememanager;
  if (!thememanager) return;
  thememanager->registerTheme(this);
}


// Destructs the themeable object
Themable::~Themable()
{
  if (mThemeManager) mThemeManager->unregisterTheme(this);
}



