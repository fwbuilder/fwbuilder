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

#include "config.h"
#include "global.h"

#include <QtCore>
#include <QtNetwork>
#include <QHttpResponseHeader>
#include <QLocale>

#include "HttpGet.h"
#include "FWBSettings.h"

using namespace std;

extern int getRegistrationStatus();

HttpGet::HttpGet(QObject *parent) : QObject(parent), strm(&contents)
{
    last_error = "";
    status = true;
    connect(&http, SIGNAL(requestFinished(int, bool)),
            this, SLOT(httpDone(int, bool)));
}

bool HttpGet::get(const QUrl &_url)
{
    url = _url;

    QTextStream err(&last_error,  QIODevice::WriteOnly);

    if (strm.isOpen())
    {
        contents.clear();
        strm.reset();
    }

    if (!url.isValid())
    {
        err << "Error: Invalid URL";
        status = false;
        return false;
    }

    if (url.scheme() != "http" && url.scheme() != "file")
    {
        err << "Error: URL must start with 'http:' or 'file:'";
        status = false;
        return false;
    }

    if (url.scheme() == "http" && url.path().isEmpty())
    {
        err << "Error: URL has no path";
        status = false;
        return false;
    }

    if (url.scheme() == "file")
    {
        QTimer::singleShot(0, this, SLOT(fileDone()));
        return true;
    }

    QString proxy = st->getCheckUpdatesProxy();
    if (!proxy.isEmpty())
    {
        QStringList parsed_proxy = proxy.split(':');
        QString proxy_host = parsed_proxy[0];
        QString proxy_port = "80";
        if (parsed_proxy.size()>1) proxy_port = parsed_proxy[1];
        if (proxy_port.isEmpty()) proxy_port = "80";
        http.setProxy(proxy_host, proxy_port.toInt());
    }

    http.setHost(url.host(), url.port(80));
    QHttpRequestHeader hdr(QLatin1String("GET"), url.toString());
    hdr.setValue("Host", url.host());

    QString locale = QLocale::system().name();//"en_US";//

    QString os;
#if defined(Q_WS_MAC)
    os = "MacOSX";
#else
  #if defined(Q_WS_WIN)
     os= "Windows";
  #else
     os = QString("%1; %2").arg(OS).arg(DISTRO);
  #endif
#endif

    QString agent = QString("fwbuilder/%1 (%2; %3; b:999999; s:%5; u)")
        .arg(VERSION).arg(os).arg(locale).arg(sig);

    hdr.setValue("User-Agent", agent);
    request_id = http.request(hdr, NULL, &strm);
    return true;
}

void HttpGet::fileDone()
{
    QString file_path = url.path();
    QFileInfo fi(file_path);
    if (fi.exists() && fi.isReadable())
    {
        QFile data(file_path);
        if (data.open(QFile::ReadOnly))
        {
            QTextStream strm(&data);
            QString line = strm.readAll();
            status = true;
            emit done(line);
            return;
        }
    }
    status = false;
    QTextStream err(&last_error,  QIODevice::WriteOnly);
    err << "Error: can not read file '" << file_path
        << "' (url=" << url.toString() << ")";
    emit done("");
}

void HttpGet::httpDone(int id, bool error)
{
    if (request_id == id)
    {
        status = true;
        last_error = "";
        QHttpResponseHeader resp = http.lastResponse();
        QTextStream err(&last_error,  QIODevice::WriteOnly);
        if (error)
        {
            err << "Error: " << qPrintable(http.errorString());
            status = false;
        }
        if (resp.isValid() && resp.statusCode()!=200)
        {
            err << "Error: " << resp.reasonPhrase();
            status = false;
        }
        emit done(toString());
    }
}

void HttpGet::abort()
{
    http.abort();
}
