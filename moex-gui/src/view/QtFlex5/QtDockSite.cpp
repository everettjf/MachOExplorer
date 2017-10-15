#include "QtDockSite.h"
#include "QtDockSide.h"
#include "QtDockWidget.h"
#include "QtFlexWidget.h"
#include "QtFlexHelper.h"
#include "QtFlexManager.h"
#include <QtCore/QUuid>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtGui/QResizeEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QStylePainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplitter>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

class DockSiteTabBar : public QTabBar
{
public:
    DockSiteTabBar(QWidget* parent) : QTabBar(parent)
    {
        setMovable(true);
        setExpanding(false);
        setProperty("Flex", true);
        setProperty("active", false);
    }
public:
    QSize tabSizeHint(int index) const
    {
        return QSize(QTabBar::tabSizeHint(index).width(), shape() == QTabBar::RoundedNorth ? 22 : 21);
    }
public:
    void closeTab(int index)
    {
        emit tabCloseRequested(index);
    }
};

class DockSiteTabMdi : public QStackedWidget
{
public:
    DockSiteTabMdi(QWidget* parent) : QStackedWidget(parent)
    {
        setProperty("Flex", true);
        setProperty("active", false);
    }
protected:
    void paintEvent(QPaintEvent*)
    {
    }
};

class DockSiteImpl
{
public:
    DockSiteImpl() : _active(false), _viewMode(Flex::HybridView), _dockMode(Flex::DockInNoneArea), _startDrag(false), _startPull(false)
    {
    }
public:
    void update(DockSite* self, Flex::DockMode dockMode);
    void update(DockSite* self, DockWidget* widget, int method);
    void adjust(DockSite* self, DockWidget* widget);

public:
    bool isTitleBarVisible(DockSite* self, QRect* rect = nullptr) const;

public:
    bool _active;
    Flex::ViewMode _viewMode;
    Flex::DockMode _dockMode;
    QVBoxLayout* _tabBarLayout;
    QVBoxLayout* _tabMdiLayout;
    DockSiteTabBar* _tabBar;
    DockSiteTabMdi* _tabMdi;
    FlexHelper* _helper = nullptr;
    bool _persistent = false;
    bool _startDrag;
    bool _startPull;
    QPoint _startPoint;
    int _titleBarHeight;
};

void DockSiteImpl::update(DockSite* self, Flex::DockMode dockMode)
{
    if (_dockMode != dockMode)
    {
        _dockMode = dockMode;
    }

    auto tempWidget = self->flexWidget();

    if (isTitleBarVisible(self))
    {
        if (tempWidget) tempWidget->setWindowTitle(Flex::window()->windowTitle());
        self->setContentsMargins(0, _titleBarHeight, 0, 0);
        if (!_helper)
        {
            _helper = new FlexHelper(self);
            _helper->setWindowInfo(_titleBarHeight, Qt::Window);
            _helper->button(Flex::Minimize)->hide();
            _helper->button(Flex::Maximize)->hide();
            self->connect(_helper, SIGNAL(clicked(Flex::Button, bool*)), SLOT(on_titleBar_buttonClicked(Flex::Button, bool*)));
        }
        if (_tabBar->isHidden())
        {
            _tabMdiLayout->setContentsMargins(1, 0, 1, 1);
        }
        else
        {
            _tabMdiLayout->setContentsMargins(1, 0, 1, 0);
        }
    }
    else
    {
        if (tempWidget) tempWidget->setWindowTitle(_tabBar->tabText(_tabBar->currentIndex()));
        self->setContentsMargins(0, 0, 0, 0);
        if (_helper)
        {
            _helper->deleteLater(); _helper = nullptr;
        }
        if (_tabBar->isHidden())
        {
            _tabMdiLayout->setContentsMargins(0, 0, 0, 0);
        }
        else
        {
            if (!tempWidget || tempWidget->isFloating())
            {
                _tabMdiLayout->setContentsMargins(0, 0, 0, 0);
            }
            else
            {
                _tabMdiLayout->setContentsMargins(1, 0, 1, 1);
            }
        }
    }

    switch (_dockMode)
    {
    case Flex::DockInMainArea:
        if (_helper)
        {
            _helper->change(Flex::DockShow, Flex::AutoHide);
        }
        break;
    case Flex::DockInSideArea:
        if (_helper)
        {
            _helper->change(Flex::AutoHide, Flex::DockShow);
        }
        break;
    default:
        break;
    }

    if (tempWidget)
    {
        tempWidget->update();
    }

    QEvent update((QEvent::Type)Flex::Update);

    for (int i = 0; i < _tabMdi->count(); ++i)
    {
        qApp->sendEvent(_tabMdi->widget(i), &update);
    }

    QResizeEvent resize(self->size(), QSize());

    qApp->sendEvent(self, &resize);
}

void DockSiteImpl::adjust(DockSite* self, DockWidget* widget)
{
    auto arranger = qobject_cast<QVBoxLayout*>(self->layout());

    QPalette tabPalette = _tabBar->palette();

    auto item0 = arranger->takeAt(0);
    auto item1 = arranger->takeAt(0);

    if (_persistent || !widget || widget->viewMode() == Flex::FileView || widget->viewMode() == Flex::FilePagesView)
    {
        tabPalette.setColor(QPalette::Active, QPalette::Highlight, QColor("#FFF29D"));
		tabPalette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#4D6082"));
		tabPalette.setColor(QPalette::Active, QPalette::Window, QColor("#5B7199"));
        tabPalette.setColor(QPalette::Inactive, QPalette::Window, QColor("#364E6F"));
        tabPalette.setColor(QPalette::Active, QPalette::WindowText, QColor("#000000"));
        tabPalette.setColor(QPalette::Inactive, QPalette::WindowText, QColor("#FFFFFF"));
        arranger->setContentsMargins(0, 0, 0, 0);
        if (item0 == nullptr)
        {
            arranger->addLayout(_tabBarLayout);
            arranger->addLayout(_tabMdiLayout);
        }
        else
        {
            if (item0->layout() != _tabBarLayout)
            {
                arranger->addItem(item1);
                arranger->addItem(item0);
            }
        }
        _tabBar->setTabsClosable(true);
        _tabBar->setShape(QTabBar::RoundedNorth);
        //_tabBar->setFixedHeight(22);
        //if (widget) _tabBar->show();
        _tabBar->show();
    }
    else
    {
        tabPalette.setColor(QPalette::Active, QPalette::Highlight, QColor("#FFFFFF"));
        tabPalette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#FFFFFF"));
        tabPalette.setColor(QPalette::Active, QPalette::Window, QColor("#5B7199"));
        tabPalette.setColor(QPalette::Inactive, QPalette::Window, QColor("#4D6082"));
        tabPalette.setColor(QPalette::Active, QPalette::WindowText, QColor("#000000"));
        tabPalette.setColor(QPalette::Inactive, QPalette::WindowText, QColor("#FFFFFF"));
        arranger->setContentsMargins(0, 0, 0, 0);
        if (item0 == nullptr)
        {
            arranger->addLayout(_tabMdiLayout);
            arranger->addLayout(_tabBarLayout);
        }
        else
        {
            if (item0->layout() != _tabMdiLayout)
            {
                arranger->addItem(item1);
                arranger->addItem(item0);
            }
        }
        _tabBar->setTabsClosable(false);
        _tabBar->setShape(QTabBar::RoundedSouth);
        //_tabBar->setFixedHeight(21);
        //if (widget) _tabBar->hide();
        _tabBar->hide();
    }

    _tabBar->setPalette(tabPalette);
    _tabBar->setDocumentMode(true);
}

void DockSiteImpl::update(DockSite* self, DockWidget* widget, int method)
{
    if (method == 0)
    {
        auto dockViewMode = widget ? widget->viewMode() : Flex::HybridView;

        if (_persistent)
        {
            _viewMode = Flex::HybridView;
        }
        else if (_tabBar->count() == 1)
        {
            _viewMode = dockViewMode;
        }
        else if (_viewMode == Flex::HybridView)
        {
            _viewMode = Flex::HybridView;
        }
        else if ((_viewMode == Flex::ToolView || _viewMode == Flex::ToolPagesView) && (dockViewMode == Flex::ToolView || dockViewMode == Flex::ToolPagesView))
        {
            _viewMode = Flex::ToolPagesView;
        }
        else if ((_viewMode == Flex::FileView || _viewMode == Flex::FilePagesView) && (dockViewMode == Flex::FileView || dockViewMode == Flex::FilePagesView))
        {
            _viewMode = Flex::FilePagesView;
        }
        else
        {
            _viewMode = Flex::HybridView;
        }
    }
    else
    {
        auto dockWidgets = self->widgets();

        if (_persistent)
        {
            _viewMode = Flex::HybridView;
        }
        else if (dockWidgets.size() == 1)
        {
            _viewMode = dockWidgets[0]->viewMode();
        }
        else if (!dockWidgets.empty())
        {
            if (_viewMode != Flex::HybridView)
            {
            }
            else if (std::all_of(dockWidgets.begin(), dockWidgets.end(), [&](DockWidget* dockWidget) { return dockWidget->viewMode() == Flex::ToolView; }))
            {
                _viewMode = Flex::ToolPagesView;
            }
            else if (std::all_of(dockWidgets.begin(), dockWidgets.end(), [&](DockWidget* dockWidget) { return dockWidget->viewMode() == Flex::FileView; }))
            {
                _viewMode = Flex::FilePagesView;
            }
        }
    }
}

bool DockSiteImpl::isTitleBarVisible(DockSite* self, QRect* rect) const
{
    auto tempWidget = self->flexWidget();

    if (_persistent || !tempWidget || (tempWidget->isFloating() && (tempWidget->viewMode() == Flex::ToolView || tempWidget->viewMode() == Flex::ToolPagesView)))
    {
        if (rect) *rect = QRect(); return false;
    }

    if (_viewMode == Flex::ToolView || _viewMode == Flex::ToolPagesView)
    {
        if (rect) *rect = QRect(0, 0, self->width(), _titleBarHeight); return true;
    }
    else
    {
        if (rect) *rect = QRect(); return false;
    }
}

DockSite::DockSite(DockWidget* widget, QSize baseSize, QWidget* parent) : QWidget(parent), impl(new DockSiteImpl)
{
    setObjectName(QUuid::createUuid().toString().toUpper());
    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::StrongFocus);
    setProperty("Flex", true);
    setProperty("Site", true);
    setBaseSize(baseSize);

    auto arranger = new QVBoxLayout(this);

    arranger->setObjectName("_flex_siteLayout");

    impl->_tabBarLayout = new QVBoxLayout();
    impl->_tabBarLayout->setObjectName("_flex_tabBarLayout");
    impl->_tabBarLayout->setSpacing(0);
    impl->_tabMdiLayout = new QVBoxLayout();
    impl->_tabMdiLayout->setObjectName("_flex_tabMdiLayout");
    impl->_tabMdiLayout->setSpacing(0);

    impl->_titleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, this) - 4;

    impl->_tabBar = new DockSiteTabBar(this);
    impl->_tabBar->setObjectName("_flex_tabBar");
    impl->_tabMdi = new DockSiteTabMdi(this);
    impl->_tabMdi->setObjectName("_flex_tabMdi");
    impl->_tabBarLayout->addWidget(impl->_tabBar);
    impl->_tabMdiLayout->addWidget(impl->_tabMdi);

    impl->_tabBar->hide();

    arranger->setSpacing(0);

    if (widget)
    {
        setWindowTitle(widget->windowTitle());
    }

    impl->_persistent = (widget == nullptr);

    impl->adjust(this, widget);

    if (widget)
    {
        impl->_tabMdi->addWidget(widget);
    }

    connect(impl->_tabBar, SIGNAL(currentChanged(int)), SLOT(on_tabBar_currentChanged(int)));
    connect(impl->_tabBar, SIGNAL(tabMoved(int, int)), SLOT(on_tabBar_tabMoved(int, int)));
    connect(impl->_tabBar, SIGNAL(tabCloseRequested(int)), SLOT(on_tabBar_tabCloseRequested(int)));
    connect(impl->_tabMdi, SIGNAL(currentChanged(int)), SLOT(on_tabMdi_currentChanged(int)));
    connect(impl->_tabMdi, SIGNAL(widgetRemoved(int)), SLOT(on_tabMdi_widgetRemoved(int)));

    if (widget)
    {
        impl->_tabBar->addTab(widget->windowTitle());
    }

    impl->_tabBar->installEventFilter(this);

    impl->update(this, widget, 0);

    impl->update(this, Flex::DockInNoneArea);

    if (widget)
    {
        resize(widget->size());
    }
}

DockSite::~DockSite()
{

}

Flex::ViewMode DockSite::viewMode() const
{
    return impl->_viewMode;
}

Flex::DockMode DockSite::dockMode() const
{
    return impl->_dockMode;
}

void DockSite::setDockMode(Flex::DockMode dockMode)
{
    if (impl->_dockMode != dockMode)
    {
        impl->update(this, dockMode);
    }
}

Flex::Features DockSite::features() const
{
    return impl->_tabMdi->count() > 0 ? qobject_cast<DockWidget*>(impl->_tabMdi->widget(0))->siteFeatures() : (Flex::AllowDockAsNorthTabPage | Flex::AllowDockAsSouthTabPage);
}

bool DockSite::addWidget(DockWidget* widget)
{
    return insertWidget(widget, -1);
}

bool DockSite::insertWidget(DockWidget* widget, int index)
{
    Q_ASSERT(widget != nullptr);

    if (impl->_tabBar->insertTab(index, widget->windowTitle()) >= 0)
    {
        impl->_tabMdi->blockSignals(true);
        impl->_tabMdi->insertWidget(index, widget);
        impl->_tabMdi->blockSignals(false);
    }

    if (impl->_tabBar->count() > 1
		|| impl->_persistent
		|| widget->viewMode() == Flex::FileView
		|| widget->viewMode() == Flex::FilePagesView)
    {
		//用resize不太完美..
		impl->_tabBar->setFixedHeight(impl->_tabBar->sizeHint().height());
        impl->_tabBar->show(); impl->_tabBar->setCurrentIndex(impl->_tabBar->count() - 1);
    }
    else
    {
        impl->adjust(this, widget);
    }

    impl->update(this, widget, 0);

    impl->update(this, impl->_dockMode);

    return true;
}

DockSide* DockSite::dockSide() const
{
    FlexWidget* flexWidget = this->flexWidget();

    if (flexWidget == nullptr)
    {
        return nullptr;
    }

    QWidget* container = parentWidget();

    if (container != flexWidget->sideContainer())
    {
        return nullptr;
    }

    if (flexWidget->dockSide(Flex::L)->hasDockSite(const_cast<DockSite*>(this)))
    {
        return flexWidget->dockSide(Flex::L);
    }

    if (flexWidget->dockSide(Flex::T)->hasDockSite(const_cast<DockSite*>(this)))
    {
        return flexWidget->dockSide(Flex::T);
    }

    if (flexWidget->dockSide(Flex::B)->hasDockSite(const_cast<DockSite*>(this)))
    {
        return flexWidget->dockSide(Flex::B);
    }

    if (flexWidget->dockSide(Flex::R)->hasDockSite(const_cast<DockSite*>(this)))
    {
        return flexWidget->dockSide(Flex::R);
    }

    return nullptr;
}

FlexWidget* DockSite::flexWidget() const
{
    QWidget* tempWidget = parentWidget();
    while (tempWidget && !qobject_cast<FlexWidget*>(tempWidget))
    {
        tempWidget = tempWidget->parentWidget();
    }
    return qobject_cast<FlexWidget*>(tempWidget);
}

int DockSite::currentIndex() const
{
    return impl->_tabMdi->currentIndex();
}

void DockSite::setCurrentIndex(int index)
{
    if (index == impl->_tabMdi->currentIndex())
    {
        if (index != -1)
        {
            impl->_tabMdi->widget(index)->setFocus();
        }
    }
    else
    {
        impl->_tabMdi->setCurrentIndex(index);
    }
}

int DockSite::count() const
{
    return impl->_tabMdi->count();
}

DockWidget* DockSite::widget(int index) const
{
    return qobject_cast<DockWidget*>(impl->_tabMdi->widget(index));
}

int DockSite::indexOf(DockWidget* widget) const
{
    return impl->_tabMdi->indexOf(widget);
}

QList<DockWidget*> DockSite::widgets() const
{
    QList<DockWidget*> dockWidgets;
    for (int i = 0; i < impl->_tabMdi->count(); ++i)
    {
        dockWidgets.append(static_cast<DockWidget*>(impl->_tabMdi->widget(i)));
    }
    return dockWidgets;
}

void DockSite::removeWidgetAt(int index)
{
    impl->_tabBar->closeTab(index);
}

void DockSite::setCurrentWidget(DockWidget* widget)
{
    if (impl->_tabMdi->currentWidget() != widget)
    {
        impl->_tabMdi->setCurrentWidget(widget);
    }
    if (widget)
    {
        widget->setFocus();
    }
}

QString DockSite::identifier()
{
    QSplitter* splitter = nullptr;

    QWidget* thisWidget = this;
    QWidget* tempWidget = parentWidget();

    QStringList parts;

    while (tempWidget)
    {
        if ((splitter = qobject_cast<QSplitter*>(tempWidget)) != nullptr)
        {
            parts.prepend(QString::number(splitter->indexOf(thisWidget)));
        }
        else
        {
            parts.prepend(tempWidget->objectName()); break;
        }

        parts.prepend(tempWidget->objectName());

        thisWidget = tempWidget;

        tempWidget = tempWidget->parentWidget();
    }

    parts.append(objectName());

    return parts.join(",");
}

bool DockSite::isActive() const
{
    return impl->_active;
}

void DockSite::setActive(bool active)
{
    if (impl->_active != active)
    {
        impl->_active = active;
        impl->_tabBar->setProperty("active", active);
        impl->_tabBar->update();
        impl->_tabMdi->setProperty("active", active);
        impl->_tabMdi->update();
        update();
    }
}

void DockSite::activate()
{
    window()->activateWindow();

    setFocus();
}

bool DockSite::load(const QJsonObject& object)
{
    setObjectName(object["name"].toString());
    setWindowTitle(object["windowTitle"].toString());
    setBaseSize(object["baseW"].toInt(0), object["baseH"].toInt(0));

    impl->_persistent = object["persistent"].toBool(false);

    QJsonArray dockWidgetObjects = object["widgets"].toArray();

    for (int i = 0; i < dockWidgetObjects.size(); ++i)
    {
        QJsonObject dockWidgetObject = dockWidgetObjects[i].toObject();

        Flex::ViewMode viewMode = (Flex::ViewMode)dockWidgetObject["viewMode"].toInt();

        QString dockWidgetName = dockWidgetObject["dockWidgetName"].toString();

        DockWidget* dockWidget = FlexManager::instance()->createDockWidget(viewMode, this, Flex::widgetFlags(), dockWidgetName);

        dockWidget->load(dockWidgetObject);

        addWidget(dockWidget);
	}


    return true;
}

bool DockSite::save(QJsonObject& object) const
{
    QSize baseSize = this->baseSize();

    object["name"] = objectName();
    object["windowTitle"] = windowTitle();
    object["baseW"] = baseSize.width();
    object["baseH"] = baseSize.height();

    object["persistent"] = impl->_persistent;

    QJsonArray dockWidgetObjects;

    for (int i = 0; i < impl->_tabMdi->count(); ++i)
    {
        QJsonObject dockWidgetObject;

        qobject_cast<DockWidget*>(impl->_tabMdi->widget(i))->save(dockWidgetObject);

        dockWidgetObjects.append(dockWidgetObject);
    }

    object["widgets"] = dockWidgetObjects;

    return true;
}

bool DockSite::event(QEvent* evt)
{
    if (evt->type() == Flex::Update)
    {
        impl->update(this, Flex::DockInMainArea);
    }
    else if (evt->type() == QEvent::ParentAboutToChange)
    {
        auto tempWidget = flexWidget();

        if (tempWidget)
        {
            if (!tempWidget->isAdjusting() && !impl->_persistent)
            {
                tempWidget->removeDockSite(this);
            }
        }
    }

    return QWidget::event(evt);
}

bool DockSite::eventFilter(QObject* obj, QEvent* evt)
{
    if (obj == impl->_tabBar)
    {
        switch (evt->type())
        {
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* mouse = static_cast<QMouseEvent*>(evt);

            if (mouse->button() == Qt::LeftButton && impl->_tabBar->tabAt(mouse->pos()) != -1)
            {
                auto tempWidget = flexWidget();

                if (!tempWidget->isFloating() || impl->_tabBar->count() > 1)
                {
                    impl->_startPull = true;
                    impl->_startPoint = mouse->pos();
                    impl->_tabBar->grabMouse();
                }
            }

            break;
        }
        case QEvent::MouseButtonRelease:
        {
            impl->_tabBar->releaseMouse();
            impl->_startPull = false;
            break;
        }
        case QEvent::MouseMove:
        {
            if (impl->_startPull)
            {
                QMouseEvent* mouse = static_cast<QMouseEvent*>(evt);

                int offset = abs(mouse->pos().y() - impl->_startPoint.y());

                if (offset < QApplication::startDragDistance() * 5)
                {
                    return QWidget::eventFilter(obj, evt);
                }

                auto pos = QCursor::pos();

                QMouseEvent event(QEvent::MouseButtonRelease, mouse->pos(), Qt::LeftButton, mouse->buttons(), mouse->modifiers());
                QApplication::sendEvent(impl->_tabBar, &event);

                auto dimension = size();

                auto flexWidget = FlexManager::instance()->createFlexWidget(viewMode(), Flex::parent(viewMode()), Flex::windowFlags());

                int current = impl->_tabBar->currentIndex();

                DockWidget* dockWidget = qobject_cast<DockWidget*>(impl->_tabMdi->widget(current));

                flexWidget->addDockSite(new DockSite(dockWidget), Flex::M, -1);

                auto tl = flexWidget->geometry().topLeft() - flexWidget->frameGeometry().topLeft();
                auto br = flexWidget->frameGeometry().bottomRight() - flexWidget->geometry().bottomRight();
                auto sz = dimension - QSize(tl.x(), tl.y()) - QSize(br.x(), br.y());
                flexWidget->setGeometry(QRect(pos + tl - impl->_startPoint, sz));
                flexWidget->show();

                impl->update(this, dockWidget, 1);

                QApplication::sendPostedEvents();

                QResizeEvent resize(sz, sz);
                QApplication::sendEvent(flexWidget, &resize);

#ifdef Q_OS_MAC
                flexWidget->grabMouse();
#endif

#ifdef Q_OS_WIN
                SendMessage((HWND)flexWidget->effectiveWinId(), WM_NCLBUTTONDOWN, HTCAPTION, MAKELONG(pos.x(), pos.y()));
#else
                QApplication::sendEvent(flexWidget, new QMouseEvent(QEvent::MouseButtonPress, flexWidget->mapFromGlobal(pos), pos, Qt::LeftButton, mouse->buttons(), mouse->modifiers()));
#endif
                return true;
            }
            break;
        default:
            break;
        }
        }
    }
    return QWidget::eventFilter(obj, evt);
}

void DockSite::paintEvent(QPaintEvent*)
{
    if (impl->_tabBar->count() == 0)
    {
        return;
    }

    QStylePainter painter(this);

    QRect titleBarRect;

    if (impl->isTitleBarVisible(this, &titleBarRect))
    {
        if (impl->_helper)
        {
            impl->_helper->buttons()->show();
            impl->_helper->extents()->show();
        }

        QStyleOptionTitleBar titleOption;
        titleOption.init(this);
        titleOption.rect = titleBarRect;
        titleOption.text = impl->_tabBar->tabText(impl->_tabBar->currentIndex());
        titleOption.subControls = QStyle::SC_TitleBarLabel;

        bool active = impl->_active;

        painter.fillRect(titleBarRect, QColor(active ? "#fff29d" : "#4D6082"));

        QRect ir = style()->subControlRect(QStyle::CC_TitleBar, &titleOption, QStyle::SC_TitleBarLabel, this);

        painter.setPen(active ? Qt::black : Qt::white);

        painter.drawText(8, (impl->_titleBarHeight - ir.height()) / 2, ir.width() - 2, ir.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, titleOption.text);
    }
    else
    {
        if (impl->_helper)
        {
            impl->_helper->buttons()->hide();
            impl->_helper->extents()->hide();
        }
    }

    int tabBarHeight = impl->_tabBar->height();

    QStyleOptionFrame frameOption;

    frameOption.initFrom(this);

    if (impl->_tabBar->isVisible())
    {
        if (impl->_tabBar->shape() == QTabBar::RoundedNorth)
        {
            frameOption.rect.adjust(0, +tabBarHeight, 0, 0);
        }
        else
        {
            frameOption.rect.adjust(0, 0, 0, -tabBarHeight);
        }
    }

    painter.drawPrimitive(QStyle::PE_Frame, frameOption);
}

void DockSite::closeEvent(QCloseEvent* evt)
{
    setParent(nullptr); evt->accept();
}

void DockSite::mousePressEvent(QMouseEvent* evt)
{
    QRect titleBarRect;

    if (evt->button() == Qt::LeftButton && impl->isTitleBarVisible(this, &titleBarRect) && !isTopLevel())
    {
        if (titleBarRect.contains(evt->pos()))
        {
            impl->_startPoint = evt->pos();

            auto tempWidget = flexWidget();

            if (!tempWidget->isFloating() || tempWidget->count() > 1)
            {
                impl->_startDrag = true;
            }
        }
    }
}

void DockSite::mouseMoveEvent(QMouseEvent* evt)
{
    QPoint offset = evt->pos() - impl->_startPoint;

    if (impl->_startDrag)
    {
        if (offset.manhattanLength() < QApplication::startDragDistance())
        {
            return;
        }

        auto pos = QCursor::pos();

#ifdef Q_OS_WIN
        SendMessage((HWND)effectiveWinId(), WM_LBUTTONUP, 0, MAKELONG(pos.x(), pos.y()));
#endif

        auto dimension = size();

        auto flexWidget = FlexManager::instance()->createFlexWidget(viewMode(), Flex::parent(viewMode()), Flex::windowFlags());

        flexWidget->addDockSite(this, Flex::M, -1);

        auto tl = flexWidget->geometry().topLeft() - flexWidget->frameGeometry().topLeft();
        auto br = flexWidget->frameGeometry().bottomRight() - flexWidget->geometry().bottomRight();
        auto sz = dimension - QSize(tl.x(), tl.y()) - QSize(br.x(), br.y());
        flexWidget->setGeometry(QRect(pos + tl - impl->_startPoint, sz));
        flexWidget->show();

        impl->_startDrag = false;

        QApplication::sendPostedEvents();

        QResizeEvent resize(sz, sz);
        QApplication::sendEvent(flexWidget, &resize);

#ifdef Q_OS_MAC
        flexWidget->grabMouse();
#endif

#ifdef Q_OS_WIN
        SendMessage((HWND)flexWidget->effectiveWinId(), WM_NCLBUTTONDOWN, HTCAPTION, MAKELONG(pos.x(), pos.y()));
#else
        QApplication::sendEvent(flexWidget, new QMouseEvent(QEvent::MouseButtonPress, flexWidget->mapFromGlobal(pos), pos, Qt::LeftButton, evt->buttons(), evt->modifiers()));
#endif
    }
}

void DockSite::mouseReleaseEvent(QMouseEvent*)
{
    impl->_startDrag = false;
}

void DockSite::mouseDoubleClickEvent(QMouseEvent* evt)
{
    QRect titleBarRect;

    if (evt->button() == Qt::LeftButton && impl->isTitleBarVisible(this, &titleBarRect) && titleBarRect.contains(evt->pos()))
    {
        auto tempWidget = flexWidget();

        if (!tempWidget->isFloating() || tempWidget->siteContainer()->count() > 1)
        {
            auto dimension = size();
            auto flexWidget = FlexManager::instance()->createFlexWidget(viewMode(), Flex::parent(viewMode()), Flex::windowFlags());
            flexWidget->addDockSite(this, Flex::M, -1);
            auto pos = evt->globalPos();
            auto tl = flexWidget->geometry().topLeft() - flexWidget->frameGeometry().topLeft();
            auto br = flexWidget->frameGeometry().bottomRight() - flexWidget->geometry().bottomRight();
            flexWidget->setGeometry(QRect(pos + tl - impl->_startPoint, dimension - QSize(tl.x(), tl.y()) - QSize(br.x(), br.y())));
            flexWidget->show();
        }
    }
}

void DockSite::on_tabBar_currentChanged(int index)
{
    auto tempWidget = flexWidget();

    if (impl->_tabBar->count() == 1 && impl->_tabBar->shape() == QTabBar::RoundedSouth)
    {
        impl->_tabBar->hide();
    }
    else if (impl->_tabBar->count() == 0)
    {
        impl->_tabBar->hide();
    }

    impl->_tabMdi->blockSignals(true);
    impl->_tabMdi->setCurrentIndex(index); 
    impl->_tabMdi->blockSignals(false);

    if (tempWidget)
    {
        if (impl->isTitleBarVisible(this))
        {
            tempWidget->setWindowTitle(Flex::window()->windowTitle());
        }
        else
        {
            tempWidget->setWindowTitle(impl->_tabBar->tabText(index));
        }

        tempWidget->update();
    }

    update();
}

void DockSite::on_tabBar_tabMoved(int from, int to)
{
    impl->_tabMdi->blockSignals(true);
    auto widget = impl->_tabMdi->widget(from);
    impl->_tabMdi->removeWidget(widget);
    impl->_tabMdi->insertWidget(to, widget);
    impl->_tabMdi->blockSignals(false);
}

void DockSite::on_tabBar_tabCloseRequested(int index)
{
    auto tempWidget = flexWidget();

    auto widget = static_cast<DockWidget*>(impl->_tabMdi->widget(index));

    FlexManager::instance()->snapshot(widget);

    impl->_tabBar->removeTab(index);

    impl->_tabMdi->blockSignals(true);
    impl->_tabMdi->removeWidget(widget);
    impl->_tabMdi->blockSignals(false);

    widget->setParent(nullptr); 

    widget->close();

    if (impl->_tabBar->count() == 0)
    {
        if (!impl->_persistent)
        {
            tempWidget->removeDockSite(this);
        }
    }

    if (tempWidget->isFloating() && tempWidget->count() == 0 && impl->_tabBar->count() == 0)
    {
        tempWidget->close();
    }

    impl->update(this, widget, 1);

    update();
}

void DockSite::on_tabMdi_currentChanged(int index)
{
    auto tempWidget = flexWidget();

    if (impl->_tabBar->count() == 1 && impl->_tabBar->shape() == QTabBar::RoundedSouth)
    {
        impl->_tabBar->hide();
    }
    else if (impl->_tabBar->count() == 0)
    {
        impl->_tabBar->hide();
    }

    impl->_tabBar->blockSignals(true);
    impl->_tabBar->setCurrentIndex(index);
    impl->_tabBar->blockSignals(false);

    if (tempWidget)
    {
        if (impl->isTitleBarVisible(this))
        {
            tempWidget->setWindowTitle(Flex::window()->windowTitle());
        }
        else
        {
            tempWidget->setWindowTitle(impl->_tabBar->tabText(index));
        }

        tempWidget->update();
    }

    update();
}

void DockSite::on_tabMdi_widgetRemoved(int index)
{
    impl->_tabBar->blockSignals(true);
    impl->_tabBar->removeTab(index);
    impl->_tabBar->blockSignals(false);

    auto tempWidget = flexWidget();

    if (impl->_tabBar->count() == 0)
    {
        if (!impl->_persistent)
        {
            tempWidget->removeDockSite(this);
        }
    }

    if (tempWidget->isFloating() && tempWidget->count() == 0 && impl->_tabBar->count() == 0)
    {
        tempWidget->close();
    }

    impl->update(this, nullptr, 1);

    update();
}

void DockSite::on_titleBar_buttonClicked(Flex::Button button, bool *accepted)
{
    switch (button)
    {
    case Flex::AutoHide:
        if (impl->_dockMode == Flex::DockInMainArea)
        {
            flexWidget()->makeSiteAutoHide(this);
        }
        break;
    case Flex::DockShow:
        if (impl->_dockMode == Flex::DockInSideArea)
        {
            flexWidget()->makeSiteDockShow(this);
        }
        break;
    case Flex::DockPull:
        flexWidget()->showSiteDockPull(this);
        break;
    default:
        break;
    }

    *accepted = true;

    if (button != Flex::Close || (impl->_viewMode != Flex::ToolView && impl->_viewMode != Flex::ToolPagesView))
    {
        return;
    }

    removeWidgetAt(currentIndex());
}
