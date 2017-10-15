#include "QtFlexManager.h"
#include "QtFlexWidget.h"
#include "QtDockWidget.h"
#include "QtDockSite.h"
#include <QtCore/QAbstractNativeEventFilter>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

int Flex::Update = QEvent::registerEventType();

#ifdef Q_OS_WIN
WId topLevelWindowAt(QWidget* widget, const QPoint& pos)
{
    WId hTmp = 0;
    HWND hWnd = GetWindow(reinterpret_cast<HWND>(widget->effectiveWinId()), GW_HWNDNEXT);
    while (hWnd != nullptr && reinterpret_cast<HWND>(hTmp) == nullptr)
    {
        POINT pnt = { pos.x(), pos.y() };
        ScreenToClient(hWnd, &pnt);
        hTmp = reinterpret_cast<WId>(ChildWindowFromPoint(hWnd, pnt));
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }
    return reinterpret_cast<WId>(GetAncestor(reinterpret_cast<HWND>(hTmp), GA_ROOT));
}
#endif

DockSite* getDockSite(QWidget* widget)
{
    for (DockSite* site = nullptr; widget && !site; widget = widget->parentWidget())
    {
        if ((site = qobject_cast<DockSite*>(widget)) != nullptr)
        {
            return site;
        }
    }
    return nullptr;
}

namespace
{
    typedef std::tuple<QByteArray, int> SnapshotNode;
    typedef QMap<int, SnapshotNode> SnapshotList;
    typedef std::tuple<QString, QString, int> SnapshotItem;
    typedef QMap<QString, SnapshotItem> SnapshotDict;
}

class FlexManagerImpl : public QAbstractNativeEventFilter
{
public:
    FlexManagerImpl() : _ready(false)
    {
    }

public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);

public:
    QString flexWidgetName(DockWidget* dockWidget) const;
    QString flexWidgetName(FlexWidget* flexWidget) const;

public:
    int generate() const;

public:
    bool equalIdentifer(const QString& id1, const QString& id2) const;

public:
    QString _flexWidgetDestorying;
    QString _dockWidgetDestorying;
    QList<FlexWidget*> _flexWidgets;
    QList<DockWidget*> _dockWidgets;
    QList<QIcon> _buttonIcons;
    SnapshotList _snapshotList;
    SnapshotDict _snapshotDict;
    bool _ready;
};

bool FlexManagerImpl::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    return false;
}

QString FlexManagerImpl::flexWidgetName(DockWidget* dockWidget) const
{
    if (_flexWidgetDestorying.isEmpty())
    {
        return dockWidget->flexWidgetName();
    }
    else
    {
        return _flexWidgetDestorying;
    }
}

QString FlexManagerImpl::flexWidgetName(FlexWidget* flexWidget) const
{
    return flexWidget->objectName();
}

int FlexManagerImpl::generate() const
{
    static int i = 0; return i++;
}

bool FlexManagerImpl::equalIdentifer(const QString& id1, const QString& id2) const
{
    QStringList parts1 = id1.split(",");
    QStringList parts2 = id2.split(",");
    if (parts1.size() != parts2.size())
    {
        return false;
    }
    if (parts1[0] != parts2[0])
    {
        return false;
    }
    for (int i = 1; i < parts1.size() - 1; i += 2)
    {
        if (parts1[i] != parts2[i])
        {
            return false;
        }
    }
    return true;
}

FlexManager::FlexManager() : impl(new FlexManagerImpl)
{
    Q_ASSERT(qApp != nullptr);
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(on_app_focusChanged(QWidget*, QWidget*)));
    connect(this, SIGNAL(guiderHover(FlexWidget*, QWidget*)), SLOT(on_flexWidget_guiderHover(FlexWidget*, QWidget*)));
    connect(this, SIGNAL(guiderShow(FlexWidget*, QWidget*)), SLOT(on_flexWidget_guiderShow(FlexWidget*, QWidget*)));
    connect(this, SIGNAL(guiderHide(FlexWidget*, QWidget*)), SLOT(on_flexWidget_guiderHide(FlexWidget*, QWidget*)));
    connect(this, SIGNAL(guiderDrop(FlexWidget*, DockWidget*)), SLOT(on_flexWidget_guiderDrop(FlexWidget*, DockWidget*)));
    connect(this, SIGNAL(guiderDrop(FlexWidget*, FlexWidget*)), SLOT(on_flexWidget_guiderDrop(FlexWidget*, FlexWidget*)));
    QPixmap extentsPixmap(":/Resources/extents.png");
    for (int i = 0; i < 3; i++)
    {
        QIcon icon;
        icon.addPixmap(extentsPixmap.copy(i * 16,  0, 16, 16), QIcon::Active, QIcon::On);
        icon.addPixmap(extentsPixmap.copy(i * 16, 16, 16, 16), QIcon::Active, QIcon::Off);
        icon.addPixmap(extentsPixmap.copy(i * 16, 32, 16, 16), QIcon::Normal, QIcon::Off);
        impl->_buttonIcons.append(icon);
    }
    QPixmap buttonsPixmap(":/Resources/buttons.png");
    for (int i = 0; i < 4; i++)
    {
        QIcon icon;
        icon.addPixmap(buttonsPixmap.copy(i * 16,  0, 16, 16), QIcon::Active, QIcon::On);
        icon.addPixmap(buttonsPixmap.copy(i * 16, 16, 16, 16), QIcon::Active, QIcon::Off);
        icon.addPixmap(buttonsPixmap.copy(i * 16, 32, 16, 16), QIcon::Normal, QIcon::Off);
        impl->_buttonIcons.append(icon);
    }
    
    qApp->installNativeEventFilter(impl.data());
}

FlexManager::~FlexManager()
{

}

FlexManager* FlexManager::instance()
{
    static FlexManager manager; return &manager;
}

FlexWidget* FlexManager::createFlexWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags, const QString& flexWidgetName)
{
#ifdef _DEBUG
    if (!flexWidgetName.isEmpty() && hasFlexWidget(flexWidgetName))
    {
        qWarning() << tr("FlexManager::createFlexWidget: FlexWidget [%1] alreay exists!").arg(flexWidgetName);
    }
#endif
    FlexWidget* widget = new FlexWidget(viewMode, parent, flags);
    widget->setObjectName(flexWidgetName.isEmpty() ? QUuid::createUuid().toString().toUpper() : flexWidgetName);
    widget->setWindowTitle(flexWidgetName);
    connect(widget, SIGNAL(destroyed(QObject*)), SLOT(on_flexWidget_destroyed(QObject*)));
    connect(widget, SIGNAL(enterMove(QObject*)), SLOT(on_flexWidget_enterMove(QObject*)));
    connect(widget, SIGNAL(leaveMove(QObject*)), SLOT(on_flexWidget_leaveMove(QObject*)));
    connect(widget, SIGNAL(moving(QObject*)), SLOT(on_flexWidget_moving(QObject*)));
    connect(widget, SIGNAL(destroying(FlexWidget*)), SLOT(on_flexWidget_destroying(FlexWidget*)));
    widget->installEventFilter(this);
    impl->_flexWidgets.append(widget);
    emit flexWidgetCreated(widget);
    return widget;
}

DockWidget* FlexManager::createDockWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags, const QString& dockWidgetName)
{
#ifdef _DEBUG
    if (!dockWidgetName.isEmpty() && hasDockWidget(dockWidgetName))
    {
        qWarning() << tr("FlexManager::createDockWidget: DockWidget <%1> alreay exists!").arg(dockWidgetName);
    }
#endif
    DockWidget* widget = new DockWidget(viewMode, parent, flags);
    widget->setObjectName(dockWidgetName.isEmpty() ? QUuid::createUuid().toString().toUpper() : dockWidgetName);
    widget->setWindowTitle(dockWidgetName);
    connect(widget, SIGNAL(destroyed(QObject*)), SLOT(on_dockWidget_destroyed(QObject*)));
    connect(widget, SIGNAL(enterMove(QObject*)), SLOT(on_dockWidget_enterMove(QObject*)));
    connect(widget, SIGNAL(leaveMove(QObject*)), SLOT(on_dockWidget_leaveMove(QObject*)));
    connect(widget, SIGNAL(moving(QObject*)), SLOT(on_dockWidget_moving(QObject*)));
    connect(widget, SIGNAL(destroying(DockWidget*)), SLOT(on_dockWidget_destroying(DockWidget*)));
    widget->installEventFilter(this);
    impl->_dockWidgets.append(widget);
    emit dockWidgetCreated(widget);
    return widget;
}

bool FlexManager::hasFlexWidget(const QString& flexWidgetName) const
{
    return std::find_if(impl->_flexWidgets.begin(), impl->_flexWidgets.end(), [&](FlexWidget* flexWidget) { return flexWidget->objectName() == flexWidgetName; }) != impl->_flexWidgets.end();
}

bool FlexManager::hasDockWidget(const QString& dockWidgetName) const
{
    return std::find_if(impl->_dockWidgets.begin(), impl->_dockWidgets.end(), [&](DockWidget* dockWidget) { return dockWidget->objectName() == dockWidgetName; }) != impl->_dockWidgets.end();
}

FlexWidget* FlexManager::flexWidget(const QString& flexWidgetName) const
{
    auto iter = std::find_if(impl->_flexWidgets.begin(), impl->_flexWidgets.end(), [&](FlexWidget* flexWidget) { return flexWidget->objectName() == flexWidgetName; });
    return iter != impl->_flexWidgets.end() ? *iter : nullptr;
}

DockWidget* FlexManager::dockWidget(const QString& dockWidgetName) const
{
    auto iter = std::find_if(impl->_dockWidgets.begin(), impl->_dockWidgets.end(), [&](DockWidget* dockWidget) { return dockWidget->objectName() == dockWidgetName; });
    return iter != impl->_dockWidgets.end() ? *iter : nullptr;
}

int FlexManager::flexWidgetCount() const
{
    return (int)impl->_flexWidgets.size();
}

int FlexManager::dockWidgetCount() const
{
    return (int)impl->_dockWidgets.size();
}

FlexWidget* FlexManager::flexWidgetAt(int index) const
{
    Q_ASSERT(index >= 0 && index < impl->_flexWidgets.size());
    return impl->_flexWidgets[index];
}

DockWidget* FlexManager::dockWidgetAt(int index) const
{
    Q_ASSERT(index >= 0 && index < impl->_dockWidgets.size());
    return impl->_dockWidgets[index];
}

QIcon FlexManager::icon(Flex::Button button)
{
    return impl->_buttonIcons[button];
}

void FlexManager::close()
{
    for (auto iter = impl->_dockWidgets.begin(); iter != impl->_dockWidgets.end(); ++iter)
    {
        if ((*iter)->isWindow())
        {
            (*iter)->deleteLater();
        }
    }
    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        if ((*iter)->isWindow())
        {
            (*iter)->deleteLater();
        }
    }
    impl->_dockWidgets.clear();
    impl->_flexWidgets.clear();
}

bool FlexManager::load(const QByteArray& content, const QMap<QString, QWidget*>& parents)
{
    //close();

    QJsonObject object = QJsonDocument::fromJson(content).object();

    QJsonArray flexWidgetObjects = object["flexWidgets"].toArray();

    for (int i = 0; i < flexWidgetObjects.size(); ++i)
    {
        QJsonObject flexWidgetObject = flexWidgetObjects[i].toObject();

        Flex::ViewMode viewMode = (Flex::ViewMode)flexWidgetObject["viewMode"].toInt();
        QWidget* parent = parents.value(flexWidgetObject["parent"].toString(), nullptr);
        Qt::WindowFlags flags = (Qt::WindowFlags)flexWidgetObject["windowFlags"].toInt();
        QString flexWidgetName = flexWidgetObject["flexWidgetName"].toString();

        FlexWidget* flexWidget = flexWidgetName == "RootFlex"
                ? this->flexWidget("RootFlex")
                : createFlexWidget(viewMode, parent, Flex::widgetFlags(), flexWidgetName);

        flexWidget->load(flexWidgetObject);
    }

    return true;
}

QByteArray FlexManager::save() const
{
    QJsonObject object;

    QJsonArray flexWidgetObjects;

    for (int i = 0; i < impl->_flexWidgets.size(); ++i)
    {
        FlexWidget* flexWidget = impl->_flexWidgets[i];

        QJsonObject flexWidgetObject;

        flexWidgetObject["viewMode"] = (int)flexWidget->viewMode();
        flexWidgetObject["parent"] = flexWidget->parentWidget() ? flexWidget->parentWidget()->objectName() : "";
        flexWidgetObject["windowFlags"] = (int)Flex::windowFlags(flexWidget->viewMode());
        flexWidgetObject["flexWidgetName"] = flexWidget->objectName();

        flexWidget->save(flexWidgetObject);

        flexWidgetObjects.append(flexWidgetObject);
    }

    object["flexWidgets"] = flexWidgetObjects;

#ifdef _DEBUG
    return QJsonDocument(object).toJson(QJsonDocument::Indented);
#else
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
#endif
}

bool FlexManager::snapshot(DockWidget* dockWidget)
{
    if (!hasDockWidget(dockWidget->objectName()))
    {
        return false;
    }

    FlexWidget* flexWidget = dockWidget->flexWidget();

    if (!flexWidget)
    {
        return false;
    }

    QString dockWidgetPath = dockWidget->identifier();

    SnapshotDict::iterator item;

    if ((item = impl->_snapshotDict.find(dockWidget->objectName())) != impl->_snapshotDict.end())
    {
        if (impl->equalIdentifer(dockWidgetPath, std::get<1>(item.value())))
        {
            return true;
        }
    }

    if (item != impl->_snapshotDict.end())
    {
        if ((--std::get<1>(impl->_snapshotList[std::get<2>(item.value())])) == 0)
        {
            impl->_snapshotList.remove(std::get<2>(item.value()));
        }
    }

    int key = impl->generate();

    auto dockWidgets = flexWidget->findChildren<DockWidget*>();

    int count = 1;

    impl->_snapshotDict[dockWidget->objectName()] = std::make_tuple(flexWidget->objectName(), dockWidgetPath, key);

    foreach (auto tempWidget, dockWidgets)
    {
        if (tempWidget == dockWidget)
        {
            continue;
        }

        dockWidgetPath = tempWidget->identifier();

        if ((item = impl->_snapshotDict.find(tempWidget->objectName())) != impl->_snapshotDict.end())
        {
            if (impl->equalIdentifer(dockWidgetPath, std::get<1>(item.value())))
            {
                continue;
            }
        }

        if (item != impl->_snapshotDict.end())
        {
            if ((--std::get<1>(impl->_snapshotList[std::get<2>(item.value())])) == 0)
            {
                impl->_snapshotList.remove(std::get<2>(item.value()));
            }
        }

        impl->_snapshotDict[tempWidget->objectName()] = std::make_tuple(flexWidget->objectName(), dockWidgetPath, key);

        count++;
    }

    impl->_snapshotList[key] = std::make_tuple(flexWidget->snapshot(), count);

    return true;
}

bool FlexManager::snapshot(FlexWidget* flexWidget)
{
    if (!hasFlexWidget(flexWidget->objectName()))
    {
        return false;
    }

    int key = impl->generate();

    auto dockWidgets = flexWidget->findChildren<DockWidget*>();

    int count = 0;

    SnapshotDict::iterator item;

    foreach(auto tempWidget, dockWidgets)
    {
        QString dockWidgetPath = tempWidget->identifier();

        if ((item = impl->_snapshotDict.find(tempWidget->objectName())) != impl->_snapshotDict.end())
        {
            if (impl->equalIdentifer(dockWidgetPath, std::get<1>(item.value())))
            {
                continue;
            }
        }

        if (item != impl->_snapshotDict.end())
        {
            if ((--std::get<1>(impl->_snapshotList[std::get<2>(item.value())])) == 0)
            {
                impl->_snapshotList.remove(std::get<2>(item.value()));
            }
        }

        impl->_snapshotDict[tempWidget->objectName()] = std::make_tuple(flexWidget->objectName(), dockWidgetPath, key);

        count++;
    }

    impl->_snapshotList[key] = std::make_tuple(flexWidget->snapshot(), count);

    return true;
}

bool FlexManager::restore(const QString& name)
{
    SnapshotDict::iterator item;

    if ((item = impl->_snapshotDict.find(name)) == impl->_snapshotDict.end())
    {
        return false;
    }

    QString flexWidgetName = std::get<0>(item.value());
    QString dockWidgetPath = std::get<1>(item.value());

    int key = std::get<2>(item.value());

    SnapshotNode& node = impl->_snapshotList[key];

    QByteArray& content = std::get<0>(node);

    int& count = std::get<1>(node);

    FlexWidget* flexWidget = nullptr;

    if ((flexWidget = this->flexWidget(flexWidgetName)) == nullptr)
    {
        QJsonObject flexWidgetObject = QJsonDocument::fromJson(content).object();

        Flex::ViewMode viewMode = (Flex::ViewMode)flexWidgetObject["viewMode"].toInt();

        Qt::WindowFlags flags = (Qt::WindowFlags)flexWidgetObject["windowFlags"].toInt();

        flexWidget = createFlexWidget(viewMode, nullptr, flags, flexWidgetName);

        flexWidget->restoreGeometry(QByteArray::fromBase64(flexWidgetObject["geometry"].toString().toLatin1()));
    }

    bool result =  flexWidget->restore(content, dockWidgetPath);

    impl->_snapshotDict.remove(name);

    if (--count == 0)
    {
        impl->_snapshotList.remove(key);
    }

    return result;
}

bool FlexManager::eventFilter(QObject* obj, QEvent* evt)
{
    if (evt->type() == QEvent::Destroy)
    {
        switch (obj->property("class").value<int>())
        {
        case Flex::DockWidget:
            impl->_dockWidgets.removeOne(static_cast<DockWidget*>(obj));
            impl->_dockWidgetDestorying = "";
            break;
        case Flex::FlexWidget:
            impl->_flexWidgets.removeOne(static_cast<FlexWidget*>(obj));
            impl->_flexWidgetDestorying = "";
            break;
        }
    }
#ifndef Q_OS_WIN
    else if (evt->type() == QEvent::WindowActivate)
    {
        FlexWidget* flexWidget;
        if ((flexWidget = qobject_cast<FlexWidget*>(obj)) != nullptr)
        {
            if (impl->_flexWidgets.front() != flexWidget && impl->_flexWidgets.removeOne(flexWidget))
            {
                impl->_flexWidgets.push_front(flexWidget);
            }
            return false;
        }
        DockWidget* dockWidget;
        if ((dockWidget = qobject_cast<DockWidget*>(obj)) != nullptr)
        {
            if (impl->_dockWidgets.front() != dockWidget && impl->_dockWidgets.removeOne(dockWidget))
            {
                impl->_dockWidgets.push_front(dockWidget);
            }
            return false;
        }
    }
#endif
    return false;
}

void FlexManager::on_dockWidget_destroying(DockWidget* widget)
{
    impl->_dockWidgetDestorying = widget->objectName();
    emit dockWidgetDestroying(widget);
}

void FlexManager::on_flexWidget_destroying(FlexWidget* widget)
{
    impl->_flexWidgetDestorying = widget->objectName();
    emit flexWidgetDestroying(widget);
}

void FlexManager::on_dockWidget_destroyed(QObject* widget)
{
    widget->setProperty("class", QVariant::fromValue<int>(Flex::DockWidget));
}

void FlexManager::on_flexWidget_destroyed(QObject* widget)
{
    widget->setProperty("class", QVariant::fromValue<int>(Flex::FlexWidget));
}

void FlexManager::on_flexWidget_guiderShow(FlexWidget* flexWidget, QWidget* widget)
{
    Q_ASSERT(widget != nullptr); 
#ifdef Q_OS_WIN
    SetWindowPos(reinterpret_cast<HWND>(flexWidget->window()->effectiveWinId()), reinterpret_cast<HWND>(widget->effectiveWinId()), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
#else
    flexWidget->window()->raise();
    if (impl->_flexWidgets.size() > 1 && impl->_flexWidgets[1] != flexWidget && impl->_flexWidgets.removeOne(flexWidget))
    {
        impl->_flexWidgets.insert(1, flexWidget);
    }
    widget->raise();
#endif
    flexWidget->showGuider(widget);
}

void FlexManager::on_flexWidget_guiderHide(FlexWidget* flexWidget, QWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    flexWidget->hideGuider(widget);
}

void FlexManager::on_flexWidget_guiderHover(FlexWidget* flexWidget, QWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    flexWidget->hoverGuider(widget);
}

void FlexManager::on_flexWidget_guiderDrop(FlexWidget* flexWidget, DockWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    if (flexWidget->dropGuider(widget))
    {
#ifdef Q_OS_WIN
        SetWindowPos(reinterpret_cast<HWND>(flexWidget->window()->effectiveWinId()), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | 1800);
#else
        flexWidget->window()->raise();
#endif
    }
    else
    {
#ifdef Q_OS_WIN
        SetWindowPos(reinterpret_cast<HWND>(widget->effectiveWinId()), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | 1800);
#else
        widget->raise();
#endif
    }
}

void FlexManager::on_flexWidget_guiderDrop(FlexWidget* flexWidget, FlexWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    if (flexWidget->dropGuider(widget))
    {
#ifdef Q_OS_WIN
        SetWindowPos(reinterpret_cast<HWND>(flexWidget->window()->effectiveWinId()), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | 1800);
#else
        flexWidget->window()->raise();
#endif
    }
    else
    {
#ifdef Q_OS_WIN
        SetWindowPos(reinterpret_cast<HWND>(widget->effectiveWinId()), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | 1800);
#else
        widget->raise();
#endif
    }
}

void FlexManager::on_flexWidget_enterMove(QObject*)
{
    impl->_ready = true;
}

void FlexManager::on_flexWidget_leaveMove(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<FlexWidget*>(object);

    auto pos = QCursor::pos();

#ifdef Q_OS_WIN
    auto top = topLevelWindowAt(widget, pos);
#endif

    auto has = false;

    impl->_ready = false;

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget == widget)
        {
            continue;
        }

        if (flexWidget->window()->isMinimized())
        {
            continue;
        }

#ifdef Q_OS_WIN
        if (!has && flexWidget->window()->effectiveWinId() == top && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#else
        if (!has && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#endif
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderDrop(flexWidget, widget);
                }
                else
                {
                    emit guiderHide(flexWidget, widget);
                }
            }
            has = true;
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget); break;
            }
        }
    }
}

void FlexManager::on_flexWidget_moving(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<FlexWidget*>(object);

    auto pos = QCursor::pos();

#ifdef Q_OS_WIN
    auto top = topLevelWindowAt(widget, pos);
#endif

    auto has = false;

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget == widget)
        {
            continue;
        }

        if (flexWidget->window()->isMinimized())
        {
            continue;
        }

#ifdef Q_OS_WIN
        if (!has && flexWidget->window()->effectiveWinId() == top && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#else
        if (!has && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#endif
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderHover(flexWidget, widget);
                }
            }
            else
            {
                emit guiderShow(flexWidget, widget);
            }

            has = true;
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget);
            }
        }
    }
}

void FlexManager::on_dockWidget_enterMove(QObject*)
{
    impl->_ready = true;
}

void FlexManager::on_dockWidget_leaveMove(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<DockWidget*>(object);

    auto pos = QCursor::pos();

#ifdef Q_OS_WIN
    auto top = topLevelWindowAt(widget, pos);
#endif

    auto has = false;

    impl->_ready = false;

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget->window()->isMinimized())
        {
            continue;
        }

#ifdef Q_OS_WIN
        if (!has && flexWidget->window()->effectiveWinId() == top && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#else
        if (!has && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#endif
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderDrop(flexWidget, widget);
                }
                else
                {
                    emit guiderHide(flexWidget, widget);
                }
            }
            has = true;
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget); break;
            }
        }
    }
}

void FlexManager::on_dockWidget_moving(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<DockWidget*>(object);

    auto pos = QCursor::pos();

#ifdef Q_OS_WIN
    auto top = topLevelWindowAt(widget, pos);
#endif

    auto has = false;

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget->window()->isMinimized())
        {
            continue;
        }

#ifdef Q_OS_WIN
        if (!has && flexWidget->window()->effectiveWinId() == top && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#else
        if (!has && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
#endif
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderHover(flexWidget, widget);
                }
            }
            else
            {
                emit guiderShow(flexWidget, widget);
            }
            has = true;
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget);
            }
        }
    }
}

void FlexManager::on_app_focusChanged(QWidget* old, QWidget* now)
{
    FlexWidget* flexWidget = nullptr;

    auto oldDockSite = getDockSite(old);
    auto nowDockSite = getDockSite(now);

    if (nowDockSite != nullptr && oldDockSite != nowDockSite)
    {
        if (oldDockSite)
        {
            oldDockSite->setActive(false);
        }

        if (!nowDockSite->isActive())
        {
            if ((flexWidget = nowDockSite->flexWidget()) != nullptr)
            {
                flexWidget->setCurrent(nowDockSite);
            }
            if (nowDockSite->isActive())
            {
                emit dockSiteActivated(nowDockSite);
            }
        }
    }
}

QWidget* Flex::window()
{
    return qobject_cast<QWidget*>(qApp->property("window").value<QObject*>());
}
