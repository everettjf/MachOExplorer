#ifndef MOEX_H
#define MOEX_H

#include <QObject>

class MoexCore : public QObject
{
    Q_OBJECT
public:
    explicit MoexCore(QObject *parent = 0);
    ~MoexCore();

};

#endif // MOEX_H
