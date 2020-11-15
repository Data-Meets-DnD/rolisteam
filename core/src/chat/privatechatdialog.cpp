/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
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

#include "privatechatdialog.h"

#include <QFormLayout>
#include <QListView>
#include <QVBoxLayout>

#include "chat/chat.h"
#include "data/person.h"
#include "data/player.h"
#include "userlist/playermodel.h"

/**************************
 * PrivateChatDialogModel *
 **************************/

PrivateChatDialogModel::PrivateChatDialogModel(QObject* parent) : QSortFilterProxyModel(parent), m_isEditable(false)
{
    // setSourceModel(PlayerModel::instance());
}

Qt::ItemFlags PrivateChatDialogModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    if(!m_isEditable)
        return Qt::ItemIsEnabled;

    // PlayerModel* playersList= PlayerModel::instance();

    // Player* player= playersList->getPlayer(mapToSource(index));

    // We should return Qt::NoItemFlags when (player == nullptr),
    // but this cause an infinite loop when the last entry is deleted.
    // This is a workaround of a Qt's bug.
    /*  if(player == nullptr || player == playersList->getLocalPlayer())
          return Qt::ItemIsEnabled;

      if(player->hasFeature("MultiChat"))
          return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;*/

    return Qt::NoItemFlags;
}

QVariant PrivateChatDialogModel::data(const QModelIndex& index, int role) const
{
    if(role == Qt::CheckStateRole)
    {
        auto idx= mapToSource(index);
        auto player= static_cast<Player*>(idx.internalPointer());
        return QVariant(m_set.contains(player));
    }

    return QAbstractProxyModel::data(index, role);
}

bool PrivateChatDialogModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!m_isEditable)
        return false;

    if(role == Qt::CheckStateRole && (index.flags() & Qt::ItemIsUserCheckable))
    {
        auto idx= mapToSource(index);
        auto player= static_cast<Player*>(idx.internalPointer());
        if(!m_set.remove(player))
            m_set.insert(player);
        emit dataChanged(index, index);
        return true;
    }

    return QAbstractProxyModel::setData(index, value, role);
}

QSet<Player*>& PrivateChatDialogModel::playersSet()
{
    return m_set;
}

void PrivateChatDialogModel::setPlayersSet(const QSet<Player*>& set)
{
    m_set= set;
    // auto playerList= PlayerModel::instance();
    // m_set.insert(playerList->getLocalPlayer());
    emit dataChanged(createIndex(0, 0), createIndex(m_set.size() - 1, 0));
}

void PrivateChatDialogModel::setEditable(bool isEditable)
{
    m_isEditable= isEditable;
}

bool PrivateChatDialogModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index= sourceModel()->index(sourceRow, 0, sourceParent);
    return !index.parent().isValid();
}

/*********************
 * PrivateChatDialog *
 *********************/

PrivateChatDialog::PrivateChatDialog(QWidget* parent) : QDialog(parent)
{
    m_name_w= new QLineEdit;

    m_owner_w= new QLineEdit;
    m_owner_w->setReadOnly(true);

    QListView* listView= new QListView;
    listView->setModel(&m_model);

    QFormLayout* formLayout= new QFormLayout;
    formLayout->addRow(tr("&Name : "), m_name_w);
    formLayout->addRow(tr("&Owner : "), m_owner_w);
    formLayout->addRow(tr("&Player : "), listView);

    m_buttonBox= new QDialogButtonBox;
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout* mainLayout= new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);

    setSizeGripEnabled(true);
}

QSize PrivateChatDialog::sizeHint() const
{
    return minimumSizeHint() * 1.4;
}

int PrivateChatDialog::edit(PrivateChat* chat)
{
    if(chat == nullptr)
    {
        return QDialog::Rejected;
    }
    if(nullptr == chat->owner())
    {
        return QDialog::Rejected;
    }

    bool isEditable= chat->belongsToLocalPlayer();
    m_name_w->setText(chat->name());
    m_name_w->setReadOnly(!isEditable);
    m_owner_w->setText(chat->owner()->name());
    m_model.setPlayersSet(chat->players());
    m_model.setEditable(isEditable);

    if(isEditable)
        m_buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    else
        m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel);

    int ret= exec();
    if(ret == QDialog::Accepted)
        chat->set(m_name_w->text(), m_model.playersSet());

    return ret;
}
