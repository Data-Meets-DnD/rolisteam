/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
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

#include "data/person.h"

#include <QUuid>

#include "cleveruri.h"
#include "network/networkmessagewriter.h"
#include "resourcesnode.h"

/**********
 * Person *
 **********/
Person::Person() : ResourcesNode(ResourcesNode::Person) {}

Person::Person(const QString& name, const QColor& color, const QString& uuid)
    : ResourcesNode(ResourcesNode::Person, uuid), m_color(color)
{
    m_name= name;
}

Person::~Person()= default;

QColor Person::getColor() const
{
    return m_color;
}

Person* Person::parentPerson() const
{
    return m_parentPerson;
}

void Person::setParentPerson(Person* parent)
{
    m_parentPerson= parent;
}

bool Person::setColor(const QColor& color)
{
    if(color == m_color)
        return false;

    m_color= color;
    emit colorChanged();
    return true;
}

const QImage& Person::getAvatar() const
{
    return m_avatar;
}
void Person::setAvatar(const QImage& p)
{
    if(p == m_avatar)
        return;

    m_avatar= p;
    emit avatarChanged();
}
bool Person::hasAvatar() const
{
    return !m_avatar.isNull();
}
Qt::CheckState Person::checkedState()
{
    return m_checkState;
}

bool Person::isLeaf() const
{
    return true;
}
void Person::setState(Qt::CheckState c)
{
    m_checkState= c;
}
ResourcesNode::TypeResource Person::type() const
{
    return ResourcesNode::Person;
}
QVariant Person::getData(ResourcesNode::DataValue data) const
{
    QVariant var;
    switch(data)
    {
    case NAME:
        var= m_name;
        break;
    case MODE:
        var= QVariant::fromValue(Core::LoadingMode::Internal);
        break;
    case DISPLAYED:
        var= true;
        break;
    case URI:
        break;
    }
    return var;
}

void Person::write(QDataStream&, bool, bool) const
{
    // default implement does nothing [virtual]
}
void Person::read(QDataStream&)
{
    // default implement does nothing [virtual]
}

QIcon Person::icon() const
{
    return QIcon(QPixmap::fromImage(m_avatar));
}
void Person::setAvatarPath(const QString& avatarPath)
{
    if(m_avatarPath == avatarPath)
        return;

    m_avatarPath= avatarPath;
    m_avatar= QImage(avatarPath);
    emit avatarPathChanged();
    emit avatarChanged();
}
QString Person::avatarPath() const
{
    return m_avatarPath;
}
