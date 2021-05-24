﻿#ifndef SHARED_H
#define SHARED_H

#include <QDebug>
#include <QDateTime>
#include <QPoint>
#include <QMetaType>
#define SCANTOTAL 1

struct INFO_SN{
    QPoint A;
    QPoint B;
    QPoint C;
    QPoint D;
    QByteArray text;
};

struct INFO{
    INFO_SN SN[SCANTOTAL];
    int total;
};
Q_DECLARE_METATYPE(INFO)

class Shared
{
public:
    Shared();
    QString getDate();
    QString getDateTime();
    QString getDateTime_FILE();
    QString getDateTime_print();
    QString getDateTime_yesterday_FILE();
};

#endif // SHARED_H
