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



static void registerCustomFonts()
{
    int ret = QFontDatabase::addApplicationFont(":/fonts/Anonymous Pro.ttf");
    assert(-1 != ret && "unable to register Anonymous Pro.ttf");

    ret = QFontDatabase::addApplicationFont(":/fonts/Inconsolata-Regular.ttf");
    assert(-1 != ret && "unable to register Inconsolata-Regular.ttf");

    // Do not issue a warning in release
    Q_UNUSED(ret)
}


int getMaxFullyDisplayedLines(QTextEdit *textEdit)
{
    QFontMetrics fontMetrics(textEdit->document()->defaultFont());
    return (textEdit->height()
            - (textEdit->contentsMargins().top()
               + textEdit->contentsMargins().bottom()
               + (int)(textEdit->document()->documentMargin() * 2)))
           / fontMetrics.lineSpacing();
}


inline QString RAddressString(unsigned long long addr)
{
    return QString::asprintf("%#010llx", addr);
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
}

HexdumpWidget::~HexdumpWidget()
{
    delete ui;
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


    connect(ui->hexHexText, &QTextEdit::selectionChanged, this, &HexdumpWidget::selectionChanged);
    connect(ui->hexASCIIText, &QTextEdit::selectionChanged, this, &HexdumpWidget::selectionChanged);
    connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::selectionChanged);
    connect(ui->hexASCIIText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::selectionChanged);

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
    int ascii_cols = cols;
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

    for (int i=0; i<cols; i++)
    {
        if (i > 0 && ((pairs && !(i&1)) || !pairs))
        {
            hexHeader << " ";
        }

        hexHeader << space << (i & 0xF);
    }

    for (int i=0; i < ascii_cols; i++)
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
        if(!scroll_disabled)
        {
            scroll_disabled = true;
            ui->hexHexText->verticalScrollBar()->setValue(ui->hexOffsetText->verticalScrollBar()->value());
            scroll_disabled = false;
        }
    };

    auto offsetASCIIFunc = [this]() {
        if(!scroll_disabled)
        {
            scroll_disabled = true;
            ui->hexASCIIText->verticalScrollBar()->setValue(ui->hexOffsetText->verticalScrollBar()->value());
            scroll_disabled = false;
        }
    };

    connect(ui->hexOffsetText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexHexText->verticalScrollBar(), offsetHexFunc);
    connect(ui->hexOffsetText, &QTextEdit::cursorPositionChanged, ui->hexHexText->verticalScrollBar(), offsetHexFunc);
    connect(ui->hexOffsetText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexASCIIText->verticalScrollBar(), offsetASCIIFunc);
    connect(ui->hexOffsetText, &QTextEdit::cursorPositionChanged, ui->hexASCIIText->verticalScrollBar(), offsetASCIIFunc);

    auto hexOffsetFunc = [this]() {
        if(!scroll_disabled)
        {
            scroll_disabled = true;
            ui->hexOffsetText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
            scroll_disabled = false;
        }
    };

    auto hexASCIIFunc = [this]() {
        if(!scroll_disabled)
        {
            scroll_disabled = true;
            ui->hexASCIIText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
            scroll_disabled = false;
        }
    };

    connect(ui->hexHexText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexOffsetText->verticalScrollBar(), hexOffsetFunc);
    connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, ui->hexOffsetText->verticalScrollBar(), hexOffsetFunc);
    connect(ui->hexHexText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexASCIIText->verticalScrollBar(), hexASCIIFunc);
    connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, ui->hexASCIIText->verticalScrollBar(), hexASCIIFunc);

    auto asciiOffsetFunc = [this]() {
        if(!scroll_disabled)
        {
            scroll_disabled = true;
            ui->hexOffsetText->verticalScrollBar()->setValue(ui->hexASCIIText->verticalScrollBar()->value());
            scroll_disabled = false;
        }
    };

    auto asciiHexFunc = [this]() {
        if(!scroll_disabled)
        {
            scroll_disabled = true;
            ui->hexHexText->verticalScrollBar()->setValue(ui->hexASCIIText->verticalScrollBar()->value());
            scroll_disabled = false;
        }
    };

    connect(ui->hexASCIIText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexOffsetText->verticalScrollBar(), asciiOffsetFunc);
    connect(ui->hexASCIIText, &QTextEdit::cursorPositionChanged, ui->hexOffsetText->verticalScrollBar(), asciiOffsetFunc);
    connect(ui->hexASCIIText->verticalScrollBar(), &QScrollBar::valueChanged, ui->hexHexText->verticalScrollBar(), asciiHexFunc);
    connect(ui->hexASCIIText, &QTextEdit::cursorPositionChanged, ui->hexHexText->verticalScrollBar(), asciiHexFunc);
}
unsigned long long HexdumpWidget::hexPositionToAddress(int position)
{
    return first_loaded_address + (position / 3);
}

int HexdumpWidget::asciiAddressToPosition(unsigned long long address)
{
    unsigned long long local_address = address - first_loaded_address;
    int position = local_address + (local_address / cols);
    return position;
}
void HexdumpWidget::setTextEditPosition(QTextEdit *textEdit, int position)
{
    QTextCursor textCursor = textEdit->textCursor();
    textCursor.setPosition(position);
    textEdit->setTextCursor(textCursor);
}
unsigned long long HexdumpWidget::asciiPositionToAddress(int position)
{
    // Each row adds one byte (because of the newline), so cols + 1 gets rid of that offset
    return first_loaded_address + (position - (position / (cols + 1)));
}
int HexdumpWidget::hexAddressToPosition(unsigned long long address)
{
    return (address - first_loaded_address) * 3;
}

void HexdumpWidget::selectionChanged()
{
    if(scroll_disabled)
    {
        return;
    }

    if(sender() == ui->hexHexText)
    {
        QTextCursor textCursor = ui->hexHexText->textCursor();
        if(!textCursor.hasSelection())
        {
            unsigned long long adr = hexPositionToAddress(textCursor.position());
            int pos = asciiAddressToPosition(adr);
            setTextEditPosition(ui->hexASCIIText, pos);

            return;
        }

        int selectionStart = textCursor.selectionStart();
        int selectionEnd = textCursor.selectionEnd();

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
    else
    {
        QTextCursor textCursor = ui->hexASCIIText->textCursor();
        if(!textCursor.hasSelection())
        {
            unsigned long long adr = asciiPositionToAddress(textCursor.position());
            int pos = hexAddressToPosition(adr);
            setTextEditPosition(ui->hexHexText, pos);
            return;
        }
        unsigned long long startAddress = asciiPositionToAddress(textCursor.selectionStart());
        unsigned long long endAddress = asciiPositionToAddress(textCursor.selectionEnd());


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

    return;
}


void HexdumpWidget::showEvent(QShowEvent *event)
{
    char * s = "hello world 111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
    refresh((unsigned long long)(void*)s, strlen(s) + 1);
}

void HexdumpWidget::refresh(unsigned long long addr, unsigned long long maxlen)
{
    if(!addr)
        return;

    updateHeaders();

    const int cols = 16;

    // Align addr to cols
    addr -= addr % cols;

    int maxDisplayLines = getMaxFullyDisplayedLines(ui->hexHexText);
    int allNeedLines = maxlen / cols + 1;
    if(allNeedLines < maxDisplayLines)
    {
        maxDisplayLines = allNeedLines;
    }

    //RVA cur_addr = addr - (bufferLines * cols);
    unsigned long long cur_addr = addr;
    first_loaded_address = cur_addr;
    last_loaded_address = cur_addr + (maxDisplayLines) * cols;


    auto hexdump = fetchHexdump(cur_addr, maxDisplayLines);

    ui->hexOffsetText->setText(hexdump[0]);
    ui->hexHexText->setText(hexdump[1]);
    ui->hexASCIIText->setText(hexdump[2]);

    QTextCursor cursor(ui->hexHexText->document()->findBlockByLineNumber(maxDisplayLines)); // ln-1 because line number starts from 0
    ui->hexHexText->moveCursor(QTextCursor::End);
    ui->hexHexText->setTextCursor(cursor);

    updateWidths();

    // Update other text areas scroll
    ui->hexOffsetText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
    ui->hexASCIIText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());

}

std::array<QString, 3> HexdumpWidget::fetchHexdump(unsigned long long  addr, int lines)
{
    // Main bytes to fetch:

    int bytes = cols * lines;

//    QJsonArray byte_array = Core()->cmdj(command).array();

    QString hexText = "";
    QString offsetText = "";
    QString asciiText = "";
    unsigned long long cur_addr = addr;
    for(int i=0; i < lines; i++)
    {
        for(int j=0; j < cols; j++)
        {
//            int b = byte_array[(i * cols) + j].toInt();
            int b = (int)'a';
            if((j > 0) && (j < cols))
            {
                hexText += " ";
            }
            // Non printable
            if((b < 0x20) || (b > 0x7E))
            {
                asciiText += ".";
            } else {
                asciiText += (char)b;
            }

            hexText += QString::number(b, 16).rightJustified(2, '0');
        }
        offsetText += RAddressString(cur_addr) + "\n";
        hexText += "\n";
        asciiText += "\n";
        cur_addr += cols;
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
