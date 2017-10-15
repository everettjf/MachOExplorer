#ifndef QTFLEXWIDGET_H
#define QTFLEXWIDGET_H

#include "QtFlexManager.h"

class FlexWidgetImpl;

class QT_FLEX_API FlexWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive)
    Q_PROPERTY(Flex::ViewMode viewMode READ viewMode)
private:
    FlexWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags);
    ~FlexWidget();

Q_SIGNALS:
    void enterMove(QObject*);
    void leaveMove(QObject*);
    void moving(QObject*);
    void destroying(FlexWidget*);

public:
    QSize sizeHint() const;

public:
    QSize minimumSizeHint() const;

public:
    bool isActive() const;

public:
    Flex::ViewMode viewMode() const;

public:
    Flex::Features dockFeatures() const;

public:
    bool isDockAllowed(DockWidget* widget, const QPoint& pos);
    bool isDockAllowed(FlexWidget* widget, const QPoint& pos);

public:
    DockSite* current() const;

public:
    void setCurrent(DockSite*);

public:
    bool addDockSite(DockSite* dockSite, Flex::DockArea area = Flex::M, int siteIndex = -1);
    bool addDockWidget(DockWidget* widget, Flex::DockArea area = Flex::M, int siteIndex = -1);
    bool addFlexWidget(FlexWidget* widget, Flex::DockArea area = Flex::M, int siteIndex = -1);

public:
    bool removeDockSite(DockSite* dockSite);

public:
    void clearDockSites(bool all = false);

public:
    bool isAdjusting() const;

public:
    QSplitter* siteContainer() const;
    QSplitter* sideContainer() const;

public:
    DockSide* dockSide(Flex::Direction direction) const;

public:
    int count() const;

    int count(Flex::Direction direction) const;

    DockSite* dockSite(int index, Flex::Direction direction = Flex::C) const;

    const QList<DockSite*>& dockSites(Flex::Direction direction = Flex::C) const;

    DockSite* dockSite(const QString& name) const;

public:
    bool isFloating() const;

public:
    void makeSiteAutoHide(DockSite* dockSite);
    void makeSiteDockShow(DockSite* dockSite);
    void showSiteDockPull(DockSite* dockSite);

public:
    bool load(const QJsonObject& object);

public:
    bool save(QJsonObject& object) const;

public:
    QByteArray snapshot() const;

public:
    bool restore(const QByteArray& snapshot, const QString& identifer);

protected:
    bool event(QEvent* evt);
    void paintEvent(QPaintEvent* evt);
    void resizeEvent(QResizeEvent*);
    bool nativeEvent(const QByteArray& eventType, void * message, long *result);

private:
    bool isGuiderExists() const;

private:
    bool isGuiderVisible() const;

private:
    void showGuider(QWidget* widget);
    void hideGuider(QWidget* widget);
    void hoverGuider(QWidget* widget);
    bool dropGuider(DockWidget* widget);
    bool dropGuider(FlexWidget* widget);

private Q_SLOTS:
    void on_titleBar_buttonClicked(Flex::Button, bool*);
    void on_side_currentChanged(DockSide*, DockSite*, DockSite*);

private:
    friend class FlexManager;

private:
    friend class FlexWidgetImpl;

private:
    QScopedPointer<FlexWidgetImpl> impl;
};

#endif
