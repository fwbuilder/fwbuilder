/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

  $Id$

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef HTTPGET_H
#define HTTPGET_H

#include <QBuffer>
#include <QHttp>
#include <QString>
#include <QByteArray>
#include <QUrl>


class HttpGet : public QObject
{
    Q_OBJECT;

private:
    QHttp http;
    QUrl url;
    QBuffer strm;
    bool status;
    QString last_error;
    QByteArray contents;
    int request_id;
    
public:
    HttpGet(QObject *parent = 0);

    bool get(const QUrl &url);
    QString getLastError() { return last_error; }
    bool getStatus() { return status; }
    QString toString() { return QString(contents); }
    void abort();

signals:
    void done(const QString &res);

private slots:
    void httpDone(int id, bool error);
    void fileDone();
};

#endif
