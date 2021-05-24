#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QToolBar>
#include <QDebug>
#include <QMessageBox>

#include "camera.h"
#include "shared.h"
#include "config.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QImage *on_Capture();
    QImage currentImage;
    QImage *currentImage_ptr;
    void Start();
    void stop(bool isCloseCamera = false);

public slots:
    void applySeting();

private:
    Ui::Widget *ui;
    Config *config;
    QDir dir;
    QFile file;
    QToolBar *toolbar;
    QAction *act_save;
    QAction *act_clear;
    QAction *act_config;
    QAction *act_open;
    void save_clicked();
    void clear_clicked();
    void config_clicked();
    void open_clicked();
    void reStart();

    //camera
    Camera *camera;

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void toolbarSelect(QAction *act);
    void displayCaptureError(int id,QCameraImageCapture::Error error,const QString &errorString);

signals:
    void closeWindow();
};

#endif // WIDGET_H
