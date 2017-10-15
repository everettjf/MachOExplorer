#ifndef QTDOCKSIDE_H
#define QTDOCKSIDE_H

#include "QtFlexManager.h"

class DockSideImpl;

class QT_FLEX_API DockSide : public QWidget
{
    Q_OBJECT
public:
    DockSide(Flex::Direction direction, QWidget* container, QWidget* parent);
    ~DockSide();

Q_SIGNALS:
    void dockSiteAttached(DockSide*, DockSite*);
    void dockSiteDetached(DockSide*, DockSite*);
    void currentChanged(DockSide*, DockSite* prev, DockSite* curr);

public:
    Flex::Direction direction() const;

public:
    QSize sizeHint() const;

public:
    void setHeadOffset(int offset);
    void setTailOffset(int offset);
    void setSpace(int space);

public:
    bool attachDockSite(DockSite* dockSite);
    bool detachDockSite(DockSite* dockSite);

public:
    bool hasDockSite(DockSite* dockSite) const;

public:
    int count() const;
    DockSite* dockSite(int index) const;
    const QList<DockSite*>& dockSites() const;
    DockSite* dockSite(const QString& name) const;
    int indexOf(DockSite* dockSite) const;

public:
    DockSite* current() const;

public:
    void makeCurrent(DockSite* dockSite);

public:
    void doneCurrent();

public:
    bool load(const QJsonObject& object);

public:
    bool save(QJsonObject& object) const;

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    friend class DockSideImpl;
private:
    QScopedPointer<DockSideImpl> impl;
};

#endif
