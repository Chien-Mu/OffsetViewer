#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <dmtx.h>

#include "widget.h"
#include "shared.h"

class scanthread : public QThread
{
    Q_OBJECT
public:
    scanthread(Widget *ref);
    void setScan(int length_SN,int delay_loop,int delay_dmtx);
    void run();
    void stop();

private:
    Widget *ref;
    bool quit;
    QByteArray scan(QImage *currentImage);
    QByteArray currentSN;
    QPoint point_null;
    int length_SN;
    int delay_loop;
    int delay_dmtx;

signals:
    void throwInfo(const INFO value);
};

#endif // SCANTHREAD_H
