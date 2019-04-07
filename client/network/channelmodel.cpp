#include "channelmodel.h"

#include "tcpclient.h"
#include "treeitem.h"

#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QSettings>

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QFont>
#include <QInputDialog>
#include <QStyle>
#endif

#include "channel.h"
#include "receiveevent.h"

quint64 computeTotalSize(const std::map<Channel*, quint64>& map)
{
    quint64 totalSize= 0;
    for(auto pair : map)
    {
        totalSize+= pair.second;
    }
    return totalSize;
}

//////////////////////////////////////
/// ClientMimeData
/////////////////////////////////////

ClientMimeData::ClientMimeData()
{
    setData("application/rolisteam.networkclient.list", QByteArray());
}

void ClientMimeData::addClient(TcpClient* m, const QModelIndex index)
{
    if(nullptr != m)
    {
        m_clientList.insert(index, m);
    }
}
const QMap<QModelIndex, TcpClient*>& ClientMimeData::getList() const
{
    return m_clientList;
}
bool ClientMimeData::hasFormat(const QString& mimeType) const
{
    return ((mimeType == "application/rolisteam.networkclient.list") | QMimeData::hasFormat(mimeType));
}

//////////////////////////////////////
/// ChannelModel
/////////////////////////////////////

ChannelModel::ChannelModel()
{
    // m_root = new Channel();
    ReceiveEvent::registerNetworkReceiver(NetMsg::AdministrationCategory, this);
}

ChannelModel::~ChannelModel()
{
    ReceiveEvent::removeNetworkReceiver(NetMsg::AdministrationCategory, this);
    qDeleteAll(m_root);
    std::vector<Channel*> keys;

    transform(std::begin(m_sizeMap), std::end(m_sizeMap), back_inserter(keys),
        [](decltype(m_sizeMap)::value_type const& pair) { return pair.first; });
    m_sizeMap.clear();
    qDeleteAll(keys);
}

QModelIndex ChannelModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();
    if(column < 0)
        return QModelIndex();

    TreeItem* childItem= nullptr;
    if(!parent.isValid())
    {
        if(m_root.size() > row)
            childItem= m_root.at(row);
    }
    else
    {
        TreeItem* parentItem= static_cast<TreeItem*>(parent.internalPointer());
        childItem= parentItem->getChildAt(row);
    }

    if(childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
        return QModelIndex();
}

std::pair<quint64, QString> ChannelModel::convert(quint64 size) const
{
    constexpr int cap= 1024;
    std::size_t i= 0;
    std::array<QString, 3> units= {tr("Bytes"), tr("KiB"), tr("MiB")};
    while(size > cap && i < units.size())
    {
        size/= cap;
        ++i;
    }
    return std::make_pair(size, units[i]);
}

QVariant ChannelModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TreeItem* tmp= static_cast<TreeItem*>(index.internalPointer());
    if(tmp != nullptr)
    {
        if((role == Qt::DisplayRole) || (Qt::EditRole == role))
        {
            if(!tmp->isLeaf() && role == Qt::DisplayRole)
            {
                auto channel= dynamic_cast<Channel*>(tmp);
                if(isAdmin(m_localPlayerId) || isGM(m_localPlayerId, channel->getId()))
                {
                    auto size= channel->memorySize();
                    auto pair= convert(size);
                    return QStringLiteral("%1 (%2 %3)").arg(tmp->getName()).arg(pair.first).arg(pair.second);
                }
            }
            return tmp->getName();
        }
#ifdef QT_WIDGETS_LIB
        else if(role == Qt::FontRole)
        {
            if(tmp->isLeaf())
            {
                auto parent= tmp->getParentItem();
                if(parent)
                {
                    auto channel= dynamic_cast<Channel*>(parent);
                    if(channel->isCurrentGm(tmp))
                    {
                        QFont font;
                        font.setBold(true);
                        return font;
                    }
                }
            }
        }
        else if(role == Qt::DecorationRole)
        {
            if(!tmp->isLeaf())
            {
                auto channel= dynamic_cast<Channel*>(tmp);
                QStyle* style= qApp->style();
                if(nullptr != style)
                {
                    if(channel->password().isEmpty())
                        return style->standardIcon(QStyle::SP_DirIcon);
                    else
                        return style->standardIcon(QStyle::SP_DirClosedIcon);
                }
            }
        }
#endif
    }
    return QVariant();
}

bool ChannelModel::setData(const QModelIndex& index, const QVariant& value, int)
{
    bool rightToSetName= isAdmin(m_localPlayerId);
    if((!rightToSetName && !localIsGM()) || !index.isValid())
        return false;

    TreeItem* tmp= static_cast<TreeItem*>(index.internalPointer());
    if(tmp != nullptr)
    {
        tmp->setName(value.toString());
        return true;
    }
    return false;
}

QModelIndex ChannelModel::parent(const QModelIndex& child) const
{
    if(!child.isValid())
        return QModelIndex();

    TreeItem* childItem= static_cast<TreeItem*>(child.internalPointer());
    if(nullptr != childItem)
    {
        TreeItem* parentItem= childItem->getParentItem();

        if(m_root.contains(childItem))
        {
            return QModelIndex();
        }
        if(m_root.contains(parentItem))
        {
            return createIndex(m_root.indexOf(parentItem), 0, parentItem);
        }
        return createIndex(parentItem->rowInParent(), 0, parentItem);
    }
    return QModelIndex();
}

int ChannelModel::rowCount(const QModelIndex& parent) const
{
    int result= 0;
    if(!parent.isValid())
    {
        result= m_root.size();
    }
    else
    {
        TreeItem* item= static_cast<TreeItem*>(parent.internalPointer());
        if(nullptr != item)
        {
            result= item->childCount();
        }
    }
    return result;
}

int ChannelModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QString ChannelModel::addChannel(QString name, QByteArray password)
{
    Channel* chan= new Channel(name);
    chan->setPassword(password);
    QModelIndex index;
    addChannelToIndex(chan, index);
    return chan->getId();
}
QModelIndex ChannelModel::addChannelToIndex(Channel* channel, QModelIndex& parent)
{
    int row= -1;
    if(!parent.isValid())
    {
        beginInsertRows(parent, m_root.size(), m_root.size());
        m_root.append(channel);
        endInsertRows();
        row= m_root.size() - 1;
    }
    else
    {
        Channel* item= static_cast<Channel*>(parent.internalPointer());
        if(nullptr != item)
        {
            beginInsertRows(parent, item->childCount(), item->childCount());
            item->addChild(channel);
            endInsertRows();
            row= item->childCount() - 1;
        }
    }
    return index(row, 0, parent);
}
bool ChannelModel::addChannelToChannel(Channel* child, Channel* parent)
{
    bool result= false;
    if(nullptr == parent)
    {
        beginInsertRows(QModelIndex(), m_root.size(), m_root.size());
        m_root.append(child);
        endInsertRows();
        result= true;
    }
    else
    {
        QModelIndex index= channelToIndex(parent);
        beginInsertRows(index, parent->childCount(), parent->childCount());
        parent->addChild(child);
        endInsertRows();
        result= true;
    }
    return result;
}
QModelIndex ChannelModel::channelToIndex(Channel* channel)
{
    QList<TreeItem*> listOfParent;
    TreeItem* tmp= channel;
    while(nullptr != tmp)
    {
        listOfParent.prepend(tmp);
        tmp= tmp->getParentItem();
    }
    QModelIndex parent;
    for(auto item : listOfParent)
    {
        if(nullptr == item->getParentItem())
        {
            parent= parent.child(m_root.indexOf(item), 0);
        }
        else
        {
            parent= parent.child(item->rowInParent(), 0);
        }
    }
    return parent;
}

void ChannelModel::setLocalPlayerId(const QString& id)
{
    m_localPlayerId= id;
}

NetWorkReceiver::SendType ChannelModel::processMessage(NetworkMessageReader* msg)
{
    if(NetMsg::AddChannel == msg->action())
    {
        QString idParent= msg->string8();
        auto parent= getItemById(idParent);
        if(nullptr == parent)
        {
            Channel* channel= new Channel();
            channel->read(*msg);

            auto item= getItemById(idParent);
            Channel* parent= static_cast<Channel*>(item);

            addChannelToChannel(channel, parent);
        }
        return NetWorkReceiver::NONE;
    }
    else if(NetMsg::AdminPassword == msg->action())
    {
        return NetWorkReceiver::ALL;
    }
    return NetWorkReceiver::NONE;
}
Qt::ItemFlags ChannelModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    TreeItem* item= static_cast<TreeItem*>(index.internalPointer());

    auto admin= isAdmin(m_localPlayerId);
    if(admin && item->isLeaf())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
    }
    else if(admin && !item->isLeaf())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}
bool ChannelModel::hasChildren(const QModelIndex& parent) const
{
    if(!parent.isValid()) // root
    {
        return !m_root.isEmpty(); //==0?false:true;
    }
    else
    {
        TreeItem* childItem= static_cast<TreeItem*>(parent.internalPointer());
        if(childItem->childCount() == 0)
            return false;
        else
            return true;
    }
}
QStringList ChannelModel::mimeTypes() const
{
    QStringList types;
    types << "application/rolisteam.networkclient.list";
    return types;
}
Qt::DropActions ChannelModel::supportedDropActions() const
{
    return Qt::MoveAction;
}
QMimeData* ChannelModel::mimeData(const QModelIndexList& indexes) const
{
    ClientMimeData* mimeData= new ClientMimeData();

    for(const QModelIndex& index : indexes)
    {
        if((index.isValid()) && (index.column() == 0))
        {
            TcpClient* item= static_cast<TcpClient*>(index.internalPointer());
            mimeData->addClient(item, index);
        }
    }
    return mimeData;
}

bool ChannelModel::moveMediaItem(
    QList<TcpClient*> items, const QModelIndex& parentToBe, int row, QList<QModelIndex>& formerPosition)
{
    Q_UNUSED(row)
    Q_UNUSED(formerPosition)
    if(isAdmin(m_localPlayerId))
    {
        if(parentToBe.isValid())
        {
            Channel* item= static_cast<Channel*>(parentToBe.internalPointer());
            QString id= item->getId();

            QByteArray pw;
#ifdef QT_WIDGETS_LIB
            if(!item->password().isEmpty())
            {
                pw= QInputDialog::getText(nullptr, tr("Channel Password"),
                    tr("Channel %1 required password:").arg(item->getName()), QLineEdit::Password)
                        .toUtf8()
                        .toBase64();
            }
#endif

            for(auto client : items)
            {
                if(!id.isEmpty())
                {
                    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::JoinChannel);
                    msg.string8(id);
                    msg.string8(client->getId());
                    msg.byteArray32(pw);
                    msg.sendToServer();
                    return true;
                }
            }
        }
    }
    return false;
}
bool ChannelModel::dropMimeData(
    const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    Q_UNUSED(column);

    if(action == Qt::IgnoreAction)
        return false;

    bool added= false;

    if(data->hasFormat("application/rolisteam.networkclient.list"))
    {
        const ClientMimeData* clientData= qobject_cast<const ClientMimeData*>(data);

        if(nullptr != clientData)
        {
            QList<TcpClient*> clientList= clientData->getList().values();
            QList<QModelIndex> indexList= clientData->getList().keys();
            {
                if(action == Qt::MoveAction)
                {
                    added= moveMediaItem(clientList, parent, row, indexList);
                }
            }
        }
    }
    return added;
}
bool ChannelModel::addConnectionToDefaultChannel(TcpClient* client)
{
    if(m_defaultChannel.isEmpty())
    {
        if(!m_root.isEmpty())
        {
            auto item= m_root.at(0);
            if(nullptr != item)
            {
                m_defaultChannel= item->getId();
            }
        }
        else
        {
            return false;
        }
    }

    return addConnectionToChannel(m_defaultChannel, client);
}

bool ChannelModel::addConnectionToChannel(QString chanId, TcpClient* client)
{
    bool found= false;
    for(auto& item : m_root)
    {
        if(nullptr != item)
        {
            found= item->addChildInto(chanId, client);
        }
    }
    return found;
}
void ChannelModel::readDataJson(const QJsonObject& obj)
{
    beginResetModel();
    for(auto& item : m_root)
    {
        item->clear();
    }
    m_root.clear();
    QJsonArray channels= obj["channel"].toArray();
    for(auto channelJson : channels)
    {
        Channel* tmp= new Channel();
        QJsonObject obj= channelJson.toObject();
        tmp->setParentItem(nullptr);
        tmp->readFromJson(obj);
        m_root.append(tmp);
    }
    endResetModel();

    auto item= getPlayerById(m_localPlayerId);
    if(nullptr != item)
    {
        auto parent= dynamic_cast<Channel*>(item->getParentItem()); // channel
        if(nullptr != parent)
        {
            qDebug() << parent->getName() << "name of parent";
            emit localPlayerGMChanged(parent->getCurrentGmId());
        }
    }
}

void ChannelModel::writeDataJson(QJsonObject& obj)
{
    QJsonArray array;
    for(auto& item : m_root) // int i = 0; i< m_root->childCount(); ++i)
    {
        if(nullptr != item)
        {
            QJsonObject jsonObj;
            item->writeIntoJson(jsonObj);
            array.append(jsonObj);
        }
    }
    obj["channel"]= array;
}

void ChannelModel::readSettings()
{
    QSettings settings("Rolisteam", "roliserver");
    QJsonDocument doc= QJsonDocument::fromVariant(settings.value("channeldata", ""));
    QJsonObject obj= doc.object();
    readDataJson(obj);
}

void ChannelModel::writeSettings()
{
    QSettings settings("Rolisteam", "roliserver");
    QJsonDocument doc;
    QJsonObject obj;
    writeDataJson(obj);
    doc.setObject(obj);
    settings.setValue("channeldata", doc);
}

void ChannelModel::kick(const QString& id, bool isAdmin, const QString& senderId)
{
    for(auto& item : m_root)
    {
        if(nullptr != item)
        {
            item->kick(id, isAdmin, senderId);
        }
    }
}

bool ChannelModel::isAdmin(const QString& id) const
{
    auto player= getPlayerById(id);
    if(nullptr == player)
        return false;
    return player->isAdmin();
}

bool ChannelModel::isGM(const QString& id, const QString& chanId) const
{
    auto player= getPlayerById(id);
    auto item= getItemById(chanId);
    if(nullptr == player || item == nullptr)
        return false;

    auto chan= dynamic_cast<Channel*>(item);
    if(nullptr == chan)
        return false;

    return chan->currentGM() == player;
}
TreeItem* ChannelModel::getItemById(QString id)
{
    for(auto& item : m_root)
    {
        if(nullptr != item)
        {
            if(item->getId() == id)
            {
                return item;
            }
            else
            {
                TreeItem* child= item->getChildById(id);
                if(nullptr != child)
                {
                    return child;
                }
            }
        }
    }
    return nullptr;
}

TcpClient* ChannelModel::getPlayerById(QString id)
{
    for(auto& item : m_root)
    {
        if(nullptr == item)
            continue;

        auto client= dynamic_cast<TcpClient*>(item);
        if(client)
        {
            if(client->getPlayerId() == id)
            {
                return client;
            }
        }
        else
        {
            auto channel= dynamic_cast<Channel*>(item);
            if(nullptr != channel)
            {
                return channel->getPlayerById(id);
            }
        }
    }
    return nullptr;
}

void ChannelModel::removeChild(QString id)
{
    auto item= getItemById(id);
    if((nullptr != item)) //&&(!item->isLeaf())
    {
        auto parent= item->getParentItem();
        if(nullptr != parent)
        {
            Channel* channel= dynamic_cast<Channel*>(parent);
            if(nullptr != channel)
            {
                QModelIndex index= channelToIndex(channel);
                beginRemoveRows(index, channel->indexOf(item), channel->indexOf(item));
                channel->removeChild(item);
                endRemoveRows();
            }
        }
        else
        {
            QModelIndex index;
            beginRemoveRows(index, m_root.indexOf(item), m_root.indexOf(item));
            m_root.removeAll(item);
            endRemoveRows();
        }
    }
}
void ChannelModel::cleanUp()
{
    beginResetModel();
    qDeleteAll(m_root);
    m_root.clear();
    endResetModel();
}
void ChannelModel::setChannelMemorySize(Channel* chan, quint64 size)
{
    if(m_shield)
        return;
    m_sizeMap[chan]= size;

    emit totalSizeChanged(computeTotalSize(m_sizeMap));
}

void ChannelModel::emptyChannelMemory()
{
    m_shield= true;
    for(auto pair : m_sizeMap)
    {
        QMetaObject::invokeMethod(pair.first, "clearData", Qt::QueuedConnection);
        pair.second= 0;
    }
    m_shield= false;
}
