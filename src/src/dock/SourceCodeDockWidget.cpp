//
// Created by everettjf on 2018/3/30.
//

#include "SourceCodeDockWidget.h"

SourceCodeDockWidget::SourceCodeDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Source Code"));

    textEdit = new QTextEdit(this);
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setReadOnly(true);

    setWidget(textEdit);

    //default message
    std::string code = R"(
#define FAT_MAGIC	0xcafebabe
#define FAT_CIGAM	0xbebafeca	/* NXSwapLong(FAT_MAGIC) */

    struct fat_header {
        uint32_t	magic;		/* FAT_MAGIC */
        uint32_t	nfat_arch;	/* number of structs that follow */
    };

    struct fat_arch {
        cpu_type_t	cputype;	/* cpu specifier (int) */
        cpu_subtype_t	cpusubtype;	/* machine specifier (int) */
        uint32_t	offset;		/* file offset to this object file */
        uint32_t	size;		/* size of this object file */
        uint32_t	align;		/* alignment as a power of 2 */
    };

                       )";
    this->setContent(QString::fromStdString(code));
}
void SourceCodeDockWidget::setContent(const QString &line)
{
    textEdit->setText(line);
}
