#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QUdpSocket>
#include <QListWidgetItem>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(unsigned short p, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addText(const QString &text);
    void sysTrayActivate(QSystemTrayIcon::ActivationReason reason);
    void addNode(const QHostAddress& nodeAddress);

private slots:
    void clipboardChanged();
    void textActivated();
    void readDatagrams();

    void pollClipboard();
    void toggledAlwaysOnTop(bool check);
    void setStaysOnTop();
    void doHide();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
protected:
    void keyPressEvent ( QKeyEvent * event );
private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    unsigned short port;
    QList<QHostAddress> nodes;
    QString currentText;
    QAction *m_observClip; /// наблюдать за буфером обмена
    QAction *m_alwaysOnTop; /// всегда наверху

    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H
