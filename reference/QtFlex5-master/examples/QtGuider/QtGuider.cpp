#include "QtGuider.h"
#include "QtFlexWidget.h"
#include "QtFlexHelper.h"
#include "QtDockGuider.h"
#include "QtDockWidget.h"
#include "QtFlexManager.h"
#include <QtCore/QSettings>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QPushButton>
#include "ui_qtguider.h"

QtCentral::QtCentral(QWidget* parent) : QWidget(parent)
{
    setObjectName("Central");

    QHBoxLayout* box = new QHBoxLayout(this);
    box->setContentsMargins(0, 0, 0, 0);

    connect(FlexManager::instance(), SIGNAL(flexWidgetCreated(FlexWidget*)), SLOT(on_flexWidgetCreated(FlexWidget*)));
    connect(FlexManager::instance(), SIGNAL(dockWidgetCreated(DockWidget*)), SLOT(on_dockWidgetCreated(DockWidget*)));
    connect(FlexManager::instance(), SIGNAL(flexWidgetDestroying(FlexWidget*)), SLOT(on_flexWidgetDestroying(FlexWidget*)));
    connect(FlexManager::instance(), SIGNAL(dockWidgetDestroying(DockWidget*)), SLOT(on_dockWidgetDestroying(DockWidget*)));

    _widget = new QPushButton("Click");

    QSettings settings("QtFlex5", "QtGuider");

    QByteArray content = settings.value("Flex").toByteArray();

    const char* bytes = content;

    QMap<QString, QWidget*> parents;

    parents[objectName()] = this;

    if (content.isEmpty())
    {
        createOne();
    }
    else
    {
        FlexManager::instance()->load(content, parents);
    }
}

void QtCentral::createOne()
{
    auto content = FlexManager::instance()->createFlexWidget(Flex::HybridView, this, Flex::widgetFlags(), "M");
    layout()->addWidget(content);
}

void QtCentral::createTwo()
{
    auto splitter = new QSplitter(this);
    auto l = FlexManager::instance()->createFlexWidget(Flex::HybridView, this, Flex::widgetFlags(), "L");
    auto r = FlexManager::instance()->createFlexWidget(Flex::HybridView, this, Flex::widgetFlags(), "R");
    splitter->addWidget(l);
    splitter->addWidget(r);
    layout()->addWidget(splitter);
}

void QtCentral::on_flexWidgetCreated(FlexWidget* flexWidget)
{
    if (flexWidget->objectName() == "M")
    {
        layout()->addWidget(flexWidget);
    }
    else
    {
        flexWidget->show();
    }
}

void QtCentral::on_dockWidgetCreated(DockWidget* dockWidget)
{
    if (dockWidget->objectName() == "View-1")
    {
        if (dockWidget->widget() != _widget)
        {
            dockWidget->setWidget(_widget);
        }
    }
}

void QtCentral::on_flexWidgetDestroying(FlexWidget* flexWidget)
{
}

void QtCentral::on_dockWidgetDestroying(DockWidget* dockWidget)
{
    if (dockWidget->objectName() == "View-1")
    {
        if (dockWidget->widget() == _widget)
        {
            dockWidget->detachWidget(_widget);
        }
    }
}

class QtGuiderImpl
{
public:
    Ui::QtGuiderClass ui;
};

QtGuider::QtGuider(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags), impl(new QtGuiderImpl)
{
    qApp->setProperty("window", QVariant::fromValue<QObject*>(this));

    impl->ui.setupUi(this);

    for (int i = 0; i < 15; i++)
    {
        impl->ui.menuFile->addAction(QString("File-%1").arg(i), this, SLOT(actionFile_X_triggered()));
    }

    for (int i = 0; i < 15; i++)
    {
        impl->ui.menuView->addAction(QString("View-%1").arg(i), this, SLOT(actionView_X_triggered()));
    }

    setCentralWidget(new QtCentral(this));

    auto docker1 = new QDockWidget("Docker1", this);
    docker1->setWidget(new QWidget(this));
    auto docker2 = new QDockWidget("Docker2", this);
    docker2->setWidget(new QWidget(this));

    addDockWidget(Qt::LeftDockWidgetArea, docker1);
    addDockWidget(Qt::LeftDockWidgetArea, docker2);

    setGeometry(QApplication::desktop()->availableGeometry().adjusted(100, 100, -100, -100));
}

QtGuider::~QtGuider()
{

}

void QtGuider::openFile_N(int n)
{
    QString dockWidgetName = QString("File-%1").arg(n);

    DockWidget* widget = nullptr;

    if ((widget = FlexManager::instance()->dockWidget(dockWidgetName)) != nullptr)
    {
        widget->activate();
    }
    else
    {
        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            FlexWidget* flexWidget = FlexManager::instance()->createFlexWidget(Flex::FileView, Flex::parent(Flex::FileView), Flex::windowFlags());
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::FileView, flexWidget, Flex::widgetFlags(), dockWidgetName);
            dockWidget->setViewMode(Flex::FileView);
            dockWidget->setWindowTitle(dockWidgetName);
            flexWidget->addDockWidget(dockWidget);
            flexWidget->show();
            flexWidget->move(geometry().center() - flexWidget->rect().center());
        }
    }
}

void QtGuider::openView_N(int n)
{
    QString dockWidgetName = QString("View-%1").arg(n);

    DockWidget* widget = nullptr;

    if ((widget = FlexManager::instance()->dockWidget(dockWidgetName)) != nullptr)
    {
        widget->activate();
    }
    else
    {
        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            FlexWidget* flexWidget = FlexManager::instance()->createFlexWidget(Flex::ToolView, Flex::parent(Flex::ToolView), Flex::windowFlags());
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::ToolView, flexWidget, Flex::widgetFlags(), dockWidgetName);
            dockWidget->setViewMode(Flex::ToolView);
            dockWidget->setWindowTitle(dockWidgetName);
            flexWidget->addDockWidget(dockWidget);
            flexWidget->show();
            flexWidget->move(geometry().center() - flexWidget->rect().center());
        }
    }
}

void QtGuider::on_actionFile_N_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("File-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        openFile_N(i);

        break;
    }
}

void QtGuider::on_actionView_N_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("View-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        openView_N(i);

        break;
    }
}

void QtGuider::on_actionTool_N_triggered()
{
    FlexManager::instance()->flexWidget("M")->clearDockSites();
}

void QtGuider::actionFile_X_triggered()
{
    openFile_N(static_cast<QAction*>(sender())->text().split("-")[1].toInt());
}

void QtGuider::actionView_X_triggered()
{
    openView_N(static_cast<QAction*>(sender())->text().split("-")[1].toInt());
}

void QtGuider::on_actionEdit_Undo_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("FileTool-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::FileView, Flex::parent(Flex::FileView), Flex::windowFlags());
            dockWidget->setViewMode(Flex::FileView);
            dockWidget->setWindowTitle(dockWidgetName);
            dockWidget->show();
            dockWidget->move(geometry().center() - dockWidget->rect().center());
        }

        break;
    }
}

void QtGuider::on_actionEdit_Redo_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("ViewTool-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::ToolView, Flex::parent(Flex::ToolView), Flex::windowFlags());
            dockWidget->setViewMode(Flex::ToolView);
            dockWidget->setWindowTitle(dockWidgetName);
            dockWidget->show();
            dockWidget->move(geometry().center() - dockWidget->rect().center());
        }

        break;
    }
}

void QtGuider::closeEvent(QCloseEvent* evt)
{
    QByteArray content = FlexManager::instance()->save();

    QSettings settings("QtFlex5", "QtGuider");

    settings.setValue("Flex", content);

    FlexManager::instance()->close();
}
