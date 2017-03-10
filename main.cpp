#include <QApplication>
#include <QSystemTrayIcon>
#include <QSettings>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("QClipboardSaver");
    a.setApplicationVersion("0.0.07");
    a.setOrganizationName("trdmsoft");
    a.setOrganizationDomain("www.voltacom.ru");
    QString hFileName = "";
    QString argId = "";

    // select port
    unsigned short port = 34123;
    for(int i = 1; i < argc - 1; i+=2) {
        argId = a.arguments().at(i);
        if(argId == "-p" || argId == "--port") {
            port = a.arguments().at(i + 1).toShort();
        }
        if(argId == "-h" || argId == "--hfile") {
            hFileName = a.arguments().at(i + 1);
        }
    }

    MainWindow w(port,hFileName);

    // add nodes to send to
    for(int i = 1; i < argc - 1; i+=2) {
        if(a.arguments().at(i) == "-n" || a.arguments().at(i) == "--node") {
            w.addNode(QHostAddress(a.arguments().at(i + 1)));
        }
    }

    QSystemTrayIcon sysTray(QIcon(":/images/clipboard"), &a);
    w.connect(&sysTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), &w, SLOT(sysTrayActivate(QSystemTrayIcon::ActivationReason)));
    if (hFileName.isEmpty()) {
        sysTray.show();
    } else {
        w.show();
        a.setQuitOnLastWindowClosed(true);
    }

    return a.exec();
}
