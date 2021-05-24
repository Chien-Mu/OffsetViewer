#include "config.h"
#include "ui_config.h"
#include <QMessageBox>
#include <QDebug>

Config::Config(QWidget *parent) : QWidget(parent),ui(new Ui::Config)
{
    ui->setupUi(this);
    //不開放功能
    ui->cb_length_SN->setVisible(false);
    ui->label_2->setVisible(false);

    //signal
    connect(ui->btn_apply,SIGNAL(clicked()),this,SLOT(closeWindow()));

    //CREATE DIR
    if(!dir.mkpath(QDir::currentPath()+"/ScannerConfig"))
        QMessageBox::critical(this,tr("Error"),"!.. " + QDir::currentPath()+"/ScannerConfig -> The dir an not be created.");

    //驗證器
    validInt = new QIntValidator(this); //限制只能 int

    //ui載入設定選項
    setUI();

    //load config
    getConfig(); 
}

void Config::getConfig(){
    QString txtPath = QDir::currentPath() + "/ScannerConfig/config.txt";
    file.setFileName(txtPath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::critical(this, tr("Error"), txtPath+ "..does not exist.");
        return;
    }

    QTextStream in(&file);
    QString line,item;
    while(!in.atEnd()){
        line = "";
        item = "";
        line = in.readLine();
        item = line.left(3);

        if(item == "LSN"){
            ui->cb_length_SN->setCurrentText(line.mid(4,line.length()));
        }else if(item == "DEL"){
            ui->cb_delay_loop->setCurrentText(line.mid(4,line.length()));
        }else if(item == "DED"){
            ui->cb_delay_dmtx->setCurrentText(line.mid(4,line.length()));
        }
    }

    file.close();
}

void Config::setConfig(){
    QString txtPath = QDir::currentPath() + "/ScannerConfig/config.txt";
    file.setFileName(txtPath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){ //檔案會覆蓋
        QMessageBox::critical(this, tr("Error"), txtPath+ "..does not exist.");
        return;
    }
    QTextStream out(&file);

    out << "LSN=" + QString::number(get_length_SN()) + "\n";
    out << "DEL=" + QString::number(get_delay_loop()) + "\n";
    out << "DED=" + QString::number(get_delay_dmtx()) + "\n";

    file.close();
}

void Config::setUI(){
    //scanner
    for(int i=1;i<=30;i++)
        ui->cb_length_SN->addItem(QString::number(i));
    for(int i=0;i<=1000;i+=50){
        ui->cb_delay_dmtx->addItem(QString::number(i));
        ui->cb_delay_loop->addItem(QString::number(i));
    }
    //default
    ui->cb_length_SN->setCurrentText(QString::number(15));
    ui->cb_delay_dmtx->setCurrentText(QString::number(200));
    ui->cb_delay_loop->setCurrentText(QString::number(0));
}

void Config::closeWindow(){
    this->close();
    emit applying();
}

int Config::get_length_SN(){ return ui->cb_length_SN->currentText().toInt(); }
int Config::get_delay_loop(){ return ui->cb_delay_loop->currentText().toInt(); }
int Config::get_delay_dmtx(){ return ui->cb_delay_dmtx->currentText().toInt(); }

Config::~Config()
{
    delete ui;
}
