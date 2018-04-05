#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    /*  FullScreen  */
    QDesktopWidget desktop;
    int hauteur = desktop.screenGeometry().height();
    int largeur = desktop.screenGeometry().width();
    w.resize(largeur, hauteur);

    w.show();

    return a.exec();
}
