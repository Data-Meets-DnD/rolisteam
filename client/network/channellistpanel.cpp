#include "channellistpanel.h"
#include "ui_channellistpanel.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>

#include "preferences/preferencesmanager.h"
#include "tcpclient.h"

ChannelListPanel::ChannelListPanel(QWidget* parent)
    : QWidget(parent), ui(new Ui::ChannelListPanel), m_model(new ChannelModel), m_currentGroup(VIEWER)
{
    ui->setupUi(this);
    ui->m_channelView->setModel(m_model);
    ui->m_channelView->setAlternatingRowColors(true);
    ui->m_channelView->setHeaderHidden(true);
    ui->m_channelView->setAcceptDrops(true);
    ui->m_channelView->setDragEnabled(true);
    ui->m_channelView->setDropIndicatorShown(true);
    ui->m_channelView->setDefaultDropAction(Qt::MoveAction);
    ui->m_channelView->setDragDropMode(QAbstractItemView::InternalMove);

    ui->m_channelView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->m_channelView, &QTreeView::customContextMenuRequested, this, &ChannelListPanel::showCustomMenu);
    connect(m_model, &ChannelModel::localPlayerGMChanged, this, &ChannelListPanel::CurrentChannelGmIdChanged);

    m_edit= new QAction(tr("Edit Channel"), this);
    m_lock= new QAction(tr("Lock Channel"), this);
    m_join= new QAction(tr("Switch to channel"), this);
    m_channelPassword= new QAction(tr("Set Channel Private"), this);
    m_addChannel= new QAction(tr("Add channel"), this);
    m_addSubchannel= new QAction(tr("Add subchannel"), this);
    m_deleteChannel= new QAction(tr("Delete Channel"), this);
    m_setDefault= new QAction(tr("Set Default"), this);
    m_admin= new QAction(tr("Log as admin"), this);
    m_kick= new QAction(tr("Kick User"), this);

    connect(m_kick, SIGNAL(triggered(bool)), this, SLOT(kickUser()));
    connect(m_edit, SIGNAL(triggered(bool)), this, SLOT(editChannel()));
    connect(m_addChannel, SIGNAL(triggered(bool)), this, SLOT(addChannelAsSibbling()));
    connect(m_addSubchannel, SIGNAL(triggered(bool)), this, SLOT(addChannel()));
    connect(m_deleteChannel, SIGNAL(triggered(bool)), this, SLOT(deleteChannel()));
    connect(m_lock, SIGNAL(triggered(bool)), this, SLOT(lockChannel()));
    connect(m_join, SIGNAL(triggered(bool)), this, SLOT(joinChannel()));
    connect(m_admin, SIGNAL(triggered(bool)), this, SLOT(logAsAdmin()));
    connect(m_channelPassword, &QAction::triggered, this, &ChannelListPanel::setPasswordOnChannel);
}

ChannelListPanel::~ChannelListPanel()
{
    delete ui;
}
void ChannelListPanel::processMessage(NetworkMessageReader* msg)
{
    switch(msg->action())
    {
    case NetMsg::Goodbye:
        break;
    case NetMsg::Kicked:
        break;
    case NetMsg::MoveChannel:
        break;
    case NetMsg::SetChannelList:
    {
        QByteArray data= msg->byteArray32();
        QJsonDocument doc= QJsonDocument::fromJson(data);
        if(!doc.isEmpty())
        {
            QJsonObject obj= doc.object();
            m_model->readDataJson(obj);
        }
    }
    break;
    case NetMsg::GMStatus:
    {
        auto isGM= msg->uint8();
        if(isGM && (m_currentGroups == VIEWER))
        {
            setCurrentGroups(m_currentGroups | GAMEMASTER);
        }
    }
    break;
    case NetMsg::AdminAuthFail:
        setCurrentGroups(VIEWER);
        break;
    case NetMsg::AdminAuthSucessed:
        setCurrentGroups(m_currentGroups | ADMIN);
        break;
    default:
        break;
    }
}

void ChannelListPanel::sendOffModel()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::SetChannelList);
    QJsonDocument doc;
    QJsonObject obj;
    m_model->writeDataJson(obj);
    doc.setObject(obj);

    msg.byteArray32(doc.toJson());
    msg.sendToServer();
}
void ChannelListPanel::showCustomMenu(QPoint pos)
{
    QMenu menu(this);
    enum ClickState
    {
        Out,
        OnChannel,
        OnUser
    };

    ClickState state= Out;
    menu.addAction(m_join);
    menu.addAction(m_kick);
    menu.addSeparator();
    menu.addAction(m_setDefault);
    menu.addAction(m_edit);
    menu.addAction(m_channelPassword);
    menu.addAction(m_lock);
    menu.addSeparator();
    menu.addAction(m_addChannel);
    menu.addAction(m_addSubchannel);
    menu.addAction(m_deleteChannel);
    menu.addAction(m_admin);

    m_index= ui->m_channelView->indexAt(pos);

    if(!m_index.isValid())
    {
        state= Out;
    }
    else
    {
        TreeItem* data= indexToPointer<TreeItem*>(m_index);
        if(data->isLeaf())
        {
            state= OnUser;
        }
        else
        {
            state= OnChannel;
        }
    }
    if(state == Out)
    {
        m_addChannel->setEnabled(true);
        m_edit->setEnabled(false);
        m_lock->setEnabled(false);
        m_deleteChannel->setEnabled(false);
        m_addSubchannel->setEnabled(false);
        m_kick->setEnabled(false);
        m_setDefault->setEnabled(false);
        m_join->setEnabled(false);
    }
    else if(state == OnChannel)
    {
        m_kick->setEnabled(false);
        m_setDefault->setEnabled(true);
        m_edit->setEnabled(true);
        m_lock->setEnabled(true);
        m_addChannel->setEnabled(true);
        m_deleteChannel->setEnabled(true);
        m_addSubchannel->setEnabled(true);
        m_channelPassword->setEnabled(true);
        m_join->setEnabled(true);
    }
    else if(state == OnUser)
    {
        m_kick->setEnabled(true);
        m_edit->setEnabled(false);
        m_lock->setEnabled(false);
        m_deleteChannel->setEnabled(false);
        m_addSubchannel->setEnabled(false);
        m_channelPassword->setEnabled(false);
        m_setDefault->setEnabled(false);
        m_join->setEnabled(false);
        m_addChannel->setEnabled(false);
    }

    if(ChannelListPanel::VIEWER == m_currentGroup)
    {
        m_kick->setEnabled(false);
        m_edit->setEnabled(false);
        m_lock->setEnabled(false);
        m_deleteChannel->setEnabled(false);
        m_addSubchannel->setEnabled(false);
        m_admin->setEnabled(true);
    }
    else if(ChannelListPanel::ADMIN == m_currentGroup)
    {
        m_admin->setEnabled(false);
    }
    menu.exec(ui->m_channelView->mapToGlobal(pos));
}
bool ChannelListPanel::isAdmin()
{
    return (ChannelListPanel::ADMIN & m_currentGroups);
}

bool ChannelListPanel::isGM()
{
    return (ChannelListPanel::GAMEMASTER & m_currentGroups);
}

void ChannelListPanel::kickUser()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            TcpClient* item= static_cast<TcpClient*>(m_index.internalPointer());
            QString id= item->getId();
            QString idPlayer= item->getPlayerId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::Kicked);
                msg.string8(id);
                msg.string8(idPlayer);
                msg.sendToServer();
            }
        }
    }
}
void ChannelListPanel::lockChannel()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            Channel* item= static_cast<Channel*>(m_index.internalPointer());
            QString id= item->getId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::LockChannel);
                msg.string8(id);
                msg.sendToServer();
            }
        }
    }
}
template <typename T>
T ChannelListPanel::indexToPointer(QModelIndex index)
{
    T item= static_cast<T>(index.internalPointer());
    return item;
}

void ChannelListPanel::banUser()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            TcpClient* item
                = indexToPointer<TcpClient*>(m_index); /// static_cast<TcpClient*>(m_index.internalPointer());
            QString id= item->getId();
            QString idPlayer= item->getPlayerId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::BanUser);
                msg.string8(id);
                msg.string8(idPlayer);
                msg.sendToServer();
            }
        }
    }
}

void ChannelListPanel::logAsAdmin()
{
    PreferencesManager* preferences= PreferencesManager::getInstance();

    QString pwadmin= preferences->value(QString("adminPassword_for_%1").arg(m_serverName), QString()).toString();

    if(pwadmin.isEmpty())
    {
        pwadmin= QInputDialog::getText(this, tr("Admin Password"), tr("Password"), QLineEdit::Password);
    }
    auto pwA= QCryptographicHash::hash(pwadmin.toUtf8(), QCryptographicHash::Sha3_512);
    sendOffLoginAdmin(pwA);
}

void ChannelListPanel::sendOffLoginAdmin(QByteArray str)
{
    if(!str.isEmpty())
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AdminPassword);
        // msg.string8(id);
        msg.byteArray32(str);
        msg.sendToServer();
    }
}
void ChannelListPanel::addChannel()
{
    if(isAdmin())
    {
        Channel* newChannel= new Channel(tr("New Channel"));

        Channel* parent= indexToPointer<Channel*>(m_index);

        QModelIndex justAdded= m_model->addChannelToIndex(newChannel, m_index);
        ui->m_channelView->edit(justAdded);

        if(nullptr != parent)
        {
            QString parentId= parent->getId();
            if(!parentId.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AddChannel);
                msg.string8(parentId);
                newChannel->fill(msg);
                msg.sendToServer();
            }
        }
    }
}
void ChannelListPanel::addChannelAsSibbling()
{
    if(isAdmin())
    {
        Channel* newChannel= new Channel(tr("New Channel"));

        auto parentIndex= m_index.parent();

        Channel* parent= nullptr;
        QString parentId("");

        if(parentIndex.isValid())
        {
            parent= indexToPointer<Channel*>(parentIndex);
            parentId= parent->getId();
        }

        QModelIndex justAdded= m_model->addChannelToIndex(newChannel, parentIndex);
        ui->m_channelView->edit(justAdded);

        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AddChannel);
        msg.string8(parentId);
        newChannel->fill(msg);
        msg.sendToServer();
    }
}

void ChannelListPanel::editChannel()
{
    // Todo display dialog to edit properties of QML
    if(isAdmin())
    {
        ui->m_channelView->edit(m_index);
    }
}

QString ChannelListPanel::serverName() const
{
    return m_serverName;
}

void ChannelListPanel::setServerName(const QString& serverName)
{
    m_serverName= serverName;
}

void ChannelListPanel::setLocalPlayerId(const QString& id)
{
    m_localPlayerId= id;
    m_model->setLocalPlayerId(id);
}
void ChannelListPanel::deleteChannel()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            Channel* item= static_cast<Channel*>(m_index.internalPointer());
            QString id= item->getId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::DeleteChannel);
                msg.string8(id);
                msg.sendToServer();
            }
        }
    }
}
#include <QCryptographicHash>

void ChannelListPanel::setPasswordOnChannel()
{
    if(!isAdmin() && !m_index.isValid())
        return;

    Channel* item= static_cast<Channel*>(m_index.internalPointer());
    auto pw= QInputDialog::getText(
        this, tr("Channel Password"), tr("Password for channel: %1").arg(item->getName()), QLineEdit::Password);

    if(pw.isEmpty())
        return;

    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ChannelPassword);
    msg.string8(item->getId());
    auto pwA= QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha3_512);
    msg.byteArray32(pwA);
    msg.sendToServer();
}

void ChannelListPanel::joinChannel()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            Channel* item= static_cast<Channel*>(m_index.internalPointer());

            QByteArray pw;
            if(!item->password().isEmpty())
                pw= QInputDialog::getText(this, tr("Channel Password"),
                    tr("Channel %1 required password:").arg(item->getName()), QLineEdit::Password)
                        .toUtf8();

            QString id= item->getId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::JoinChannel);
                msg.string8(id);
                msg.string8(m_localPlayerId);
                auto pwA= QCryptographicHash::hash(pw, QCryptographicHash::Sha3_512);
                msg.byteArray32(pwA);
                msg.sendToServer();
            }
        }
    }
}

ChannelListPanel::Groups ChannelListPanel::currentGroups() const
{
    return m_currentGroups;
}

void ChannelListPanel::setCurrentGroups(const Groups& currentGroups)
{
    m_currentGroups= currentGroups;
}
void ChannelListPanel::cleanUp()
{
    if(nullptr != m_model)
        m_model->cleanUp();
}
