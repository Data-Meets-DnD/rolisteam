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
#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <QHash>
#include <QPixmap>
#include <QString>
#include <QVariant>

namespace vmap
{
class RectController;
class TextController;
class LineController;
class PathController;
class ImageController;
class CharacterItemController;
class EllipseController;
} // namespace vmap

class DiceAliasModel;
class DiceAlias;
class CharacterStateModel;
class CharacterState;
class AbstractMediaController;
class ImageController;
class NetworkMessageWriter;
class NetworkMessageReader;
class PlayerModel;
class CleverURI;
class WebpageController;
class PdfController;
class VectorialMapController;
class MessageHelper
{
public:
    MessageHelper();

    static void sendOffGoodBye();
    static void closeMedia(const QString& id);

    static void sendOffAllDiceAlias(DiceAliasModel* model);
    static void sendOffOneDiceAlias(DiceAlias* da, int row);

    static void sendOffAllCharacterState(CharacterStateModel* model);
    static void sendOffOneCharacterState(CharacterState* da, int row);

    static QString readPlayerId(NetworkMessageReader& msg);
    static void sendOffOpenMedia(AbstractMediaController* ctrl);

    // vmap
    static void readVectorialMapData(NetworkMessageReader* msg, VectorialMapController* map);
    static void sendOffVMap(VectorialMapController* ctrl);

    static void sendOffRect(const vmap::RectController* ctrl, const QString& mapId);
    static const std::map<QString, QVariant> readRect(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readEllipse(NetworkMessageReader* msg);

    static void sendOffText(const vmap::TextController* ctrl, const QString& mapId);
    static void sendOffLine(const vmap::LineController* ctrl, const QString& mapId);
    static void sendOffEllispe(const vmap::EllipseController* ctrl, const QString& mapId);
    static void sendOffPath(const vmap::PathController* ctrl, const QString& mapId);
    static void sendOffImage(const vmap::ImageController* ctrl, const QString& mapId);
    static void sendOffCharacter(const vmap::CharacterItemController* ctrl, const QString& mapId);

    // image
    static void sendOffImage(ImageController* ctrl);
    static CleverURI* readImageData(NetworkMessageReader* msg, QPixmap& pix);

    // PDF
    static void sendOffPdfFile(PdfController* ctrl);
    static QHash<QString, QVariant> readPdfData(NetworkMessageReader* msg);

    // webpage
    static void shareWebpage(WebpageController* ctrl);
    static void updateWebpage(WebpageController* ctrl);
    static QHash<QString, QVariant> readWebPageData(NetworkMessageReader* msg);

    static void updatePerson(NetworkMessageReader& data, PlayerModel* playerModel);
};

#endif // MESSAGEHELPER_H
