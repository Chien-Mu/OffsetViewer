#ifndef CONFIG_H
#define CONFIG_H

#include <QWidget>
#include <QValidator>
#include <QDir>

namespace Ui {
class Config;
}

class Config : public QWidget
{
    Q_OBJECT

public:
    explicit Config(QWidget *parent = 0);
    ~Config();

    //scanner
    int get_length_SN();
    int get_delay_loop();
    int get_delay_dmtx();

public slots:
    void closeWindow();
    void setConfig(); //寫入檔案

private:
    Ui::Config *ui;
    QDir dir;
    QFile file;
    QValidator *validInt;
    void setUI();
    void getConfig(); //讀取檔案

signals:
    void applying();
};

#endif // CONFIG_H
