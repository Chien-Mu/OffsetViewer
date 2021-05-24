﻿#include "camera.h"
#include <QDebug>
#include <QMessageBox>

Camera::Camera(QByteArray deviceName)
{
    //統一解析度
    /*C310 Support
     * 320X240
     * 640X480
     * 1280 X 720
     * 5.0MP (軟體增強處理) */
    this->W = 640;
    this->H = 480;

    setCamera(deviceName);
}

void Camera::setCamera(QByteArray deviceName){
    if(deviceName == "")
        cameraDevice = new QCamera; //QCamera 控制相機用
    else
        cameraDevice = new QCamera(deviceName);

    connect(cameraDevice,SIGNAL(error(QCamera::Error)),this,SLOT(on_cameraError()));

    //viewfinder
    videoWidget = new VideoWidget(W,H);
    cameraDevice->setViewfinder(videoWidget->refVideoSurface());

    //設定捕捉模式
    cameraDevice->setCaptureMode(QCamera::CaptureStillImage);

    //設定捕捉過程
    imageCapture = new QCameraImageCapture(cameraDevice);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    //設定相機抓傳入時解析度
    QImageEncoderSettings imageSettings;
    //imageSettings.setCodec("image/jpeg");
    imageSettings.setResolution(W,H);
    imageCapture->setEncodingSettings(imageSettings); //匯入設定影像編碼選項

    //error
    connect(imageCapture,SIGNAL(error(int,QCameraImageCapture::Error,QString)),
            this,SIGNAL(Error(int,QCameraImageCapture::Error,QString)));
}

QImage Camera::getCurrentImage(){
    videoWidget->refImageSurface()->getlock();
    return videoWidget->refImageSurface()->getCurrentImage();
}

void Camera::drawVideoWidget(INFO info){
    videoWidget->lock();
    videoWidget->draw(info);
}

void Camera::saveRect(){
    videoWidget->saveRect();
}

void Camera::clearRect(){
    videoWidget->clearRect();
}

void Camera::loadRect(INFO info){
    videoWidget->loadRect(info);
}

void Camera::CameraStrat(){
    cameraDevice->start();
}

void Camera::CameraStop(){
    if(cameraDevice->isAvailable())
        cameraDevice->stop();
}

void Camera::on_cameraError(){
    QMessageBox::critical(NULL,"Camera error",cameraDevice->errorString());
}
