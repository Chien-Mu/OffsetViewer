#include "videowidget.h"
#include <QDebug>
#include <QtMath>

VideoWidget::VideoWidget(int W, int H, QWidget *parent) : QWidget(parent)
{
    shared = new Shared;
    this->W = W;
    this->H = H;
    this->Wratio = 640.0/(float)W;
    this->Hratio = 480.0/(float)H;
    this->isSaveRect = false;
    this->offsetA = 0;
    this->offsetB = 0;
    this->offsetC = 0;
    this->offsetD = 0;

    this->isPush = true;
    this->isdraw = false;

    point_null.setX(0);
    point_null.setY(0);
    for(int i=0;i<SCANTOTAL;i++){
        this->point_SN.push_back(point_null);
        info.SN[i].text = "";
        info.SN[i].A = point_null;
        info.SN[i].B = point_null;
        info.SN[i].C = point_null;
        info.SN[i].D = point_null;
    }
    info.total = 0;

    this->saveInfo = this->info;

    surface = new MyVideoSurface(this,W,H);
    this->setFixedSize(640,480); //如果沒設畫板大小，可能顯示會有問題。
}

VideoWidget::~VideoWidget(){
    delete surface;
}

void VideoWidget::saveRect(){
    this->isSaveRect = true;
}

void VideoWidget::clearRect(){
    this->saveInfo.total = 0;
}

void VideoWidget::loadRect(INFO info){
    this->saveInfo.total = 0;
    this->saveInfo = info;
}

float VideoWidget::AreaS(INFO_SN &info){
    float c,d,e; //各邊長,e是中間那條線
    float ACD; //三角形面積
    float p;

    //算出個邊長
    c = (float)qSqrt(qPow(info.A.x() - info.C.x(),2) +
                     qPow(info.A.y() - info.C.y(),2));
    d = (float)qSqrt(qPow(info.C.x() - info.D.x(),2) +
                     qPow(info.C.y() - info.D.y(),2));
    e = (float)qSqrt(qPow(info.A.x() - info.D.x(),2) +
                     qPow(info.A.y() - info.D.y(),2));

    //任意三角形面積
    p = (c+d+e)/2;
    ACD = (float)qSqrt(p*(p-c)*(p-d)*(p-e)); //海倫公式
    return ACD;
}

void VideoWidget::lock(){
    this->isPush = true;
}

void VideoWidget::draw(INFO info){
    while(isdraw)
        continue;

    for(int i=0;i<SCANTOTAL && !isdraw;i++){
        if(i<info.total || !info.SN[i].A.isNull()){
            this->info.SN[i].A.setX(info.SN[i].A.x()*Wratio);
            this->info.SN[i].A.setY(info.SN[i].A.y()*Hratio);
            this->info.SN[i].B.setX(info.SN[i].B.x()*Wratio);
            this->info.SN[i].B.setY(info.SN[i].B.y()*Hratio);
            this->info.SN[i].C.setX(info.SN[i].C.x()*Wratio);
            this->info.SN[i].C.setY(info.SN[i].C.y()*Hratio);
            this->info.SN[i].D.setX(info.SN[i].D.x()*Wratio);
            this->info.SN[i].D.setY(info.SN[i].D.y()*Hratio);
            this->info.SN[i].text = info.SN[i].text;
            this->point_SN[i] = QPoint(this->info.SN[i].B.x(),this->info.SN[i].B.y()-40);
        }else{
            this->info.SN[i].A = point_null;
            this->info.SN[i].B = point_null;
            this->info.SN[i].C = point_null;
            this->info.SN[i].D = point_null;
            this->info.SN[i].text = "";
            this->point_SN[i] = point_null;
        }
    }
    this->info.total = info.total;

    if(this->isSaveRect){
        //copy
        this->saveInfo = this->info;

        //txt
        if(saveInfo.total != 0){
            QString txtPath = QDir::currentPath() + "/log/" + shared->getDateTime_FILE() + "calibrate.txt";
            file.setFileName(txtPath);
            file.open(QIODevice::WriteOnly|QIODevice::Text);

            QTextStream out(&file);
            out << "TOL=" + QString::number(saveInfo.total) + "\n";
            for(int i=0;i<saveInfo.total;i++){
                out << "POA=" + QString::number(saveInfo.SN[i].A.x()) + "," + QString::number(saveInfo.SN[i].A.y()) + "\n";
                out << "POB=" + QString::number(saveInfo.SN[i].B.x()) + "," + QString::number(saveInfo.SN[i].B.y()) + "\n";
                out << "POC=" + QString::number(saveInfo.SN[i].C.x()) + "," + QString::number(saveInfo.SN[i].C.y()) + "\n";
                out << "POD=" + QString::number(saveInfo.SN[i].D.x()) + "," + QString::number(saveInfo.SN[i].D.y()) + "\n";
                out << "TXT=" + saveInfo.SN[i].text + "\n";
            }
            file.close();
        }

        //end
        this->isSaveRect = false;
    }

    isPush = false;
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (surface->isActive()) {
        surface->Drawing(true);
        QPainter painter;
        painter.begin(this); //實驗這樣把 .begin() .end() 寫出來效率會更快
        surface->paintImage(&painter);

        //偵測
        pen.setBrush(Qt::yellow);
        pen.setWidth(2);
        painter.setPen(pen);
        if(!isPush){
            isdraw = true;
            if(info.total != 0)
                for(int i=0;i<SCANTOTAL;i++){
                    //在畫上三角形
                    painter.drawLine(info.SN[i].A,info.SN[i].D);
                    painter.drawLine(info.SN[i].D,info.SN[i].C);
                    painter.drawLine(info.SN[i].C,info.SN[i].A);
                    //畫SN
                    painter.drawText(point_SN[i],info.SN[i].text);
                    //畫兩面積相減
                    if(saveInfo.total != 0)
                        painter.drawText(info.SN[i].A.x()-40,info.SN[i].A.y()+40,
                                         QString::number((int)AreaS(info.SN[i]) - (int)AreaS(saveInfo.SN[i])));
                }

            isdraw = false;
        }

        //draw saveRect
        if(saveInfo.total != 0 && !isPush){
            //畫上點
            pen.setBrush(Qt::red);
            pen.setWidth(6);
            painter.setPen(pen);
            for(int i=0;i<SCANTOTAL;i++){
                painter.drawPoint(saveInfo.SN[i].A);
                painter.drawPoint(saveInfo.SN[i].C);
                painter.drawPoint(saveInfo.SN[i].D);
            }
            //畫上三角形
            pen.setBrush(Qt::red);
            pen.setWidth(1);
            painter.setPen(pen);
            for(int i=0;i<SCANTOTAL;i++){
                painter.drawLine(saveInfo.SN[i].A,saveInfo.SN[i].D);
                painter.drawLine(saveInfo.SN[i].D,saveInfo.SN[i].C);
                painter.drawLine(saveInfo.SN[i].C,saveInfo.SN[i].A);
            }

            //offset
            if(info.total != 0){
                for(int i=0;i<SCANTOTAL;i++){
                    pen.setBrush(Qt::red);
                    pen.setWidth(2);
                    painter.setPen(pen);
                    painter.drawLine(saveInfo.SN[i].A,info.SN[i].A);
                    painter.drawLine(saveInfo.SN[i].C,info.SN[i].C);
                    painter.drawLine(saveInfo.SN[i].D,info.SN[i].D);
                    offsetA = (float)qSqrt(qPow(saveInfo.SN[i].A.x() - info.SN[i].A.x(),2) +
                                    qPow(saveInfo.SN[i].A.y() - info.SN[i].A.y(),2));
                    offsetC = (float)qSqrt(qPow(saveInfo.SN[i].C.x() - info.SN[i].C.x(),2) +
                                    qPow(saveInfo.SN[i].C.y() - info.SN[i].C.y(),2));
                    offsetD = (float)qSqrt(qPow(saveInfo.SN[i].D.x() - info.SN[i].D.x(),2) +
                                    qPow(saveInfo.SN[i].D.y() - info.SN[i].D.y(),2));
                    painter.drawText(saveInfo.SN[i].A.x(),saveInfo.SN[i].A.y()-10,QString::number(offsetA,'f',2));
                    painter.drawText(saveInfo.SN[i].C.x(),saveInfo.SN[i].C.y()+20,QString::number(offsetC,'f',2));
                    painter.drawText(saveInfo.SN[i].D.x(),saveInfo.SN[i].D.y()+20,QString::number(offsetD,'f',2));
                }
            }
        }

        painter.end();
        surface->Drawing(false);
    }
}
