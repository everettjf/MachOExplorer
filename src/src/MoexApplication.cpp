#include "MoexApplication.h"
#include "base/AppInfo.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QCommandLineParser>

MoexApplication::MoexApplication(int argc,char *argv[]) : QApplication(argc,argv){
    setApplicationName("MachOExplorer");
    setOrganizationName("everettjf");
    setOrganizationDomain("everettjf.com");
    setApplicationVersion(AppInfo::Instance().GetAppVersion());
    setWindowIcon(QIcon(":MachOExplorer.icns"));

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QCommandLineParser cmd_parser;
    cmd_parser.setApplicationDescription(QObject::tr("MachO Explorer"));
    cmd_parser.addHelpOption();
    cmd_parser.addVersionOption();
    cmd_parser.addPositionalArgument("filename", QObject::tr("Filename to open."));

    cmd_parser.process(*this);

    QStringList args = cmd_parser.positionalArguments();

    mainWindow = new MainWindow();
    if(args.empty()){
        mainWindow->displayNewFileDialog();
    }else{
        mainWindow->openNewFile(args[0]);
    }
}


MoexApplication::~MoexApplication(){

}
