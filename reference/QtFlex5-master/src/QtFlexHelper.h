#ifndef QT_FLEXHELPER_H
#define QT_FLEXHELPER_H

#include "QtFlexManager.h"

class FlexHelperImpl;

class QT_FLEX_API FlexHelper : public QObject
{
    Q_OBJECT
public:
    FlexHelper(QWidget* parent);
    ~FlexHelper();

Q_SIGNALS:
    void clicked(Flex::Button button, bool* accepted);

public:
    bool eventFilter(QObject*, QEvent*);

public:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);

public:
    void setWindowInfo(int titleBarHeight, Qt::WindowFlags windowFlags);

public:
    QWidget* buttons() const;
    QWidget* extents() const;

public:
    QToolButton* button(Flex::Button button) const;

public:
    void change(Flex::Button src, Flex::Button dst);

private Q_SLOTS:
    void on_button_clicked();

private:
    friend class FlexHelperImpl;

private:
    QScopedPointer<FlexHelperImpl> impl;
};

#endif
