/***************************************************************************
                          networkdlg.cpp  -  description
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


#include "networkdlg.h"
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qwidgetstack.h>

extern const char* LSKAT_SERVICE;

// Create the dialog 
NetworkDlg::NetworkDlg( QWidget *parent, const char *name )
    : NetworkDlgBase( parent, name, TRUE )
{
  browser = new DNSSD::ServiceBrowser(QString::fromLatin1(LSKAT_SERVICE));
  connect(browser,SIGNAL(finished()),SLOT(gamesFound()));
  browser->startBrowse();
}

NetworkDlg::~NetworkDlg()
{
  delete browser;
}

void NetworkDlg::SetHost(const QString& host)
{
  hostname->setText(host);
}

void NetworkDlg::SetName(const QString& name)
{
  serverName->setText(name);
}
QString NetworkDlg::QueryName() const 
{
  return serverName->text();
}

void NetworkDlg::SetPort(unsigned short port)
{
  this->port->setValue(port);
}

void NetworkDlg::gamesFound()
{
  bool autoselect=false;
  if (!clientName->count() && group->selectedId()==1) autoselect=true;
  clientName->clear();
  QStringList names;
  QValueList<DNSSD::RemoteService::Ptr>::ConstIterator itEnd = browser->services().end();
  for (QValueList<DNSSD::RemoteService::Ptr>::ConstIterator it = browser->services().begin();
    it!=itEnd; ++it) names << (*it)->serviceName();
  clientName->insertStringList(names);
  if (autoselect && clientName->count()) gameSelected(0);
}

void NetworkDlg::gameSelected(int nr)
{
 if (nr>=browser->services().count() || nr<0) return;
 DNSSD::RemoteService::Ptr srv = browser->services()[nr];
 if (!srv->isResolved() && !srv->resolve()) return;
 hostname->setText(srv->hostName());
 port->setValue(srv->port());
}

unsigned short NetworkDlg::QueryPort() const
{
  return port->value();
}

QString NetworkDlg::QueryHost() const
{
  return hostname->text();
}

void NetworkDlg::toggleServerClient()
{
  stack->raiseWidget(group->selectedId());
  if (group->selectedId()==1) {
    gameSelected(clientName->currentItem());
    hostname->setEnabled(true);
    }
    else {
      hostname->setText(QString::null);
      hostname->setEnabled(false);
    }
}

#include "networkdlg.moc"
