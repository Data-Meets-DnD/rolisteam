/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "sessionitemmodel.h"
#include "data/cleveruri.h"
#include "data/cleverurimimedata.h"

#include <QDebug>
#include <QFileInfo>
#include <QIcon>
#include <QUrl>

//////////////////////////////////////
/// SessionItemModel
/////////////////////////////////////

SessionItemModel::SessionItemModel() : m_rootItem(new Chapter())
{
    m_header << tr("Name") << tr("Loading Mode") << tr("Displayed") << tr("Path");

    m_rootItem->setParentNode(nullptr);

    connect(m_rootItem.get(), &Chapter::openResource, this, &SessionItemModel::openResource);
}

SessionItemModel::~SessionItemModel() {}
QModelIndex SessionItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();

    ResourcesNode* parentItem= nullptr;

    if(!parent.isValid())
        parentItem= m_rootItem.get();
    else
        parentItem= static_cast<ResourcesNode*>(parent.internalPointer());

    ResourcesNode* childItem= parentItem->getChildAt(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
bool SessionItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    if(Qt::EditRole == role)
    {
        ResourcesNode* childItem= static_cast<ResourcesNode*>(index.internalPointer());
        childItem->setName(value.toString());
        return true;
    }
    return false;
}
Qt::ItemFlags SessionItemModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags | Qt::ItemIsDropEnabled;

    ResourcesNode* childItem= static_cast<ResourcesNode*>(index.internalPointer());
    if(!childItem->isLeaf())
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}
QStringList SessionItemModel::mimeTypes() const
{
    QStringList types;
    types << "rolisteam/cleverurilist"
          << "text/uri-list";
    return types;
}

QMimeData* SessionItemModel::mimeData(const QModelIndexList& indexes) const
{
    CleverUriMimeData* mimeData= new CleverUriMimeData();

    for(const QModelIndex& index : indexes)
    {
        if((index.isValid()) && (index.column() == 0))
        {
            ResourcesNode* item= static_cast<ResourcesNode*>(index.internalPointer());
            mimeData->addResourceNode(item, index);
        }
    }
    return mimeData;
}

/*void SessionItemModel::updateNode(ResourcesNode* node)
{
    QModelIndex nodeIndex;
    QModelIndex parent;
    QList<ResourcesNode*> path;
    if(m_rootItem->seekNode(path, node))
    {
        ResourcesNode* parentItem= nullptr;
        for(auto& i : path)
        {
            if(nullptr != parentItem)
            {
                nodeIndex= index(parentItem->indexOf(i), 0, parent);
                parent= nodeIndex;
            }
            parentItem= i;
        }
    }
    emit dataChanged(nodeIndex, nodeIndex);
}*/

Qt::DropActions SessionItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
bool SessionItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                    const QModelIndex& parent)
{
    Q_UNUSED(column);

    if(action == Qt::IgnoreAction)
        return false;

    bool added= false;

    if(data->hasFormat("rolisteam/cleverurilist"))
    {
        const CleverUriMimeData* mediaData= qobject_cast<const CleverUriMimeData*>(data);

        if(nullptr != mediaData)
        {
            QList<ResourcesNode*> mediaList= mediaData->getList().values();
            QList<QModelIndex> indexList= mediaData->getList().keys();
            {
                if(action == Qt::MoveAction)
                {
                    added= moveMediaItem(mediaList, parent, row, indexList);
                }
            }
        }
    }
    else if((data->hasUrls()) && (!added))
    {
        QList<QUrl> list= data->urls();
        for(QUrl& url : list)
        {
            if(url.isLocalFile())
            {
                QFileInfo fileInfo(url.toLocalFile());
                if(fileInfo.isFile())
                {
                }
                // addChildMediaTo(medium,parent);
            }
            // else if(url.isLocalFile())
        }
    }
    return added;
}

bool SessionItemModel::moveMediaItem(QList<ResourcesNode*> items, const QModelIndex& parentToBe, int row,
                                     QList<QModelIndex>& formerPosition)
{
    ResourcesNode* parentItem= static_cast<ResourcesNode*>(parentToBe.internalPointer());

    if(nullptr == parentItem)
    {
        parentItem= m_rootItem.get();
    }
    int orignRow= row;

    QList<int> listRow;

    if((!items.isEmpty()) && (!formerPosition.isEmpty()))
    {
        ResourcesNode* item= items.at(0);
        ResourcesNode* parent= item->parentNode();
        QModelIndex formerPositionIndex= formerPosition.at(0);
        QModelIndex sourceParent= formerPositionIndex.parent();
        QModelIndex destinationParent= parentToBe;

        int sourceFirst= parent->indexOf(item);
        int sourceLast= parent->indexOf(item) + items.size() - 1;

        int destinationRow= orignRow < 0 ? parentItem->childrenCount() : orignRow;
        if((sourceParent == destinationParent)
           && ((destinationRow == parentItem->childrenCount()) || (destinationRow > sourceFirst)))
        {
            destinationRow-= items.size() - 1;
        }
        if((sourceParent == destinationParent) && (sourceFirst == destinationRow))
        {
            destinationRow-= items.size();
            return false;
        }

        if(!beginMoveRows(sourceParent, sourceFirst, sourceLast, destinationParent, destinationRow))
            return false;
    }

    for(int i= items.size() - 1; i >= 0; --i)
    {
        while(listRow.contains(row))
        {
            ++row;
        }

        ResourcesNode* item= items.at(i);
        ResourcesNode* parent= item->parentNode();
        QModelIndex formerPositionIndex= formerPosition.at(i);

        if(nullptr != parent)
        {
            parent->removeChild(item);
            if((orignRow == -1 && parentItem == m_rootItem.get()))
            {
                orignRow= parentItem->childrenCount();
                row= orignRow;
            }
            else if(formerPositionIndex.row() < orignRow && parentToBe == formerPositionIndex.parent())
            {
                orignRow-= 1;
                row= orignRow;
            }

            parentItem->insertChildAt(orignRow, item); // row
            //---
            // WARNING test move item and remove this code
            int oldRow= formerPositionIndex.row();
            if(oldRow > orignRow && parentItem == m_rootItem.get() && parent == m_rootItem.get())
            {
                oldRow+= items.size() - 1 - i;
            }
            listRow.append(row);
        }
    }

    endMoveRows();
    return true;
}
QModelIndex SessionItemModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    ResourcesNode* childItem= static_cast<ResourcesNode*>(index.internalPointer());
    ResourcesNode* parentItem= childItem->parentNode();

    if(parentItem == m_rootItem.get())
        return QModelIndex();

    return createIndex(parentItem->rowInParent(), 0, parentItem);
}
int SessionItemModel::rowCount(const QModelIndex& index) const
{
    if(index.isValid())
    {
        ResourcesNode* tmp= static_cast<ResourcesNode*>(index.internalPointer());
        return tmp->childrenCount();
    }
    else
    {
        return m_rootItem->childrenCount();
    }
}
int SessionItemModel::columnCount(const QModelIndex&) const
{
    return m_header.size();
}
QVariant SessionItemModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    ResourcesNode* tmp= static_cast<ResourcesNode*>(index.internalPointer());
    if(!tmp)
        return {};

    QVariant var;
    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        var= tmp->getData(static_cast<ResourcesNode::DataValue>(index.column()));
        break;
    case Qt::DecorationRole:
        if(index.column() == Name)
            var= tmp->icon();
        break;
    case Qt::BackgroundRole:
    {
        if(tmp->type() != ResourcesNode::Cleveruri)
            break;
        auto const& cleverUri= dynamic_cast<CleverURI*>(tmp);
        if(!cleverUri)
            break;
        if(cleverUri->hasData() || cleverUri->exists() || cleverUri->contentType() == Core::ContentType::WEBVIEW)
            break;

        var= QColor(Qt::red).lighter();
    }
    break;
    }
    return var;
}

void SessionItemModel::addResource(ResourcesNode* node, const QModelIndex& parent)
{
    if(!node)
        return;

    if(m_rootItem->contains(node->uuid()))
        return;

    Chapter* parentItem= nullptr;
    auto parentbis= parent;
    if(!parent.isValid())
    {
        parentItem= m_rootItem.get();
    }
    else
    {
        ResourcesNode* node= static_cast<ResourcesNode*>(parent.internalPointer());
        if(node->isLeaf())
        {
            node= node->parentNode(); // leaf's parent is not a leaf indeed
            parentbis= parentbis.parent();
        }
        parentItem= dynamic_cast<Chapter*>(node); // nullptr when it is not a chapter.
    }

    if(nullptr == parentItem)
        return;

    beginInsertRows(parentbis, parentItem->childrenCount(), parentItem->childrenCount());
    parentItem->addResource(node);
    endInsertRows();
}

void SessionItemModel::remove(QModelIndex& index)
{
    if(!index.isValid())
        return;
    ResourcesNode* indexItem= static_cast<ResourcesNode*>(index.internalPointer());
    QModelIndex parent= index.parent();
    ResourcesNode* parentItem= nullptr;

    if(!parent.isValid())
        parentItem= m_rootItem.get();
    else
        parentItem= static_cast<ResourcesNode*>(parent.internalPointer());

    beginRemoveRows(index.parent(), index.row(), index.row());
    parentItem->removeChild(indexItem);
    endRemoveRows();
}

void SessionItemModel::removeNode(ResourcesNode* node)
{
    auto idx= createIndex(node->rowInParent(), 0, node);
    remove(idx);
    /*QModelIndex parent;
    QList<ResourcesNode*> path;
    if(m_rootItem->seekNode(path, node))
    {
        int row= -1;
        ResourcesNode* parentItem= nullptr;
        ResourcesNode* tmpItem= nullptr;

        for(int i= 0; i < path.size(); ++i)
        {
            ResourcesNode* current= path.at(i);
            if(nullptr != tmpItem)
            {
                row= tmpItem->indexOf(current);
                if(i + 1 < path.size())
                {
                    parent= index(row, 0, parent); // recursive parenting
                }
                parentItem= tmpItem;
            }
            tmpItem= current;
        }

        beginRemoveRows(parent, row, row);
        parentItem->removeChild(node);
        endRemoveRows();
    }*/
}

void SessionItemModel::removeMedia(const QString& id)
{
    auto item= m_rootItem->findNode(id);
    // auto idx= createIndex(item->rowInParent(), 0, item);

    removeNode(item);
}

void SessionItemModel::addMedia(const QString& id, const QString& path, Core::ContentType type, const QString& name)
{
    auto uri= new CleverURI(name, path, type);
    uri->setUuid(id);
    addResource(uri, QModelIndex());
}

void SessionItemModel::clearData()
{
    if(!m_rootItem)
        return;

    m_rootItem->clear();
}

QVariant SessionItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var;
    if(orientation == Qt::Horizontal)
    {
        switch(role)
        {
        case Qt::DisplayRole:
            var= m_header.at(section);
            break;
        case Qt::TextAlignmentRole:
            var= Qt::AlignHCenter;
            break;
        default:
            break;
        }
    }
    return var;
}
void SessionItemModel::saveModel(QDataStream& out) const
{
    m_rootItem->write(out);
}

void SessionItemModel::loadModel(QDataStream& in)
{
    beginResetModel();
    QString str;
    in >> str;
    m_rootItem->read(in);
    endResetModel();
}
