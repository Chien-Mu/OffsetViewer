#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{

#if defined (RASPBERRY)
    if(argc == 1){  //使用者無特別指定下才設定 xcb
        argv[1] = (char *)"-platform";
        argv[2] = (char *)"xcb";
        argc = 3;
    }
#endif

    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
