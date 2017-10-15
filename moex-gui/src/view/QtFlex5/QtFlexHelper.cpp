#include "QtFlexHelper.h"
#include "QtDockGuider.h"
#include <QtCore/QLibrary>
#include <QtCore/QEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>
#include <QtGui/QWindow>
#include <QtWidgets/QStylePainter>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QApplication>

#ifdef Q_OS_WIN
#define _WIN32_WINNT 0x0600

#include <qpa/qplatformnativeinterface.h>
#endif

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include <private/qwidgetresizehandler_p.h>

class FlexButton : public QToolButton
{
public:
    FlexButton(QWidget* parent, Flex::Button button) : QToolButton(parent), _button(button)
    {
        setFixedSize(16, 16); setFocusPolicy(Qt::NoFocus);
    }
    ~FlexButton()
    {
    }
    Flex::Button button() const
    {
        return _button;
    }
    void setButton(Flex::Button button)
    {
        _button = button; update();
    }
public:
    void setOver(bool over)
    {
        _over = over;
    }
    void setDown(bool down)
    {
        _down = down;
    }
protected:
    void mousePressEvent(QMouseEvent* evt)
    {
        auto save = _down;
        QToolButton::mousePressEvent(evt);
        _down = isDown();
        if (_down != save)
        {
            update();
        }
    }
    void mouseMoveEvent(QMouseEvent* evt)
    {
        auto save = _down;
        QToolButton::mouseMoveEvent(evt);
        _down = isDown();
        if (_down != save)
        {
            update();
        }
    }
    void mouseReleaseEvent(QMouseEvent* evt)
    {
        auto save = _down;
        QToolButton::mouseReleaseEvent(evt);
        _down = isDown();
        if (_down != save)
        {
            update();
        }
    }
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent* evt)
    {
        _over = true;
        if (isEnabled()) update();
        QToolButton::enterEvent(evt);
    }
    void leaveEvent(QEvent* evt)
    {
        _over = false;
        if (isEnabled()) update();
        QToolButton::leaveEvent(evt);
    }
private:
    Flex::Button _button;
    bool _over = false;
    bool _down = false;
};

class FlexButtons : public QWidget
{
public:
    FlexButtons(QWidget* parent, QWidget* docket) : QWidget(parent), _widget(docket)
    {
        setFocusPolicy(Qt::NoFocus);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        _layout = new QHBoxLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setSpacing(1);
        _clsButton = new FlexButton(this, Flex::Close);
        _maxButton = new FlexButton(this, Flex::Maximize);
        _minButton = new FlexButton(this, Flex::Minimize);
        _clsButton->setObjectName("_flex_close");
        _maxButton->setObjectName("_flex_maximize");
        _minButton->setObjectName("_flex_minimize");
        _clsButton->setToolTip(QWidget::tr("Close"));
        _maxButton->setToolTip(QWidget::tr("Maximize"));
        _minButton->setToolTip(QWidget::tr("Minimize"));
        _layout->addWidget(_minButton);
        _layout->addWidget(_maxButton);
        _layout->addWidget(_clsButton);
    }
    FlexButton* minButton() const { return _minButton; }
    FlexButton* maxButton() const { return _maxButton; }
    FlexButton* clsButton() const { return _clsButton; }

public:
    QWidget* _widget;
    QHBoxLayout* _layout;
    FlexButton* _clsButton;
    FlexButton* _minButton;
    FlexButton* _maxButton;
};

class FlexExtents : public QWidget
{
public:
    FlexExtents(QWidget* parent, QWidget* docket) : QWidget(parent), _docket(docket)
    {
        setFocusPolicy(Qt::NoFocus);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        _layout = new QHBoxLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setSpacing(1);
        _dockPullButton = new FlexButton(this, Flex::DockPull);
        _autoHideButton = new FlexButton(this, Flex::AutoHide);
        _dockPullButton->setObjectName("_flex_dockPull");
        _autoHideButton->setObjectName("_flex_autoHide");
        _layout->addWidget(_dockPullButton);
        _layout->addWidget(_autoHideButton);
    }
    FlexButton* dockPullButton() const { return _dockPullButton; }
    FlexButton* autoHideButton() const { return _autoHideButton; }

public:
    QWidget* _docket;
    QHBoxLayout* _layout;
    FlexButton* _dockPullButton;
    FlexButton* _autoHideButton;
};

void FlexButton::paintEvent(QPaintEvent*)
{
    auto widget = static_cast<FlexButtons*>(parentWidget())->_widget;

    bool active = widget->property("active").toBool();

    QRect geom = rect();

    QStylePainter painter(this);

    QIcon icon = FlexManager::instance()->icon(_button);

    painter.setPen(QColor("#E5C365"));
    painter.setBrush(QColor(_down ? "#FFE8A6" : "#FFFCF4"));

    if (_over)
    {
        painter.drawRect(geom.adjusted(0, 0, -1, -1));
    }

    QIcon::Mode mode = QIcon::Normal;

    Flex::ViewMode viewMode = (Flex::ViewMode)widget->property("viewMode").value<int>();

    if (viewMode == Flex::ToolView || viewMode == Flex::ToolPagesView)
    {
        mode = active ? QIcon::Active : QIcon::Normal;
    }

    icon.paint(&painter, geom, Qt::AlignCenter, mode, _over ? QIcon::On : QIcon::Off);
}

class FlexHelperImpl
{
public:
    FlexHelperImpl()
    {
    }

public:
    bool _moving = false;

    Qt::WindowFlags _windowFlags;

    FlexButtons* _buttons;
    FlexExtents* _extents;

    int _titleBarHeight = 27;
};

#ifdef Q_OS_WIN

typedef struct _MARGINS
{
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} MARGINS, *PMARGINS;

typedef HRESULT(WINAPI *ApiDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT(WINAPI *ApiDwmExtendFrameIntoClientArea)(HWND, PMARGINS);

static ApiDwmIsCompositionEnabled PtrDwmIsCompositionEnabled = reinterpret_cast<ApiDwmIsCompositionEnabled>(QLibrary::resolve("dwmapi.dll", "DwmIsCompositionEnabled"));
static ApiDwmExtendFrameIntoClientArea PtrDwmExtendFrameIntoClientArea = reinterpret_cast<ApiDwmExtendFrameIntoClientArea>(QLibrary::resolve("dwmapi.dll", "DwmExtendFrameIntoClientArea"));

#define FLEX_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define FLEX_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

class FlexHelperImplWin : public FlexHelperImpl
{
public:
    FlexHelperImplWin()
    {
        _dwmEnabled = (_dwmAllowed && PtrDwmIsCompositionEnabled && PtrDwmIsCompositionEnabled(&_dwmEnabled) == S_OK && _dwmEnabled);
        _borders[0] = 0;
        _borders[1] = 0;
        _borders[2] = 0;
        _borders[3] = 0;
    }

public:
    void notifyFrame(HWND hwnd);
    void redrawFrame(HWND hwnd);
    void updateFrame(HWND hwnd);
    void updateStyle(HWND hwnd);

public:
    BOOL modifyStyle(HWND hwnd, DWORD rsStyle, DWORD asStyle, UINT nFlags);

public:
    static LRESULT WINAPI keyEvent(int nCode, WPARAM wParam, LPARAM lParam);

public:
    BOOL _dwmAllowed = TRUE;
    BOOL _dwmEnabled = TRUE;

    BOOL _lock = 0;
    BOOL _skip = 1;
    BOOL _calc = 0;
    BOOL _curr = 1;
    BOOL _ctrl = 0;

    static HHOOK _hook;

    int _test = 0;
    int _wndW = 500;
    int _wndH = 500;
    int _borders[4];
};

HHOOK FlexHelperImplWin::_hook;

Q_DECLARE_METATYPE(QMargins)

void FlexHelperImplWin::notifyFrame(HWND hwnd)
{
    QGuiApplication::platformNativeInterface()->setWindowProperty(QWidget::find(reinterpret_cast<WId>(hwnd))->windowHandle()->handle(), QByteArrayLiteral("WindowsCustomMargins"), QVariant::fromValue(QMargins(-8, -31, -8, -8)));
}

void FlexHelperImplWin::redrawFrame(HWND hwnd)
{
    if (_dwmEnabled)
    {
        return;
    }

    RECT rc;
    RECT cc;
    GetWindowRect(hwnd, &rc);
    GetClientRect(hwnd, &cc);
    POINT pt = { cc.left, cc.top };
    ClientToScreen(hwnd, &pt);
    int h = rc.bottom - rc.top;
    int w = rc.right - rc.left;
    RECT sc = { 0, 0, w, h };
    HDC hDc = GetWindowDC(hwnd);
    HDC cDc = CreateCompatibleDC(hDc);
    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = rc.right - rc.left;
    bi.biHeight = rc.bottom - rc.top;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    HBITMAP hBitmap = CreateDIBSection(hDc, (BITMAPINFO*)&bi, 0, NULL, NULL, 0);
    HGDIOBJ hPixmap = SelectObject(cDc, hBitmap);
    HBRUSH hBr = CreateSolidBrush(_curr ? RGB(0, 125, 250) : RGB(125, 125, 125));
    HBRUSH hBb = CreateSolidBrush(RGB(125, 125, 125));
    HBRUSH hBa = CreateSolidBrush(RGB(252, 252, 252));
    HBRUSH hBc = CreateSolidBrush(RGB(232, 17, 35));
    int bl = pt.x - rc.left;
    int bt = pt.y - rc.top;
    int br = rc.right - pt.x - cc.right + cc.left;
    int bb = rc.bottom - pt.y - cc.bottom + cc.top;
    FillRect(cDc, &sc, hBr);
    RECT tmp1 = { sc.right - 36 * 1, 2, sc.right - 36 * 0 - 2, 32 };
    FillRect(cDc, &tmp1, _test == HTCLOSE ? hBc : hBb);
    RECT tmp2 = { sc.right - 36 * 2, 2, sc.right - 36 * 1 - 2, 32 };
    FillRect(cDc, &tmp2, _test == HTMAXBUTTON ? hBa : hBb);
    RECT tmp3 = { sc.right - 36 * 3, 2, sc.right - 36 * 2 - 2, 32 };
    FillRect(cDc, &tmp3, _test == HTMINBUTTON ? hBa : hBb);
    BitBlt(hDc, 0, 0, bl, h, cDc, 0, 0, SRCCOPY);
    BitBlt(hDc, 0, 0, w, bt, cDc, 0, 0, SRCCOPY);
    BitBlt(hDc, w - br, 0, br, h, cDc, 0, 0, SRCCOPY);
    BitBlt(hDc, 0, h - bb, w, bb, cDc, 0, 0, SRCCOPY);
    SelectObject(cDc, hPixmap);
    DeleteObject(hBa);
    DeleteObject(hBb);
    DeleteObject(hBc);
    DeleteObject(hBr);
    DeleteObject(hBitmap);
    DeleteDC(cDc);
    ReleaseDC(hwnd, hDc);
}

void FlexHelperImplWin::updateFrame(HWND hwnd)
{
    if (_calc)
    {
        return;
    }

    _calc = TRUE;
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    if (_wndW != w || _wndH != h)
    {
        if (_dwmEnabled)
        {
            MARGINS tmp = { 1, 1, 1, 1 };
            if (PtrDwmExtendFrameIntoClientArea(hwnd, &tmp) == S_OK)
            {
            }
        }
        else
        {
            DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
            DWORD exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
            if (!(dwStyle & WS_MAXIMIZE) || !(exStyle & WS_EX_MDICHILD))
            {
                HRGN hRgn = 0;
                if (dwStyle & WS_MAXIMIZE)
                {
                    int frameBorder = 2;
                    int frameRegion = frameBorder - (exStyle & WS_EX_CLIENTEDGE ? 2 : 0);
                    hRgn = CreateRectRgn(frameRegion, frameRegion, w - frameRegion, h - frameRegion);
                    _borders[0] = frameBorder;
                    _borders[1] = frameBorder;
                    _borders[2] = frameBorder;
                    _borders[3] = frameBorder;
                }
                else
                {
                    int frameBorder = 1;
                    hRgn = CreateRectRgn(0, 0, w, h);
                    _borders[0] = frameBorder;
                    _borders[1] = frameBorder;
                    _borders[2] = frameBorder;
                    _borders[3] = frameBorder;
                }
                SetWindowRgn(hwnd, hRgn, TRUE);
            }
        }
    }

    _wndW = w;
    _wndH = h;
    _calc = FALSE;
}

void FlexHelperImplWin::updateStyle(HWND hwnd)
{
    if (_dwmEnabled)
    {
        DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
        DWORD rsStyle = WS_THICKFRAME | WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL;
        if (dwStyle & rsStyle)
        {
            RECT rc;
            _lock = TRUE;
            SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle & ~rsStyle);
            GetWindowRect(hwnd, &rc);
            SendMessage(hwnd, WM_NCCALCSIZE, FALSE, (LPARAM)&rc);
            if (_windowFlags & Qt::WindowMinimizeButtonHint)
            {
                dwStyle |= WS_MINIMIZEBOX;
            }
            if (_windowFlags & Qt::WindowMaximizeButtonHint)
            {
                dwStyle |= WS_MAXIMIZEBOX;
            }
            if (_windowFlags & Qt::WindowSystemMenuHint)
            {
                dwStyle |= WS_SYSMENU;
            }
            SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle | WS_CAPTION);
            _lock = FALSE;
            notifyFrame(hwnd);
        }
    }
    else
    {
        DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
        DWORD rsStyle = WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL;
        if (dwStyle & rsStyle)
        {
            RECT rc;
            _lock = TRUE;
            SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle & ~rsStyle);
            GetWindowRect(hwnd, &rc);
            SendMessage(hwnd, WM_NCCALCSIZE, FALSE, (LPARAM)&rc);
            SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle);
            _lock = FALSE;
            redrawFrame(hwnd);
        }
    }
}

BOOL FlexHelperImplWin::modifyStyle(HWND hwnd, DWORD rsStyle, DWORD asStyle, UINT nFlags)
{
    DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
    DWORD nsStyle = (dwStyle & ~rsStyle) | asStyle;

    if (dwStyle == nsStyle)
    {
        return FALSE;
    }

    SetWindowLongPtr(hwnd, GWL_STYLE, nsStyle);

    if (nFlags != 0)
    {
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
    }

    return TRUE;
}

#endif

#ifdef Q_OS_MAC

class FlexHandler : public QWidgetResizeHandler
{
public:
    FlexHandler(QWidget* parent, QWidget* widget) : QWidgetResizeHandler(parent, widget)
    {
        setMovingEnabled(false);
    }

public:
    bool eventFilter(QObject* obj, QEvent* evt)
    {
        if (obj->property("Site").isValid())
        {
            return false;
        }
        else
        {
            return QWidgetResizeHandler::eventFilter(obj, evt);
        }
    }
};

class FlexHelperImplMac : public FlexHelperImpl
{
public:
    FlexHelperImplMac()
    {
    }
public:
    void hittest(QWidget* widget, const QPoint& pos);
public:
    bool eventFilter(QObject*, QEvent*);
public:
    QWidgetResizeHandler* _handler = nullptr;
    int _hit = -1;
    QPoint _old;
};

void FlexHelperImplMac::hittest(QWidget* widget, const QPoint& pos)
{
    QRect rect = widget->rect();

    int x = rect.x();
    int y = rect.y();
    int w = rect.width();

    if (pos.y() >= y + 4 && pos.y() < y + _titleBarHeight && pos.x() >= x + 4 && pos.x() < x + w - 4)
    {
        _hit = 0;
    }
    else
    {
        _hit = -1;
    }
}

bool FlexHelperImplMac::eventFilter(QObject* obj, QEvent* evt)
{
    auto widget = qobject_cast<QWidget*>(obj);

    switch (evt->type())
    {
    case QEvent::WindowStateChange:
    {
        auto state = widget->windowState();

        if (state & Qt::WindowMaximized)
        {
            _buttons->maxButton()->setButton(Flex::Restore);
            _buttons->maxButton()->setToolTip(QWidget::tr("Restore"));
        }
        else
        {
            _buttons->maxButton()->setToolTip(QWidget::tr("Maximize"));
            _buttons->maxButton()->setButton(Flex::Maximize);
        }

        break;
    }
    case QEvent::MouseButtonPress:
    {
        if (widget->isMaximized())
        {
            break;
        }

        QMouseEvent* event = static_cast<QMouseEvent*>(evt);

        if (event->button() == Qt::LeftButton)
        {
            hittest(widget, event->pos());

            if (_hit >= 0)
            {
                widget->grabKeyboard();

                if (_hit == 0)
                {
                    _moving = true; QMetaObject::invokeMethod(widget, "enterMove", Q_ARG(QObject*, widget));
                }

                _old = event->globalPos();
            }
        }

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        if (widget->isMaximized())
        {
            break;
        }

        QMouseEvent* event = static_cast<QMouseEvent*>(evt);

        if (event->button() == Qt::LeftButton)
        {
            widget->releaseKeyboard();

            if (_moving)
            {
                QMetaObject::invokeMethod(widget, "leaveMove", Q_ARG(QObject*, widget));
            }

            _moving = false;
        }

        break;
    }
    case QEvent::MouseMove:
    {
        if (widget->isMaximized())
        {
            break;
        }

        QMouseEvent* event = static_cast<QMouseEvent*>(evt);

        if (_moving)
        {
            if (widget->testAttribute(Qt::WA_WState_ConfigPending))
            {
                break;
            }

            QPoint off = event->globalPos() - _old;

            if (_hit == 0)
            {
                widget->move(widget->pos() + off);
            }

            _old = event->globalPos();
        }
        else
        {
            hittest(widget, event->pos());
        }

        break;
    }
    case QEvent::MouseButtonDblClick:
    {
        if (widget->isMinimized())
        {
            break;
        }

        QMouseEvent* event = static_cast<QMouseEvent*>(evt);

        if (event->button() == Qt::LeftButton)
        {
            if (_hit == 0)
            {
                widget->showMinimized(); _hit = -1;
            }
        }

        break;
    }
    case QEvent::KeyPress:
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(evt);
        if (_moving && (event->key() == Qt::Key_Control || event->key() == Qt::Key_Meta))
        {
            if (DockGuider::instance())
            {
                DockGuider::instance()->hide();
            }
        }
        break;
    }
    case QEvent::KeyRelease:
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(evt);
        if (_moving && (event->key() == Qt::Key_Control || event->key() == Qt::Key_Meta))
        {
            if (DockGuider::instance())
            {
                DockGuider::instance()->show();
            }
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return false;
}

#endif

#ifdef Q_OS_WIN
FlexHelper::FlexHelper(QWidget* parent) : QObject(parent), impl(new FlexHelperImplWin)
{
    auto d = static_cast<FlexHelperImplWin*>(impl.data());
    d->_buttons = new FlexButtons(parent, parent);
    d->_extents = new FlexExtents(parent, parent);
    connect(d->_extents->_dockPullButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_extents->_autoHideButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_buttons->_clsButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_buttons->_maxButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_buttons->_minButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    parent->installEventFilter(this);
}
#endif

#ifdef Q_OS_MAC
FlexHelper::FlexHelper(QWidget* parent) : QObject(parent), impl(new FlexHelperImplMac)
{
    auto d = static_cast<FlexHelperImplMac*>(impl.data());
    d->_handler = new FlexHandler(parent, parent);
    d->_buttons = new FlexButtons(parent, parent);
    d->_extents = new FlexExtents(parent, parent);
    connect(d->_extents->_dockPullButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_extents->_autoHideButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_buttons->_clsButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_buttons->_maxButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    connect(d->_buttons->_minButton, SIGNAL(clicked()), SLOT(on_button_clicked()));
    parent->installEventFilter(this);
}
#endif

FlexHelper::~FlexHelper()
{
    impl->_buttons->setParent(nullptr);
    impl->_extents->setParent(nullptr);
    impl->_buttons->deleteLater();
    impl->_extents->deleteLater();
}

QWidget* FlexHelper::buttons() const
{
    return impl->_buttons;
}

QWidget* FlexHelper::extents() const
{
    return impl->_extents;
}

QToolButton* FlexHelper::button(Flex::Button button) const
{
    switch (button)
    {
    case Flex::DockPull:
        return impl->_extents->_dockPullButton;
    case Flex::AutoHide:
        return impl->_extents->_autoHideButton;
    case Flex::DockShow:
        return impl->_extents->_autoHideButton;
    case Flex::Minimize:
        return impl->_buttons->minButton();
    case Flex::Maximize:
        return impl->_buttons->maxButton();
    case Flex::Restore:
        return impl->_buttons->maxButton();
    case Flex::Close:
        return impl->_buttons->clsButton();
    default:
        Q_ASSERT(false);
    }
    return nullptr;
}

void FlexHelper::change(Flex::Button src, Flex::Button dst)
{
    static_cast<FlexButton*>(button(src))->setButton(dst);
}

void FlexHelper::setWindowInfo(int titleBarHeight, Qt::WindowFlags windowFlags)
{
    impl->_titleBarHeight = titleBarHeight;
    impl->_windowFlags = windowFlags;
}

bool FlexHelper::eventFilter(QObject* obj, QEvent* evt)
{
#ifdef Q_OS_WIN
    auto hwnd = qobject_cast<QWidget*>(obj)->internalWinId();
#endif
#ifdef Q_OS_WIN
    auto d = static_cast<FlexHelperImplWin*>(impl.data());
#endif
#ifdef Q_OS_MAC
    auto d = static_cast<FlexHelperImplMac*>(impl.data());
#endif

    if (evt->type() == QEvent::Resize)
    {
        auto tmp = static_cast<QResizeEvent*>(evt);

        int w = tmp->size().width();

        QSize buttonsSize = impl->_buttons->sizeHint();
        QSize extentsSize = impl->_extents->sizeHint();

        auto aw = 0;
        auto bw = buttonsSize.width();
        auto ew = extentsSize.width();
        auto by = (impl->_titleBarHeight - buttonsSize.height()) / 2 + 1;
        auto ey = (impl->_titleBarHeight - extentsSize.height()) / 2 + 1;

        impl->_buttons->minButton()->setOver(false);
        impl->_buttons->maxButton()->setOver(false);
        impl->_buttons->clsButton()->setOver(false);
        impl->_buttons->setGeometry(QRect(QPoint(w - bw - aw - 5, by), buttonsSize));
        impl->_extents->dockPullButton()->setOver(false);
        impl->_extents->autoHideButton()->setOver(false);
        impl->_extents->setGeometry(QRect(QPoint(w - bw - ew - 6, ey), extentsSize));

        return false;
    }

    if (obj->property("Site").isValid())
    {
        return false;
    }

    if (evt->type() == QEvent::WinIdChange)
    {
#ifdef Q_OS_WIN
        d->updateStyle(reinterpret_cast<HWND>(hwnd));
#endif
    }
    else if (evt->type() == QEvent::Move)
    {
        if (impl->_moving)
        {
            QMetaObject::invokeMethod(obj, "moving", Q_ARG(QObject*, obj));
        }
    }
#ifndef Q_OS_WIN
    else
    {
        return d->eventFilter(obj, evt);
    }
#endif

    return false;
}

#ifdef Q_OS_WIN
LRESULT WINAPI FlexHelperImplWin::keyEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx(FlexHelperImplWin::_hook, nCode, wParam, lParam);
    }
 
    if (wParam == VK_CONTROL && DockGuider::instance())
    {
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            DockGuider::instance()->hide();
        }
        else
        {
            DockGuider::instance()->show();
        }
    }

    return CallNextHookEx(FlexHelperImplWin::_hook, nCode, wParam, lParam);
}
#endif

bool FlexHelper::nativeEvent(const QByteArray&, void* event, long* result)
{
#ifdef Q_OS_WIN
    auto d = static_cast<FlexHelperImplWin*>(impl.data());
    auto msg = reinterpret_cast<MSG*>(event);
    auto hwnd = msg->hwnd;
    auto message = msg->message;
    auto lParam = msg->lParam;
    auto wParam = msg->wParam;

    if (!d->_dwmEnabled && d->_lock)
    {
        switch (message)
        {
        case WM_STYLECHANGING:
        case WM_STYLECHANGED:
        case WM_WINDOWPOSCHANGED:
        case WM_NCPAINT:
            *result = 0;
            return true;
        case WM_WINDOWPOSCHANGING:
            *result = 0;
            reinterpret_cast<WINDOWPOS*>(lParam)->flags &= ~SWP_FRAMECHANGED;
            return true;
        }
    }

    QObject* object = parent();

    switch (message)
    {
    case WM_ENTERSIZEMOVE:
    {
        if (d->_moving)
        {
            d->_moving = true;
            QMetaObject::invokeMethod(object, "enterMove", Q_ARG(QObject*, object));
            if (FlexHelperImplWin::_hook == nullptr)
            {
                FlexHelperImplWin::_hook = SetWindowsHookEx(WH_KEYBOARD, FlexHelperImplWin::keyEvent, NULL, GetCurrentThreadId());
            }
        }
        QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        if (d->_moving)
        {
            d->_moving = false;
            if (FlexHelperImplWin::_hook != nullptr)
            {
                UnhookWindowsHookEx(FlexHelperImplWin::_hook);
                FlexHelperImplWin::_hook = nullptr;
            }
            QMetaObject::invokeMethod(object, "leaveMove", Q_ARG(QObject*, object));
        }
        QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        break;
    }
    case WM_DWMCOMPOSITIONCHANGED:
    {
        BOOL dwmEnabled = TRUE;
        dwmEnabled = (d->_dwmAllowed && PtrDwmIsCompositionEnabled && PtrDwmIsCompositionEnabled(&dwmEnabled) == S_OK && dwmEnabled);
        if (d->_dwmEnabled != dwmEnabled)
        {
            d->_dwmEnabled = dwmEnabled;
            SetWindowRgn(hwnd, 0, TRUE);
            if (!d->_dwmEnabled)
            {
                d->updateStyle(hwnd);
            }
            SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        break;
    }
    case WM_WINDOWPOSCHANGING:
    {
        DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (((wp->flags & SWP_NOSIZE) == 0) && (wp->cx != d->_wndW || wp->cy != d->_wndH) && ((dwStyle & WS_CHILD) == 0))
        {
            d->_skip = TRUE;
        }
        break;
    }
    case WM_WINDOWPOSCHANGED:
    {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if ((wp->flags & SWP_FRAMECHANGED) && !d->_calc)
        {
            d->updateFrame(hwnd);
        }
        break;
    }
    case WM_SIZE:
    {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        if (w != d->_wndW || h != d->_wndH)
        {
            d->updateFrame(hwnd);
            d->redrawFrame(hwnd);
        }
        if (wParam == SIZE_MINIMIZED)
        {
            if (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_MAXIMIZE)
            {
                d->modifyStyle(hwnd, WS_MAXIMIZE, 0, 0);
            }
        }
        else if (wParam == SIZE_RESTORED)
        {
            d->_buttons->maxButton()->setButton(Flex::Maximize);
            d->_buttons->maxButton()->setToolTip(QWidget::tr("Maximize"));
        }
        else if (wParam == SIZE_MAXIMIZED)
        {
            d->_buttons->maxButton()->setToolTip(QWidget::tr("Restore"));
            d->_buttons->maxButton()->setButton(Flex::Restore);
        }
        break;
    }
    case WM_STYLECHANGED:
    {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        d->updateFrame(hwnd);
        d->redrawFrame(hwnd);
        SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        RedrawWindow(hwnd, &rc, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
        *result = 0;
        return true;
    }
    case WM_NCRBUTTONUP:
    {
        SendMessage(hwnd, 0x0313, (WPARAM)hwnd, lParam);
        break;
    }
    case WM_SYSCOMMAND:
    {
        if ((msg->wParam & 0xFFF0) == SC_MOVE && !IsIconic(hwnd) && !(GetKeyState(VK_CONTROL) & 0x8000))
        {
            d->_moving = true;
        }
        break;
    }
    case WM_GETMINMAXINFO:
    {
        *result = DefWindowProc(hwnd, message, wParam, lParam);
        MINMAXINFO* lpmmi = reinterpret_cast<MINMAXINFO*>(lParam);
        int yMin = 40;
        int xMin = 3 * yMin;
        xMin += GetSystemMetrics(SM_CYSIZE) + 2 * GetSystemMetrics(SM_CXEDGE);
        //lpmmi->ptMaxPosition.x = -4;
        //lpmmi->ptMaxPosition.y = -4;
        lpmmi->ptMinTrackSize.x = std::max(lpmmi->ptMinTrackSize.x, (LONG)xMin);
        lpmmi->ptMinTrackSize.y = std::max(lpmmi->ptMinTrackSize.y, (LONG)yMin);
        //lpmmi->ptMaxTrackSize.x -= 4;
        //lpmmi->ptMaxTrackSize.y -= 4;
        return true;
    }
    }

    if (d->_dwmEnabled)
    {
        switch (message)
        {
        case WM_NCCALCSIZE:
        {
            if (!d->_lock && wParam)
            {
                auto lpncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

                auto l = lpncsp->rgrc[0].left;
                auto r = lpncsp->rgrc[0].right;
                auto t = lpncsp->rgrc[0].top;
                auto b = lpncsp->rgrc[0].bottom;

                *result = DefWindowProc(hwnd, message, wParam, lParam);

                DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);

                if ((dwStyle & (WS_CHILD | WS_MINIMIZE)) != (WS_CHILD | WS_MINIMIZE))
                {
                    lpncsp->rgrc[0].left   = l;
                    lpncsp->rgrc[0].right  = r;
                    lpncsp->rgrc[0].top    = t;
                    lpncsp->rgrc[0].bottom = b;
                }

                return true;
            }
            break;
        }
        case WM_NCHITTEST:
        {
            POINT pt;
            pt.x = FLEX_GET_X_LPARAM(lParam);
            pt.y = FLEX_GET_Y_LPARAM(lParam);

            QWidget* window = QWidget::find(reinterpret_cast<WId>(hwnd));

            QPoint pos = window->mapFromGlobal(QPoint(pt.x, pt.y));

            if (d->_buttons->geometry().contains(pos) || d->_extents->geometry().contains(pos))
            {
                *result = HTCLIENT; return true;
            }

            RECT rw;
            GetWindowRect(hwnd, &rw);

            if (pt.y >= rw.top + 4 && pt.y < rw.top + d->_titleBarHeight && pt.x >= rw.left + 4 && pt.x < rw.right - 4)
            {
                *result = HTCAPTION; return true;
            }

            int row = 1;
            int col = 1;

            if (pt.y >= rw.top && pt.y < rw.top + 4)
            {
                row = 0;
            }
            else if (pt.y < rw.bottom && pt.y >= rw.bottom - 4)
            {
                row = 2;
            }

            if (pt.x >= rw.left && pt.x < rw.left + 4)
            {
                col = 0;
            }
            else if (pt.x < rw.right && pt.x >= rw.right - 4)
            {
                col = 2;
            }

            LRESULT hitTests[3][3] =
            {
                { HTTOPLEFT, HTTOP, HTTOPRIGHT },
                { HTLEFT, HTCLIENT, HTRIGHT },
                { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
            };

            *result = hitTests[row][col];

            return true;
        }
        }
    }
    else
    {
        switch (message)
        {
        case WM_NCCALCSIZE:
        {
            if (!d->_lock && wParam)
            {
                auto lpncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                lpncsp->rgrc[0].left += d->_borders[0];
                lpncsp->rgrc[0].top += d->_borders[1];
                lpncsp->rgrc[0].right -= d->_borders[2];
                lpncsp->rgrc[0].bottom -= d->_borders[3];
                DWORD dwStyle = ::GetWindowLongPtr(hwnd, GWL_STYLE);
                if (((dwStyle & (WS_MAXIMIZE | WS_CHILD)) == WS_MAXIMIZE))
                {
                    APPBARDATA abd; abd.cbSize = sizeof(APPBARDATA);
                    if ((SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0)
                    {
                        lpncsp->rgrc[0].bottom -= 1;
                    }
                }
                *result = 0;
                return true;
            }
            break;
        }
        case WM_NCHITTEST:
        {
            POINT pt;
            pt.x = FLEX_GET_X_LPARAM(lParam);
            pt.y = FLEX_GET_Y_LPARAM(lParam);

            QWidget* window = QWidget::find(reinterpret_cast<WId>(hwnd));

            QPoint pos = window->mapFromGlobal(QPoint(pt.x, pt.y));

            if (d->_buttons->geometry().contains(pos) || d->_extents->geometry().contains(pos))
            {
                *result = HTCLIENT; return true;
            }

            RECT rw;
            GetWindowRect(hwnd, &rw);

            if (pt.y >= rw.top + 4 && pt.y < rw.top + d->_titleBarHeight && pt.x >= rw.left + 4 && pt.x < rw.right - 4)
            {
                *result = HTCAPTION; return true;
            }

            int row = 1;
            int col = 1;

            if (pt.y >= rw.top && pt.y < rw.top + 4)
            {
                row = 0;
            }
            else if (pt.y < rw.bottom && pt.y >= rw.bottom - 4)
            {
                row = 2;
            }

            if (pt.x >= rw.left && pt.x < rw.left + 4)
            {
                col = 0;
            }
            else if (pt.x < rw.right && pt.x >= rw.right - 4)
            {
                col = 2;
            }

            LRESULT hitTests[3][3] =
            {
                { HTTOPLEFT, HTTOP, HTTOPRIGHT },
                { HTLEFT, HTCLIENT, HTRIGHT },
                { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
            };

            *result = hitTests[row][col];

            return true;
        }
        case WM_NCPAINT:
        {
            *result = 0;
            if (d->_skip)
            {
                d->_skip = FALSE;
                return true;
            }
            DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
            DWORD exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
            if (!(dwStyle & WS_MAXIMIZE) || !(exStyle & WS_EX_MDICHILD))
            {
                d->redrawFrame(hwnd);
                d->_skip = TRUE;
            }
            return true;
        }
        case WM_NCACTIVATE:
        {
            DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
            if (dwStyle & WS_VISIBLE)
            {
                DWORD rsStyle = WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL;
                if (dwStyle & rsStyle)
                {
                    d->_lock = TRUE;
                    SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle & ~rsStyle);
                    RECT rc;
                    GetWindowRect(hwnd, &rc);
                    SendMessage(hwnd, WM_NCCALCSIZE, FALSE, (LPARAM)&rc);
                    SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle);
                    d->_lock = FALSE;
                    d->redrawFrame(hwnd);
                }
                d->_lock = TRUE;
                if (dwStyle & WS_SIZEBOX)
                {
                    d->modifyStyle(hwnd, WS_SIZEBOX, 0, 0);
                }
                *result = DefWindowProc(hwnd, message, wParam, lParam);
                if (dwStyle & WS_SIZEBOX)
                {
                    d->modifyStyle(hwnd, 0, WS_SIZEBOX, 0);
                }
                d->_lock = FALSE;
                return true;
            }
            break;
        }
        case WM_ACTIVATEAPP:
        {
            if (d->_curr != wParam)
            {
                d->_curr = wParam;
                d->redrawFrame(hwnd);
            }
            break;
        }
        }
    }

    return false;
#else
    return false;
#endif
}

void FlexHelper::on_button_clicked()
{
    bool accepted = false;

    auto object = static_cast<FlexButton*>(sender());

    emit clicked(object->button(), &accepted);

    if (!accepted)
    {
        auto window = object->window();
        switch (object->button())
        {
        case Flex::Minimize:
            if (window->isTopLevel())
            {
#ifdef Q_OS_WIN
                ::SendMessage(reinterpret_cast<HWND>(window->internalWinId()), WM_SYSCOMMAND, SC_MINIMIZE, 0);
#else
                window->showMinimized();
#endif
            }
            break;
        case Flex::Maximize:
            if (window->isTopLevel())
            {
#ifdef Q_OS_WIN
                SendMessage(reinterpret_cast<HWND>(window->internalWinId()), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#else
                window->showMaximized();
#endif
            }
            break;
        case Flex::Restore:
            if (window->isTopLevel())
            {
#ifdef Q_OS_WIN
                SendMessage(reinterpret_cast<HWND>(window->internalWinId()), WM_SYSCOMMAND, SC_RESTORE, 0);
#else
                window->showNormal();
#endif
            }
            break;
        case Flex::Close:
            if (window->isTopLevel())
            {
#ifdef Q_OS_WIN
                SendMessage(reinterpret_cast<HWND>(window->internalWinId()), WM_SYSCOMMAND, SC_CLOSE, 0);
#else
                window->close();
#endif
            }
            break;
        default:
            break;
        }
    }
}
