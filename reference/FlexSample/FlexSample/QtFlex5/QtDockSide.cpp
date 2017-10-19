#include "QtDockSide.h"
#include "QtDockSite.h"
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

class DockSideImpl
{
public:
    DockSideImpl() : _over(-1), _space(12), _headOffset(0), _tailOffset(0), _curr(-1)
    {
    }

public:
    void hittest(DockSide* self, const QPoint& pos);

public:
    int _over;
    int _space;
    int _headOffset;
    int _tailOffset;
    int _curr;
    Flex::Direction _direction;
    QList<DockSite*> _dockSites;
    QWidget* _container;
};

void DockSideImpl::hittest(DockSide* self, const QPoint& pos)
{
    int v = _direction == Flex::L || _direction == Flex::R ? pos.y() : pos.x();

    int h = _over;

    int offset = _headOffset;

    _over = -1;

    for (int i = 0; i < _dockSites.size(); ++i)
    {
        int width = self->fontMetrics().width(_dockSites[i]->windowTitle());

        if (v >= offset && v < offset + width)
        {
            _over = i; break;
        }

        offset += width + _space;
    }

    if (_over != h)
    {
        self->update();
    }
}

DockSide::DockSide(Flex::Direction direction, QWidget* container, QWidget* parent) : QWidget(parent), impl(new DockSideImpl)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::NoFocus);

    impl->_container = container;
    impl->_direction = direction;

    switch (impl->_direction)
    {
    case Flex::L:
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        break;
    case Flex::T:
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        break;
    case Flex::R:
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        break;
    case Flex::B:
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        break;
    default:
        break;
    }
}

DockSide::~DockSide()
{

}

Flex::Direction DockSide::direction() const
{
    return impl->_direction;
}

bool DockSide::attachDockSite(DockSite* dockSite)
{
    doneCurrent();

    if (std::find(impl->_dockSites.begin(), impl->_dockSites.end(), dockSite) == impl->_dockSites.end())
    {
        impl->_dockSites.append(dockSite);
        dockSite->setDockMode(Flex::DockInSideArea);
        dockSite->hide();
        emit dockSiteAttached(this, dockSite);
        update();
    }

    impl->_over = -1;
    impl->_curr = -1;

    if (!impl->_dockSites.empty())
    {
        show();
    }

    return true;
}

bool DockSide::detachDockSite(DockSite* dockSite)
{
    doneCurrent();

    if (impl->_dockSites.removeOne(dockSite))
    {
        dockSite->setDockMode(Flex::DockInMainArea);
        dockSite->hide();
        emit dockSiteDetached(this, dockSite);
        update();
    }

    impl->_over = -1;
    impl->_curr = -1;

    if (impl->_dockSites.empty())
    {
        hide();
    }

    return true;
}

QSize DockSide::sizeHint() const
{
    return QSize(30, 30);
}

void DockSide::setHeadOffset(int offset)
{
    if (impl->_headOffset != offset)
    {
        impl->_headOffset = offset;
        update();
    }
}

void DockSide::setTailOffset(int offset)
{
    if (impl->_tailOffset != offset)
    {
        impl->_tailOffset = offset;
        update();
    }
}

void DockSide::setSpace(int space)
{
    if (impl->_space != space)
    {
        impl->_space = space;
        update();
    }
}

void DockSide::paintEvent(QPaintEvent*)
{
    auto dim = size();

    int pw = 0;
    int ph = 0;

    switch (impl->_direction)
    {
    case Flex::L:
    case Flex::R:
        pw = dim.height();
        ph = dim.width();
        break;
    case Flex::T:
    case Flex::B:
        pw = dim.width();
        ph = dim.height();
        break;
    default:
        break;
    }

    QPixmap pixmap(pw, ph);

    pixmap.fill(Qt::transparent);

    QPainter pixmapPainter(&pixmap);

    pixmapPainter.initFrom(this);

    int offset = impl->_headOffset;

    for (int i = 0; i < impl->_dockSites.size(); ++i)
    {
        auto dockText = impl->_dockSites[i]->windowTitle();
        auto dockRect = pixmapPainter.fontMetrics().boundingRect(dockText);
        switch (impl->_direction)
        {
        case Flex::L:
        case Flex::B:
            pixmapPainter.fillRect(offset, ph - 6, dockRect.width(), 6, impl->_over == i ? QColor("#9BA7B7") : QColor("#465A7D"));
            pixmapPainter.drawText(offset, ph - dockRect.height(), dockText);
            break;
        case Flex::T:
        case Flex::R:
            pixmapPainter.fillRect(offset, 0, dockRect.width(), 6, impl->_over == i ? QColor("#9BA7B7") : QColor("#465A7D"));
            pixmapPainter.drawText(offset, ph - dockRect.height(), dockText);
            break;
        default:
            break;
        }
        offset += dockRect.width() + impl->_space;
    }

    pixmapPainter.end();

    QPainter painter(this);

    if (impl->_direction == Flex::L || impl->_direction == Flex::R)
    {
        painter.translate(ph, 0);
        painter.rotate(90);
    }

    painter.drawPixmap(QRect(0, 0, pw, ph), pixmap);
}

void DockSide::mouseMoveEvent(QMouseEvent* evt)
{
    impl->hittest(this, evt->pos());
}

void DockSide::mousePressEvent(QMouseEvent* evt)
{
    impl->hittest(this, evt->pos());

    if (impl->_over == -1)
    {
        return;
    }

    auto curr = impl->_dockSites[impl->_over];
    if (impl->_curr == impl->_over)
    {
        curr->blockSignals(true);
        if (curr->isActive())
        {
            curr->hide();
        }
        else
        {
            curr->raise();
            curr->show();
            curr->activate();
        }
        curr->blockSignals(false);
        return;
    }

    auto prev = impl->_curr != -1 ? impl->_dockSites[impl->_curr] : nullptr;
    impl->_curr = impl->_over;
    emit currentChanged(this, prev, curr);
}

void DockSide::enterEvent(QEvent*)
{
    impl->hittest(this, mapFromGlobal(QCursor::pos()));
}

void DockSide::leaveEvent(QEvent*)
{
    impl->_over = -1; update();
}

bool DockSide::hasDockSite(DockSite* dockSite) const
{
    return impl->_dockSites.contains(dockSite);
}

int DockSide::count() const
{
    return impl->_dockSites.size();
}

DockSite* DockSide::dockSite(int index) const
{
    return impl->_dockSites[index];
}

const QList<DockSite*>& DockSide::dockSites() const
{
    return impl->_dockSites;
}

DockSite* DockSide::dockSite(const QString& name) const
{
    auto iter = std::find_if(impl->_dockSites.begin(), impl->_dockSites.end(), [&](const DockSite* tempSite) { return tempSite->objectName() == name; });

    if (iter != impl->_dockSites.end())
    {
        return *iter;
    }

    return nullptr;
}

int DockSide::indexOf(DockSite* dockSite) const
{
    return impl->_dockSites.indexOf(dockSite);
}

DockSite* DockSide::current() const
{
    return impl->_curr != -1 ? impl->_dockSites[impl->_curr] : nullptr;
}

void DockSide::makeCurrent(DockSite* dockSite)
{
    if (!hasDockSite(dockSite))
    {
        return;
    }

    auto prev = impl->_curr != -1 ? impl->_dockSites[impl->_curr] : nullptr;

    auto curr = dockSite;

    impl->_curr = indexOf(dockSite);

    emit currentChanged(this, prev, curr);
}

void DockSide::doneCurrent()
{
    if (impl->_curr != -1)
    {
        auto curr = impl->_dockSites[impl->_curr];
        impl->_curr = -1;
        emit currentChanged(this, curr, nullptr);
    }
}

bool DockSide::load(const QJsonObject& object)
{
    setSpace(object["space"].toInt());
    setHeadOffset(object["headOffset"].toInt());
    setTailOffset(object["tailOffset"].toInt());
    QJsonArray dockSiteObjects = object["dockSites"].toArray();
    for (int i = 0; i < dockSiteObjects.size(); ++i)
    {
        QJsonObject dockSiteObject = dockSiteObjects[i].toObject();
        DockSite* dockSite = new DockSite(nullptr, QSize(), impl->_container);
        dockSite->load(dockSiteObject);
        attachDockSite(dockSite);
    }
    return true;
}

bool DockSide::save(QJsonObject& object) const
{
    object["space"] = impl->_space;
    object["headOffset"] = impl->_headOffset;
    object["tailOffset"] = impl->_tailOffset;
    QJsonArray dockSites;
    for (int i = 0; i < impl->_dockSites.size(); ++i)
    {
        QJsonObject dockSite;
        impl->_dockSites[i]->save(dockSite);
        dockSites.append(dockSite);
    }
    object["dockSites"] = dockSites;
    return true;
}

