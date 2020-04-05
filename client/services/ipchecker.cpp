/*************************************************************************
 *   Copyright (C) 2013 by Renaud Guezennec                              *
 *                                                                       *
 *   https://rolisteam.org/                                           *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#include "ipchecker.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QUrl>

IpChecker::IpChecker(QObject* parent) : QObject(parent), m_manager(nullptr) {}
void IpChecker::readText(QNetworkReply* p)
{
    if(p->error() != QNetworkReply::NoError)
    {
        m_ip= tr("Error to read server IP.");
    }
    else
    {
        auto text= p->readAll();
        QRegularExpression ipRegex("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
        QRegularExpression ipV6Regex("^.*:.*:.*:.*:.*:.*$");
        auto match= ipRegex.match(text);
        auto matchV6= ipV6Regex.match(text);

        auto hasMatch= match.hasMatch() || matchV6.hasMatch();
        if(hasMatch)
        {
            m_ip= text;
            emit finished(m_ip);
        }
    }
}
void IpChecker::startCheck()
{
#ifdef HAVE_QT_NETWORK
    m_manager= new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(readText(QNetworkReply*)));
    m_manager->get(QNetworkRequest(QUrl("https://rolisteam.org/ip.php")));
#endif
}
