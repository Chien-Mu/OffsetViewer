#include "widget.h"
#include "ui_widget.h"

#include <QCameraInfo>
#include <QFileDialog>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include "scanthread.h"
scanthread *scanner;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //configure
    config = new Config;
    connect(config,SIGNAL(applying()),this,SLOT(applySeting()));
    connect(this,SIGNAL(closeWindow()),config,SLOT(setConfig())); //主視窗關閉時儲存資訊

    //toolbar
    toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(40,40));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); //文字顯示在icon下面
    act_open = new QAction(QIcon(":/img/load.png"),"Open",this);
    act_open->setIconText(QString::fromLocal8Bit("Open Rect"));
    act_open->setShortcut(Qt::CTRL|Qt::Key_O);
    act_open->setToolTip("Ctrl + O");
    toolbar->addAction(act_open);
    act_save = new QAction(QIcon(":/img/save.png"),"Save",this);
    act_save->setIconText(QString::fromLocal8Bit("Save"));
    act_save->setShortcut(Qt::CTRL|Qt::Key_S);
    act_save->setToolTip("Ctrl+s");
    toolbar->addAction(act_save);
    act_clear = new QAction(QIcon(":/img/clear.png"),"Clear",this);
    act_clear->setIconText(QString::fromLocal8Bit("Clear"));
    act_clear->setShortcut(Qt::CTRL|Qt::Key_C);
    act_clear->setToolTip("Ctrl+c");
    toolbar->addAction(act_clear);
    act_config = new QAction(QIcon(":/img/settings.png"),"Config",this);
    act_config->setIconText(QString::fromLocal8Bit("Configure"));
    act_config->setShortcut(Qt::CTRL|Qt::Key_Comma); //","
    act_config->setToolTip("Ctrl + ,");
    toolbar->addAction(act_config);
    ui->VL_toolbar->addWidget(toolbar); //在多設一個layout比較好控制
    connect(toolbar,SIGNAL(actionTriggered(QAction*)),this,SLOT(toolbarSelect(QAction*)));


    //列出相機
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    //qDebug() << cameras[0].deviceName(); //設備id
    //qDebug() << cameras[0].description(); //設備名稱

    //camera
    //camera = new Camera(cameras[0].deviceName().toLocal8Bit());
    camera = new Camera("");
    connect(camera,SIGNAL(Error(int,QCameraImageCapture::Error,QString)),
            this,SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));
    camera->CameraStrat();

    //scanner
    scanner = new scanthread(this);

    //draw
    qRegisterMetaType<INFO>("INFO");
    connect(scanner,SIGNAL(throwInfo(INFO)),camera,SLOT(drawVideoWidget(INFO)),Qt::BlockingQueuedConnection);
    ui->VL_viewer->addWidget(camera->getVideoWidget());

    //initialization
    Start();
}

void Widget::Start(){
    //CREATE DIR
    if(!dir.mkpath(QDir::currentPath()+"/log"))
        QMessageBox::critical(this,tr("Error"),QDir::currentPath()+"/log -> The dir an not be created.");
    //-------------------------------

    //scanner
    scanner->setScan(config->get_length_SN(),
                     config->get_delay_loop(),
                     config->get_delay_dmtx());
    scanner->start();
}

void Widget::reStart(){
    this->stop();
    while(scanner->isRunning())
        QThread::msleep(200);
    this->Start();
}

void Widget::applySeting(){
    //鎖定介面
    QProgressDialog pd;
    pd.setWindowTitle("Please Wait");
    pd.setLabelText("Seting.....");
    pd.setRange(0,0);
    pd.setCancelButton(NULL);
    pd.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);//不顯示“最大化”，“最小化”；“關閉”不可用
    //把會把GUI鎖住的耗能分出去
    QFutureWatcher<void> watcher(this);
    QFuture<void> future = QtConcurrent::run(this,&Widget::reStart);
    connect(&watcher,SIGNAL(finished()),&pd,SLOT(reset()));
    connect(&pd,SIGNAL(canceled()),&watcher,SLOT(cancel()));
    watcher.setFuture(future);
    pd.exec();
    watcher.waitForFinished();
}

void Widget::toolbarSelect(QAction *act){
    if(act->text() == "Save")
        save_clicked();
    else if(act->text() == "Clear")
        clear_clicked();
    else if(act->text() == "Config")
        config_clicked();
    else if(act->text() == "Open")
        open_clicked();
}

void Widget::open_clicked(){
    QFileDialog::Options options;
    options = QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog;
    QString selectFilter;
    QString txtPath = QFileDialog::getOpenFileName(this,"Open file", QDir::currentPath()+"/log",
                                       tr("Text File (*.txt);;All File (*)"),&selectFilter,options);
    file.setFileName(txtPath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::critical(this, tr("Error"), txtPath+ "..does not exist.");
        return;
    }

    QTextStream in(&file);
    INFO info;
    int i=0;
    QString line,item;
    while(!in.atEnd()){
        line = "";
        item = "";
        line = in.readLine();
        item = line.left(3);

        if(item == "TOL"){
            info.total = line.mid(4,line.length()).toInt();
        }else if(item == "POA"){
            info.SN[i].A.setX(line.mid(4,line.indexOf(',')-4).toInt());
            info.SN[i].A.setY(line.mid(line.indexOf(',')+1,line.length()).toInt());
        }else if(item == "POB"){
            info.SN[i].B.setX(line.mid(4,line.indexOf(',')-4).toInt());
            info.SN[i].B.setY(line.mid(line.indexOf(',')+1,line.length()).toInt());
        }else if(item == "POC"){
            info.SN[i].C.setX(line.mid(4,line.indexOf(',')-4).toInt());
            info.SN[i].C.setY(line.mid(line.indexOf(',')+1,line.length()).toInt());
        }else if(item == "POD"){
            info.SN[i].D.setX(line.mid(4,line.indexOf(',')-4).toInt());
            info.SN[i].D.setY(line.mid(line.indexOf(',')+1,line.length()).toInt());
        }else if(item == "TXT"){
            info.SN[i].text = line.mid(4,line.length()).toLocal8Bit();
            i++;
        }
    }

    camera->loadRect(info);
    file.close();
}

void Widget::save_clicked(){
    camera->saveRect();
}

void Widget::clear_clicked(){
    camera->clearRect();
}

void Widget::config_clicked(){
    config->show();
    config->raise();
}

QImage* Widget::on_Capture(){
    this->currentImage =  camera->getCurrentImage(); //by value
    this->currentImage_ptr = &this->currentImage;
    return currentImage_ptr;
}

void Widget::displayCaptureError(int id, QCameraImageCapture::Error error, const QString &errorString){
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
}

void Widget::stop(bool isCloseCamera){
    if(scanner->isRunning())
        scanner->stop();

    if(isCloseCamera){
        camera->CameraStop();
    }
}

void Widget::closeEvent(QCloseEvent *e){
    Q_UNUSED(e);
    stop(true);
    emit closeWindow();
}

Widget::~Widget()
{
    delete ui;
}
