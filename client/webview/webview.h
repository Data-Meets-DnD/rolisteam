#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>
#include <QWebEngineView>
#include <QAction>
#include <QMouseEvent>

#include "data/mediacontainer.h"
#include "network/networkmessagewriter.h"

class WebView : public MediaContainer
{
    Q_OBJECT

public:
    enum ShareType {URL, HTML};
    explicit WebView(bool localIsGM, QWidget *parent = 0);
    virtual ~WebView();

    virtual bool readFileFromUri();
    virtual void saveMedia();
    virtual void putDataIntoCleverUri();

    virtual void cleverURIHasChanged(CleverURI*,CleverURI::DataValue);

    void fill(NetworkMessageWriter &message);
    void readMessage(NetworkMessageReader& msg);
protected:
    void mousePressEvent(QMouseEvent *mouseEvent);
    void showEvent(QShowEvent* event);

    void createActions();
    void creationToolBar();

    void updateTitle();
    void sendOffClose();
private:
    bool m_keepSharing = false;
    QWebEngineView* m_view = nullptr;
    QAction* m_shareAsLink = nullptr;
    QAction* m_shareAsHtml = nullptr;
    QAction* m_shareAsView = nullptr;
    QAction* m_hideAddress = nullptr;
    QAction* m_next = nullptr;
    QAction* m_previous = nullptr;
    QAction* m_reload = nullptr;
    QLineEdit* m_addressEdit = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;



};

#endif // WEBVIEW_H
