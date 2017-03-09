#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingthdlg.h"

#include <QtGui/QClipboard>
#include <QMimeData>
#include <QCloseEvent>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>


MainWindow::MainWindow(unsigned short p, QString &hFileNm, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    socket(0),
    port(p)
{
    ui->setupUi(this);
    m_isHistoryView = !hFileNm.isEmpty();
    m_historyViewFileNm = hFileNm;

    // busted in Qt5
    //connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged()));
    connect(ui->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(textActivated()));
    QString sTitle = "QClipSaver";
    currentText = ""; //QApplication::clipboard()->text();
    if (!m_isHistoryView){
        QTimer *timer = new QTimer(this);
        timer->setInterval(700);
        connect(timer, SIGNAL(timeout()), this, SLOT(pollClipboard()));
        timer->start();

        socket = new QUdpSocket(this);
        socket->bind(QHostAddress::Any, port);

        connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
    } else {
        sTitle.append(" histori view: ").append(m_historyViewFileNm);
    }
    setWindowTitle(sTitle); //    ui->centralWidget->setWindowTitle(sTitle);


    QSettings s;
    if(s.contains("geom")) {
        QByteArray geom = s.value("geom").toByteArray();
        restoreGeometry(geom);
    }
    ui->mainToolBar->setIconSize(QSize(16, 16));
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    if (!m_isHistoryView){
        ui->listWidget->setMouseTracking(true);
        m_observClip = new QAction(QString::fromUtf8("Наблюдать"), this);
        ui->mainToolBar->addAction(m_observClip);
        m_observClip->setCheckable(true);
        m_observClip->setChecked(true);
        m_observClip->setShortcut(QKeySequence("Ctrl+O"));
        m_observClip->setToolTip(QString::fromUtf8("Наблюдать (Ctrl+O)"));
    }

    m_alwaysOnTop = new QAction(QString::fromUtf8("Всегда наверху"), this); /// всегда наверху
    ui->mainToolBar->addAction(m_alwaysOnTop);
    m_alwaysOnTop->setCheckable(true); //   m_alwaysOnTop->setChecked(true);
    m_alwaysOnTop->setShortcut(QKeySequence("Ctrl+T"));
    m_alwaysOnTop->setToolTip(QString::fromUtf8("Всегда наверху (Ctrl+T)"));

    connect(m_alwaysOnTop, SIGNAL(toggled(bool)), this, SLOT(toggledAlwaysOnTop(bool)));
    if (!m_isHistoryView) {
        m_settingth = new QAction(QString::fromUtf8("Настройки"), this); /// всегда наверху
        #ifdef Q_OS_WIN32
            m_settingth->setIcon(QIcon(":/images/settings"));
            m_settingth->setIconText(QString::fromUtf8("Настройки"));
            ui->mainToolBar->addAction(m_settingth);
            connect(m_settingth, SIGNAL(triggered(bool)), this, SLOT(doSettingth()));
        #endif


        m_hideAct = new QAction(QString::fromUtf8("Скрыть"), this);
        ui->mainToolBar->addAction(m_hideAct);
        connect(m_hideAct, SIGNAL(triggered(bool)), this, SLOT( doHide() ));

        m_historyAct = new QAction(QString::fromUtf8("История"), this);
        ui->mainToolBar->addAction(m_historyAct);
        connect(m_historyAct, SIGNAL(triggered(bool)), this, SLOT( doHistory() ));
    }
    m_exit = new QAction(QString::fromUtf8("Выход"), this);
    ui->mainToolBar->addAction(m_exit);
    connect(m_exit, SIGNAL(triggered(bool)), qApp, SLOT( quit() ));

    m_saveData = false;
    m_saveDataDir = "";
    m_saveDataFileName = "";
    if (!m_isHistoryView) {
        doSettingthLoad();
    } else {
        doSettingthLoad();
        doHistoryLoad();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    delete socket;
}

void MainWindow::toggledAlwaysOnTop(bool check)
{
    setStaysOnTop();
}

void MainWindow::setStaysOnTop()
{
    Qt::WindowFlags flags = windowFlags();
    bool sot = m_alwaysOnTop->isChecked();
    if (sot) {
        flags |= Qt::WindowStaysOnTopHint;
        setWindowFlags(flags);
    } else {
        flags ^= Qt::WindowStaysOnTopHint;
        setWindowFlags(flags);
    }
    show(); // Вот тут и пряталось ОНО!! show_ыв
}

void MainWindow::doSplitIndent()
{
    if (ui->listWidget->count() == 0)
        return;
    QListWidgetItem *it = ui->listWidget->currentItem();
    if (it){
        QString vText = it->text();


    }
}

/// Будем скрывать только тогда, когда не стайонтоп
void MainWindow::doHide()
{
    bool sot = m_alwaysOnTop->isChecked();
    if (!sot)
        hide();
   if (m_isHistoryView)
       qApp->quit();
}

void MainWindow::doSettingth()
{
    SettingthDlg* sDlg = new SettingthDlg(this);
    sDlg->exec();    
    sDlg->deleteLater();
    doSettingthLoad();
}

void MainWindow::doHistory()
{
    ///\todo - тут надо сделать считывание файлов из директории m_saveDataDir
    if(m_saveDataDir.isEmpty())
        return;

    QDir dir(m_saveDataDir);


    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Выберите файл истории (*.hdat)"),
                                m_saveDataDir,
                                tr("Text Files (*.hdat)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty()) {
        QString sUrl = qApp->applicationFilePath();
//        sUrl.append(" -h \"").append(fileName).append("\"");
//        qDebug() << "sUrl" << sUrl;
//        sUrl = sUrl.replace("/","\\");
//        QDesktopServices::openUrl(QUrl(sUrl));

        sUrl = "\"";
        sUrl.append(fileName).append("\"");
        QString program = qApp->applicationFilePath();
        program = program.replace("/","\\");
        sUrl = sUrl.replace("/","\\");

        QStringList arguments;
        arguments << "-h" << sUrl;

        QProcess::startDetached(program, arguments);

        //QProcess myProcess;        myProcess.start(program, arguments);
    }


}


void MainWindow::sysTrayActivate(QSystemTrayIcon::ActivationReason reason) {
    qDebug() << "Activate reason:" << reason;
    if(reason == QSystemTrayIcon::MiddleClick /*|| reason == QSystemTrayIcon::DoubleClick*/) {
        qApp->quit();
    } else {
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::addNode(const QHostAddress& nodeAddress) {
    nodes.append(nodeAddress);
}

void MainWindow::addText(const QString &text) {
    if(ui->listWidget->count() > 0 && text == ui->listWidget->item(0)->text())
        return;
    bool alreadyExist = false;
    QList<QListWidgetItem *> matches = ui->listWidget->findItems(text, Qt::MatchCaseSensitive);
    foreach(QListWidgetItem *i, matches) {
        alreadyExist = true;
        delete i;
    }

    ui->listWidget->insertItem(0, text);
    QListWidgetItem *it = ui->listWidget->item(0);//    it->setData(Qt::DisplayRole, text.mid(0, 300));
    it->setSizeHint(QSize(600,16)); //    it->setStatusTip(text);
    it->setToolTip(text);

    foreach(QHostAddress addy, nodes) {
        socket->writeDatagram(text.toUtf8(), addy, port);
    }
    if (!alreadyExist)
        addTextToHistory(text);
}

void MainWindow::addTextToHistory(const QString &text)
{
    if (!m_saveData)
        return;
    QFile hFile(m_saveDataFileName);
    QDataStream stream(&hFile);
    stream.setVersion(QDataStream::Qt_4_2);

//    hFile.open( QIODevice::WriteOnly | QIODevice::Append);
    hFile.open( QIODevice::ReadWrite | QIODevice::Append);
    QDateTime dt_today = QDateTime::currentDateTime();
    //stream << dt_today << text;
    stream << text;
    hFile.flush();
    hFile.close();
}

void MainWindow::doHistoryLoad()
{
    // -h "C:\Users\Settingth\QClipSaver\histori2017_03.dat"
    if (m_historyViewFileNm.isEmpty())
        return;
    QFile hFile(m_historyViewFileNm);
    QDataStream stream(&hFile);
    stream.setVersion(QDataStream::Qt_4_2);

    bool res = hFile.open( QIODevice::ReadOnly);
    QDateTime hist_dt;
    QString hist_text;
    QStringList li;

    while (!stream.atEnd()) {
       //stream >> hist_dt >> hist_text;        //ui->listWidget->addItem(hist_text);
       stream >> hist_text;        //ui->listWidget->addItem(hist_text);
       if (li.contains(hist_text))
           continue;
       QListWidgetItem *it = new QListWidgetItem(hist_text, ui->listWidget);
       it->setSizeHint(QSize(600,16));
       it->setToolTip(hist_text);

    }
    hFile.close();
}

void MainWindow::clipboardChanged() {
    qDebug() << "Clipboard changed";
    if(qApp->clipboard()->mimeData()->hasText()) {
        addText(qApp->clipboard()->mimeData()->text());
    }
}

void MainWindow::closeEvent(QCloseEvent *e) {
    e->ignore();
    QSettings s;
    s.setValue("geom", this->saveGeometry());
    doHide();
}

void MainWindow::textActivated() {
    qApp->clipboard()->setText(ui->listWidget->currentItem()->text());
    QSettings s;
    s.setValue("geom", this->saveGeometry());
    doHide();
}

void MainWindow::readDatagrams() {
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        qApp->clipboard()->setText(QString::fromUtf8(datagram.data()));
    }
}

void MainWindow::pollClipboard()
{
    if(QApplication::clipboard()->text() != currentText) {
        if (m_dt_today != QDate::currentDate()){
            m_dt_today = QDate::currentDate();
            doSettingthLoad();
        }
        currentText = QApplication::clipboard()->text();
        if (m_observClip->isChecked())
            clipboardChanged();
    }
}


void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current) {
        QString txt = current->text();
        ui->textEdit->setPlainText(txt);
    }

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() ==  Qt::Key_Escape) {
        doHide();
    }
    if (ui->listWidget->hasFocus()){
        switch (event->key()) {
        case Qt::Key_Delete:
            {
                int cRowCnt, cRow = ui->listWidget->currentRow();
                if (cRow>=0){
                    delete ui->listWidget->takeItem(cRow);
                }
                cRowCnt = ui->listWidget->count();
                if (cRowCnt>0){
                    if (cRow<=cRowCnt){
                        ui->listWidget->setCurrentRow(cRow);
                    } else {
                        ui->listWidget->setCurrentRow(cRowCnt);
                    }
                }
            }
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            {
                int cRowCnt, cRow = ui->listWidget->currentRow();
                if (cRow>=0){
                    textActivated();
                }
            }
            break;
        default:
            break;
        }
    }

}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}

void MainWindow::doSettingthLoad()
{
    QSettings s;
    m_saveData = s.value("saveData").toBool();
    m_saveDataDir = s.value("saveDataDir").toString();
    m_saveDataFileName = "";
    if (m_saveData){
        m_dt_today = QDate::currentDate();
        m_saveDataFileName = m_saveDataDir; // m_saveDataFileName.append(QDir::separator());
        m_saveDataFileName.append("/histori").append(m_dt_today.toString("yyyy_MM")).append(".hdat");
    }
    //qDebug() << "m_saveDataFileName" << m_saveDataFileName;
}
