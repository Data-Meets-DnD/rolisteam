/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef MEDIATYPE_H
#define MEDIATYPE_H

#include <QHash>
#include <QObject>

namespace Core
{
Q_NAMESPACE
enum PermissionMode
{
    GM_ONLY,
    PC_MOVE,
    PC_ALL
};
Q_ENUM_NS(PermissionMode)

/*    TOKEN,
    SONG,
    SONGLIST,*/
enum class ContentType : int
{
    VECTORIALMAP,
    PICTURE,
    ONLINEPICTURE,
    NOTES,
    CHARACTERSHEET,
    SHAREDNOTE,
    PDF,
    WEBVIEW,
    INSTANTMESSAGING,
    MINDMAP,
    UNKNOWN
};
Q_ENUM_NS(ContentType)

enum class SharingPermission : quint8
{
    None,
    ReadOnly,
    ReadWrite
};

inline uint qHash(Core::ContentType type, uint seed)
{
    return ::qHash(static_cast<uint>(type), seed);
}

enum class LoadingMode
{
    Internal,
    Linked
};
Q_ENUM_NS(LoadingMode)

enum State
{
    Unloaded,
    Hidden,
    Displayed
};
Q_ENUM_NS(State)

enum VisibilityMode
{
    HIDDEN,
    FOGOFWAR,
    ALL
};
Q_ENUM_NS(VisibilityMode)

enum class GridPattern : quint8
{
    NONE,
    SQUARE,
    HEXAGON,
    OCTOGON
};
Q_ENUM_NS(GridPattern)

enum ScaleUnit
{
    M,
    KM,
    CM,
    MILE,
    YARD,
    INCH,
    FEET,
    PX
};
Q_ENUM_NS(ScaleUnit)

enum CharacterScope
{
    SelectionOnly,
    AllCharacter,
    AllNPC
};

enum class EditionMode : int
{
    Painting,
    Mask,
    Unmask
};

enum SelectableTool
{
    PEN,
    LINE,
    EMPTYRECT,
    FILLRECT,
    EMPTYELLIPSE,
    FILLEDELLIPSE,
    TEXT,
    HANDLER,
    NonPlayableCharacter,
    PlayableCharacter,
    IMAGE,
    RULE,
    PATH,
    ANCHOR,
    TEXTBORDER,
    PIPETTE,
    BUCKET,
    HIGHLIGHTER
};
Q_ENUM_NS(SelectableTool)

enum Properties
{
    ShowNpcName,
    ShowPcName,
    ShowNpcNumber,
    ShowHealthStatus,
    ShowInitScore,
    ShowGrid,
    LocalIsGM,
    GridPatternProperty,
    GridColor,
    GridSize,
    Scale,
    Unit,
    EnableCharacterVision,
    PermissionModeProperty,
    FogOfWarStatus,
    CollisionStatus,
    GridAbove,
    HideOtherLayers,
    VisibilityModeProperty,
    ShowHealthBar,
    MapLayer
};
Q_ENUM_NS(Properties)

enum class Layer : quint8
{
    GROUND,
    OBJECT,
    CHARACTER_LAYER,
    FOG,
    GRIDLAYER,
    NONE
};
Q_ENUM_NS(Layer)

} // namespace Core

#endif // MEDIATYPE_H
