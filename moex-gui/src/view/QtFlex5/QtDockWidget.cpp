#include "QtDockWidget.h"
#include "QtDockSite.h"
#include "QtDockSide.h"
#include "QtFlexWidget.h"
#include "QtFlexHelper.h"
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtGui/QMoveEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStylePainter>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDesktopWidget>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

class DockWidgetImpl
{
public:
    DockWidgetImpl() : _active(false), _helper(nullptr)
    {
        _background.setRgbF(1.0f * qrand() / RAND_MAX, 1.0f * qrand() / RAND_MAX, 1.0f * qrand() / RAND_MAX);
        _dockFeatures = Flex::AllowDockAsNorthTabPage | Flex::AllowDockAsSouthTabPage;
        _siteFeatures = Flex::AllowDockAsNorthTabPage | Flex::AllowDockAsSouthTabPage;
    }

public:
    void update(DockWidget* self);

public:
    bool isTitleBarVisible(DockWidget* self, QRect* rect = nullptr) const;

public:
    bool _active;
    QColor _background;
    Flex::ViewMode _viewMode;
    Flex::Features _dockFeatures;
    Flex::Features _siteFeatures;
    QWidget* _widget;
    QString _flexWidgetName;
    QVBoxLayout* _layout;
    FlexHelper* _helper;
    int _titleBarHeight;
};

void DockWidgetImpl::update(DockWidget* self)
{
    auto palette = self->palette();

    switch (_viewMode)
    {
    case Flex::ToolView:
        palette.setColor(QPalette::Active, self->backgroundRole(), QColor("#FFF29D"));
        palette.setColor(QPalette::Inactive, self->backgroundRole(), QColor("#4D6082"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#FFF29D"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#4D6082"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#000000"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#FFFFFF"));
        self->setDockFeatures(Flex::AllowDockAsNorthTabPage | Flex::AllowDockAsSouthTabPage);
        self->setSiteFeatures(Flex::AllowDockAsSouthTabPage);
        break;
    case Flex::FileView:
        palette.setColor(QPalette::All, self->backgroundRole(), QColor("#293955"));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor("#293955"));
        palette.setColor(QPalette::Active, self->foregroundRole(), QColor("#FFFFFF"));
        palette.setColor(QPalette::Inactive, self->foregroundRole(), QColor("#7F8899"));
        self->setDockFeatures(Flex::AllowDockAsNorthTabPage);
        self->setSiteFeatures(Flex::AllowDockAsNorthTabPage | Flex::AllowDockAsSouthTabPage);
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    self->setPalette(palette);

    if (!self->isWindow())
    {
        if (_helper)
        {
            _helper->deleteLater(); _helper = nullptr;
        }

        self->setContentsMargins(0, 0, 0, 0);
    }
    else
    {
        if (!_helper)
        {
            _helper = new FlexHelper(self);
            self->connect(_helper, SIGNAL(clicked(Flex::Button, bool*)), SLOT(on_titleBar_buttonClicked(Flex::Button, bool*)));
        }

        switch (_viewMode)
        {
        case Flex::FileView:
            _helper->button(Flex::Minimize)->show();
            _helper->button(Flex::DockPull)->hide();
            _helper->button(Flex::AutoHide)->hide();
            _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) - 4;
            break;
        case Flex::ToolView:
            _helper->button(Flex::Minimize)->hide();
            _helper->button(Flex::DockPull)->show();
            _helper->button(Flex::AutoHide)->hide();
            _titleBarHeight = self->style()->pixelMetric(QStyle::PM_TitleBarHeight, nullptr, self) - 4;
            break;
        default:
            break;
        }

        _helper->setWindowInfo(_titleBarHeight, Flex::windowFlags(_viewMode));

        self->setContentsMargins(0, _titleBarHeight, 0, 0);
    }
}

bool DockWidgetImpl::isTitleBarVisible(DockWidget* self, QRect* rect) const
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

DockWidget::DockWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags) : QWidget(parent, flags), impl(new DockWidgetImpl)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::StrongFocus);
    setProperty("Flex", true);

    impl->_widget = new QWidget(this);

    impl->_viewMode = viewMode;

    impl->update(this);

    impl->_layout = new QVBoxLayout(this);
    impl->_layout->setContentsMargins(0, 0, 0, 0);
    impl->_layout->setSpacing(0);

    impl->_layout->addWidget(impl->_widget);
}

DockWidget::~DockWidget()
{
    emit destroying(this);
}

bool DockWidget::event(QEvent* evt)
{
    if (evt->type() == QEvent::WinIdChange)
    {
        if (!internalWinId())
        {
            impl->update(this);
        }
    }
    else if (evt->type() == Flex::Update)
    {
        impl->_flexWidgetName = flexWidgetName();
    }
    return QWidget::event(evt);
}

bool DockWidget::nativeEvent(const QByteArray& eventType, void *message, long *result)
{
    if (impl->_helper && impl->_helper->nativeEvent(eventType, message, result))
    {
        return true;
    }
    else
    {
        return QWidget::nativeEvent(eventType, message, result);
    }
}

void DockWidget::paintEvent(QPaintEvent*)
{
    QStylePainter painter(this);

    painter.fillRect(rect(), impl->_background);

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
        titleOption.text = windowTitle();
        titleOption.icon = windowIcon();
        titleOption.subControls = QStyle::SC_TitleBarLabel;
        titleOption.titleBarState = windowState();
        titleOption.titleBarFlags = Flex::windowFlags(impl->_viewMode);

        bool isActive = (titleOption.state & QStyle::State_Active) != 0;

        painter.fillRect(titleBarRect, titleOption.palette.color(QPalette::Highlight));

        bool hasIcon = titleOption.titleBarFlags & Qt::WindowSystemMenuHint;

        QRect lr = style()->subControlRect(QStyle::CC_TitleBar, &titleOption, QStyle::SC_TitleBarLabel, this);

        if (hasIcon)
        {
            titleOption.icon.paint(&painter, 5, (impl->_titleBarHeight - 16) / 2, 16, 16, Qt::AlignCenter, QIcon::Active, isActive ? QIcon::On : QIcon::Off);
        }

        painter.drawText(8 + (hasIcon ? 18 : 0), (impl->_titleBarHeight - lr.height()) / 2, lr.width() - 2, lr.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, titleOption.text);

        QStyleOptionFrame frameOption;
        frameOption.initFrom(this);
        painter.drawPrimitive(QStyle::PE_Frame, frameOption);
    }
    else
    {
        if (impl->_helper)
        {
            impl->_helper->buttons()->hide();
            impl->_helper->extents()->hide();
        }
    }
}

void DockWidget::closeEvent(QCloseEvent*)
{
    
}

QSize DockWidget::sizeHint() const
{
    return isFloating() ? QSize(640, 480) : QWidget::sizeHint();
}

QSize DockWidget::minimumSizeHint() const
{
    return isFloating() ? QSize(80, 40) : QWidget::minimumSizeHint();
}

Flex::ViewMode DockWidget::viewMode() const
{
    return impl->_viewMode;
}

void DockWidget::setViewMode(Flex::ViewMode viewMode)
{
    if (impl->_viewMode != viewMode)
    {
        impl->_viewMode = viewMode;
        impl->update(this);
    }
}

Flex::Features DockWidget::dockFeatures() const
{
    return impl->_dockFeatures;
}

void DockWidget::setDockFeatures(Flex::Features features)
{
    impl->_dockFeatures = features;
}

Flex::Features DockWidget::siteFeatures() const
{
    return impl->_siteFeatures;
}

void DockWidget::setSiteFeatures(Flex::Features features)
{
    impl->_siteFeatures = features;
}

DockSite* DockWidget::dockSite() const
{
    QWidget* tempWidget = parentWidget();
    while (tempWidget && !qobject_cast<DockSite*>(tempWidget))
    {
        tempWidget = tempWidget->parentWidget();
    }
    return qobject_cast<DockSite*>(tempWidget);
}

DockSide* DockWidget::dockSide() const
{
    FlexWidget* flexWidget = this->flexWidget();

    if (flexWidget == nullptr)
    {
        return nullptr;
    }

    DockSite* dockSite = this->dockSite();

    if (dockSite == nullptr)
    {
        return nullptr;
    }

    return dockSite->dockSide();
}

FlexWidget* DockWidget::flexWidget() const
{
    QWidget* tempWidget = parentWidget();
    while (tempWidget && !qobject_cast<FlexWidget*>(tempWidget))
    {
        tempWidget = tempWidget->parentWidget();
    }
    return qobject_cast<FlexWidget*>(tempWidget);
}

QString DockWidget::flexWidgetName() const
{
    if (impl->_flexWidgetName.isEmpty())
    {
        return [](FlexWidget* widget) { return widget ? widget->objectName() : ""; }(flexWidget());
    }
    else
    {
        return impl->_flexWidgetName;
    }
}

QWidget* DockWidget::widget() const
{
    return impl->_widget;
}

void DockWidget::attachWidget(QWidget* widget)
{
    if (impl->_widget != widget)
    {
        if (impl->_widget)
        {
            delete impl->_layout->takeAt(0);
        }
        impl->_layout->addWidget(widget);
        impl->_widget = widget;
    }
    else
    {
        Q_ASSERT(false);
    }
}

void DockWidget::detachWidget(QWidget* widget)
{
    if (impl->_widget == widget)
    {
        if (impl->_widget)
        {
            delete impl->_layout->takeAt(0);
        }
        impl->_widget->setParent(nullptr);
        impl->_widget = nullptr;
    }
    else
    {
        Q_ASSERT(false);
    }
}

void DockWidget::setWidget(QWidget* widget)
{
    if (impl->_widget != widget)
    {
        if (impl->_widget)
        {
            delete impl->_layout->takeAt(0);
        }

        impl->_widget->deleteLater();

        impl->_layout->addWidget(widget);

        impl->_widget = widget;
    }
}

void DockWidget::activate()
{
    QWidget* window = this->window();

    if (!isFloating() && !window->isActiveWindow())
    {
        window->activateWindow();
    }

    DockSite* dockSite = this->dockSite();

    if (dockSite)
    {
        dockSite->setCurrentWidget(this);
    }
    else
    {
        setFocus();
    }

    DockSide* dockSide = this->dockSide();

    if (dockSide)
    {
        dockSide->makeCurrent(dockSite);
    }
}

bool DockWidget::isFloating() const
{
    return isTopLevel();
}

bool DockWidget::isActive() const
{
    return isActiveWindow();
}

bool DockWidget::load(const QJsonObject& object)
{
    setWindowTitle(object["dockWidgetName"].toString());
    setViewMode((Flex::ViewMode)object["viewMode"].toInt());
    setDockFeatures((Flex::Features)object["dockFeatures"].toInt());
    setSiteFeatures((Flex::Features)object["siteFeatures"].toInt());
    return true;
}

bool DockWidget::save(QJsonObject& object) const
{
    object["dockWidgetName"] = windowTitle();
    object["viewMode"] = (int)viewMode();
    object["dockFeatures"] = (int)dockFeatures();
    object["siteFeatures"] = (int)siteFeatures();
    return true;
}

QString DockWidget::identifier()
{
    DockSite* dockSite = this->dockSite();

    if (dockSite == nullptr)
    {
        return objectName();
    }
    else
    {
        return QString("%1,%2,%3").arg(dockSite->identifier()).arg(dockSite->indexOf(this)).arg(objectName());
    }
}

void DockWidget::on_titleBar_buttonClicked(Flex::Button button, bool*)
{
    switch (button)
    {
    case Flex::AutoHide:
        break;
    case Flex::DockShow:
        break;
    default:
        break;
    }
}
