#ifndef QTDOCKGUIDER_H
#define QTDOCKGUIDER_H

#include "QtFlexManager.h"

class DockGuiderImpl;

class QT_FLEX_API DockGuider : public QWidget
{
    Q_OBJECT
public:
    DockGuider(const QRect& widgetRect, Flex::ViewMode viewMode);
    ~DockGuider();

public:
    Flex::DockArea area() const;

public:
    int siteIndex() const;

public:
    void addSite(const QRect& siteRect, Flex::ViewMode viewMode);

public:
    static DockGuider* instance();

protected:
    void paintEvent(QPaintEvent*);

protected:
    void mouseMoveEvent(QMouseEvent* evt);
    void showEvent(QShowEvent*);
    void hideEvent(QHideEvent*);

private:
    friend class DockGuiderImpl;

private:
    QScopedPointer<DockGuiderImpl> impl;
};

#endif
