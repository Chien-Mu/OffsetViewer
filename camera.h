#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QWidget>
#include <QCamera>
#include <QCameraImageCapture>
#include <QImageEncoderSettings>

#include "videowidget.h"
#include "shared.h"

class Camera : public QObject
{
    Q_OBJECT
public:
    Camera(QByteArray deviceName);
    //set
    void CameraStrat();
    void CameraStop();
    void saveRect();
    void clearRect();
    void loadRect(INFO info);

    //get
    QWidget *getVideoWidget() { return videoWidget; }
    QImage getCurrentImage();

public slots:
    void drawVideoWidget(INFO info);

private:
    //camera and control
    QCamera *cameraDevice;
    QCameraImageCapture *imageCapture;

    //viewfinder
    VideoWidget *videoWidget;

    //方法
    void setCamera(QByteArray deviceName);

    //屬性
    int W;
    int H;

private slots:
    void on_cameraError();

signals:
    void Error(int id,QCameraImageCapture::Error error,const QString &errorString);
};

#endif // CAMERA_H
