#include "QtFlexStyle.h"
#include "QtFlexManager.h"
#include "QtFlexWidget.h"
#include "QtDockSite.h"
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOption>

class FlexStyleImpl
{
public:
    FlexStyleImpl()
    {
    }
};

FlexStyle::FlexStyle(QStyle* style) : QProxyStyle(style), impl(new FlexStyleImpl)
{
    setObjectName(QLatin1String("FlexStyle"));
}

FlexStyle::~FlexStyle()
{

}

void FlexStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (pe)
    {
    case QStyle::PE_IndicatorTabClose:
        if (!w || !w->parent()->property("Flex").isValid())
        {
            break;
        }
        else
        {
            bool active = w->parentWidget()->property("active").toBool();
            bool highlight = (opt->state & State_Selected) && active;
            bool mouseover = opt->state & State_MouseOver;

            p->setPen(QColor("#E5C365"));
            p->setBrush(QColor("#FFFCF4"));

            if (mouseover)
            {
                p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            }

            FlexManager::instance()->icon(Flex::Close).paint(p, opt->rect, Qt::AlignCenter, (mouseover || highlight) ? QIcon::Active : QIcon::Normal, mouseover ? QIcon::On : QIcon::Off);
        }
        return;
    case QStyle::PE_Frame:
        if (!w || !w->property("Flex").isValid())
        {
            break;
        }
        if (auto tmp = qstyleoption_cast<const QStyleOptionFrame*>(opt))
        {
            p->setPen(QColor("#8692B1"));
            p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            if (tmp->midLineWidth >= 2)
            {
                p->setPen(tmp->palette.color(QPalette::Highlight));
                p->drawRect(opt->rect.adjusted(1, 1, -2, -2));
                p->drawRect(opt->rect.adjusted(2, 2, -3, -3));
            }
            return;
        }
        break;
    case PE_FrameTabBarBase:
        if (!w || !w->property("Flex").isValid())
        {
            break;
        }
        if (auto tbb = qstyleoption_cast<const QStyleOptionTabBarBaseV2*>(opt))
        {
            auto active = w->property("active").toBool();
            auto colour = tbb->palette.color(active ? QPalette::Active : QPalette::Inactive, QPalette::Highlight);
            p->save();
            switch (tbb->shape) 
            {
            case QTabBar::RoundedNorth:
                p->setPen(QPen(colour, 1));
                p->drawLine(tbb->rect.bottomLeft() + QPoint(0, -1), tbb->rect.bottomRight() + QPoint(0, -1));
                p->drawLine(tbb->rect.bottomLeft() + QPoint(0, +0), tbb->rect.bottomRight() + QPoint(0, +0));
                break;
            case QTabBar::RoundedSouth:
                p->setPen(QPen(QColor("#8692B1"), 1));
                p->drawLine(tbb->rect.left(), tbb->rect.top() + 0, tbb->rect.right(), tbb->rect.top() + 0);
                break;
            default:
                break;
            }
            p->restore();
        }
        return;
    default:
        break;
    }
    QProxyStyle::drawPrimitive(pe, opt, p, w);
}

void FlexStyle::drawControl(ControlElement ce, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (ce)
    {
    case CE_TabBarTabShape:
        if (!w || !w->property("Flex").isValid())
        {
            break;
        }
        if (auto tab = qstyleoption_cast<const QStyleOptionTabV3*>(opt))
        {
            bool highlight = tab->state & State_Selected;
            bool mouseover = tab->state & State_MouseOver;
            bool active = w->property("active").toBool();

            QRect rect = tab->rect;

            switch (tab->shape)
            {
            case QTabBar::RoundedNorth:
                rect.adjust(0, 0, 0, highlight ? -2 : -3);
                break;
            case QTabBar::RoundedSouth:
                rect.adjust(0, highlight ? -1 : 1, 0, highlight ? -1 : 1);
                break;
            default:
                break;
            }

            auto bgColor = tab->palette.color(QPalette::Inactive, w->backgroundRole());

            if (highlight)
            {
                bgColor = tab->palette.color(active ? QPalette::Active : QPalette::Inactive, QPalette::Highlight);
            }
            else if (mouseover)
            {
                bgColor = tab->palette.color(QPalette::Active, w->backgroundRole());
            }

            p->setBrush(bgColor);

            p->setPen(tab->shape == QTabBar::RoundedSouth && highlight ? QColor("#8692B1") : bgColor);

            p->drawRect(rect);

            if (tab->shape == QTabBar::RoundedSouth && !highlight)
            {
                p->setPen(QColor("#4B5C74"));
                p->drawLine(tab->rect.bottomLeft() - QPoint(0, 0), tab->rect.bottomRight() - QPoint(0, 0));
                p->drawLine(tab->rect.bottomLeft() - QPoint(0, 1), tab->rect.bottomRight() - QPoint(0, 1));
            }

            return;
        }
        break;
    case CE_TabBarTabLabel:
        if (!w || !w->property("Flex").isValid())
        {
            break;
        }
        if (auto tab = qstyleoption_cast<const QStyleOptionTabV3*>(opt))
        {
            bool active = w->property("active").toBool();
            bool highlight = tab->state & State_Selected;
            auto palette = tab->palette;
            if (tab->shape == QTabBar::RoundedNorth)
            {
                palette.setCurrentColorGroup(active && highlight ? QPalette::Active : QPalette::Inactive);
            }
            else
            {
                palette.setCurrentColorGroup(highlight ? QPalette::Active : QPalette::Inactive);
            }
            QStyleOptionTabV3 tmp = *tab;
            tmp.palette = palette;
#ifdef Q_OS_MAC
            QCommonStyle::drawControl(ce, &tmp, p, w);
#else
            QProxyStyle::drawControl(ce, &tmp, p, w);
#endif
            return;
        }
        break;
    case CE_Splitter:
        if (!w || !w->parent()->property("Flex").isValid())
        {
            break;
        }
        else
        {
            return;
        }
        break;
    default:
        break;
    }
    QProxyStyle::drawControl(ce, opt, p, w);
}

int FlexStyle::pixelMetric(PixelMetric pm, const QStyleOption *opt, const QWidget *w) const
{
    switch (pm)
    {
    case PM_TitleBarHeight:
        if (!w || !w->property("Flex").isValid())
        {
            break;
        }
        return 27;
    case PM_TabBarTabShiftVertical:
        if (!w || !w->property("Flex").isValid())
        {
            break;
        }
        return 0;
    case PM_TabBarTabHSpace:
        if (!w || !w->property("Flex").isValid())
        {
            break;
        }
        return 8;
    default:
        break;
    }
    return QProxyStyle::pixelMetric(pm, opt, w);
}

QRect FlexStyle::subElementRect(SubElement se, const QStyleOption *opt, const QWidget *w) const
{
    return QProxyStyle::subElementRect(se, opt, w);
}
