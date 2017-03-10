#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QUdpSocket>
#include <QListWidgetItem>
#include <QDate>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(unsigned short p, QString& hFileNm, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addText(const QString &text);
    void addTextToHistory(const QString &text);
    void sysTrayActivate(QSystemTrayIcon::ActivationReason reason);
    void addNode(const QHostAddress& nodeAddress);

private slots:
    void clipboardChanged();
    void textActivated();
    void readDatagrams();

    void pollClipboard();
    void toggledAlwaysOnTop(bool check);
    void setStaysOnTop();
    void doSplitIndent();
    void doHide();
    void doSettingth();
    void doHistory();
    void doHistoryLoad();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
protected:
    void keyPressEvent ( QKeyEvent * event );
    void contextMenuEvent ( QContextMenuEvent * event );
    void doSettingthLoad();
private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    unsigned short port;
    QList<QHostAddress> nodes;
    QString currentText;
    QAction *m_observClip; /// наблюдать за буфером обмена
    QAction *m_alwaysOnTop; /// всегда наверху
    QAction *m_settingth; ///  настройки
    QAction *m_hideAct; ///  спрятать
    QAction *m_historyAct; ///  история
    QAction *m_exit; ///  Выход

    bool m_saveData;
    bool m_isHistoryView; /// режим просмотра истории
    QString m_historyViewFileNm; /// имя файла просмотра истории
    QString m_saveDataDir;
    QString m_saveDataFileName; /// Куда пишем
    QDate m_dt_today;
    QString m_idGeom; /// Идентификатор настроек геометрии.
    int m_minClipLenght; /// Количество символов меньше которого клип не сохраняется.

    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H
