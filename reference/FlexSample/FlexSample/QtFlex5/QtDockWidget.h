#ifndef QTDOCKWIDGET_H
#define QTDOCKWIDGET_H

#include "QtFlexManager.h"

class DockWidgetImpl;

class QT_FLEX_API DockWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive)
    Q_PROPERTY(Flex::ViewMode viewMode READ viewMode)
private:
    DockWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags);
    ~DockWidget();

public:
    Flex::ViewMode viewMode() const;
    void setViewMode(Flex::ViewMode viewMode);

public:
    Flex::Features dockFeatures() const;
    void setDockFeatures(Flex::Features features);
    Flex::Features siteFeatures() const;
    void setSiteFeatures(Flex::Features features);

public:
    DockSite* dockSite() const;
    DockSide* dockSide() const;

public:
    FlexWidget* flexWidget() const;

public:
    QString flexWidgetName() const;

public:
    QWidget* widget() const;
    void attachWidget(QWidget* widget);
    void detachWidget(QWidget* widget);

public:
    void activate();

public:
    void setWidget(QWidget* widget);

public:
    QSize sizeHint() const;

public:
    QSize minimumSizeHint() const;

Q_SIGNALS:
    void enterMove(QObject*);
    void leaveMove(QObject*);
    void moving(QObject*);
    void activated(DockWidget*);
    void destroying(DockWidget*);

public:
    bool isFloating() const;

public:
    bool isActive() const;

public:
    bool load(const QJsonObject& object);

public:
    bool save(QJsonObject& object) const;

public:
    QString identifier();

protected:
    bool event(QEvent* evt);
    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    bool nativeEvent(const QByteArray& eventType, void * message, long *result);

private Q_SLOTS:
    void on_titleBar_buttonClicked(Flex::Button, bool*);

private:
    friend class FlexManager;

private:
    friend class DockWidgetImpl;

private:
    QScopedPointer<DockWidgetImpl> impl;
};

#endif
