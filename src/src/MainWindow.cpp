//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "MainWindow.h"
#include <QDockWidget>
#include <QMenuBar>
#include "src/utility/Utility.h"
#include <QFileDialog>
#include "src/controller/Workspace.h"
#include "src/dialog/CheckUpdateDialog.h"
#include "src/base/AppInfo.h"
#include "src/dialog/AboutDialog.h"
#include <QProcess>
#include <QStatusBar>
#include <QFontDatabase>
#include <QTabWidget>
#include <QPalette>
#include <QSettings>
#include <QEvent>
#include <QInputDialog>
#include <QLineEdit>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QTimer>
#include <climits>
#include <algorithm>
#include <cstring>
#include <memory>
#include <vector>

#ifdef Q_OS_MAC
#include <libproc.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#endif

#ifdef Q_OS_MAC
namespace {

struct dyld_image_info_min {
    mach_vm_address_t imageLoadAddress;
    mach_vm_address_t imageFilePath;
    uint64_t imageFileModDate;
};

struct dyld_all_image_infos_min {
    uint32_t version;
    uint32_t infoArrayCount;
    mach_vm_address_t infoArray;
};

struct RemoteSegmentInfo {
    uint64_t vmaddr = 0;
    uint64_t vmsize = 0;
    uint64_t fileoff = 0;
    uint64_t filesize = 0;
};

static bool ReadRemote(task_t task, mach_vm_address_t address, void *buffer, size_t size, QString &error)
{
    mach_vm_size_t out_size = 0;
    const kern_return_t kr = mach_vm_read_overwrite(task, address, size, reinterpret_cast<mach_vm_address_t>(buffer), &out_size);
    if (kr != KERN_SUCCESS || out_size != size) {
        error = QString("mach_vm_read_overwrite failed at 0x%1 (kr=%2, size=%3, out=%4)")
                .arg(QString::number(address, 16))
                .arg(kr)
                .arg(size)
                .arg(out_size);
        return false;
    }
    return true;
}

static QString ReadRemoteCString(task_t task, mach_vm_address_t address, size_t max_len, QString &error)
{
    std::vector<char> buf(max_len + 1, 0);
    if (!ReadRemote(task, address, buf.data(), max_len, error)) {
        return QString();
    }
    buf[max_len] = 0;
    size_t len = 0;
    while (len < max_len && buf[len] != 0) ++len;
    return QString::fromUtf8(buf.data(), static_cast<int>(len));
}

static bool DumpProcessMainImageToFile(pid_t pid, QString &snapshot_path, QString &error)
{
    task_t task = MACH_PORT_NULL;
    const kern_return_t tfp = task_for_pid(mach_task_self(), pid, &task);
    if (tfp != KERN_SUCCESS || task == MACH_PORT_NULL) {
        error = QString("task_for_pid failed (kr=%1). This mode requires root/codesign entitlements.").arg(tfp);
        return false;
    }

    task_dyld_info_data_t dyld_info{};
    mach_msg_type_number_t dyld_count = TASK_DYLD_INFO_COUNT;
    const kern_return_t dyld_kr = task_info(task, TASK_DYLD_INFO, reinterpret_cast<task_info_t>(&dyld_info), &dyld_count);
    if (dyld_kr != KERN_SUCCESS || dyld_info.all_image_info_addr == 0) {
        error = QString("task_info(TASK_DYLD_INFO) failed (kr=%1)").arg(dyld_kr);
        return false;
    }

    dyld_all_image_infos_min infos_header{};
    if (!ReadRemote(task, dyld_info.all_image_info_addr, &infos_header, sizeof(infos_header), error)) {
        return false;
    }
    if (infos_header.infoArray == 0 || infos_header.infoArrayCount == 0) {
        error = "Remote dyld infoArray is empty";
        return false;
    }

    const uint32_t image_count = std::min<uint32_t>(infos_header.infoArrayCount, 4096);
    std::vector<dyld_image_info_min> image_infos(image_count);
    if (!ReadRemote(task, static_cast<mach_vm_address_t>(infos_header.infoArray),
                    image_infos.data(), sizeof(dyld_image_info_min) * image_count, error)) {
        return false;
    }

    char exec_path[PROC_PIDPATHINFO_MAXSIZE] = {0};
    proc_pidpath(pid, exec_path, sizeof(exec_path));
    const QString exec_path_q = QString::fromUtf8(exec_path);

    mach_vm_address_t image_load_addr = static_cast<mach_vm_address_t>(image_infos[0].imageLoadAddress);
    for (const auto &info : image_infos) {
        if (info.imageLoadAddress == 0 || info.imageFilePath == 0) continue;
        QString read_error;
        const QString remote_path = ReadRemoteCString(task,
                                                      reinterpret_cast<mach_vm_address_t>(info.imageFilePath),
                                                      PROC_PIDPATHINFO_MAXSIZE - 1,
                                                      read_error);
        if (!read_error.isEmpty()) continue;
        if (!exec_path_q.isEmpty() && remote_path == exec_path_q) {
            image_load_addr = static_cast<mach_vm_address_t>(info.imageLoadAddress);
            break;
        }
    }

    qv_mach_header_64 hdr64{};
    if (!ReadRemote(task, image_load_addr, &hdr64, sizeof(hdr64), error)) {
        return false;
    }

    const bool is64 = (hdr64.magic == MH_MAGIC_64 || hdr64.magic == MH_CIGAM_64);
    const bool is32 = (hdr64.magic == MH_MAGIC || hdr64.magic == MH_CIGAM);
    if (!is64 && !is32) {
        error = QString("Remote image magic not Mach-O: 0x%1").arg(QString::number(hdr64.magic, 16));
        return false;
    }

    uint32_t ncmds = 0;
    uint32_t sizeofcmds = 0;
    size_t header_size = 0;
    if (is64) {
        ncmds = hdr64.ncmds;
        sizeofcmds = hdr64.sizeofcmds;
        header_size = sizeof(qv_mach_header_64);
    } else {
        qv_mach_header hdr32{};
        if (!ReadRemote(task, image_load_addr, &hdr32, sizeof(hdr32), error)) {
            return false;
        }
        ncmds = hdr32.ncmds;
        sizeofcmds = hdr32.sizeofcmds;
        header_size = sizeof(qv_mach_header);
    }

    if (ncmds > 65536 || sizeofcmds > (64u * 1024u * 1024u)) {
        error = QString("Suspicious load commands (ncmds=%1 sizeofcmds=%2)").arg(ncmds).arg(sizeofcmds);
        return false;
    }

    std::vector<uint8_t> hdr_blob(header_size + sizeofcmds);
    if (!ReadRemote(task, image_load_addr, hdr_blob.data(), hdr_blob.size(), error)) {
        return false;
    }

    std::vector<RemoteSegmentInfo> segments;
    const uint8_t *cmd_cur = hdr_blob.data() + header_size;
    uint32_t consumed = 0;
    for (uint32_t i = 0; i < ncmds; ++i) {
        if (consumed + sizeof(qv_load_command) > sizeofcmds) {
            error = "Malformed remote load commands (truncated command)";
            return false;
        }
        const auto *lc = reinterpret_cast<const qv_load_command *>(cmd_cur);
        if (lc->cmdsize < sizeof(qv_load_command) || consumed + lc->cmdsize > sizeofcmds) {
            error = "Malformed remote load commands (invalid cmdsize)";
            return false;
        }
        if (lc->cmd == LC_SEGMENT_64 && lc->cmdsize >= sizeof(qv_segment_command_64)) {
            const auto *seg = reinterpret_cast<const qv_segment_command_64 *>(lc);
            segments.push_back({seg->vmaddr, seg->vmsize, seg->fileoff, seg->filesize});
        } else if (lc->cmd == LC_SEGMENT && lc->cmdsize >= sizeof(qv_segment_command)) {
            const auto *seg = reinterpret_cast<const qv_segment_command *>(lc);
            segments.push_back({seg->vmaddr, seg->vmsize, seg->fileoff, seg->filesize});
        }
        consumed += lc->cmdsize;
        cmd_cur += lc->cmdsize;
    }

    if (segments.empty()) {
        error = "No segment commands found in remote image";
        return false;
    }

    uint64_t vmaddr_file0 = 0;
    bool vmaddr_file0_found = false;
    uint64_t output_size = 0;
    for (const auto &seg : segments) {
        if (!vmaddr_file0_found && seg.fileoff == 0 && seg.filesize > 0) {
            vmaddr_file0 = seg.vmaddr;
            vmaddr_file0_found = true;
        }
        const uint64_t seg_end = seg.fileoff + seg.filesize;
        if (seg_end > output_size) output_size = seg_end;
    }
    if (!vmaddr_file0_found) {
        error = "Cannot determine image slide (missing fileoff=0 segment)";
        return false;
    }
    if (output_size == 0 || output_size > (1024ULL * 1024ULL * 1024ULL * 2ULL)) {
        error = QString("Suspicious output size: %1").arg(output_size);
        return false;
    }

    const uint64_t slide = image_load_addr - vmaddr_file0;
    std::vector<uint8_t> output(static_cast<size_t>(output_size), 0);
    if (hdr_blob.size() <= output.size()) {
        memcpy(output.data(), hdr_blob.data(), hdr_blob.size());
    }

    constexpr size_t kChunkSize = 1024 * 1024;
    std::vector<uint8_t> chunk(kChunkSize);
    for (const auto &seg : segments) {
        if (seg.filesize == 0) continue;
        if (seg.fileoff + seg.filesize > output_size) continue;
        const uint64_t remote_base = seg.vmaddr + slide;
        uint64_t copied = 0;
        while (copied < seg.filesize) {
            const size_t to_read = static_cast<size_t>(std::min<uint64_t>(kChunkSize, seg.filesize - copied));
            if (!ReadRemote(task, remote_base + copied, chunk.data(), to_read, error)) {
                return false;
            }
            memcpy(output.data() + seg.fileoff + copied, chunk.data(), to_read);
            copied += to_read;
        }
    }

    const QString temp_name = QString("MachOExplorer-attach-%1-%2.macho")
            .arg(pid)
            .arg(QDateTime::currentDateTimeUtc().toString("yyyyMMdd-hhmmss-zzz"));
    const QString temp_path = QDir(QDir::tempPath()).filePath(temp_name);
    QFile out_file(temp_path);
    if (!out_file.open(QIODevice::WriteOnly)) {
        error = QString("Cannot create snapshot file: %1").arg(temp_path);
        return false;
    }
    if (out_file.write(reinterpret_cast<const char *>(output.data()), static_cast<qint64>(output.size())) != static_cast<qint64>(output.size())) {
        error = QString("Write snapshot failed: %1").arg(temp_path);
        return false;
    }
    out_file.close();
    snapshot_path = temp_path;
    return true;
}

}
#endif

namespace {
static void StopProcessSync(QProcess *proc)
{
    if (proc == nullptr) return;
    if (proc->state() == QProcess::NotRunning) return;
    proc->terminate();
    if (!proc->waitForFinished(2000)) {
        proc->kill();
        proc->waitForFinished(2000);
    }
}
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = WS()->ui();

    setMinimumSize(QSize(600,400));

    menu = new MainWindowMenu();
    action = new MainWindowAction();

    setWindowTitle(tr("MachOExplorer"));
    createUI();
    createActions();
    createStatusBar();
    setThemeMode(loadThemeMode(), false);
}

MainWindow::~MainWindow()
{
    if (dyldExtractProcess_) {
        disconnect(dyldExtractProcess_, nullptr, this, nullptr);
        StopProcessSync(dyldExtractProcess_);
    }
    dyldExtractInProgress_ = false;
    dyldExtractProcess_.clear();
}

void MainWindow::displayNewFileDialog()
{
    openFileDialog = new OpenFileDialog();
    openFileDialog->setAttribute(Qt::WA_DeleteOnClose);
    openFileDialog->show();
}

void MainWindow::openNewFile(const QString &filePath)
{
    this->showMaximized();

    WS()->openFile(filePath);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::ApplicationPaletteChange && themeMode == ThemeMode::System) {
        applyVisualRefresh();
    }
    return QMainWindow::event(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (dyldExtractProcess_) {
        disconnect(dyldExtractProcess_, nullptr, this, nullptr);
        StopProcessSync(dyldExtractProcess_);
        dyldExtractInProgress_ = false;
        dyldExtractProcess_.clear();
    }
    QMainWindow::closeEvent(event);
}

#define InitDock(dockvar,dockclass,dockaction) \
    dockvar = new dockclass(this); \
    ui->dockWidgets.push_back(dockvar); \
    connect(dockvar,&QDockWidget::visibilityChanged,this,[this](bool checked){\
    dockaction->setChecked(checked);\
    });

void MainWindow::createUI()
{
    // Main Window
    ui->main = new CentralWidget(this);
    setCentralWidget(ui->main);

    setDockNestingEnabled(true);
    setDockOptions(dockOptions() | QMainWindow::AllowTabbedDocks | QMainWindow::AnimatedDocks);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    // Layout View
    InitDock(ui->layout,LayoutDockWidget,action->showLayoutWindow);
    InitDock(ui->log,LogDockWidget,action->showLogWindow);
    InitDock(ui->hex,HexDockWidget,action->showHexWindow);
    InitDock(ui->information,InformationDockWidget,action->showInformationWindow);
//    InitDock(ui->sourcecode,SourceCodeDockWidget,action->showSourceCodeWindow);

    setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::BottomDockWidgetArea);
    setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner,Qt::BottomDockWidgetArea);


    // Left(Layout)
    addDockWidget(Qt::LeftDockWidgetArea,ui->layout);
    addDockWidget(Qt::RightDockWidgetArea,ui->hex);

    // Bottom(Log)
    addDockWidget(Qt::BottomDockWidgetArea,ui->log);

    // Log | Information | SourceCode
    splitDockWidget(ui->log,ui->information,Qt::Horizontal);
//    splitDockWidget(ui->information,ui->sourcecode,Qt::Horizontal);
}

#define InitAction(name,actionvar,dockvar) \
    actionvar = new QAction(tr(name)); \
    menu->window->addAction(actionvar);\
    actionvar->setCheckable(true);\
    actionvar->setChecked(true);\
    connect(actionvar,&QAction::triggered,this,[this](bool checked){\
        dockvar->setVisible(checked);\
    });


void MainWindow::createActions()
{
    // File
    menu->file = menuBar()->addMenu(tr("File"));

    action->newWindow = new QAction(tr("&New Window"));
    menu->file->addAction(action->newWindow);
    connect(action->newWindow,&QAction::triggered,this,[this](bool checked){
        QProcess process(this);
        process.setEnvironment(QProcess::systemEnvironment());
        process.startDetached(qApp->applicationFilePath());
    });

    action->closeWindow = new QAction(tr("&Close Window"));
    menu->file->addAction(action->closeWindow);
    connect(action->closeWindow,&QAction::triggered,this,[this](bool checked){
        this->close();
        delete this;
    });

    action->openFile = new QAction(tr("&Open File"));
    menu->file->addAction(action->openFile);
    connect(action->openFile,&QAction::triggered,this,[this](bool checked){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("MachO File (*)"));
        if(fileName.length() == 0)
            return;
        WS()->openFile(fileName);
    });

    action->attachProcessPath = new QAction(tr("Attach by PID (&Executable Path)"));
    menu->file->addAction(action->attachProcessPath);
    connect(action->attachProcessPath, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        bool ok = false;
        int pid = QInputDialog::getInt(this, tr("Attach Process"), tr("Process ID:"), 0, 1, INT_MAX, 1, &ok);
        if(!ok) return;

        QString path;
#ifdef Q_OS_MAC
        char exec_path[PROC_PIDPATHINFO_MAXSIZE] = {0};
        const int ret = proc_pidpath(pid, exec_path, sizeof(exec_path));
        if(ret > 0){
            path = QString::fromUtf8(exec_path);
        }
#endif
        if(path.isEmpty()){
            util::showError(this, tr("Unable to resolve process executable path.\nOn macOS this may require extra privileges for target PID."));
            return;
        }
        if(!QFileInfo::exists(path)){
            util::showError(this, tr("Resolved executable path does not exist:\n%1").arg(path));
            return;
        }

        this->showMaximized();
        WS()->openFile(path);
        statusBar()->showMessage(tr("Attached by path to PID %1: %2").arg(pid).arg(path), 6000);
    });

    action->attachProcessSnapshot = new QAction(tr("Attach by PID (&Memory Snapshot)"));
    menu->file->addAction(action->attachProcessSnapshot);
    connect(action->attachProcessSnapshot, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        bool ok = false;
        int pid = QInputDialog::getInt(this, tr("Attach Process (Memory Snapshot)"), tr("Process ID:"), 0, 1, INT_MAX, 1, &ok);
        if(!ok) return;

#ifdef Q_OS_MAC
        QString snapshot_path;
        QString error;
        if(!DumpProcessMainImageToFile(pid, snapshot_path, error)){
            util::showError(this, tr("Attach snapshot failed for PID %1\n%2").arg(pid).arg(error));
            return;
        }

        this->showMaximized();
        WS()->openFile(snapshot_path);
        statusBar()->showMessage(tr("Attached by memory snapshot to PID %1: %2").arg(pid).arg(snapshot_path), 8000);
#else
        util::showError(this, tr("Memory snapshot attach is only available on macOS."));
#endif
    });

    action->extractDyldImage = new QAction(tr("Extract Image From Dyld Cache..."));
    menu->file->addAction(action->extractDyldImage);
    connect(action->extractDyldImage, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        if (dyldExtractInProgress_) {
            statusBar()->showMessage(tr("An extraction task is already running."), 4000);
            return;
        }

        QString cachePath = WS()->currentFilePath();
        if (cachePath.isEmpty() || !QFileInfo(cachePath).fileName().startsWith("dyld_shared_cache")) {
            cachePath = QFileDialog::getOpenFileName(
                    this, tr("Select dyld shared cache file"),
                    "/System/Volumes/Preboot/Cryptexes/OS/System/Library/dyld",
                    tr("Dyld Shared Cache (*)"));
        }
        if(cachePath.isEmpty()) return;

        QString tool = QCoreApplication::applicationDirPath() + "/moex-cache-extract";
        if(!QFileInfo::exists(tool)){
            tool = QDir::current().absoluteFilePath("build/moex-cache-extract");
        }
        if(!QFileInfo::exists(tool)){
            util::showError(this, tr("Cannot find moex-cache-extract tool.\nBuild from source first."));
            return;
        }

        QString imageSelector;
        {
            QString listTool = QCoreApplication::applicationDirPath() + "/moex-cache-list";
            if(!QFileInfo::exists(listTool)){
                listTool = QDir::current().absoluteFilePath("build/moex-cache-list");
            }

            QStringList candidateImages;
            if(QFileInfo::exists(listTool)){
                QProcess listProc;
                listProc.start(listTool, {"--json", "--limit=5000", cachePath});
                const bool finished = listProc.waitForFinished(15000);
                if(!finished){
                    listProc.kill();
                    listProc.waitForFinished(2000);
                    WS()->addLog(tr("[cache-list] timeout for %1").arg(cachePath));
                    statusBar()->showMessage(tr("Image preloading timed out, fallback to manual selector."), 4000);
                } else if (listProc.exitStatus() != QProcess::NormalExit || listProc.exitCode() != 0) {
                    const QString err = QString::fromUtf8(listProc.readAllStandardError()).trimmed();
                    WS()->addLog(tr("[cache-list] failed exit=%1 status=%2 err=%3")
                                         .arg(listProc.exitCode())
                                         .arg(listProc.exitStatus() == QProcess::NormalExit ? "normal" : "crash")
                                         .arg(err.isEmpty() ? "(empty)" : err));
                } else {
                    const QByteArray outputBytes = listProc.readAllStandardOutput();
                    QJsonParseError parseError{};
                    const QJsonDocument doc = QJsonDocument::fromJson(outputBytes, &parseError);
                    if(parseError.error == QJsonParseError::NoError && doc.isObject()){
                        const QJsonArray images = doc.object().value("images").toArray();
                        for(const QJsonValue &v : images){
                            const QString path = v.toObject().value("path").toString().trimmed();
                            if(path.isEmpty()) continue;
                            candidateImages.push_back(path);
                        }
                    } else {
                        // fallback for older CLI output format
                        const QString output = QString::fromUtf8(outputBytes);
                        const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
                        for(const QString &line : lines){
                            if(!line.startsWith("0x")) continue;
                            const int p = line.indexOf(' ');
                            if(p <= 0 || p + 1 >= line.size()) continue;
                            const QString path = line.mid(p + 1).trimmed();
                            if(path.isEmpty()) continue;
                            candidateImages.push_back(path);
                            if(candidateImages.size() >= 5000) break;
                        }
                    }
                }
            }

            bool ok = false;
            if (!candidateImages.isEmpty()) {
                candidateImages.removeDuplicates();
                std::sort(candidateImages.begin(), candidateImages.end(), [](const QString &a, const QString &b) {
                    return a.compare(b, Qt::CaseInsensitive) < 0;
                });
            }
            if(!candidateImages.isEmpty()){
                imageSelector = QInputDialog::getItem(
                        this,
                        tr("Image Selector"),
                        tr("Choose image path (searchable, editable):"),
                        candidateImages,
                        0,
                        true,
                        &ok);
            } else {
                imageSelector = QInputDialog::getText(
                        this,
                        tr("Image Selector"),
                        tr("Image path (exact or substring):"),
                        QLineEdit::Normal,
                        "libswiftCore.dylib",
                        &ok);
            }
            imageSelector = imageSelector.trimmed();
            if(!ok || imageSelector.isEmpty()) return;
            if (imageSelector.contains('\n') || imageSelector.contains('\r')) {
                util::showError(this, tr("Invalid image selector: newline is not allowed."));
                return;
            }
        }

        const QString outputPath = QDir::temp().absoluteFilePath(
                QString("MachOExplorer-cache-extract-%1-%2")
                        .arg(QFileInfo(cachePath).fileName())
                        .arg(QDateTime::currentDateTimeUtc().toString("yyyyMMdd-hhmmss-zzz")));

        auto *progress = new QProgressDialog(tr("Extracting image from dyld shared cache..."),
                                             tr("Cancel"), 0, 0, this);
        progress->setWindowModality(Qt::WindowModal);
        progress->setMinimumDuration(0);
        progress->setAutoClose(false);
        progress->setAutoReset(false);
        progress->show();

        auto *proc = new QProcess(this);
        auto *timeout = new QTimer(this);
        auto elapsed = std::make_shared<QElapsedTimer>();
        elapsed->start();
        timeout->setSingleShot(true);
        dyldExtractInProgress_ = true;
        dyldExtractProcess_ = proc;
        action->extractDyldImage->setEnabled(false);
        WS()->addLog(tr("[extract] start cache=%1 selector=%2 out=%3")
                             .arg(cachePath, imageSelector, outputPath));
        statusBar()->showMessage(tr("Extracting image: %1").arg(imageSelector), 3000);

        connect(progress, &QProgressDialog::canceled, this, [this, proc]() {
            if (proc->state() != QProcess::NotRunning) proc->kill();
            WS()->addLog("[extract] canceled by user");
            statusBar()->showMessage(tr("Extraction canceled."), 5000);
        });

        connect(timeout, &QTimer::timeout, this, [this, proc, progress]() {
            if (proc->state() == QProcess::NotRunning) return;
            progress->cancel();
            proc->kill();
            WS()->addLog("[extract] canceled by timeout");
            statusBar()->showMessage(tr("Extraction timed out and was canceled."), 6000);
        });

        connect(proc, &QProcess::finished, this,
                [this, proc, progress, timeout, elapsed, outputPath](int exitCode, QProcess::ExitStatus exitStatus) {
            const qint64 elapsedMs = elapsed->elapsed();
            timeout->stop();
            timeout->deleteLater();
            const QString stderrText = QString::fromUtf8(proc->readAllStandardError());
            const QString stdoutText = QString::fromUtf8(proc->readAllStandardOutput());
            const bool canceled = progress->wasCanceled();

            dyldExtractInProgress_ = false;
            dyldExtractProcess_.clear();
            action->extractDyldImage->setEnabled(true);
            progress->hide();
            progress->deleteLater();

            if (canceled) {
                QFile::remove(outputPath);
                WS()->addLog(tr("[extract] finished with canceled state (%1 ms)").arg(elapsedMs));
                proc->deleteLater();
                return;
            }

            if (exitStatus != QProcess::NormalExit || exitCode != 0) {
                QFile::remove(outputPath);
                const QString details = (stderrText + "\n" + stdoutText).trimmed();
                WS()->addLog(tr("[extract] failed exit=%1 status=%2 details=%3")
                                     .arg(exitCode)
                                     .arg(exitStatus == QProcess::NormalExit ? "normal" : "crash")
                                     .arg(details.isEmpty() ? "(no process output)" : details)
                             + tr(" (%1 ms)").arg(elapsedMs));
                util::showError(this, tr("Extraction failed:\n%1").arg(details.isEmpty() ? tr("(no process output)") : details));
                proc->deleteLater();
                return;
            }
            const QFileInfo outInfo(outputPath);
            if (!outInfo.exists() || outInfo.size() <= 0) {
                QFile::remove(outputPath);
                WS()->addLog(tr("[extract] failed output missing/empty: %1").arg(outputPath));
                util::showError(this, tr("Extraction completed but output is missing or empty:\n%1").arg(outputPath));
                proc->deleteLater();
                return;
            }

            this->showMaximized();
            WS()->openFile(outputPath);
            WS()->addLog(tr("[extract] success out=%1 size=%2 (%3 ms)")
                                 .arg(outputPath).arg(outInfo.size()).arg(elapsedMs));
            statusBar()->showMessage(tr("Extracted and opened: %1").arg(outputPath), 8000);
            proc->deleteLater();
        });

        connect(proc, &QProcess::errorOccurred, this, [this, proc, progress, timeout, elapsed](QProcess::ProcessError) {
            if (!dyldExtractInProgress_) return;
            const qint64 elapsedMs = elapsed->elapsed();
            const QString err = proc->errorString();
            dyldExtractInProgress_ = false;
            dyldExtractProcess_.clear();
            action->extractDyldImage->setEnabled(true);
            timeout->stop();
            timeout->deleteLater();
            progress->hide();
            progress->deleteLater();
            WS()->addLog(tr("[extract] process start error: %1 (%2 ms)").arg(err).arg(elapsedMs));
            util::showError(this, tr("Failed to start extraction:\n%1").arg(err));
            proc->deleteLater();
        });

        proc->start(tool, {"--compact", cachePath, imageSelector, outputPath});
        timeout->start(600000);
    });

    action->quit = new QAction(tr("&Quit"));
    menu->file->addAction(action->quit);
    connect(action->quit,&QAction::triggered,this,[this](bool checked){
        util::quitApp();
    });


    // Window
    menu->window = menuBar()->addMenu(tr("Window"));

    InitAction("Layout",action->showLayoutWindow,ui->layout);
    InitAction("Hex",action->showHexWindow,ui->hex);
    InitAction("Log",action->showLogWindow,ui->log);
    InitAction("Information",action->showInformationWindow,ui->information);
//    InitAction("SourceCode",action->showSourceCodeWindow,ui->sourcecode);
    createThemeActions();



    // Help
    menu->help = menuBar()->addMenu(tr("Help"));

    action->viewSource = new QAction(tr("View Source"));
    menu->help->addAction(action->viewSource);
    connect(action->viewSource,&QAction::triggered,this,[this](bool checked){
        util::openURL("https://github.com/everettjf/MachOExplorer");
    });

    action->reportIssue = new QAction(tr("Report Issue"));
    menu->help->addAction(action->reportIssue);
    connect(action->reportIssue,&QAction::triggered,this,[this](bool checked){
        util::openURL("https://github.com/everettjf/MachOExplorer/issues");
    });

    action->checkUpdate = new QAction(tr("Check Update"));
    menu->help->addAction(action->checkUpdate);
    connect(action->checkUpdate,&QAction::triggered,this,[this](bool checked){
        CheckUpdateDialog dlg(this);
        dlg.exec();
    });

    action->about = new QAction(tr("About"));
    menu->help->addAction(action->about);
    connect(action->about,&QAction::triggered,this,[this](bool checked){
        AboutDialog dlg(this);
        dlg.exec();
    });
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Welcome"));
}

void MainWindow::createThemeActions()
{
    QMenu *themeMenu = menu->window->addMenu(tr("Theme"));

    themeActionGroup = new QActionGroup(this);
    themeActionGroup->setExclusive(true);

    action->themeSystem = themeMenu->addAction(tr("Follow System"));
    action->themeSystem->setCheckable(true);
    themeActionGroup->addAction(action->themeSystem);
    connect(action->themeSystem, &QAction::triggered, this, [this](bool checked) {
        if (checked) setThemeMode(ThemeMode::System, true);
    });

    action->themeLight = themeMenu->addAction(tr("Light"));
    action->themeLight->setCheckable(true);
    themeActionGroup->addAction(action->themeLight);
    connect(action->themeLight, &QAction::triggered, this, [this](bool checked) {
        if (checked) setThemeMode(ThemeMode::Light, true);
    });

    action->themeDark = themeMenu->addAction(tr("Dark"));
    action->themeDark->setCheckable(true);
    themeActionGroup->addAction(action->themeDark);
    connect(action->themeDark, &QAction::triggered, this, [this](bool checked) {
        if (checked) setThemeMode(ThemeMode::Dark, true);
    });
}

MainWindow::ThemeMode MainWindow::loadThemeMode() const
{
    QSettings settings;
    const QString saved = settings.value("ui/theme_mode", "system").toString();
    if (saved == "light") return ThemeMode::Light;
    if (saved == "dark") return ThemeMode::Dark;
    return ThemeMode::System;
}

bool MainWindow::isDarkMode() const
{
    if (themeMode == ThemeMode::Dark) return true;
    if (themeMode == ThemeMode::Light) return false;
    return palette().color(QPalette::Window).lightness() < 128;
}

void MainWindow::updateThemeActionChecks()
{
    if (!action->themeSystem || !action->themeLight || !action->themeDark) return;
    action->themeSystem->setChecked(themeMode == ThemeMode::System);
    action->themeLight->setChecked(themeMode == ThemeMode::Light);
    action->themeDark->setChecked(themeMode == ThemeMode::Dark);
}

void MainWindow::setThemeMode(ThemeMode mode, bool persist)
{
    themeMode = mode;
    updateThemeActionChecks();
    applyVisualRefresh();

    if (!persist) return;
    QSettings settings;
    QString value = "system";
    if (mode == ThemeMode::Light) value = "light";
    if (mode == ThemeMode::Dark) value = "dark";
    settings.setValue("ui/theme_mode", value);
}

void MainWindow::applyVisualRefresh()
{
    QFontDatabase::addApplicationFont(":/res/fonts/Inconsolata-Regular.ttf");
    QFontDatabase::addApplicationFont(":/res/fonts/Anonymous Pro.ttf");

    QFont uiFont("Inconsolata", 12);
    if (!uiFont.exactMatch()) {
        uiFont = font();
        uiFont.setPointSize(11);
    }
    setFont(uiFont);

    const QString style = isDarkMode() ? R"(
QMainWindow {
    background: #0b1020;
}
QMenuBar {
    background: #0f172a;
    color: #e2e8f0;
    padding: 4px;
}
QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #1e293b;
}
QMenu {
    background: #111827;
    color: #e5e7eb;
    border: 1px solid #334155;
}
QMenu::item:selected {
    background: #1f2937;
}
QStatusBar {
    background: #020617;
    color: #cbd5e1;
}
QDockWidget {
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
    font-weight: 600;
}
QDockWidget::title {
    text-align: left;
    background: #0f172a;
    color: #e2e8f0;
    padding: 6px 10px;
}
QTableView, QTreeView, QTextEdit {
    background: #0f172a;
    alternate-background-color: #111827;
    color: #e2e8f0;
    gridline-color: #334155;
    selection-background-color: #0284c7;
    selection-color: #ffffff;
    border: 1px solid #334155;
}
QHeaderView::section {
    background: #1e293b;
    color: #e2e8f0;
    border: 0;
    border-right: 1px solid #334155;
    border-bottom: 1px solid #334155;
    padding: 6px;
    font-weight: 600;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: #0b1220;
}
QTabBar::tab {
    background: #1e293b;
    color: #cbd5e1;
    padding: 6px 12px;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background: #0284c7;
    color: #ffffff;
}
)" : R"(
QMainWindow {
    background: #f4f6f8;
}
QMenuBar {
    background: #111827;
    color: #f8fafc;
    padding: 4px;
}
QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #1f2937;
}
QMenu {
    background: #ffffff;
    color: #111827;
    border: 1px solid #d1d5db;
}
QMenu::item:selected {
    background: #e5e7eb;
}
QStatusBar {
    background: #111827;
    color: #cbd5e1;
}
QDockWidget {
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
    font-weight: 600;
}
QDockWidget::title {
    text-align: left;
    background: #0f172a;
    color: #e2e8f0;
    padding: 6px 10px;
}
QTableView, QTreeView, QTextEdit {
    background: #ffffff;
    alternate-background-color: #f8fafc;
    color: #0f172a;
    gridline-color: #e2e8f0;
    selection-background-color: #0ea5e9;
    selection-color: #ffffff;
    border: 1px solid #dbe3ea;
}
QHeaderView::section {
    background: #e2e8f0;
    color: #0f172a;
    border: 0;
    border-right: 1px solid #cbd5e1;
    border-bottom: 1px solid #cbd5e1;
    padding: 6px;
    font-weight: 600;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: #f1f5f9;
}
QTabBar::tab {
    background: #cbd5e1;
    color: #0f172a;
    padding: 6px 12px;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background: #0ea5e9;
    color: #ffffff;
}
)";
    setStyleSheet(style);
}
