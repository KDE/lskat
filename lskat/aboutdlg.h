/****************************************************************************
** Form interface generated from reading ui file 'aboutDlg.ui'
**
** Created: Thu Nov 23 11:42:06 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <qdialog.h>
#include "movielabel.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class aboutDlg : public QDialog
{ 
    Q_OBJECT

public:
    aboutDlg( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~aboutDlg();
    void SetMovie(EMovie *movie);

    QFrame* Frame5;
    //QLabel* PixmapLabel1;
	  QMovieLabel *MovieLabel;
    QLabel* TextLabel3;
    QLabel* TextLabel4;
    QPushButton* PushButton15;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;

private: 
};

#endif // ABOUTDLG_H
