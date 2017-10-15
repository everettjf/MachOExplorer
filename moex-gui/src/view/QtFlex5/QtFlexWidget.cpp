#include "QtFlexWidget.h"
#include "QtFlexHelper.h"
#include "QtDockWidget.h"
#include "QtDockGuider.h"
#include "QtDockSite.h"
#include "QtDockSide.h"
#include <QtCore/QUuid>
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtGui/QMouseEvent>
#include <QtGui/QWindow>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QStylePainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStyle>
#include <algorithm>
#include <numeric>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#endif

class FlexWidgetHole : public QWidget
{
public:
    FlexWidgetHole(QWidget* parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
};

class FlexWidgetImpl
{
public:
    FlexWidgetImpl()
    {
        _sides[0] = nullptr;
        _sides[1] = nullptr;
        _sides[2] = nullptr;
        _sides[3] = nullptr;
    }

public:
    void updateDockSides(FlexWidget* self);
    void updateViewMode(FlexWidget* self, Flex::ViewMode viewMode, bool first);
    void updateViewMode(FlexWidget* self);
    void update(FlexWidget* self);

public:
    void addDockSite(FlexWidget* self, DockSite* dockSite, Qt::Orientation orientation, int direction, QSplitter* parent = nullptr);
    void addDockSite(FlexWidget* self, DockSite* dockSite, int siteIndex, Qt::Orientation orientation, int direction, int hint);
    void addFlexWidget(FlexWidget* self, FlexWidget* widget, Qt::Orientation orientation, int direction);
    void addFlexWidget(FlexWidget* self, FlexWidget* widget, int siteIndex, Qt::Orientation orientation, int direction, int hint);

public:
    void simplify(FlexWidget* self, DockSite*);

public:
    bool isTitleBarVisible(FlexWidget* self, QRect* rect = nullptr) const;

public:
    bool load(FlexWidget* self, const QJsonArray& objects, QSplitter* splitter);

public:
    bool save(const FlexWidget* self, QJsonArray& objects, QSplitter* splitter) const;

public:
    QJsonObject find(const QJsonObject& object, const QStringList& path, int& index) const;

public:
    QString generate() const;

public:
    int _titleBarHeight = 23;
    int _frameWidth;
    Qt::WindowFlags _windowFlags;
    DockSide* _sides[4];
    QList<DockSite*> _sites;
    DockSite* _current = nullptr;
    DockGuider* _guider = nullptr;
    QWidget* _hole = nullptr;
    QSplitter* _siteContainer = nullptr;
    QSplitter* _sideContainer = nullptr;
    FlexHelper* _helper = nullptr;
    QPoint _startPoint;
    Flex::ViewMode _viewMode = Flex::HybridView;
    bool _adjusting = false;
    bool _reserving = false;
};

bool FlexWidgetImpl::load(FlexWidget* self, const QJsonArray& objects, QSplitter* splitter)
{
    QList<int> sizes;

    for (int i = 0; i < objects.count(); ++i)
    {
        QJsonObject object = objects[i].toObject();

        if (object["type"] == "wrapper")
        {
			auto arr = object["value"].toArray();
            QSplitter* subSplitter = new QSplitter((Qt::Orientation)arr[0].toObject()["orientation"].toInt(), splitter);
            subSplitter->setProperty("Flex", true);
            subSplitter->setObjectName(object["name"].toString());
            subSplitter->setChildrenCollapsible(false);
            load(self, arr, subSplitter);
        }
        else
        {
			auto site = new DockSite();
			_sites.append(site);
			addDockSite(self, site, (Qt::Orientation)object["orientation"].toInt(), 0, splitter);
			site->load(object["value"].toObject());
			site->show(); site->setFocus();
        }

        //splitter->setOrientation((Qt::Orientation)object["orientation"].toInt());

        sizes << object["size"].toInt();
    }

    splitter->setSizes(sizes);

    return true;
}

bool FlexWidgetImpl::save(const FlexWidget* self, QJsonArray& objects, QSplitter* splitter) const
{
    auto sizes = splitter->sizes();

    for (int i = 0; i < splitter->count(); ++i)
    {
        QWidget* widget = splitter->widget(i);

        QJsonObject node;

        if (qobject_cast<QSplitter*>(widget))
        {
            node["type"] = "wrapper";
            node["name"] = widget->objectName();
            QJsonArray widgets;
            save(self, widgets, qobject_cast<QSplitter*>(widget));
            node["value"] = widgets;
        }
        else
        {
            node["type"] = "content";
            QJsonObject object;
            qobject_cast<DockSite*>(widget)->save(object);
            node["value"] = object;
        }

        node["size"] = sizes[i];

        node["orientation"] = (int)splitter->orientation();

        objects.append(node);
    }

    return true;
}

QJsonObject FlexWidgetImpl::find(const QJsonObject& object, const QStringList& path, int& index) const
{
    if (path.size() <= 1)
    {
        index = -1; return QJsonObject();
    }
    else
    {
        index = -1;
        if (path[1] == "_flex_siteContainer")
        {
            QJsonArray objects = object["dockSites"].toArray();

            for (int i = 2; i < path.length() - 1; i += 2)
            {
                int number = path[i].toInt();

                QJsonObject target = objects[number].toObject();

                if (!target.contains("type"))
                {
                    index = number; return target;
                }

                if (target["type"].toString() == "wrapper")
                {
                    objects = target["value"].toArray();
                }
                else
                {
                    objects = target["value"].toObject()["widgets"].toArray();
                }
            }
        }
        return QJsonObject();
    }
}

void FlexWidgetImpl::updateDockSides(FlexWidget* self)
{
    if (_sides[Flex::T]->isVisible())
    {
        _sides[Flex::L]->setHeadOffset(_sides[Flex::T]->height());
        _sides[Flex::R]->setHeadOffset(_sides[Flex::T]->height());
    }
    else
    {
        _sides[Flex::L]->setHeadOffset(0);
        _sides[Flex::R]->setHeadOffset(0);
    }

    if (_sides[Flex::B]->isVisible())
    {
        _sides[Flex::L]->setTailOffset(_sides[Flex::B]->height());
        _sides[Flex::R]->setTailOffset(_sides[Flex::B]->height());
    }
    else
    {
        _sides[Flex::L]->setTailOffset(0);
        _sides[Flex::R]->setTailOffset(0);
    }

    int lMargin = _sides[Flex::L]->isVisible() ? 1 : _frameWidth;
    int tMargin = _sides[Flex::T]->isVisible() ? 1 : _frameWidth;
    int rMargin = _sides[Flex::R]->isVisible() ? 1 : _frameWidth;
    int bMargin = _sides[Flex::B]->isVisible() ? 1 : _frameWidth;

    if (self->isWindow())
    {
        self->setContentsMargins(lMargin, _titleBarHeight, rMargin, bMargin);
    }
    else
    {
        self->setContentsMargins(lMargin, tMargin, rMargin, bMargin);
    }

    if (_sideContainer)
    {
        _sideContainer->setGeometry(_siteContainer->geometry());
    }
}

void FlexWidgetImpl::updateViewMode(FlexWidget* self, Flex::ViewMode viewMode, bool first)
{
    if (_reserving)
    {
        return;
    }

    auto prevViewMode = self->viewMode();

    if (!self->isFloating() || viewMode == Flex::HybridView)
    {
        _viewMode = Flex::HybridView;
    }
    else
    {
        if (first)
        {
            if (viewMode == Flex::ToolPanesView || viewMode == Flex::ToolPagesView)
            {
                _viewMode = Flex::ToolView;
            }
            else
            {
                _viewMode = viewMode;
            }
        }
        else if ((prevViewMode == Flex::ToolView || prevViewMode == Flex::ToolPanesView || prevViewMode == Flex::ToolPagesView) && (viewMode == Flex::ToolView || viewMode == Flex::ToolPanesView || viewMode == Flex::ToolPagesView))
        {
            _viewMode = _sites.size() > 1 ? Flex::ToolPanesView : Flex::ToolPagesView;
        }
        else if ((prevViewMode == Flex::FileView || prevViewMode == Flex::FilePanesView || prevViewMode == Flex::FilePagesView) && (viewMode == Flex::FileView || viewMode == Flex::FilePanesView || viewMode == Flex::FilePagesView))
        {
            _viewMode = _sites.size() > 1 ? Flex::FilePanesView : Flex::FilePagesView;
        }
        else
        {
            _viewMode = Flex::HybridView;
        }
    }

    QEvent event((QEvent::Type)Flex::Update);

    for (auto site : _sites)
    {
        QApplication::sendEvent(site, &event);
    }

    if (_viewMode != prevViewMode)
    {
        update(self);
    }
}

void FlexWidgetImpl::updateViewMode(FlexWidget* self)
{
    if (_reserving)
    {
        return;
    }

    auto prevViewMode = self->viewMode();

    if (self->isFloating())
    {
        if (_sites.size() == 1)
        {
            _viewMode = _sites[0]->viewMode();
        }
        else if (!_sites.empty())
        {
            if (_viewMode != Flex::HybridView)
            {
                _viewMode = prevViewMode;
            }
            else if (std::all_of(_sites.begin(), _sites.end(), [&](DockSite* site) { return site->viewMode() == Flex::ToolView || site->viewMode() == Flex::ToolPanesView || site->viewMode() == Flex::ToolPagesView; }))
            {
                _viewMode = Flex::ToolPanesView;
            }
            else if (std::all_of(_sites.begin(), _sites.end(), [&](DockSite* site) { return site->viewMode() == Flex::FileView || site->viewMode() == Flex::FilePanesView || site->viewMode() == Flex::FilePagesView; }))
            {
                _viewMode = Flex::FilePanesView;
            }
        }
    }

    for (auto site : _sites)
    {
        QApplication::postEvent(site, new QEvent((QEvent::Type)Flex::Update));
    }

    if (_viewMode != prevViewMode)
    {
        update(self);
    }
}

void FlexWidgetImpl::update(FlexWidget* self)
{
    auto oldTitleBarHeight = _titleBarHeight;

    auto palette = self->palette();

    switch (self->viewMode())
    {
    case Flex::ToolView:
        palette.setColor(QPalette::Active, self->backgroundRole(), QColor("#FFF29D"));
        palette.setColor(QPalette::Inactive, self->backgroundRole(), QColor("#4D6082"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#FFF29D"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#4D6082"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#000000"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#FFFFFF"));
        _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) - 4;
        _frameWidth = 1;
        break;
    case Flex::ToolPanesView:
        palette.setColor(QPalette::All, self->backgroundRole(), QColor("#293955"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#FFFFFF"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#7F8899"));
        _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) + 8;
        _frameWidth = self->style()->pixelMetric(QStyle::PM_SplitterWidth, nullptr, self);
        break;
    case Flex::ToolPagesView:
        palette.setColor(QPalette::All, self->backgroundRole(), QColor("#293955"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#FFF29D"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#000000"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#FFFFFF"));
        _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) - 4;
        _frameWidth = 1;
        break;
    case Flex::FileView:
        palette.setColor(QPalette::All, self->backgroundRole(), QColor("#293955"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#FFFFFF"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#7F8899"));
        _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) + 8;
        _frameWidth = self->style()->pixelMetric(QStyle::PM_SplitterWidth, nullptr, self);
        break;
    case Flex::FilePanesView:
        palette.setColor(QPalette::All, self->backgroundRole(), QColor("#293955"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#FFFFFF"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#7F8899"));
        _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) + 8;
        _frameWidth = self->style()->pixelMetric(QStyle::PM_SplitterWidth, nullptr, self);
        break;
    case Flex::FilePagesView:
        palette.setColor(QPalette::All, self->backgroundRole(), QColor("#293955"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#FFFFFF"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#7F8899"));
        _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) + 8;
        _frameWidth = self->style()->pixelMetric(QStyle::PM_SplitterWidth, nullptr, self);
        break;
    case Flex::HybridView:
        palette.setColor(QPalette::All, self->backgroundRole(), QColor("#293955"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#FFFFFF"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#7F8899"));
        _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) + 8;
        _frameWidth = self->style()->pixelMetric(QStyle::PM_SplitterWidth, nullptr, self);
        break;
    }

    self->setPalette(palette);

    if (self->isWindow())
    {
        self->setContentsMargins(_frameWidth, _titleBarHeight, _frameWidth, _frameWidth);
    }
    else
    {
        self->setContentsMargins(_frameWidth, _frameWidth, _frameWidth, _frameWidth);
    }

    if (!self->isWindow())
    {
        return;
    }

    QWidget* parent = nullptr;

    auto dockPullButton = _helper->button(Flex::DockPull);
    auto autoHideButton = _helper->button(Flex::AutoHide);
    auto minButton = _helper->button(Flex::Minimize);
    auto maxButton = _helper->button(Flex::Maximize);
    auto clsButton = _helper->button(Flex::Close);

    switch (self->viewMode())
    {
    case Flex::ToolView:
        minButton->setFixedSize(16, 16);
        maxButton->setFixedSize(16, 16);
        clsButton->setFixedSize(16, 16);
        minButton->hide();
        dockPullButton->show();
        autoHideButton->hide();
        break;
    case Flex::ToolPanesView:
        minButton->setFixedSize(24, 24);
        maxButton->setFixedSize(24, 24);
        clsButton->setFixedSize(24, 24);
        minButton->hide();
        dockPullButton->hide();
        autoHideButton->hide();
        break;
    case Flex::ToolPagesView:
        minButton->setFixedSize(16, 16);
        maxButton->setFixedSize(16, 16);
        clsButton->setFixedSize(16, 16);
        minButton->hide();
        dockPullButton->show();
        autoHideButton->hide();
        break;
    case Flex::FileView:
        minButton->setFixedSize(24, 24);
        maxButton->setFixedSize(24, 24);
        clsButton->setFixedSize(24, 24);
        minButton->show();
        dockPullButton->hide();
        autoHideButton->hide();
        break;
    case Flex::FilePanesView:
        minButton->setFixedSize(24, 24);
        maxButton->setFixedSize(24, 24);
        clsButton->setFixedSize(24, 24);
        minButton->show();
        dockPullButton->hide();
        autoHideButton->hide();
        break;
    case Flex::FilePagesView:
        minButton->setFixedSize(24, 24);
        maxButton->setFixedSize(24, 24);
        clsButton->setFixedSize(24, 24);
        minButton->show();
        dockPullButton->hide();
        autoHideButton->hide();
        break;
    case Flex::HybridView:
        minButton->setFixedSize(24, 24);
        maxButton->setFixedSize(24, 24);
        clsButton->setFixedSize(24, 24);
        minButton->show();
        dockPullButton->hide();
        autoHideButton->hide();
        break;
    }

    _helper->setWindowInfo(_titleBarHeight, Flex::windowFlags(self->viewMode()));

    auto geometry = self->geometry();
    auto frameGeometry = self->frameGeometry();

    auto tl = geometry.topLeft() - frameGeometry.topLeft();
    auto br = frameGeometry.bottomRight() - geometry.bottomRight();

    auto prevVisible = self->isVisible();

    if ((parent = Flex::parent(_viewMode, self)) != self->parentWidget())
    {
        self->setParent(parent, self->windowFlags());
    }

    auto currVisible = self->isVisible();

    if (prevVisible && !_sites.empty())
    {
        self->setGeometry(QRect(geometry.topLeft() + tl, geometry.size() - QSize(tl.x(), tl.y()) - QSize(br.x(), br.y())));
    }

    self->setMinimumSize(self->minimumSizeHint());

    if (prevVisible && !_sites.empty())
    {
        if (prevVisible != currVisible)
        {
            self->show();
        }
        else
        {
            qApp->postEvent(self, new QShowEvent());
        }
        if (_titleBarHeight != oldTitleBarHeight)
        {
            qApp->postEvent(self, new QResizeEvent(self->size(), self->size()));
        }
    }
}

void FlexWidgetImpl::addDockSite(FlexWidget* self, DockSite* dockSite, Qt::Orientation orientation, int direction, QSplitter* parentSpliter)
{
	if (!parentSpliter)
	{
		parentSpliter = _siteContainer;
	}
    int width = parentSpliter->width();
    int height = parentSpliter->height();
    int space = parentSpliter->handleWidth() / 2;
    int newValue = (orientation == Qt::Horizontal ? width : height) / 2 - space;
    if (parentSpliter->count() == 1)
    {
		parentSpliter->setOrientation(orientation);
    }
    if (parentSpliter->orientation() == orientation)
    {
        auto sizes = parentSpliter->sizes();
        std::for_each(sizes.begin(), sizes.end(), [=](int& n) { n = 0.5f * n; });
        if (direction == 0)
        {
            sizes.prepend(newValue);
        }
        else
        {
            sizes.append(newValue);
        }
		//parentSpliter->insertWidget(0, dockSite);
		parentSpliter->addWidget(dockSite);
		parentSpliter->setSizes(sizes);
    }
    else
    {
        QSplitter* splitter = new QSplitter(orientation);
        splitter->setProperty("Flex", true);
        splitter->setChildrenCollapsible(false);
        if (direction == 0)
        {
            splitter->addWidget(dockSite);
            splitter->addWidget(parentSpliter);
        }
        else
        {
            splitter->addWidget(parentSpliter);
            splitter->addWidget(dockSite);
        }
        splitter->setSizes(QList<int>() << newValue << newValue);
        qobject_cast<QGridLayout*>(self->layout())->addWidget(splitter, 1, 1, 1, 1);
		parentSpliter->setObjectName(generate());
		_siteContainer = splitter;
		parentSpliter->setObjectName("_flex_siteContainer");
    }
}

void FlexWidgetImpl::addDockSite(FlexWidget* /*self*/, DockSite* dockSite, int siteIndex, Qt::Orientation orientation, int direction, int hint)
{
    DockSite* mainSite = _sites[siteIndex];
    QSplitter* splitter = qobject_cast<QSplitter*>(mainSite->parentWidget());
    int width = mainSite->width();
    int height = mainSite->height();
    int space = splitter->handleWidth() / 2;
    int index = splitter->indexOf(mainSite);
    int newValue = (orientation == Qt::Horizontal ? width : height) / 2;
    if (hint == 1)
    {
        if (orientation == Qt::Horizontal)
        {
            if (dockSite->width() < newValue)
            {
                newValue = dockSite->width();
            }
        }
        else
        {
            if (dockSite->height() < newValue)
            {
                newValue = dockSite->height();
            }
        }
    }
    int subValue = (orientation == Qt::Horizontal ? width : height) - newValue - space;
    newValue -= space;
    if (splitter->count() == 1)
    {
        splitter->setOrientation(orientation);
    }
    if (splitter->orientation() == orientation)
    {
        auto sizes = splitter->sizes();
        sizes.insert(index + direction, subValue);
        sizes[index + direction] = newValue;
        splitter->insertWidget(index + direction, dockSite);
        splitter->setSizes(sizes);
    }
    else
    {
        auto sizes = splitter->sizes();
        QSplitter* subSplitter = new QSplitter(orientation);
        subSplitter->setProperty("Flex", true);
        subSplitter->setChildrenCollapsible(false);
        subSplitter->setObjectName(generate());
        if (direction == 0)
        {
            subSplitter->addWidget(dockSite);
            subSplitter->addWidget(mainSite);
            subSplitter->setSizes(QList<int>() << newValue << subValue);
        }
        else
        {
            subSplitter->addWidget(mainSite);
            subSplitter->addWidget(dockSite);
            subSplitter->setSizes(QList<int>() << subValue << newValue);
        }
        splitter->insertWidget(index, subSplitter);
        splitter->setSizes(sizes);
    }
}

void FlexWidgetImpl::addFlexWidget(FlexWidget* self, FlexWidget* widget, Qt::Orientation orientation, int direction)
{
    QSplitter* widgetContainer = widget->siteContainer();
    int width = _siteContainer->width();
    int height = _siteContainer->height();
    int space = _siteContainer->handleWidth() / 2;
    int newValue = (orientation == Qt::Horizontal ? width : height) / 2 - space;
    if (_siteContainer->count() == 1)
    {
        _siteContainer->setOrientation(orientation);
    }
    if (widgetContainer->count() == 1)
    {
        widgetContainer->setOrientation(orientation);
    }
    if (_siteContainer->orientation() == orientation)
    {
        if (widgetContainer->orientation() == orientation)
        {
            auto rects = widgetContainer->sizes();
            int oldValue = std::accumulate(rects.begin(), rects.end(), 0);
            std::for_each(rects.begin(), rects.end(), [=](int& n) { n = 1.0f * n * newValue / oldValue; });
            auto sizes = _siteContainer->sizes();
            std::for_each(sizes.begin(), sizes.end(), [=](int& n) { n = 0.5f * n; });
            if (direction == 0)
            {
                for (int i = widgetContainer->count() - 1; i >= 0; --i)
                {
                    _siteContainer->insertWidget(0, widgetContainer->widget(i));
                }
                _siteContainer->setSizes(rects + sizes);
            }
            else
            {
                for (int i = 0; i < widgetContainer->count(); ++i)
                {
                    _siteContainer->addWidget(widgetContainer->widget(i));
                }
                _siteContainer->setSizes(sizes + rects);
            }
        }
        else
        {
            auto sizes = _siteContainer->sizes();
            std::for_each(sizes.begin(), sizes.end(), [=](int& n) { n = 0.5f * n; });
            if (direction == 0)
            {
                sizes.prepend(newValue);
                _siteContainer->insertWidget(0, widgetContainer);
            }
            else
            {
                sizes.append(newValue);
                _siteContainer->addWidget(widgetContainer);
            }
            _siteContainer->setSizes(sizes);
        }
    }
    else
    {
        if (widgetContainer->count() == 1)
        {
            addDockSite(self, static_cast<DockSite*>(widgetContainer->widget(0)), orientation, direction);
        }
        else
        {
            QSplitter* splitter = new QSplitter(orientation);
            splitter->setProperty("Flex", true);
            splitter->setChildrenCollapsible(false);
            if (direction == 0)
            {
                splitter->addWidget(widgetContainer);
                splitter->addWidget(_siteContainer);
            }
            else
            {
                splitter->addWidget(_siteContainer);
                splitter->addWidget(widgetContainer);
            }
            splitter->setSizes(QList<int>() << newValue << newValue);
            qobject_cast<QGridLayout*>(self->layout())->addWidget(splitter, 1, 1, 1, 1);
            _siteContainer->setObjectName(generate());
            _siteContainer = splitter;
            _siteContainer->setObjectName("_flex_siteContainer");
        }
    }
}

void FlexWidgetImpl::addFlexWidget(FlexWidget* self, FlexWidget* widget, int siteIndex, Qt::Orientation orientation, int direction, int hint)
{
    QSplitter* widgetContainer = widget->siteContainer();
    DockSite* mainSite = _sites[siteIndex];
    QSplitter* splitter = qobject_cast<QSplitter*>(mainSite->parentWidget());
    int width = mainSite->width();
    int height = mainSite->height();
    int space = splitter->handleWidth() / 2;
    int index = splitter->indexOf(mainSite);
    int newValue = (orientation == Qt::Horizontal ? width : height) / 2;
    if (hint == 1)
    {
        if (orientation == Qt::Horizontal)
        {
            if (widgetContainer->width() < newValue)
            {
                newValue = widgetContainer->width();
            }
        }
        else
        {
            if (widgetContainer->height() < newValue)
            {
                newValue = widgetContainer->height();
            }
        }
    }
    int subValue = (orientation == Qt::Horizontal ? width : height) - newValue - space;
    newValue -= space;
    if (splitter->count() == 1)
    {
        splitter->setOrientation(orientation);
    }
    if (widgetContainer->count() == 1)
    {
        widgetContainer->setOrientation(orientation);
    }
    if (splitter->orientation() == orientation)
    {
        if (widgetContainer->orientation() == orientation)
        {
            auto rects = widgetContainer->sizes();

            int oldValue = std::accumulate(rects.begin(), rects.end(), 0);

            std::for_each(rects.begin(), rects.end(), [=](int& n) { n = 1.0f * n * newValue / oldValue; });

            auto sizes = splitter->sizes();

            sizes[index] = subValue;

            QList<int> newSizes;

            std::copy(sizes.begin(), sizes.begin() + index + direction, std::back_inserter(newSizes));
            std::copy(rects.begin(), rects.end(), std::back_inserter(newSizes));
            std::copy(sizes.begin() + index + direction, sizes.end(), std::back_inserter(newSizes));

            for (int i = widgetContainer->count() - 1; i >= 0; --i)
            {
                splitter->insertWidget(index + direction, widgetContainer->widget(i));
            }

            splitter->setSizes(newSizes);
        }
        else
        {
            auto sizes = splitter->sizes();
            sizes.insert(index + direction, subValue);
            sizes[index + direction] = newValue;
            splitter->insertWidget(index + direction, widgetContainer);
            splitter->setSizes(sizes);
        }
    }
    else
    {
        if (widgetContainer->count() == 1)
        {
            addDockSite(self, static_cast<DockSite*>(widgetContainer->widget(0)), siteIndex, orientation, direction, hint);
        }
        else
        {
            auto sizes = splitter->sizes();
            auto subSplitter = new QSplitter(orientation);
            subSplitter->setProperty("Flex", true);
            subSplitter->setChildrenCollapsible(false);
            subSplitter->setObjectName(generate());
            if (direction == 0)
            {
                subSplitter->addWidget(widgetContainer);
                subSplitter->addWidget(mainSite);
                subSplitter->setSizes(QList<int>() << newValue << subValue);
            }
            else
            {
                subSplitter->addWidget(mainSite);
                subSplitter->addWidget(widgetContainer);
                subSplitter->setSizes(QList<int>() << subValue << newValue);
            }
            splitter->insertWidget(index, subSplitter);
            splitter->setSizes(sizes);
        }
    }
}

bool FlexWidgetImpl::isTitleBarVisible(FlexWidget* self, QRect* rect) const
{
    if (self->isFloating())
    {
        if (rect) *rect = QRect(0, 0, self->width(), _titleBarHeight); return true;
    }
    else
    {
        if (rect) *rect = QRect(); return false;
    }
}

void FlexWidgetImpl::simplify(FlexWidget*, DockSite* dockSite)
{
    auto adjusting = _adjusting;

    QSplitter* container = nullptr;

    _adjusting = true;

    if ((container = qobject_cast<QSplitter*>(dockSite->parentWidget())) != nullptr && container != _siteContainer)
    {
        if (container->count() == 1)
        {
            for (QSplitter* parent = static_cast<QSplitter*>(container->parentWidget()); parent != _siteContainer; parent = static_cast<QSplitter*>(parent->parentWidget()))
            {
                if (parent->count() == 1)
                {
                    container = parent;
                }
                else
                {
                    break;
                }
            }
            if (container)
            {
                container->deleteLater();
            }
        }
        else if (container->count() == 2)
        {
#if 0
            QSplitter* parentSplitter = qobject_cast<QSplitter*>(container->parentWidget());

            int i = container->indexOf(dockSite);

            QSplitter* remainSplitter = qobject_cast<QSplitter*>(container->widget(i == 0 ? 1 : 0));

            if (remainSplitter != nullptr)
            {
                int j = parentSplitter->indexOf(container);

                container->setParent(nullptr);

                parentSplitter->insertWidget(j, remainSplitter);

                remainSplitter->setObjectName(container->objectName());

                container->deleteLater();
            }
#endif
        }
    }

    _adjusting = adjusting;
}

QString FlexWidgetImpl::generate() const
{
    return QUuid::createUuid().toString().toUpper();
}

FlexWidget::FlexWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags) : QWidget(parent, flags), impl(new FlexWidgetImpl)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::NoFocus);
    setAutoFillBackground(true);
    setProperty("Flex", true);

    impl->_viewMode = viewMode;

    impl->_siteContainer = new QSplitter(this);
    impl->_sideContainer = new QSplitter(this);

    impl->_sides[Flex::L] = new DockSide(Flex::L, impl->_sideContainer, this);
    impl->_sides[Flex::L]->setObjectName("_flex_lSide");
    connect(impl->_sides[Flex::L], SIGNAL(currentChanged(DockSide*, DockSite*, DockSite*)), SLOT(on_side_currentChanged(DockSide*, DockSite*, DockSite*)));
    impl->_sides[Flex::T] = new DockSide(Flex::T, impl->_sideContainer, this);
    impl->_sides[Flex::T]->setObjectName("_flex_tSide");
    connect(impl->_sides[Flex::T], SIGNAL(currentChanged(DockSide*, DockSite*, DockSite*)), SLOT(on_side_currentChanged(DockSide*, DockSite*, DockSite*)));
    impl->_sides[Flex::R] = new DockSide(Flex::R, impl->_sideContainer, this);
    impl->_sides[Flex::R]->setObjectName("_flex_rSide");
    connect(impl->_sides[Flex::R], SIGNAL(currentChanged(DockSide*, DockSite*, DockSite*)), SLOT(on_side_currentChanged(DockSide*, DockSite*, DockSite*)));
    impl->_sides[Flex::B] = new DockSide(Flex::B, impl->_sideContainer, this);
    impl->_sides[Flex::B]->setObjectName("_flex_bSide");
    connect(impl->_sides[Flex::B], SIGNAL(currentChanged(DockSide*, DockSite*, DockSite*)), SLOT(on_side_currentChanged(DockSide*, DockSite*, DockSite*)));

    impl->_siteContainer->setProperty("Flex", true);
    impl->_siteContainer->setObjectName("_flex_siteContainer");
    impl->_siteContainer->setChildrenCollapsible(false);
    impl->_sideContainer->setProperty("Flex", true);
    impl->_sideContainer->setObjectName("_flex_sideContainer");
    impl->_sideContainer->setChildrenCollapsible(false);

    auto arranger = new QGridLayout(this);
    arranger->setContentsMargins(0, 0, 0, 0);
    arranger->setHorizontalSpacing(0);
    arranger->setVerticalSpacing(0);

    arranger->addWidget(impl->_sides[Flex::L], 0, 0, 3, 1);
    arranger->addWidget(impl->_sides[Flex::T], 0, 1, 1, 1);
    arranger->addWidget(impl->_sides[Flex::R], 0, 2, 3, 1);
    arranger->addWidget(impl->_sides[Flex::B], 2, 1, 1, 1);
    arranger->addWidget(impl->_siteContainer, 1, 1, 1, 1);

    arranger->setColumnStretch(1, 1);
    arranger->setRowStretch(1, 1);

    for (int i = 0; i < 4; ++i)
    {
        impl->_sides[i]->hide();
    }

    if (isWindow())
    {
        impl->_helper = new FlexHelper(this);
    }
    else
    {
        addDockSite(new DockSite(), Flex::M, -1);
    }

    impl->update(this);

    if (impl->_helper)
    {
        connect(impl->_helper, SIGNAL(clicked(Flex::Button, bool*)), SLOT(on_titleBar_buttonClicked(Flex::Button, bool*)));
    }

    impl->_hole = new FlexWidgetHole();

    impl->_sideContainer->setHandleWidth(3);
    impl->_sideContainer->addWidget(impl->_hole);
    impl->_sideContainer->setStretchFactor(0, 4);
    impl->_sideContainer->hide();

    if (impl->_siteContainer->count() > 0)
    {
        impl->_siteContainer->setStretchFactor(0, 1);
    }

#ifdef Q_OS_WIN
    winId();
#endif

}

FlexWidget::~FlexWidget()
{
    emit destroying(this);

    if (impl->_guider)
    {
        impl->_guider->deleteLater(); impl->_guider = NULL;
    }
}

bool FlexWidget::event(QEvent* evt)
{
    if (evt->type() == QEvent::WindowActivate)
    {
        for (auto dockSide : impl->_sides)
        {
            if (dockSide->current() != impl->_current)
            {
                dockSide->doneCurrent();
            }
        }

        if (impl->_current)
        {
            impl->_current->setActive(true);
        }
    }
    else if (evt->type() == QEvent::WindowDeactivate)
    {
        for (auto dockSide : impl->_sides)
        {
            if (dockSide->current() != impl->_current)
            {
                dockSide->doneCurrent();
            }
        }

        if (impl->_current)
        {
            impl->_current->setActive(false);
        }
    }
    return QWidget::event(evt);
}

void FlexWidget::paintEvent(QPaintEvent*)
{
    if (!isWindow())
    {
        return;
    }

    QRect titleBarRect;

    QStylePainter painter(this);

    if (impl->isTitleBarVisible(this, &titleBarRect))
    {
        impl->_helper->buttons()->show();
        impl->_helper->extents()->show();

        QStyleOptionTitleBar titleOption;
        titleOption.init(this);
        titleOption.rect = titleBarRect;
        titleOption.text = windowTitle();
        titleOption.icon = windowIcon();
        titleOption.subControls = QStyle::SC_TitleBarLabel;
        titleOption.titleBarState = windowState();
        titleOption.titleBarFlags = Flex::windowFlags(impl->_viewMode);

        bool hasIcon = titleOption.titleBarFlags & Qt::WindowSystemMenuHint;

        if (impl->_viewMode == Flex::ToolPagesView)
        {
            painter.fillRect(titleBarRect, titleOption.palette.color(QPalette::Highlight));
        }

        QRect lr = style()->subControlRect(QStyle::CC_TitleBar, &titleOption, QStyle::SC_TitleBarLabel, this);

        if (hasIcon)
        {
            titleOption.icon.paint(&painter, 10, (impl->_titleBarHeight - 24) / 2, 24, 24, Qt::AlignCenter, QIcon::Active, isActive() ? QIcon::On : QIcon::Off);
        }

        painter.drawText(8 + (hasIcon ? 32 : 0), (impl->_titleBarHeight - lr.height()) / 2, lr.width() - 2, lr.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, titleOption.text);

        QStyleOptionFrame frameOption;
        frameOption.initFrom(this);
        frameOption.lineWidth = 1;
        frameOption.midLineWidth = impl->_frameWidth - 1;
        painter.drawPrimitive(QStyle::PE_Frame, frameOption);
    }
    else
    {
        impl->_helper->buttons()->hide();
        impl->_helper->extents()->hide();
    }
}

void FlexWidget::resizeEvent(QResizeEvent*)
{
    if (impl->_sideContainer)
    {
        impl->_sideContainer->setGeometry(impl->_siteContainer->geometry());
    }
}

#ifdef Q_OS_WIN

#define QT_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define QT_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#endif

bool FlexWidget::nativeEvent(const QByteArray& eventType, void * message, long *result)
{
#ifdef Q_OS_WIN
    MSG* msg = reinterpret_cast<MSG*>(message);

    if (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MOUSELAST)
    {
        if (impl->_sideContainer->isVisible())
        {
            int x = QT_GET_X_LPARAM(msg->lParam);
            int y = QT_GET_Y_LPARAM(msg->lParam);

            auto pos = impl->_sideContainer->mapFromParent(QPoint(x, y));

            impl->_sideContainer->setAttribute(Qt::WA_TransparentForMouseEvents, impl->_hole->geometry().contains(pos));
        }
    }
#endif
    if (impl->_helper && impl->_helper->nativeEvent(eventType, message, result))
    {
        return true;
    }
    else
    {
        return QWidget::nativeEvent(eventType, message, result);
    }
}

void FlexWidget::makeSiteAutoHide(DockSite* dockSite)
{
    if (dockSite->dockMode() != Flex::DockInMainArea || !impl->_sites.contains(dockSite))
    {
        return;
    }

    auto mainSite = impl->_sites[0];

    auto direction = Flex::L;

    if (dockSite != mainSite)
    {
        auto mTl = mainSite->mapTo(this, QPoint(0, 0));
        auto dTl = dockSite->mapTo(this, QPoint(0, 0));
        auto mBr = mainSite->mapTo(this, QPoint(mainSite->width(), mainSite->height()));
        auto dBr = dockSite->mapTo(this, QPoint(dockSite->width(), dockSite->height()));

        if (dTl.x() < mTl.x() && dBr.x() <= mTl.x())
        {
            direction = Flex::L;
        }
        else if (dTl.y() < mTl.y() && dBr.y() <= mTl.y())
        {
            direction = Flex::T;
        }
        else if (dTl.x() > mBr.x() && dBr.x() >= mBr.x())
        {
            direction = Flex::R;
        }
        else
        {
            direction = Flex::B;
        }
    }

    impl->_reserving = true;
    while (dockSite->count() > 0)
    {
        impl->_sides[direction]->attachDockSite(new DockSite(dockSite->widget(0), dockSite->size(), impl->_sideContainer));
    }
    impl->_reserving = false;

    impl->updateDockSides(this);
}

void FlexWidget::makeSiteDockShow(DockSite* dockSite)
{
    if (dockSite->dockMode() != Flex::DockInSideArea || impl->_sites.contains(dockSite))
    {
        return;
    }

    for (auto dockSide : impl->_sides)
    {
        if (dockSide->hasDockSite(dockSite) && dockSide->detachDockSite(dockSite))
        {
            addDockSite(dockSite, Flex::M, -1);
        }
    }

    impl->updateDockSides(this);
}

void FlexWidget::showSiteDockPull(DockSite*)
{
}

bool FlexWidget::load(const QJsonObject& object)
{
    clearDockSites(true);

    QJsonArray dockSites = object["dockSites"].toArray();
//     for (int i = 0; i < dockSites.count(); ++i)
//     {
//         addDockSite(new DockSite());
//     }
	impl->_siteContainer->setOrientation((Qt::Orientation)dockSites[0].toObject()["orientation"].toInt());
	impl->_adjusting = true;
    bool result = impl->load(this, dockSites, impl->_siteContainer);
    impl->_adjusting = false;

    impl->_siteContainer->setStretchFactor(0, 1);

    QJsonArray dockSides = object["dockSides"].toArray();

    if (!dockSides.empty())
    {
        impl->_sides[Flex::L]->load(dockSides[0].toObject());
        impl->_sides[Flex::T]->load(dockSides[1].toObject());
        impl->_sides[Flex::R]->load(dockSides[2].toObject());
        impl->_sides[Flex::B]->load(dockSides[3].toObject());
    }

    impl->updateDockSides(this);

    impl->updateViewMode(this, viewMode(), impl->_sites.size() == 1);

    impl->update(this);

    if (!impl->_sites.empty())
    {
        impl->_sites[0]->show(); impl->_sites[0]->setFocus();
    }

    if (isFloating())
    {
        restoreGeometry(QByteArray::fromBase64(object["geometry"].toString().toLatin1()));
    }

    return result;
}

bool FlexWidget::save(QJsonObject& object) const
{
    QJsonArray dockSites;
    bool result = impl->save(this, dockSites, impl->_siteContainer);
    object["dockSites"] = dockSites;

    QJsonArray dockSides;
    QJsonObject lSide;
    impl->_sides[Flex::L]->save(lSide);
    dockSides.append(lSide);
    QJsonObject tSide;
    impl->_sides[Flex::T]->save(tSide);
    dockSides.append(tSide);
    QJsonObject rSide;
    impl->_sides[Flex::R]->save(rSide);
    dockSides.append(rSide);
    QJsonObject bSide;
    impl->_sides[Flex::B]->save(bSide);
    dockSides.append(bSide);

    object["dockSides"] = dockSides;

    if (isFloating())
    {
        object["geometry"] = QString(saveGeometry().toBase64());
    }

    return result;
}

QByteArray FlexWidget::snapshot() const
{
    QJsonObject object;

    object["viewMode"] = (int)viewMode();
    object["windowFlags"] = (int)Flex::windowFlags(viewMode());
    object["flexWidgetName"] = objectName();

    save(object);

#ifdef _DEBUG
    return QJsonDocument(object).toJson(QJsonDocument::Indented);
#else
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
#endif
}

bool FlexWidget::restore(const QByteArray& snapshot, const QString& identifer)
{
    QStringList dockWidgetPath = identifer.split(",");

    int dockWidgetIndex = -1;

    int length = dockWidgetPath.size();

    QJsonObject flexWidgetObject = QJsonDocument::fromJson(snapshot).object();

    QJsonObject dockWidgetObject = impl->find(flexWidgetObject, dockWidgetPath, dockWidgetIndex);

    DockSite* dockSite = nullptr;

    QByteArray flexWidgetTemp = this->snapshot();

    if ((dockSite = this->dockSite(dockWidgetPath[length - 3])) != nullptr)
    {
        if (!dockWidgetObject.isEmpty())
        {
            Flex::ViewMode viewMode = (Flex::ViewMode)dockWidgetObject["viewMode"].toInt();

            QString dockWidgetName = dockWidgetObject["dockWidgetName"].toString();

            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(viewMode, dockSite, Flex::widgetFlags(), dockWidgetName);

            dockWidget->load(dockWidgetObject);

            dockSite->insertWidget(dockWidget, dockWidgetIndex);

            dockWidget->activate();

            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (length <= 1)
        {
            return false;
        }
        else
        {
            if (dockWidgetPath[1] == "_flex_siteContainer")
            {
                QSplitter* parSplitter = impl->_siteContainer;

                QJsonArray objects = flexWidgetObject["dockSites"].toArray();

                auto adjusting = impl->_adjusting;

                impl->_adjusting = true;

                for (int i = 2; i < dockWidgetPath.length() - 3; i += 2)
                {
                    int number = dockWidgetPath[i].toInt();

                    QJsonObject target = objects[number].toObject();

                    if (target["type"].toString() == "wrapper")
                    {
                        QSplitter* subSplitter = nullptr;

                        QString subSplitterName = target["name"].toString();

                        if ((subSplitter = parSplitter->findChild<QSplitter*>(subSplitterName)) == nullptr)
                        {
                            subSplitter = new QSplitter();
                            subSplitter->setProperty("Flex", true);
                            subSplitter->setObjectName(subSplitterName);
                            subSplitter->setChildrenCollapsible(false);
                            parSplitter->insertWidget(number, subSplitter);
                        }

                        QList<int> sizes;

                        for (int j = 0; j < objects.count(); ++j)
                        {
                            sizes << objects[j].toObject()["size"].toInt();
                        }

                        parSplitter->setSizes(sizes);

                        parSplitter = subSplitter;

                        objects = target["value"].toArray();
                    }
                    else
                    {
                        QJsonObject median = target["value"].toObject();

                        QList<int> sizes;

                        for (int j = 0; j < objects.count(); ++j)
                        {
                            QJsonObject source = objects[j].toObject();

                            if (parSplitter)
                            {
                                parSplitter->setOrientation((Qt::Orientation)source["orientation"].toInt());
                            }

                            sizes << source["size"].toInt();
                        }

                        int dockWidgetIndex = dockWidgetPath[i + 2].toInt();

                        objects = median["widgets"].toArray();

                        median["widgets"] = QJsonArray() << objects[dockWidgetIndex];

                        DockSite* tempSite = new DockSite();

                        tempSite->load(median);

                        impl->_sites.append(tempSite);

                        parSplitter->insertWidget(number, tempSite);

                        parSplitter->setSizes(sizes);

                        tempSite->widget(0)->activate();
                    }
                }

                impl->_adjusting = adjusting;

                impl->updateViewMode(this, viewMode(), impl->_sites.size() == 1);

                impl->update(this);

                return true;
            }
            if (dockWidgetPath[1] == "_flex_sideContainer")
            {
                QSplitter* parSplitter = impl->_siteContainer;

                QJsonArray objects = flexWidgetObject["dockSides"].toArray();

                QString dockSiteName = dockWidgetPath[length - 3];

                QJsonObject median;

                QJsonArray lDockSites = objects[Flex::L].toObject()["dockSites"].toArray();
                QJsonArray tDockSites = objects[Flex::T].toObject()["dockSites"].toArray();
                QJsonArray rDockSites = objects[Flex::R].toObject()["dockSites"].toArray();
                QJsonArray bDockSites = objects[Flex::B].toObject()["dockSites"].toArray();

                Flex::Direction direction = Flex::C;

                if (median.isEmpty())
                {
                    for (int i = 0; i < lDockSites.size(); ++i)
                    {
                        if (lDockSites[i].toObject()["name"].toString() == dockSiteName)
                        {
                            direction = Flex::L; median = lDockSites[i].toObject(); break;
                        }
                    }
                }
                if (median.isEmpty())
                {
                    for (int i = 0; i < tDockSites.size(); ++i)
                    {
                        if (tDockSites[i].toObject()["name"].toString() == dockSiteName)
                        {
                            direction = Flex::T; median = tDockSites[i].toObject(); break;
                        }
                    }
                }
                if (median.isEmpty())
                {
                    for (int i = 0; i < rDockSites.size(); ++i)
                    {
                        if (rDockSites[i].toObject()["name"].toString() == dockSiteName)
                        {
                            direction = Flex::R; median = rDockSites[i].toObject(); break;
                        }
                    }
                }
                if (median.isEmpty())
                {
                    for (int i = 0; i < bDockSites.size(); ++i)
                    {
                        if (bDockSites[i].toObject()["name"].toString() == dockSiteName)
                        {
                            direction = Flex::B; median = bDockSites[i].toObject(); break;
                        }
                    }
                }

                if (direction == Flex::C)
                {
                    return false;
                }

                DockSite* tempSite = new DockSite(NULL, QSize(), impl->_sideContainer);

                tempSite->load(median);

                auto reserving = impl->_reserving;
                impl->_reserving = true;
                impl->_sides[direction]->attachDockSite(tempSite);
                impl->_reserving = reserving;

                impl->updateDockSides(this);

                impl->_sides[direction]->makeCurrent(tempSite);

                return true;
            }
        }
    }

    return true;
}

void FlexWidget::showGuider(QWidget* widget)
{
    auto viewMode = static_cast<Flex::ViewMode>(widget->property("viewMode").toInt());

    QRect geom = impl->_siteContainer->geometry();

    QPoint pos = mapToGlobal(geom.topLeft());

    impl->_guider = new DockGuider(widget->rect(), viewMode);

    impl->_guider->setGeometry(pos.x(), pos.y(), geom.width(), geom.height());

    for (auto site : impl->_sites)
    {
        impl->_guider->addSite(QRect(site->mapTo(this, QPoint(0, 0)) - geom.topLeft(), site->size()), site->viewMode());
    }

    qApp->postEvent(impl->_guider, new QMouseEvent(QEvent::MouseMove, impl->_guider->mapFromGlobal(QCursor::pos()), Qt::LeftButton, Qt::LeftButton, 0));

    impl->_guider->show();
}

void FlexWidget::hideGuider(QWidget*)
{
    if (impl->_guider)
    {
        impl->_guider->deleteLater(); impl->_guider = NULL;
    }
}

void FlexWidget::hoverGuider(QWidget*)
{
    if (impl->_guider)
    {
        qApp->postEvent(impl->_guider, new QMouseEvent(QEvent::MouseMove, impl->_guider->mapFromGlobal(QCursor::pos()), Qt::LeftButton, Qt::LeftButton, 0));
    }
}

bool FlexWidget::dropGuider(DockWidget* widget)
{
    if (impl->_guider)
    {
        impl->_guider->hide();
    }

    bool result = false;

    if (impl->_guider && impl->_guider->area() != Flex::None)
    {
        result = addDockWidget(widget, impl->_guider->area(), impl->_guider->siteIndex());
    }

    if (impl->_guider)
    {
        impl->_guider->deleteLater(); impl->_guider = NULL;
    }

    return result;
}

bool FlexWidget::dropGuider(FlexWidget* widget)
{
    if (impl->_guider)
    {
        impl->_guider->hide();
    }

    bool result = false;

    if (impl->_guider && impl->_guider->area() != Flex::None)
    {
        result = addFlexWidget(widget, impl->_guider->area(), impl->_guider->siteIndex());
    }

    if (result)
    {
        if (widget->testAttribute(Qt::WA_DeleteOnClose))
        {
            widget->close();
        }
    }

    if (impl->_guider)
    {
        impl->_guider->deleteLater(); impl->_guider = NULL;
    }

    return result;
}

QSize FlexWidget::sizeHint() const
{
    return isFloating() ? QSize(640, 480) : QWidget::sizeHint();
}

QSize FlexWidget::minimumSizeHint() const
{
    return isFloating() ? QSize(80, 40) : QWidget::minimumSizeHint();
}

bool FlexWidget::isGuiderExists() const
{
    return impl->_guider != nullptr;
}

bool FlexWidget::isGuiderVisible() const
{
    return impl->_guider ? impl->_guider->isVisible() : false;
}

bool FlexWidget::isDockAllowed(DockWidget* widget, const QPoint& pos)
{
    if (!rect().contains(pos))
    {
        return false;
    }

    for (auto iter = impl->_sites.begin(); iter != impl->_sites.end(); ++iter)
    {
        if (QRect((*iter)->mapTo(this, QPoint(0, 0)), (*iter)->size()).contains(pos))
        {
            return ((*iter)->features() & widget->dockFeatures()) != 0;
        }
    }

    return true;
}

bool FlexWidget::isDockAllowed(FlexWidget* widget, const QPoint& pos)
{
    if (!rect().contains(pos))
    {
        return false;
    }

    for (auto iter = impl->_sites.begin(); iter != impl->_sites.end(); ++iter)
    {
        if (QRect((*iter)->mapTo(this, QPoint(0, 0)), (*iter)->size()).contains(pos))
        {
            return ((*iter)->features() & widget->dockFeatures()) != 0;
        }
    }

    return true;
}

bool FlexWidget::addDockSite(DockSite* dockSite, Flex::DockArea area, int siteIndex)
{
    Q_ASSERT(siteIndex < impl->_sites.size());

    auto siteViewMode = dockSite->viewMode();

    bool first = impl->_sites.empty();

    impl->_adjusting = true;

    impl->_sites.append(dockSite);

    switch (area)
    {
    case Flex::M:
    {
        if (siteIndex < 0)
        {
            impl->_siteContainer->addWidget(dockSite);
        }
        break;
    }
    case Flex::L0:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Horizontal, 0, 0);
        break;
    }
    case Flex::L1:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Horizontal, 0, 1);
        break;
    }
    case Flex::L2:
    {
        impl->addDockSite(this, dockSite, Qt::Horizontal, 0);
        break;
    }
    case Flex::T0:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Vertical, 0, 0);
        break;
    }
    case Flex::T1:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Vertical, 0, 1);
        break;
    }
    case Flex::T2:
    {
        impl->addDockSite(this, dockSite, Qt::Vertical, 0);
        break;
    }
    case Flex::R0:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Horizontal, 1, 0);
        break;
    }
    case Flex::R1:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Horizontal, 1, 1);
        break;
    }
    case Flex::R2:
    {
        impl->addDockSite(this, dockSite, Qt::Horizontal, 1);
        break;
    }
    case Flex::B0:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Vertical, 1, 0);
        break;
    }
    case Flex::B1:
    {
        impl->addDockSite(this, dockSite, siteIndex, Qt::Vertical, 1, 1);
        break;
    }
    case Flex::B2:
    {
        impl->addDockSite(this, dockSite, Qt::Vertical, 1);
        break;
    }
    default:
    {
        Q_ASSERT(false);
    }
    }

    impl->_adjusting = false;

    impl->updateViewMode(this, siteViewMode, first);

    if (dockSite)
    {
        dockSite->show(); dockSite->setFocus();
    }

    return true;
}

bool FlexWidget::addDockWidget(DockWidget* widget, Flex::DockArea area, int siteIndex)
{
    if (area != Flex::M || siteIndex < 0)
    {
        return addDockSite(new DockSite(widget), area, siteIndex);
    }

    bool result = impl->_sites[siteIndex]->addWidget(widget);

    QEvent event((QEvent::Type)Flex::Update);

    for (auto site : impl->_sites)
    {
        QApplication::sendEvent(site, &event);
    }

    impl->_sites[siteIndex]->setFocus();

    return result;
}

bool FlexWidget::addFlexWidget(FlexWidget* widget, Flex::DockArea area, int siteIndex)
{
    Q_ASSERT(siteIndex < impl->_sites.size());

    auto flexViewMode = widget->viewMode();

    bool first = impl->_sites.empty();

    widget->hide();

    if (area == Flex::M && siteIndex >= 0)
    {
        for (int i = Flex::C; i <= Flex::B; i++)
        {
            for (auto dockSite : widget->dockSites(static_cast<Flex::Direction>(i)))
            {
                for (auto dockWidget : dockSite->widgets())
                {
                    impl->_sites[siteIndex]->addWidget(dockWidget);
                }
            }
        }
    }
    else
    {
        QSplitter* widgetContainer = widget->siteContainer();

        widgetContainer->setObjectName(impl->generate());

        for (auto dockSite : widget->dockSites(Flex::C))
        {
            impl->_sites.append(dockSite);
        }

        impl->_adjusting = true;

        switch (area)
        {
        case Flex::M:
        {
            if (siteIndex < 0)
            {
                impl->_siteContainer->addWidget(widgetContainer);
            }
            break;
        }
        case Flex::L0:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Horizontal, 0, 0);
            break;
        }
        case Flex::L1:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Horizontal, 0, 1);
            break;
        }
        case Flex::L2:
        {
            impl->addFlexWidget(this, widget, Qt::Horizontal, 0);
            break;
        }
        case Flex::T0:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Vertical, 0, 0);
            break;
        }
        case Flex::T1:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Vertical, 0, 1);
            break;
        }
        case Flex::T2:
        {
            impl->addFlexWidget(this, widget, Qt::Vertical, 0);
            break;
        }
        case Flex::R0:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Horizontal, 1, 0);
            break;
        }
        case Flex::R1:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Horizontal, 1, 1);
            break;
        }
        case Flex::R2:
        {
            impl->addFlexWidget(this, widget, Qt::Horizontal, 1);
            break;
        }
        case Flex::B0:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Vertical, 1, 0);
            break;
        }
        case Flex::B1:
        {
            impl->addFlexWidget(this, widget, siteIndex, Qt::Vertical, 1, 1);
            break;
        }
        case Flex::B2:
        {
            impl->addFlexWidget(this, widget, Qt::Vertical, 1);
            break;
        }
        default:
        {
            Q_ASSERT(false);
        }
        }

        for (int i = Flex::L; i <= Flex::B; i++)
        {
            for (auto dockSite : widget->dockSites(static_cast<Flex::Direction>(i)))
            {
                impl->_sides[static_cast<Flex::Direction>(i)]->attachDockSite(new DockSite(dockSite->widget(0), dockSite->baseSize(), impl->_sideContainer));
            }
        }

        impl->updateDockSides(this);

        impl->_adjusting = false;
    }

    impl->updateViewMode(this, flexViewMode, first);

    if (area == Flex::M && siteIndex >= 0)
    {
        impl->_sites[siteIndex]->setFocus();
    }
    else
    {
        if (!impl->_sites.empty())
        {
            impl->_sites.back()->setFocus();
        }
    }

    return true;
}

bool FlexWidget::removeDockSite(DockSite* dockSite)
{
    Q_ASSERT(dockSite != nullptr);

    if (impl->_current == dockSite)
    {
        impl->_current = nullptr;
    }

    auto adjusting = impl->_adjusting;

    if (dockSite->dockMode() == Flex::DockInNoneArea)
    {
        qWarning() << "Dock site is not docked in this flexWidget!"; return false;
    }

    if (dockSite->dockMode() == Flex::DockInSideArea)
    {
        for (auto dockSide : impl->_sides)
        {
            if (dockSide->hasDockSite(dockSite))
            {
                dockSide->detachDockSite(dockSite); break;
            }
        }

        impl->_adjusting = true;
        if (dockSite->count() == 0)
        {
            dockSite->setParent(nullptr); dockSite->deleteLater();
        }
        impl->_adjusting = adjusting;

        impl->updateViewMode(this);
        impl->updateDockSides(this);

        return true;
    }

    impl->simplify(this, dockSite);

    dockSite->setActive(false);

    bool result = impl->_sites.removeOne(dockSite);

    impl->_adjusting = true;
    if (dockSite->count() == 0)
    {
        dockSite->setParent(nullptr); dockSite->deleteLater();
    }
    impl->_adjusting = adjusting;

    impl->updateViewMode(this);

    return result;
}

void FlexWidget::clearDockSites(bool all)
{
    impl->_current = nullptr;

    impl->_hole->setParent(nullptr);

    auto adjusting = impl->_adjusting;
    impl->_adjusting = true;
    while (!impl->_sites.empty())
    {
        auto dockSite = impl->_sites.back();

        dockSite->setParent(nullptr);

        dockSite->deleteLater();

        impl->_sites.pop_back();
    }
    impl->_adjusting = adjusting;

    while (impl->_siteContainer->count() > 0)
    {
        QWidget* widget = impl->_siteContainer->widget(0);
        widget->setParent(nullptr);
        widget->deleteLater();
    }

    auto reserving = impl->_reserving;
    impl->_reserving = true;
    for (int i = Flex::L; i <= Flex::B; ++i)
    {
        for (int j = 0; j < this->count(static_cast<Flex::Direction>(i)); ++j)
        {
            auto dockSite = this->dockSite(j, static_cast<Flex::Direction>(i));
            impl->_sides[i]->detachDockSite(dockSite);
            dockSite->deleteLater();
        }
        impl->_sides[i]->hide();
    }
    impl->_reserving = reserving;

    while (impl->_siteContainer->count() > 0)
    {
        QWidget* widget = impl->_sideContainer->widget(0);
        widget->setParent(nullptr);
        widget->deleteLater();
    }

    impl->_sideContainer->hide();

    if (!all)
    {
        if (!isWindow())
        {
            addDockSite(new DockSite(), Flex::M, -1);
        }
    }

    impl->updateViewMode(this);

    impl->updateDockSides(this);
}

bool FlexWidget::isActive() const
{
    return isActiveWindow();
}

Flex::ViewMode FlexWidget::viewMode() const
{
    return impl->_viewMode;
}

Flex::Features FlexWidget::dockFeatures() const
{
    return Flex::AllowDockAsNorthTabPage | Flex::AllowDockAsSouthTabPage;
}

DockSite* FlexWidget::current() const
{
    return impl->_current;
}

void FlexWidget::setCurrent(DockSite* current)
{
    if (impl->_current != current)
    {
        for (auto dockSide : impl->_sides)
        {
            if (dockSide->current() != current)
            {
                dockSide->doneCurrent();
            }
        }

        if (impl->_current)
        {
            impl->_current->setActive(false);
        }

        impl->_current = current;

        if (impl->_current)
        {
            impl->_current->setActive(true);
        }
    }
}

DockSide* FlexWidget::dockSide(Flex::Direction direction) const
{
    Q_ASSERT(direction != Flex::C);
    return impl->_sides[direction];
}

int FlexWidget::count() const
{
    return count(Flex::C) + count(Flex::L) + count(Flex::T) + count(Flex::R) + count(Flex::B);
}

int FlexWidget::count(Flex::Direction direction) const
{
    switch (direction)
    {
    case Flex::C:
        return impl->_sites.size();
    default:
        return impl->_sides[direction]->count();
    }
}

DockSite* FlexWidget::dockSite(int index, Flex::Direction direction) const
{
    switch (direction)
    {
    case Flex::C:
        return impl->_sites[index];
    default:
        return impl->_sides[direction]->dockSite(index);
    }
}

const QList<DockSite*>& FlexWidget::dockSites(Flex::Direction direction) const
{
    switch (direction)
    {
    case Flex::C:
        return impl->_sites;
    default:
        return impl->_sides[direction]->dockSites();
    }
}

DockSite* FlexWidget::dockSite(const QString& name) const
{
    auto iter = std::find_if(impl->_sites.begin(), impl->_sites.end(), [&](const DockSite* tempSite) { return tempSite->objectName() == name; });

    if (iter != impl->_sites.end())
    {
        return *iter;
    }

    DockSite* tempSite = nullptr;

    if ((tempSite = impl->_sides[Flex::L]->dockSite(name)) != nullptr)
    {
        return tempSite;
    }

    if ((tempSite = impl->_sides[Flex::T]->dockSite(name)) != nullptr)
    {
        return tempSite;
    }

    if ((tempSite = impl->_sides[Flex::R]->dockSite(name)) != nullptr)
    {
        return tempSite;
    }

    if ((tempSite = impl->_sides[Flex::B]->dockSite(name)) != nullptr)
    {
        return tempSite;
    }

    return nullptr;
}

QSplitter* FlexWidget::siteContainer() const
{
    return impl->_siteContainer;
}

QSplitter* FlexWidget::sideContainer() const
{
    return impl->_sideContainer;
}

bool FlexWidget::isAdjusting() const
{
    return impl->_adjusting;
}

bool FlexWidget::isFloating() const
{
    return isTopLevel();
}

void FlexWidget::on_titleBar_buttonClicked(Flex::Button button, bool* accepted)
{
    if (button == Flex::Close)
    {
        FlexManager::instance()->snapshot(this);
    }

    if (button != Flex::Close || (impl->_viewMode != Flex::ToolPagesView))
    {
        return;
    }

    if (impl->_sites.size() == 1 && impl->_sites[0]->count() > 1)
    {
        impl->_sites[0]->removeWidgetAt(impl->_sites[0]->currentIndex()); *accepted = true;
    }
    else
    {
        *accepted = false;
    }
}

void FlexWidget::on_side_currentChanged(DockSide* side, DockSite* prev, DockSite* curr)
{
    if (prev != nullptr)
    {
        qApp->blockSignals(true);
        prev->hide();
        qApp->blockSignals(false);

        if (curr != nullptr || (curr == nullptr && !impl->_reserving))
        {
            prev->setBaseSize(prev->size());
        }
    }

    if (curr == nullptr)
    {
        if (!impl->_reserving)
        {
            impl->_sideContainer->hide();
        }
    }
    else
    {
        for (auto dockSide : impl->_sides)
        {
            if (dockSide != side && dockSide->current() != nullptr)
            {
                dockSide->current()->setBaseSize(dockSide->current()->size());
            }
        }

        curr->raise();
        curr->show();

        switch (side->direction())
        {
        case Flex::L:
            impl->_sideContainer->setOrientation(Qt::Horizontal);
            impl->_sideContainer->insertWidget(0, curr);
            impl->_sideContainer->insertWidget(1, impl->_hole);
            impl->_sideContainer->setStretchFactor(0, 0);
            impl->_sideContainer->setStretchFactor(1, 1);
            impl->_sideContainer->setSizes(QList<int>() << curr->baseSize().width() << 10);
            break;
        case Flex::T:
            impl->_sideContainer->setOrientation(Qt::Vertical);
            impl->_sideContainer->insertWidget(0, curr);
            impl->_sideContainer->insertWidget(1, impl->_hole);
            impl->_sideContainer->setStretchFactor(0, 0);
            impl->_sideContainer->setStretchFactor(1, 1);
            impl->_sideContainer->setSizes(QList<int>() << curr->baseSize().height() << 10);
            break;
        case Flex::R:
            impl->_sideContainer->setOrientation(Qt::Horizontal);
            impl->_sideContainer->insertWidget(0, impl->_hole);
            impl->_sideContainer->insertWidget(1, curr);
            impl->_sideContainer->setStretchFactor(0, 1);
            impl->_sideContainer->setStretchFactor(1, 0);
            impl->_sideContainer->setSizes(QList<int>() << 10 << curr->baseSize().width());
            break;
        case Flex::B:
            impl->_sideContainer->setOrientation(Qt::Vertical);
            impl->_sideContainer->insertWidget(0, impl->_hole);
            impl->_sideContainer->insertWidget(1, curr);
            impl->_sideContainer->setStretchFactor(0, 1);
            impl->_sideContainer->setStretchFactor(1, 0);
            impl->_sideContainer->setSizes(QList<int>() << 10 << curr->baseSize().height());
            break;
        default:
            break;
        }

        impl->_sideContainer->raise();
        impl->_sideContainer->show();

        impl->_reserving = true;
        curr->activate();
        impl->_reserving = false;
    }
}
