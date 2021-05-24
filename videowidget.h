﻿#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QVector>
#include <QFile>
#include <QDir>

#include "myvideosurface.h"
#include "shared.h"

class VideoWidget : public QWidget
{
public:
    explicit VideoWidget(int W,int H, QWidget *parent = 0);
    ~VideoWidget();
    QAbstractVideoSurface *refVideoSurface() const { return surface; }
    MyVideoSurface *refImageSurface() { return surface; }
    void lock();
    void draw(INFO info);
    void saveRect();
    void clearRect();
    void loadRect(INFO info);

protected:
    void paintEvent(QPaintEvent *event);

private:
    MyVideoSurface *surface;
    int W;
    int H;
    float Wratio;
    float Hratio;
    QPen pen;
    INFO info;
    QVector<QPoint> point_SN;
    QPoint point_null;

    volatile bool isPush;
    volatile bool isdraw;

    //offset
    INFO saveInfo;
    bool isSaveRect;
    Shared *shared;
    QDir dir;
    QFile file;
    float offsetA,offsetB,offsetC,offsetD;
    float AreaS(INFO_SN &info);



};

#endif // VIDEOWIDGET_H
