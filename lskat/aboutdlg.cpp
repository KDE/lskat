/****************************************************************************
** Form implementation generated from reading ui file 'aboutDlg.ui'
**
** Created: Thu Nov 23 11:42:20 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "aboutdlg.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <klocale.h>


/* 
 *  Constructs a aboutDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
aboutDlg::aboutDlg( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name ) setName( "aboutDlg" );
    resize( 315, 318 ); 
    setCaption( i18n( "About Lieutnant sKat"  ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    Frame5 = new QFrame( this, "Frame5" );
    Frame5->setFrameShadow( QFrame::Sunken );
    Frame5->setFrameShape( QFrame::StyledPanel );
    Frame5->setBackgroundColor(Qt::white);
    Frame5->setLineWidth( 3 );
    Frame5->setMidLineWidth( 2 );
    vbox_2 = new QVBoxLayout( Frame5 ); 
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

  MovieLabel= new QMovieLabel(Frame5,"Movei1");
  MovieLabel->setMinimumSize( QSize( 160, 60 ) );
  MovieLabel->setMaximumSize( QSize( 160, 60 ) );
  MovieLabel->setEMovie(0);
  hbox->addWidget( MovieLabel );

  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  hbox->addItem( spacer );

    TextLabel3 = new QLabel( Frame5, "TextLabel3" );
    TextLabel3->setText( i18n( "Lieutnant\nSkat\n\nVersion 0.91"  ) );
    TextLabel3->setAlignment( int( QLabel::AlignCenter ) );
    TextLabel3->setBackgroundColor(Qt::white);
    hbox->addWidget( TextLabel3 );
    vbox_2->addLayout( hbox );

    TextLabel4 = new QLabel( Frame5, "TextLabel4" );
    TextLabel4->setText( i18n( "(c) 2000 Martin Heni\nEmail: martin@heni-online.de\n\nGamefeatures:\n  - Multiplayer network game\n  - Up to two Computerplayer\n\n\n Many thanks to Laura for betatesting!"  ) );
    TextLabel4->setBackgroundColor(Qt::white);
    vbox_2->addWidget( TextLabel4 );
    vbox->addWidget( Frame5 );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_2->addItem( spacer_2 );

    PushButton15 = new QPushButton( this, "PushButton15" );
    PushButton15->setText( i18n( "OK"  ) );
    hbox_2->addWidget( PushButton15 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_2->addItem( spacer_3 );
    vbox->addLayout( hbox_2 );

    // signals and slots connections
    connect( PushButton15, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
aboutDlg::~aboutDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void aboutDlg::SetMovie(EMovie *movie)
{
  MovieLabel->setEMovie(movie);
}

#include "aboutdlg.moc"
