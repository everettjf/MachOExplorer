#include "HexdumpWidget.h"
#include "ui_HexdumpWidget.h"
#include <QTextStream>
#include <QScrollBar>
#include <QFontMetrics>
#include <QElapsedTimer>
#include <QTextCursor>
#include <QTextBlock>

#define kGuiBackgroundColor QColor(255, 255, 255)
#define kBorderTextColor QColor(0, 0, 0)
#define kHighlightTextColor QColor(210, 210, 255)


namespace helper{
    int getMaxFullyDisplayedLines(QTextEdit *textEdit)
    {
        QFontMetrics fontMetrics(textEdit->document()->defaultFont());
        return (textEdit->height()
                - (textEdit->contentsMargins().top()
                   + textEdit->contentsMargins().bottom()
                   + (int)(textEdit->document()->documentMargin() * 2)))
               / fontMetrics.lineSpacing();
    }


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

}

HexdumpWidget::~HexdumpWidget()
{
    delete ui;
}

void HexdumpWidget::setupFont()
{
//    QFont font = QFont("Inconsolata", 12);

//    ui->hexOffsetText->setFont(font);
//    ui->hexHexText->setFont(font);
//    ui->hexASCIIText->setFont(font);

//    ui->offsetHeaderLabel->setFont(font);
//    ui->hexHeaderLabel->setFont(font);
//    ui->asciiHeaderLabel->setFont(font);


//    connect(ui->hexHexText, &QTextEdit::selectionChanged, this, &HexdumpWidget::selectionChanged);
//    connect(ui->hexASCIIText, &QTextEdit::selectionChanged, this, &HexdumpWidget::selectionChanged);
//    connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::selectionChanged);
//    connect(ui->hexASCIIText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::selectionChanged);

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
    int cols = 16;
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

void HexdumpWidget::selectionChanged()
{
//    if(scroll_disabled)
//    {
//        return;
//    }
//    connectScroll(true);

//    if(sender() == ui->hexHexText)
//    {
//        QTextCursor textCursor = ui->hexHexText->textCursor();
//        if(!textCursor.hasSelection())
//        {
//            clearParseWindow();
//            RVA adr = hexPositionToAddress(textCursor.position());
//            int pos = asciiAddressToPosition(adr);
//            setTextEditPosition(ui->hexASCIIText, pos);
//            sent_seek = true;
//            Core()->seek(adr);
//            sent_seek = false;
//            connectScroll(false);
//            return;
//        }

//        int selectionStart = textCursor.selectionStart();
//        int selectionEnd = textCursor.selectionEnd();

//        QChar start = ui->hexHexText->document()->characterAt(selectionStart);
//        QChar end = ui->hexHexText->document()->characterAt(selectionEnd);

//        // This adjusts the selection to make sense with the chosen format
//        switch(format)
//        {
//        case Hex:
//            // Handle the spaces/newlines (if it's at the start, move forward,
//            // if it's at the end, move back)

//            if (!start.isLetterOrNumber())
//            {
//                selectionStart += 1;
//            }
//            else if(ui->hexHexText->document()->characterAt(selectionStart-1).isLetterOrNumber())
//            {
//                selectionStart += 2;
//            }

//            if (!end.isLetterOrNumber())
//            {
//                selectionEnd += 1;
//            }
//            break;
//        case Octal:
//            if (!start.isLetterOrNumber())
//            {
//                selectionStart += 1;
//            }
//            if (!end.isLetterOrNumber())
//            {
//                selectionEnd += 1;
//            }
//            break;
//        }

//        // In hextext we have the spaces that we need to somehow handle.
//        RVA startAddress = hexPositionToAddress(selectionStart);
//        RVA endAddress = hexPositionToAddress(selectionEnd);

//        updateParseWindow(startAddress, endAddress - startAddress);

//        int startPosition = asciiAddressToPosition(startAddress);
//        int endPosition = asciiAddressToPosition(endAddress);
//        QTextCursor targetTextCursor = ui->hexASCIIText->textCursor();
//        targetTextCursor.setPosition(startPosition);
//        targetTextCursor.setPosition(endPosition, QTextCursor::KeepAnchor);
//        ui->hexASCIIText->setTextCursor(targetTextCursor);
//        sent_seek = true;
//        Core()->seek(startAddress);
//        sent_seek = false;
//    }
//    else
//    {
//        QTextCursor textCursor = ui->hexASCIIText->textCursor();
//        if(!textCursor.hasSelection())
//        {
//            clearParseWindow();
//            RVA adr = asciiPositionToAddress(textCursor.position());
//            int pos = hexAddressToPosition(adr);
//            setTextEditPosition(ui->hexHexText, pos);
//            connectScroll(false);
//            sent_seek = true;
//            Core()->seek(adr);
//            sent_seek = false;
//            return;
//        }
//        RVA startAddress = asciiPositionToAddress(textCursor.selectionStart());
//        RVA endAddress = asciiPositionToAddress(textCursor.selectionEnd());

//        updateParseWindow(startAddress, endAddress - startAddress);

//        int startPosition = hexAddressToPosition(startAddress);
//        int endPosition = hexAddressToPosition(endAddress);

//        // End position -1 because the position we get above is for the next
//        // entry, so including the space/newline
//        endPosition -= 1;
//        QTextCursor targetTextCursor = ui->hexHexText->textCursor();
//        targetTextCursor.setPosition(startPosition);
//        targetTextCursor.setPosition(endPosition, QTextCursor::KeepAnchor);
//        ui->hexHexText->setTextCursor(targetTextCursor);
//        sent_seek = true;
//        Core()->seek(startAddress);
//        sent_seek = false;
//    }

//    connectScroll(false);
    return;
}

void HexdumpWidget::connectScroll(bool disconnect_)
{
    scroll_disabled = disconnect_;
    if (disconnect_)
    {
        disconnect(ui->hexHexText->verticalScrollBar(), &QScrollBar::valueChanged, this,
                &HexdumpWidget::scrollChanged);
        disconnect(ui->hexHexText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::scrollChanged);
    }
    else
    {
        connect(ui->hexHexText->verticalScrollBar(), &QScrollBar::valueChanged, this,
                &HexdumpWidget::scrollChanged);
        connect(ui->hexHexText, &QTextEdit::cursorPositionChanged, this, &HexdumpWidget::scrollChanged);

    }
}


void HexdumpWidget::scrollChanged()
{
    connectScroll(true);

//    int firstLine = getDisplayedLined(ui->hexHexText);
//    if(firstLine < (bufferLines/2))
//    {
//        auto hexdump = fetchHexdump(first_loaded_address, bufferLines);
//        first_loaded_address -= bufferLines * cols;
//        prependWithoutScroll(ui->hexOffsetText, hexdump[0]);
//        prependWithoutScroll(ui->hexHexText, hexdump[1]);
//        prependWithoutScroll(ui->hexASCIIText, hexdump[2]);

//        removeBottomLinesWithoutScroll(ui->hexOffsetText, bufferLines);
//        removeBottomLinesWithoutScroll(ui->hexHexText, bufferLines);
//        removeBottomLinesWithoutScroll(ui->hexASCIIText, bufferLines);

//        ui->hexOffsetText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());
//        ui->hexASCIIText->verticalScrollBar()->setValue(ui->hexHexText->verticalScrollBar()->value());

//    }

//    int blocks  = ui->hexHexText->document()->blockCount();
//    int lastLine = getDisplayedLined(ui->hexHexText, true);
//    if(blocks - lastLine < (bufferLines/2))
//    {
//        auto hexdump = fetchHexdump(last_loaded_address, bufferLines);
//        last_loaded_address += bufferLines * cols;
//        removeTopLinesWithoutScroll(ui->hexOffsetText, bufferLines);
//        removeTopLinesWithoutScroll(ui->hexHexText, bufferLines);
//        removeTopLinesWithoutScroll(ui->hexASCIIText, bufferLines);
//        appendWithoutScroll(ui->hexOffsetText, hexdump[0]);
//        appendWithoutScroll(ui->hexHexText, hexdump[1]);
//        appendWithoutScroll(ui->hexASCIIText, hexdump[2]);
//    }
    //    connectScroll(false);
}

void HexdumpWidget::showEvent(QShowEvent *event)
{

    char * s = "hello world 111111111111111111111111111111111111111111111111111";
    refresh((unsigned long long)s, strlen(s) + 1);
}

void HexdumpWidget::refresh(unsigned long long addr, unsigned long long maxlen)
{
    if(!addr)
        return;

    connectScroll(true);

    updateHeaders();

    const int cols = 16;

    // Align addr to cols
    addr -= addr % cols;

    int maxDisplayLines = helper::getMaxFullyDisplayedLines(ui->hexHexText);
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

    connectScroll(false);
}

std::array<QString, 3> HexdumpWidget::fetchHexdump(unsigned long long  addr, int lines)
{
    // Main bytes to fetch:
    const int cols = 16;

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
