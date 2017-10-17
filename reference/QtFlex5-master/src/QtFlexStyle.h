#ifndef QT_FLEXSTYLE_H
#define QT_FLEXSTYLE_H

#include "QtFlexManager.h"

#include <QProxyStyle>

class FlexStyleImpl;

class QT_FLEX_API FlexStyle : public QProxyStyle
{
public:
    FlexStyle(QStyle* style = nullptr);
    ~FlexStyle();

public:
    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = 0) const;
    void drawControl(ControlElement ce, const QStyleOption *opt, QPainter *p, const QWidget *w = 0) const;
public:
    int pixelMetric(PixelMetric pm, const QStyleOption *opt = 0, const QWidget *w = 0) const;
public:
    QRect subElementRect(SubElement se, const QStyleOption *opt, const QWidget *w = 0) const;

private:
    friend class FlexStyleImpl;
private:
    QScopedPointer<FlexStyleImpl> impl;
};

#endif
