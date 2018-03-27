#include "HexdumpWidget.h"
#include "ui_HexdumpWidget.h"
#include <QTextStream>
#include <QScrollBar>
#include <QFontMetrics>
#include <QElapsedTimer>
#include <QTextCursor>
#include <QTextBlock>
#include <cassert>

#define kGuiBackgroundColor QColor(255, 255, 255)
#define kBorderTextColor QColor(0, 0, 0)
#define kHighlightTextColor QColor(210, 210, 255)
#define kTextFont QFont("Inconsolata", 12)


void setTextEditPosition(QTextEdit *textEdit, int position)
{
    QTextCursor textCursor = textEdit->textCursor();
    textCursor.setPosition(position);
    textEdit->setTextCursor(textCursor);
}

static void registerCustomFonts()
{
    int ret = QFontDatabase::addApplicationFont(":/fonts/Anonymous Pro.ttf");
    assert(-1 != ret && "unable to register Anonymous Pro.ttf");

    ret = QFontDatabase::addApplicationFont(":/fonts/Inconsolata-Regular.ttf");
    assert(-1 != ret && "unable to register Inconsolata-Regular.ttf");

    // Do not issue a warning in release
    Q_UNUSED(ret)
}


//int getMaxFullyDisplayedLines(QTextEdit *textEdit)
//{
//    QFontMetrics fontMetrics(textEdit->document()->defaultFont());
//    return (textEdit->height()
//            - (textEdit->contentsMargins().top()
//               + textEdit->contentsMargins().bottom()
//               + (int)(textEdit->document()->documentMargin() * 2)))
//           / fontMetrics.lineSpacing();
//}


inline QString RAddressString32(unsigned long long addr)
{
    return QString::asprintf("%#010llx", addr);
}

inline QString RAddressString64(unsigned long long addr)
{
    return QString::asprintf("%#018llx", addr);
}



HexdumpWidget::HexdumpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HexdumpWidget)
{
    ui->setupUi(this);

    registerCustomFonts();

    int margin = 0;

    margin = static_cast<int>(ui->hexOffsetText->document()->documentMargin());
    ui->offsetHeaderLabel->setContentsMargins(margin,0,margin,0);

    margin = static_cast<int>(ui->hexHexText->document()->documentMargin());
    ui->hexHeaderLabel->setContentsMargins(margin,0,margin,0);

    margin = static_cast<int>(ui->hexASCIIText->document()->documentMargin());
    ui->asciiHeaderLabel->setContentsMargins(margin,0,margin,0);

    setupFont();
    setupColors();

    updateHeaders();

    setupScrollSync();
    setupSelection();
}

HexdumpWidget::~HexdumpWidget()
{
    delete ui;
}

void HexdumpWidget::showEvent(QShowEvent *event)
{
}

void HexdumpWidget::loadAddress(unsigned long long displayOffset, unsigned long long addr, unsigned long long len)
{
    m_displayOffset = displayOffset;
    m_addr = addr;
    m_length = len;

    refresh();
}

void HexdumpWidget::selectRange(unsigned long long addr, unsigned long long len)
{
    unsigned long long startAddress = addr;
    unsigned long long endAddress = addr + len;


    int startPosition = hexAddressToPosition(startAddress);
    int endPosition = hexAddressToPosition(endAddress);

    // End position -1 because the position we get above is for the next
    // entry, so including the space/newline
    endPosition -= 1;

    QTextCursor targetTextCursor = ui->hexHexText->textCursor();
    targetTextCursor.setPosition(startPosition);
    targetTextCursor.setPosition(endPosition, QTextCursor::KeepAnchor);
    ui->hexHexText->setTextCursor(targetTextCursor);
}

void HexdumpWidget::setupFont()
{
    QFont font = kTextFont;

    ui->hexOffsetText->setFont(font);
    ui->hexHexText->setFont(font);
    ui->hexASCIIText->setFont(font);

    ui->offsetHeaderLabel->setFont(font);
    ui->hexHeaderLabel->setFont(font);
    ui->asciiHeaderLabel->setFont(font);

}

void HexdumpWidget::setupColors()
{
    QString styleSheet = QString("QTextEdit { background-color: %1; color: %2; }")
            .arg(kGuiBackgroundColor.name())
            .arg(kBorderTextColor.name());

    ui->hexOffsetText->setStyleSheet(styleSheet);
    ui->hexHexText->setStyleSheet(styleSheet);
    ui->hexASCIIText->setStyleSheet(styleSheet);
}

void HexdumpWidget::updateHeaders()
{
    bool pairs = false;

    QString hexHeaderString;
    QString asciiHeaderString;

    QTextStream hexHeader(&hexHeaderString);
    QTextStream asciiHeader(&asciiHeaderString);

    hexHeader.setIntegerBase(16);
    hexHeader.setNumberFlags(QTextStream::UppercaseDigits);
    asciiHeader.setIntegerBase(16);
    asciiHeader.setNumberFlags(QTextStream::UppercaseDigits);

    // Custom spacing for the header
    QString space = " ";
    space = space.repeated(1);

    for (int i=0; i<m_columnCount; i++)
    {
        if (i > 0 && ((pairs && !(i&1)) || !pairs))
        {
            hexHeader << " ";
        }

        hexHeader << space << (i & 0xF);
    }

    for (int i=0; i < m_columnCount; i++)
    {
        asciiHeader << (i & 0xF);
    }

    hexHeader.flush();
    asciiHeader.flush();

    ui->hexHeaderLabel->setText(hexHeaderString);
    ui->asciiHeaderLabel->setText(asciiHeaderString);
}

void HexdumpWidget::setupScrollSync()
{
    /*
     * For some reason, QScrollBar::valueChanged is not emitted when
     * the scrolling happened from moving the cursor beyond the visible content,
     * so QTextEdit::cursorPositionChanged has to be connected as well.
     */

    auto offsetHexFunc = [this]() {
        ui->hexHexText->verticalScrollBar()->setValue(ui->hexOffsetText->verticalScrollBar()->value());
    };

    auto offsetASCIIFunc = [this]() {
        ui->hexASCIIText->verticalScrollBar()->setValue(ui->hexOffsetText->verticalScrollBar()->value());
    };

    connect(ui->hexOffsetText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexHexText->verticalScrollBar(), offsetHexFunc);
    connect(ui->hexOffsetText, &QTextEdit::cursorPositionChanged, ui->hexHexText->verticalScrollBar(), offsetHexFunc);
    connect(ui->hexOffsetText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexASCIIText->verticalScrollBar(), offsetASCIIFunc);
    connect(ui->hexOffsetText, &QTextEdit::cursorPositionChanged, ui->hexASCIIText->verticalScrollBar(), offsetASCIIFunc);

    auto hexOffsetFunc = [this]() {
        ui->hexOffsetText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
    };

    auto hexASCIIFunc = [this]() {
        ui->hexASCIIText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
    };

    connect(ui->hexHexText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexOffsetText->verticalScrollBar(), hexOffsetFunc);
    connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, ui->hexOffsetText->verticalScrollBar(), hexOffsetFunc);
    connect(ui->hexHexText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexASCIIText->verticalScrollBar(), hexASCIIFunc);
    connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, ui->hexASCIIText->verticalScrollBar(), hexASCIIFunc);

    auto asciiOffsetFunc = [this]() {
        ui->hexOffsetText->verticalScrollBar()->setValue(ui->hexASCIIText->verticalScrollBar()->value());
    };

    auto asciiHexFunc = [this]() {
        ui->hexHexText->verticalScrollBar()->setValue(ui->hexASCIIText->verticalScrollBar()->value());
    };

    connect(ui->hexASCIIText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexOffsetText->verticalScrollBar(), asciiOffsetFunc);
    connect(ui->hexASCIIText, &QTextEdit::cursorPositionChanged, ui->hexOffsetText->verticalScrollBar(), asciiOffsetFunc);
    connect(ui->hexASCIIText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexHexText->verticalScrollBar(), asciiHexFunc);
    connect(ui->hexASCIIText, &QTextEdit::cursorPositionChanged, ui->hexHexText->verticalScrollBar(), asciiHexFunc);
}

void HexdumpWidget::setupSelection()
{
    connect(ui->hexHexText, &QTextEdit::selectionChanged, this, &HexdumpWidget::selectionChanged);
    connect(ui->hexASCIIText, &QTextEdit::selectionChanged, this, &HexdumpWidget::selectionChanged);
    connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::selectionChanged);
    connect(ui->hexASCIIText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::selectionChanged);

}
unsigned long long HexdumpWidget::hexPositionToAddress(unsigned long long position)
{
    return m_addr + (position / 3); // two kinds "00 " "00\n"
}

int HexdumpWidget::asciiAddressToPosition(unsigned long long address)
{
    unsigned long long local_address = address - m_addr;
    int position = local_address + (local_address / m_columnCount); // plus \n count
    return position;
}

unsigned long long HexdumpWidget::asciiPositionToAddress(unsigned long long position)
{
    // Each row adds one byte (because of the newline), so cols + 1 gets rid of that offset
    return m_addr + (position - (position / (m_columnCount + 1)));
}
unsigned long long HexdumpWidget::hexAddressToPosition(unsigned long long address)
{
    return (address - m_addr) * 3;
}

void HexdumpWidget::refresh()
{
    updateHeaders();

    auto hexdump = fetchHexdump();

    ui->hexOffsetText->setText(hexdump[0]);
    ui->hexHexText->setText(hexdump[1]);
    ui->hexASCIIText->setText(hexdump[2]);

    QTextCursor cursor(ui->hexHexText->document()->findBlockByLineNumber(0)); // ln-1 because line number starts from 0
    ui->hexHexText->moveCursor(QTextCursor::Start);
    ui->hexHexText->setTextCursor(cursor);

    updateWidths();

    // Update other text areas scroll
    ui->hexOffsetText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
    ui->hexASCIIText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
}

std::array<QString, 3> HexdumpWidget::fetchHexdump()
{
    // Main bytes to fetch:

    unsigned long long bytes = m_length;
    unsigned long long needLines = bytes / m_columnCount + 1;

    char *byte_array = (char*)m_addr;

    QString hexText = "";
    QString offsetText = "";
    QString asciiText = "";

    unsigned long long cur_addr = m_addr;
    for(unsigned long long i=0; i < needLines; i++)
    {
        for(int j=0; j < m_columnCount; j++)
        {
            unsigned long long curPos = (i * m_columnCount) + j;
            int b = byte_array[curPos];

            if((j > 0) && (j < m_columnCount))
            {
                hexText += " ";
            }

            if(curPos >= m_length){
                hexText += "  ";
                asciiText += " ";

            }else{
                hexText += QString::number(b, 16).rightJustified(2, '0');

                // Non printable
                if((b < 0x20) || (b > 0x7E))
                {
                    asciiText += ".";
                } else {
                    asciiText += (char)b;
                }
            }
        }

        if(m_isAddr64bit){
            offsetText += RAddressString64(cur_addr - m_displayOffset) + "\n";
        }else{
            offsetText += RAddressString32(cur_addr - m_displayOffset) + "\n";
        }
        hexText += "\n";
        asciiText += "\n";

        cur_addr += m_columnCount;
    }

    return {{offsetText, hexText, asciiText}};
}
void HexdumpWidget::updateWidths()
{
    // Update width
    ui->hexHexText->document()->adjustSize();
    ui->hexHexText->setFixedWidth(ui->hexHexText->document()->size().width());

    ui->hexOffsetText->document()->adjustSize();
    ui->hexOffsetText->setFixedWidth(ui->hexOffsetText->document()->size().width());

    ui->hexASCIIText->document()->adjustSize();
    ui->hexASCIIText->setMinimumWidth(ui->hexASCIIText->document()->size().width());
}

void HexdumpWidget::selectionChanged()
{
    if(m_isSelecting)
        return;
    m_isSelecting = true;

    if(sender() == ui->hexHexText)
    {
        QTextCursor textCursor = ui->hexHexText->textCursor();
        if(!textCursor.hasSelection())
        {
            unsigned long long adr = hexPositionToAddress(textCursor.position());
            int pos = asciiAddressToPosition(adr);
            setTextEditPosition(ui->hexASCIIText, pos);

            m_isSelecting = false;

            return;
        }

        unsigned long long selectionStart = textCursor.selectionStart();
        unsigned long long selectionEnd = textCursor.selectionEnd();

        QChar start = ui->hexHexText->document()->characterAt(selectionStart);
        QChar end = ui->hexHexText->document()->characterAt(selectionEnd);


        // Handle the spaces/newlines (if it's at the start, move forward,
        // if it's at the end, move back)

        if (!start.isLetterOrNumber())
        {
            selectionStart += 1;
        }
        else if(ui->hexHexText->document()->characterAt(selectionStart-1).isLetterOrNumber())
        {
            selectionStart += 2;
        }

        if (!end.isLetterOrNumber())
        {
            selectionEnd += 1;
        }

        // In hextext we have the spaces that we need to somehow handle.
        unsigned long long startAddress = hexPositionToAddress(selectionStart);
        unsigned long long endAddress = hexPositionToAddress(selectionEnd);


        int startPosition = asciiAddressToPosition(startAddress);
        int endPosition = asciiAddressToPosition(endAddress);

        QTextCursor targetTextCursor = ui->hexASCIIText->textCursor();
        targetTextCursor.setPosition(startPosition);
        targetTextCursor.setPosition(endPosition, QTextCursor::KeepAnchor);
        ui->hexASCIIText->setTextCursor(targetTextCursor);
    }
    else if(sender() == ui->hexASCIIText)
    {
        QTextCursor textCursor = ui->hexASCIIText->textCursor();
        if(!textCursor.hasSelection())
        {
            unsigned long long adr = asciiPositionToAddress(textCursor.position());
            unsigned long long pos = hexAddressToPosition(adr);
            setTextEditPosition(ui->hexHexText, pos);

            m_isSelecting = false;

            return;
        }
        unsigned long long startAddress = asciiPositionToAddress(textCursor.selectionStart());
        unsigned long long endAddress = asciiPositionToAddress(textCursor.selectionEnd());


        unsigned long long startPosition = hexAddressToPosition(startAddress);
        unsigned long long endPosition = hexAddressToPosition(endAddress);

        // End position -1 because the position we get above is for the next
        // entry, so including the space/newline
        endPosition -= 1;

        QTextCursor targetTextCursor = ui->hexHexText->textCursor();
        targetTextCursor.setPosition(startPosition);
        targetTextCursor.setPosition(endPosition, QTextCursor::KeepAnchor);
        ui->hexHexText->setTextCursor(targetTextCursor);
    }

    m_isSelecting = false;
}


