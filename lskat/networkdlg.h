/***************************************************************************
                         networkdlg.h  -  description
                             -------------------
    copyright            : (C) 2004 by Jakub Stachowski
    email                : qbast@go2.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NETWORKDLG_H
#define NETWORKDLG_H
 
#include <qstring.h>
#include <dnssd/servicebrowser.h>
#include "networkdlgbase.h"


class NetworkDlg : public NetworkDlgBase
{
   Q_OBJECT
   
   public:
    NetworkDlg(QWidget* parent=NULL, const char* name=NULL);
    ~NetworkDlg();
    void SetName(const QString& name);
    void SetHost(const QString& host);
    void SetPort(unsigned short port);
    QString QueryName() const;
    unsigned short QueryPort() const;
    QString QueryHost() const;
   protected:
    virtual void toggleServerClient();
    virtual void gameSelected(int nr);
   private slots:
    void gamesFound();
   private:
    DNSSD::ServiceBrowser* browser;
};

#endif // NETWORKDLG_H

