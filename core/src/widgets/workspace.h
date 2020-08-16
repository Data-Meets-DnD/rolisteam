/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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
#ifndef WORKSPACE_AMELIORE_H
#define WORKSPACE_AMELIORE_H

#include <QImage>
#include <QMdiArea>
#include <QPaintEvent>
#include <QPointer>
#include <QWidget>

#include "data/mediacontainer.h"

class ContentController;
class ImageController;
class VectorialMapController;
class CharacterSheetController;
class WebpageController;
class SharedNoteController;
class MapController;
class PdfController;
class NoteController;
class InstantMessagingController;
/**
 * @brief The ImprovedWorkspace class
 */
class Workspace : public QMdiArea
{
    Q_OBJECT
public:
    enum Positioning
    {
        TopLeft,
        BottomLeft,
        Center,
        TopRight,
        BottomRight,
        Scaled,
        Filled,
        Repeated
    };
    Workspace(QToolBar* toolbar, ContentController* ctrl, InstantMessagingController* instantCtrl,
              QWidget* parent= nullptr);
    ~Workspace();

    QString backgroundImagePath() const;

    QWidget* addWindow(QWidget*, QAction* action);

    void addContainerMedia(MediaContainer* mediac);
    void preferencesHasChanged(QString);
    void removeMediaContainer(MediaContainer* mediac);
    QVector<QMdiSubWindow*> getAllSubWindowFromId(const QString& id) const;
    bool closeAllSubs();
signals:
    void removedAction(QAction*);
    void vmapActive();

public slots:
    void setTabbedMode(bool);
    void ensurePresent();
    void addWidgetToMdi(MediaContainer*, QString title);
    bool closeActiveSub();

protected slots:
    void addMedia(MediaControllerBase* ctrl);
    void addImage(ImageController* ctrl);
    void addVectorialMap(VectorialMapController* ctrl);
    void addCharacterSheet(CharacterSheetController* ctrl);
    void addWebpage(WebpageController* ctrl);
    void addSharedNote(SharedNoteController* ctrl);
    void addNote(NoteController* ctrl);
#ifdef WITH_PDF
    void addPdf(PdfController* ctrl);
#endif

protected:
    void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject* object, QEvent* event);
    bool updateTitleTab();
    bool closeSub(MediaContainer* container);

private:
    void updateBackGround();
    void updateActiveMediaContainer(QMdiSubWindow* window);
    void addWindowAction(const QString& name, MediaContainer* window);

private:
    QPointer<ContentController> m_ctrl;
    std::vector<std::unique_ptr<MediaContainer>> m_mediaContainers;
    QPixmap m_backgroundPicture;
    QPixmap m_variableSizeBackground;
    QPointer<QToolBar> m_toolbar;
    QHash<QMdiSubWindow*, QString> m_titleBar;
    QPointer<MediaContainer> m_activeMediaContainer;
    QPointer<QMdiSubWindow> m_instantMessageView;
    bool m_visible= false;
};

#endif
