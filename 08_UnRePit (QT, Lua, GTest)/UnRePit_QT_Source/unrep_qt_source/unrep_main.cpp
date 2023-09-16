#include "unrep_winmain.h"

#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    unrep_winmain w;
    w.show();
    return a.exec();
}
