/*
    This file is part of the KDE games library

    Copyright 2008 Andreas Pakulat <apaku@gmx.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "cardcache.h"
#include "cardcache_p.h"

#include <QMap>
#include <QPixmap>
#include <QPair>
#include <QImage>
#include <QMutexLocker>
#include <QPainter>
#include <QDateTime>
#include <QSizeF>
#include <QFileInfo>
#include <QDir>

#include <qsvgrenderer.h>
#include <kpixmapcache.h>
#include <kconfig.h>
#include <kstandarddirs.h>

#include "carddeckinfo.h"

#include <kdebug.h>

#define DECKLIST_LENGTH 53

KCardInfo fullDeckList[DECKLIST_LENGTH] = {
    KCardInfo( KCardInfo::Club, KCardInfo::Ace ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Ace ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Ace ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Ace ),
    KCardInfo( KCardInfo::Club, KCardInfo::King ),
    KCardInfo( KCardInfo::Heart, KCardInfo::King ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::King ),
    KCardInfo( KCardInfo::Spade, KCardInfo::King ),
    KCardInfo( KCardInfo::Club, KCardInfo::Queen ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Queen ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Queen ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Queen ),
    KCardInfo( KCardInfo::Club, KCardInfo::Jack ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Jack ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Jack ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Jack ),
    KCardInfo( KCardInfo::Club, KCardInfo::Ten ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Ten ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Ten ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Ten ),
    KCardInfo( KCardInfo::Club, KCardInfo::Nine ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Nine ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Nine ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Nine ),
    KCardInfo( KCardInfo::Club, KCardInfo::Eight ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Eight ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Eight ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Eight ),
    KCardInfo( KCardInfo::Club, KCardInfo::Seven ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Seven ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Seven ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Seven ),
    KCardInfo( KCardInfo::Club, KCardInfo::Six ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Six ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Six ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Six ),
    KCardInfo( KCardInfo::Club, KCardInfo::Five ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Five ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Five ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Five ),
    KCardInfo( KCardInfo::Club, KCardInfo::Four ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Four ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Four ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Four ),
    KCardInfo( KCardInfo::Club, KCardInfo::Three ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Three ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Three ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Three ),
    KCardInfo( KCardInfo::Club, KCardInfo::Two ),
    KCardInfo( KCardInfo::Heart, KCardInfo::Two ),
    KCardInfo( KCardInfo::Diamond, KCardInfo::Two ),
    KCardInfo( KCardInfo::Spade, KCardInfo::Two ),
    KCardInfo( KCardInfo::None, KCardInfo::Joker )
};

KCardInfo::KCardInfo( KCardInfo::Suit s, KCardInfo::Card c )
    : m_suit( s ), m_card( c )
{
}

KCardInfo::Card KCardInfo::card() const
{
    return m_card;
}

KCardInfo::Suit KCardInfo::suit() const
{
    return m_suit;
}

void KCardInfo::setCard( KCardInfo::Card c )
{
    m_card = c;
}

void KCardInfo::setSuit( KCardInfo::Suit s )
{
    m_suit = s;
}

bool KCardInfo::operator==( const KCardInfo& c ) const
{
    return ( c.card() == card() && c.suit() == suit() );
}

QString KCardInfo::svgName() const
{
    QString s;
    if( card() == KCardInfo::Ace )
        s += QLatin1String( "1_" );
    if( card() == KCardInfo::King )
        s += QLatin1String( "king_" );
    if( card() == KCardInfo::Queen )
        s += QLatin1String( "queen_" );
    if( card() == KCardInfo::Jack )
        s += QLatin1String( "jack_" );
    if( card() == KCardInfo::Ten )
        s += QLatin1String( "10_" );
    if( card() == KCardInfo::Nine )
        s += QLatin1String( "9_" );
    if( card() == KCardInfo::Eight )
        s += QLatin1String( "8_" );
    if( card() == KCardInfo::Seven )
        s += QLatin1String( "7_" );
    if( card() == KCardInfo::Six )
        s += QLatin1String( "6_" );
    if( card() == KCardInfo::Five )
        s += QLatin1String( "5_" );
    if( card() == KCardInfo::Four )
        s += QLatin1String( "4_" );
    if( card() == KCardInfo::Three )
        s += QLatin1String( "3_" );
    if( card() == KCardInfo::Two )
        s += QLatin1String( "2_" );
    if( suit() == KCardInfo::Club )
        s += QLatin1String( "club" );
    if( suit() == KCardInfo::Spade )
        s += QLatin1String( "spade" );
    if( suit() == KCardInfo::Diamond )
        s += QLatin1String( "diamond" );
    if( suit() == KCardInfo::Heart )
        s += QLatin1String( "heart" );
    return s;
}

QPixmap doRender( const QString& element, QSvgRenderer* r, const QSize& s )
{
    QPixmap pix = QPixmap( s );
    pix.fill( Qt::transparent );
    QPainter p( &pix );
    r->render( &p, element );
    p.end();
    return pix;
}

QString keyForPixmap( const QString& theme, const QString& element, const QSize& s )
{
    return theme + QLatin1Char( '_' ) + element + QLatin1Char( '_' )
                  + QString::number( s.width() ) + QLatin1Char( '_' )
                  + QString::number( s.height() );
}

QSvgRenderer* KCardCachePrivate::frontRenderer()
{
    if ( !frontSvgRenderer )
    {
        kDebug() << "Loading front SVG renderer";
        frontSvgRenderer = new QSvgRenderer( CardDeckInfo::frontSVGFilePath( frontTheme ) );
    }
    return frontSvgRenderer;
}

QSvgRenderer* KCardCachePrivate::backRenderer()
{
    if ( !backSvgRenderer )
    {
        kDebug() << "Loading back SVG renderer";
        backSvgRenderer = new QSvgRenderer( CardDeckInfo::backSVGFilePath( backTheme ) );
    }
    return backSvgRenderer;
}

void KCardCachePrivate::ensureNonNullPixmap( QPixmap& pix )
{
    if( pix.isNull() )
    {
        kWarning() << "Couldn't produce a non-null pixmap, creating a red cross";
        pix = QPixmap( size );
        QPainter p(&pix);
        p.fillRect( QRect( 0,0, pix.width(), pix.height() ), QBrush( Qt::white ) );
        QPen pen = p.pen();
        pen.setWidth( 4 );
        pen.setColor( QColor( Qt::red ) );
        p.setPen( pen );
        p.drawLine( QPoint( 2,2 ), QPoint( pix.width()-2, pix.height()-2 ) );
        p.drawLine( QPoint( pix.width()-2,2 ), QPoint( 2, pix.height()-2 ) );
        p.end();
    }
}

QPixmap KCardCachePrivate::renderFrontSvg( const QString& element )
{
    kDebug() << "Renderering" << element << "in main thread.";
    QMutexLocker l( frontRendererMutex );
    return doRender( element, frontRenderer(), size );
}

QPixmap KCardCachePrivate::renderBackSvg( const QString& element )
{
    QMutexLocker l( backRendererMutex );
    return doRender( element, backRenderer(), size );
}

void KCardCachePrivate::submitRendering( const QString& key, const QImage& image )
{
    kDebug() << "Received render of" << key << "from rendering thread.";
    QPixmap pix = QPixmap::fromImage( image );
    QMutexLocker l( frontcacheMutex );
    frontcache->insert( key, pix );
}

LoadThread::LoadThread( KCardCachePrivate* d_ )
    : d( d_ ), doKill( false ), killMutex( new QMutex )
{
}

void LoadThread::setSize( const QSize & s )
{
    size = s;
}

void LoadThread::setFrontTheme( const QString & frontTheme_ )
{
    frontTheme = frontTheme_;
}

void LoadThread::setBackTheme( const QString & backTheme_ )
{
    backTheme = backTheme_;
}

void LoadThread::setElementsToLoad( const QStringList& elements_ )
{
    elementsToRender = elements_;
}

void LoadThread::kill()
{
    QMutexLocker l(killMutex);
    doKill = true;
}

void LoadThread::run()
{
    {
        // Load the renderer even if we don't have any elements to render.
        QMutexLocker l( d->frontRendererMutex );
        d->frontRenderer();
    }

    foreach( const QString& element, elementsToRender )
    {
        {
            QMutexLocker l( killMutex );
            if( doKill )
                return;
        }
        QImage img = QImage( size, QImage::Format_ARGB32 );
        img.fill( Qt::transparent );
        QPainter p( &img );
        {
            QMutexLocker l( d->frontRendererMutex );
            d->frontRenderer()->render( &p, element );
        }
        p.end();

        QString key = keyForPixmap( frontTheme, element, size );
        emit renderingDone( key, img );
    }
}

KCardCache::KCardCache()
    : d( new KCardCachePrivate )
{
    d->frontcache = 0;
    d->backcache = 0;
    d->frontcacheMutex = new QMutex();
    d->backcacheMutex = new QMutex();
    d->frontRendererMutex = new QMutex();
    d->backRendererMutex = new QMutex();
    d->frontSvgRenderer = 0;
    d->backSvgRenderer = 0;
    d->loadThread = 0;
}

KCardCache::~KCardCache()
{
    if( d->loadThread && d->loadThread->isRunning() )
    {
        d->loadThread->kill();
    }
    delete d->loadThread;
    delete d->frontcache;
    delete d->backcache;
    delete d->frontcacheMutex;
    delete d->backcacheMutex;
    delete d->frontRendererMutex;
    delete d->backRendererMutex;
    delete d->frontSvgRenderer;
    delete d->backSvgRenderer;
    delete d;
}

QPixmap KCardCache::backside( int variant ) const
{
    QPixmap pix;
    if( d->backTheme.isEmpty() || d->size.isEmpty() )
        return pix;
    QString element = QLatin1String( "back" );
    if( variant > 0 )
    {
        element += QString::number(variant);
    }
    QString key = keyForPixmap( d->backTheme, element, d->size );

    {
        QMutexLocker l( d->backcacheMutex );
        if( d->backcache && ( !d->backcache->find( key, pix ) || pix.isNull() ) )
        {
            pix = d->renderBackSvg( element );
            d->backcache->insert( key, pix );
        }
    }
    // Make sure we never return an invalid pixmap
    d->ensureNonNullPixmap( pix );
    return pix;
}

QPixmap KCardCache::frontside( const KCardInfo& info ) const
{
    QPixmap pix;
    if( d->frontTheme.isEmpty() || d->size.isEmpty() )
        return pix;
    QString key = keyForPixmap( d->frontTheme, info.svgName() , d->size );

    {
        QMutexLocker l( d->frontcacheMutex );
        if( d->frontcache && ( !d->frontcache->find( key, pix ) || pix.isNull() ) )
        {
            pix = d->renderFrontSvg( info.svgName() );
            d->frontcache->insert( key, pix );
        }
    }
    // Make sure we never return an invalid pixmap
    d->ensureNonNullPixmap( pix );
    return pix;
}

void KCardCache::setSize( const QSize& s )
{
    if( s != d->size )
        d->size = s;
}

QSize KCardCache::size() const
{
    return d->size;
}

void KCardCache::setFrontTheme( const QString& theme )
{
    {
        QMutexLocker l( d->frontcacheMutex );
        delete d->frontcache;
        d->frontcache = new KPixmapCache( QString::fromLatin1(  "kdegames-cards_%1" ).arg( theme ) );
        d->frontcache->setUseQPixmapCache( true );
        QDateTime dt = QFileInfo( CardDeckInfo::frontSVGFilePath( theme ) ).lastModified();
        if( d->frontcache->timestamp() < dt.toTime_t() )
        {
            d->frontcache->discard();
            d->frontcache->setTimestamp( dt.toTime_t() );
        }
    }
    {
        QMutexLocker l( d->frontRendererMutex );
        delete d->frontSvgRenderer;
        d->frontSvgRenderer = 0;
    }
    d->frontTheme = theme;
}

QString KCardCache::frontTheme() const
{
    return d->frontTheme;
}

void KCardCache::setBackTheme( const QString& theme )
{
    {
        QMutexLocker l( d->backcacheMutex );
        delete d->backcache;
        d->backcache = new KPixmapCache( QString::fromLatin1(  "kdegames-cards_%1" ).arg( theme ) );
        d->backcache->setUseQPixmapCache( true );
        QDateTime dt = QFileInfo( CardDeckInfo::backSVGFilePath( theme ) ).lastModified();
        if( d->backcache->timestamp() < dt.toTime_t() )
        {
            d->backcache->discard();
            d->backcache->setTimestamp( dt.toTime_t() );
        }
    }
    {
        QMutexLocker l( d->backRendererMutex );
        delete d->backSvgRenderer;
        d->backSvgRenderer = 0;
    }
    d->backTheme = theme;
}

QString KCardCache::backTheme() const
{
    return d->backTheme;
}

void KCardCache::loadTheme( LoadInfos infos )
{
    if( d->loadThread && d->loadThread->isRunning() )
    {
        d->loadThread->kill();
        d->loadThread->wait();
    }
    delete d->loadThread;

    // We have to compile the list of elements to load here, because we can't
    // check the contents of the KPixmapCache from outside the GUI thread.
    QStringList elements;
    QPixmap pix;
    if( infos & KCardCache::LoadFrontSide )
    {
        int numCards;
        if( infos & KCardCache::Load53Cards )
            numCards = 53;
        else if( infos & KCardCache::Load52Cards )
            numCards = 52;
        else
            numCards = 32;

        for( int i = 0; i < numCards ; i++ )
        {
            QString element = fullDeckList[i].svgName();
            QString key = keyForPixmap( d->frontTheme, element, d->size );
            {
                QMutexLocker l( d->frontcacheMutex );
                if( d->frontcache && !d->frontcache->find( key, pix ) )
                    elements << element;
            }
        }
    }

    d->loadThread = new LoadThread( d );
    d->loadThread->setBackTheme( d->backTheme );
    d->loadThread->setFrontTheme( d->frontTheme );
    d->loadThread->setSize( d->size );
    d->loadThread->setElementsToLoad( elements );
    d->connect( d->loadThread, SIGNAL(renderingDone(QString,QImage)), SLOT(submitRendering(QString,QImage)), Qt::QueuedConnection );
    d->loadThread->start( QThread::IdlePriority );
}

QSizeF KCardCache::defaultFrontSize( const KCardInfo& info ) const
{
    if( d->frontTheme.isEmpty() )
        return QSizeF();

    QPixmap pix;
    QString key = d->frontTheme + QLatin1Char( '_' ) + info.svgName() + QLatin1String( "_default" );
    {
        QMutexLocker( d->frontcacheMutex );
        if ( d->frontcache && d->frontcache->find( key, pix ) )
            return pix.size();
    }

    {
        QMutexLocker( d->frontRendererMutex );
        QSizeF size = d->frontRenderer()->boundsOnElement( info.svgName() ).size();
        pix = QPixmap( size.toSize() );
    }

    {
        QMutexLocker( d->frontcacheMutex );
        if( d->frontcache )
            d->frontcache->insert( key, pix );
    }

    return pix.size();
}

QSizeF KCardCache::defaultBackSize( int variant ) const
{
    if( d->backTheme.isEmpty() )
        return QSizeF();

    QString element = QLatin1String( "back" );
    if( variant > 0 )
    {
        element += QString::number(variant);
    }

    QPixmap pix;
    QString key = d->backTheme + QLatin1Char( '_' ) + element + QLatin1String( "_default" );
    {
        QMutexLocker( d->backcacheMutex );
        if ( d->backcache && d->backcache->find( key, pix ) )
            return pix.size();
    }

    {
        QMutexLocker( d->backRendererMutex );
        QSizeF size = d->backRenderer()->boundsOnElement( element ).size();
        pix = QPixmap( size.toSize() );
    }

    {
        QMutexLocker( d->backcacheMutex );
        if( d->backcache )
            d->frontcache->insert( key, pix );
    }

    return pix.size();
}

void KCardCache::invalidateFrontside()
{
    QMutexLocker l( d->frontcacheMutex );
    if( d->frontcache )
        d->frontcache->discard();
}

void KCardCache::invalidateBackside()
{
    QMutexLocker l( d->backcacheMutex );
    if( d->backcache )
        d->backcache->discard();
}

#include "cardcache_p.moc"
