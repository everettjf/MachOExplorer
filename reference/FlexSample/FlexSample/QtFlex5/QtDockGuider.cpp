#include "QtDockGuider.h"
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

namespace
{
    class Icons
    {
    private:
        Icons()
        {
            QPixmap pixmap(":/Resources/guiders.png");
            mGuider = pixmap.copy(30, 30, 30, 30);
            lGuiders[0] = pixmap.copy(0, 30, 30, 30);
            lGuiders[1] = pixmap.copy(0, 0, 30, 30);
            tGuiders[0] = pixmap.copy(30, 0, 30, 30);
            tGuiders[1] = pixmap.copy(60, 0, 30, 30);
            rGuiders[0] = pixmap.copy(60, 30, 30, 30);
            rGuiders[1] = pixmap.copy(60, 60, 30, 30);
            bGuiders[0] = pixmap.copy(30, 60, 30, 30);
            bGuiders[1] = pixmap.copy(0, 60, 30, 30);
            guiders[0].resize(17);
            guiders[0].setPoint(0, 41, 84);
            guiders[0].setPoint(1, 77, 84);
            guiders[0].setPoint(2, 84, 77);
            guiders[0].setPoint(3, 84, 41);
            guiders[0].setPoint(4, 126, 41);
            guiders[0].setPoint(5, 126, 77);
            guiders[0].setPoint(6, 133, 84);
            guiders[0].setPoint(7, 168, 84);
            guiders[0].setPoint(8, 168, 126);
            guiders[0].setPoint(9, 133, 126);
            guiders[0].setPoint(10, 126, 133);
            guiders[0].setPoint(11, 126, 167);
            guiders[0].setPoint(12, 84, 167);
            guiders[0].setPoint(13, 84, 133);
            guiders[0].setPoint(14, 77, 126);
            guiders[0].setPoint(15, 41, 126);
            guiders[0].setPoint(16, 41, 84);
            guidersBoundingRect[0] = guiders[0].boundingRect();
            guiders[1].resize(17);
            guiders[1].setPoint(0, 0, 84);
            guiders[1].setPoint(1, 77, 84);
            guiders[1].setPoint(2, 84, 77);
            guiders[1].setPoint(3, 84, 0);
            guiders[1].setPoint(4, 126, 0);
            guiders[1].setPoint(5, 126, 77);
            guiders[1].setPoint(6, 133, 84);
            guiders[1].setPoint(7, 210, 84);
            guiders[1].setPoint(8, 210, 126);
            guiders[1].setPoint(9, 133, 126);
            guiders[1].setPoint(10, 126, 133);
            guiders[1].setPoint(11, 126, 210);
            guiders[1].setPoint(12, 84, 210);
            guiders[1].setPoint(13, 84, 133);
            guiders[1].setPoint(14, 77, 126);
            guiders[1].setPoint(15, 0, 126);
            guiders[1].setPoint(16, 0, 84);
            guidersBoundingRect[1] = guiders[1].boundingRect();
        }

    public:
        static Icons* ptr() { static Icons icons; return &icons; }

    public:
        QPixmap lGuiders[2];
        QPixmap tGuiders[2];
        QPixmap rGuiders[2];
        QPixmap bGuiders[2];
        QPixmap mGuider;
        QPolygon guiders[2];
        QRect guidersBoundingRect[2];
    };
}

class DockGuiderImpl
{
public:
    DockGuiderImpl()
    {
    }

public:
    void updateGuidersVisibility();

public:
    QRect _widgetRect;
    Flex::ViewMode _viewMode;
    QList<QRect> _siteRects;
    QList<Flex::ViewMode> _viewModes;
    int _siteIndex = -1;
    int _guidersIndex = -1;
    bool _lGuiderVisible[3];
    bool _tGuiderVisible[3];
    bool _rGuiderVisible[3];
    bool _bGuiderVisible[3];
    bool _mGuiderVisible;
    QRect _lGuiderRect[3];
    QRect _tGuiderRect[3];
    QRect _rGuiderRect[3];
    QRect _bGuiderRect[3];
    QRect _mGuiderRect;
    Flex::DockArea _area = Flex::None;

public:
    static DockGuider* _instance;
};

DockGuider* DockGuiderImpl::_instance = nullptr;

void DockGuiderImpl::updateGuidersVisibility()
{
    auto siteViewMode = (_siteIndex >= 0) ? _viewModes[_siteIndex] : Flex::HybridView;

    if ((siteViewMode == Flex::ToolView || siteViewMode == Flex::ToolPagesView) && (_viewMode != Flex::ToolView && _viewMode != Flex::ToolPagesView))
    {
        _mGuiderVisible = false;
    }
    else
    {
        _mGuiderVisible = true;
    }

    if (siteViewMode == Flex::FileView || siteViewMode == Flex::FilePagesView || siteViewMode == Flex::HybridView)
    {
        _guidersIndex = 1;
        _lGuiderVisible[0] = true;
        _tGuiderVisible[0] = true;
        _rGuiderVisible[0] = true;
        _bGuiderVisible[0] = true;
        _lGuiderVisible[1] = true;
        _tGuiderVisible[1] = true;
        _rGuiderVisible[1] = true;
        _bGuiderVisible[1] = true;
        _lGuiderVisible[2] = true;
        _tGuiderVisible[2] = true;
        _rGuiderVisible[2] = true;
        _bGuiderVisible[2] = true;
        _lGuiderRect[1] = QRect(4, 90, 30, 30);
        _tGuiderRect[1] = QRect(90, 4, 30, 30);
        _rGuiderRect[1] = QRect(175, 90, 30, 30);
        _bGuiderRect[1] = QRect(90, 175, 30, 30);
    }
    else
    {
        _guidersIndex = 0;
        _lGuiderVisible[0] = false;
        _tGuiderVisible[0] = false;
        _rGuiderVisible[0] = false;
        _bGuiderVisible[0] = false;
        _lGuiderVisible[1] = true;
        _tGuiderVisible[1] = true;
        _rGuiderVisible[1] = true;
        _bGuiderVisible[1] = true;
        _lGuiderVisible[2] = true;
        _tGuiderVisible[2] = true;
        _rGuiderVisible[2] = true;
        _bGuiderVisible[2] = true;
        _lGuiderRect[1] = QRect(46, 90, 30, 30);
        _tGuiderRect[1] = QRect(90, 46, 30, 30);
        _rGuiderRect[1] = QRect(133, 90, 30, 30);
        _bGuiderRect[1] = QRect(90, 133, 30, 30);
    }
}

DockGuider::DockGuider(const QRect& widgetRect, Flex::ViewMode viewMode) : QWidget(NULL, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), impl(new DockGuiderImpl)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setMouseTracking(true);
    setProperty("Flex", true);

    impl->_widgetRect = widgetRect;

    impl->_viewMode = viewMode;

    impl->_lGuiderRect[0] = QRect(46, 90, 30, 30);
    impl->_tGuiderRect[0] = QRect(90, 46, 30, 30);
    impl->_rGuiderRect[0] = QRect(133, 90, 30, 30);
    impl->_bGuiderRect[0] = QRect(90, 133, 30, 30);

    impl->_lGuiderRect[1] = QRect(4, 90, 30, 30);
    impl->_tGuiderRect[1] = QRect(90, 4, 30, 30);
    impl->_rGuiderRect[1] = QRect(175, 90, 30, 30);
    impl->_bGuiderRect[1] = QRect(90, 175, 30, 30);

    impl->_mGuiderRect = QRect(90, 90, 30, 30);

    DockGuiderImpl::_instance = this;
}

DockGuider::~DockGuider()
{
    DockGuiderImpl::_instance = nullptr;
}

Flex::DockArea DockGuider::area() const
{
    return impl->_area;
}

void DockGuider::paintEvent(QPaintEvent*)
{
    if (impl->_guidersIndex < 0)
    {
        return;
    }

    QRect client = rect();
    int x, y, w, h;
    client.getRect(&x,  &y, &w, &h);

    QRect siteRect = (impl->_siteIndex >= 0) ? impl->_siteRects[impl->_siteIndex] : client;
    int sx, sy, sw, sh;
    siteRect.getRect(&sx, &sy, &sw, &sh);

    int wx, wy, ww, wh;
    impl->_widgetRect.getRect(&wx, &wy, &ww, &wh);

    QPoint offset = siteRect.center() - Icons::ptr()->guidersBoundingRect[impl->_guidersIndex].center();

    QPainter painter(this);

    painter.setBrush(QBrush(QColor("#424242")));
    painter.setPen(QPen(QColor("#424242")));

    painter.setOpacity(0.75);

    QColor background("#CCCCCC");

    switch (impl->_area)
    {
    case Flex::M:
        painter.fillRect(siteRect, background);
        break;
    case Flex::L0:
        painter.fillRect(sx, sy, sw / 2, sh, background);
        break;
    case Flex::L1:
        painter.fillRect(sx, sy, (ww < sw / 2) ? ww : sw / 2, sh, background);
        break;
    case Flex::L2:
        painter.fillRect(x, y, w / 2, h, background);
        break;
    case Flex::T0:
        painter.fillRect(sx, sy, sw, sh / 2, background);
        break;
    case Flex::T1:
        painter.fillRect(sx, sy, sw, (wh < sh / 2) ? wh : sh / 2, background);
        break;
    case Flex::T2:
        painter.fillRect(x, y, w, h / 2, background);
        break;
    case Flex::R0:
        painter.fillRect(sx + sw / 2, sy, sw / 2, sh, background);
        break;
    case Flex::R1:
        painter.fillRect(sx + ((ww < sw / 2) ? (sw - ww) : sw / 2), sy, (ww < sw / 2) ? ww : sw / 2, sh, background);
        break;
    case Flex::R2:
        painter.fillRect(x + w / 2, y, w / 2, h, background);
        break;
    case Flex::B0:
        painter.fillRect(sx, sy + sh / 2, sw, sh / 2, background);
        break;
    case Flex::B1:
        painter.fillRect(sx, sy + ((wh < sh / 2) ? (sh - wh) : sh / 2), sw, (wh < sh / 2) ? wh : sh / 2, background);
        break;
    case Flex::B2:
        painter.fillRect(x, y + h / 2, w, h / 2, background);
        break;
    default:
        break;
    }

    if (impl->_siteRects.size() > 0 )
    {
        if (impl->_lGuiderVisible[2]) painter.drawPixmap(impl->_lGuiderRect[2].topLeft(), Icons::ptr()->lGuiders[1]);
        if (impl->_tGuiderVisible[2]) painter.drawPixmap(impl->_tGuiderRect[2].topLeft(), Icons::ptr()->tGuiders[1]);
        if (impl->_rGuiderVisible[2]) painter.drawPixmap(impl->_rGuiderRect[2].topLeft(), Icons::ptr()->rGuiders[1]);
        if (impl->_bGuiderVisible[2]) painter.drawPixmap(impl->_bGuiderRect[2].topLeft(), Icons::ptr()->bGuiders[1]);
    }
    
    painter.translate(offset);
    painter.setBrush(QBrush(QColor("#FFFFFF")));
    painter.drawPolygon(Icons::ptr()->guiders[impl->_guidersIndex]);

    if (impl->_siteRects.size() > 0)
    {
        if (impl->_lGuiderVisible[1]) painter.drawPixmap(impl->_lGuiderRect[1].topLeft(), Icons::ptr()->lGuiders[1]);
        if (impl->_tGuiderVisible[1]) painter.drawPixmap(impl->_tGuiderRect[1].topLeft(), Icons::ptr()->tGuiders[1]);
        if (impl->_rGuiderVisible[1]) painter.drawPixmap(impl->_rGuiderRect[1].topLeft(), Icons::ptr()->rGuiders[1]);
        if (impl->_bGuiderVisible[1]) painter.drawPixmap(impl->_bGuiderRect[1].topLeft(), Icons::ptr()->bGuiders[1]);
    }

    if (impl->_mGuiderVisible) painter.drawPixmap(impl->_mGuiderRect.topLeft(), Icons::ptr()->mGuider);

    if (impl->_siteRects.size() > 0)
    {
        if (impl->_lGuiderVisible[0]) painter.drawPixmap(impl->_lGuiderRect[0].topLeft(), Icons::ptr()->lGuiders[0]);
        if (impl->_tGuiderVisible[0]) painter.drawPixmap(impl->_tGuiderRect[0].topLeft(), Icons::ptr()->tGuiders[0]);
        if (impl->_rGuiderVisible[0]) painter.drawPixmap(impl->_rGuiderRect[0].topLeft(), Icons::ptr()->rGuiders[0]);
        if (impl->_bGuiderVisible[0]) painter.drawPixmap(impl->_bGuiderRect[0].topLeft(), Icons::ptr()->bGuiders[0]);
    }
}

void DockGuider::mouseMoveEvent(QMouseEvent* evt)
{
    if (!isVisible())
    {
        return;
    }

    QRect client = rect();

    auto siteIndex = impl->_siteIndex;

    if (impl->_siteIndex < 0 || !impl->_siteRects[impl->_siteIndex].contains(evt->pos()))
    {
        for (int i = 0; i < impl->_siteRects.size(); ++i)
        {
            if (impl->_siteRects[i].contains(evt->pos()))
            {
                impl->_siteIndex = i; update(); break;
            }
        }
    }

    if (impl->_siteIndex != siteIndex)
    {
        impl->updateGuidersVisibility();
    }

    auto siteRect = (impl->_siteIndex >= 0) ? impl->_siteRects[impl->_siteIndex] : client;

    QPoint pos = evt->pos() - siteRect.center() + Icons::ptr()->guidersBoundingRect[impl->_guidersIndex].center();

    if (impl->_mGuiderVisible && impl->_mGuiderRect.contains(pos))
    {
        if (impl->_area != Flex::M)
        {
            impl->_area = Flex::M;  update();
        }
        return;
    }

    if (impl->_siteRects.count() == 0)
    {
        if (impl->_area != Flex::None)
        {
            impl->_area = Flex::None; update();
        }
        return;
    }

    if (impl->_lGuiderVisible[0] && impl->_lGuiderRect[0].contains(pos))
    {
        if (impl->_area != Flex::L0)
        {
            impl->_area = Flex::L0;  update();
        }
        return;
    }

    if (impl->_tGuiderVisible[0] && impl->_tGuiderRect[0].contains(pos))
    {
        if (impl->_area != Flex::T0)
        {
            impl->_area = Flex::T0;  update();
        }
        return;
    }

    if (impl->_bGuiderVisible[0] && impl->_rGuiderRect[0].contains(pos))
    {
        if (impl->_area != Flex::R0)
        {
            impl->_area = Flex::R0;  update();
        }
        return;
    }

    if (impl->_rGuiderVisible[0] && impl->_bGuiderRect[0].contains(pos))
    {
        if (impl->_area != Flex::B0)
        {
            impl->_area = Flex::B0;  update();
        }
        return;
    }

    if (impl->_lGuiderVisible[1] && impl->_lGuiderRect[1].contains(pos))
    {
        if (impl->_area != Flex::L1)
        {
            impl->_area = Flex::L1;  update();
        }
        return;
    }

    if (impl->_tGuiderVisible[1] && impl->_tGuiderRect[1].contains(pos))
    {
        if (impl->_area != Flex::T1)
        {
            impl->_area = Flex::T1;  update();
        }
        return;
    }

    if (impl->_bGuiderVisible[1] && impl->_rGuiderRect[1].contains(pos))
    {
        if (impl->_area != Flex::R1)
        {
            impl->_area = Flex::R1;  update();
        }
        return;
    }

    if (impl->_rGuiderVisible[1] && impl->_bGuiderRect[1].contains(pos))
    {
        if (impl->_area != Flex::B1)
        {
            impl->_area = Flex::B1;  update();
        }
        return;
    }

    pos = evt->pos();

    if (impl->_lGuiderVisible[2] && impl->_lGuiderRect[2].contains(pos))
    {
        if (impl->_area != Flex::L2)
        {
            impl->_area = Flex::L2;  update();
        }
        return;
    }

    if (impl->_tGuiderVisible[2] && impl->_tGuiderRect[2].contains(pos))
    {
        if (impl->_area != Flex::T2)
        {
            impl->_area = Flex::T2;  update();
        }
        return;
    }

    if (impl->_bGuiderVisible[2] && impl->_rGuiderRect[2].contains(pos))
    {
        if (impl->_area != Flex::R2)
        {
            impl->_area = Flex::R2;  update();
        }
        return;
    }

    if (impl->_rGuiderVisible[2] && impl->_bGuiderRect[2].contains(pos))
    {
        if (impl->_area != Flex::B2)
        {
            impl->_area = Flex::B2;  update();
        }
        return;
    }

    if (impl->_area != Flex::None)
    {
        impl->_area = Flex::None; update();
    }
}

void DockGuider::showEvent(QShowEvent* evt)
{
    QRect client = rect();
    int w = client.width();
    int h = client.height();

    impl->_lGuiderRect[2] = QRect(5, (h - 30) / 2, 30, 30);
    impl->_tGuiderRect[2] = QRect((w - 30) / 2, 5, 30, 30);
    impl->_rGuiderRect[2] = QRect(w - 30 - 5, (h - 30) / 2, 30, 30);
    impl->_bGuiderRect[2] = QRect((w - 30) / 2, h - 30 - 5, 30, 30);

    QWidget::showEvent(evt);
}

void DockGuider::hideEvent(QHideEvent*)
{
}

void DockGuider::addSite(const QRect& siteRect, Flex::ViewMode viewMode)
{
    impl->_siteRects.append(siteRect);
    impl->_viewModes.append(viewMode);
}

int DockGuider::siteIndex() const
{
    return impl->_siteIndex;
}

DockGuider* DockGuider::instance()
{
    return DockGuiderImpl::_instance;
}
