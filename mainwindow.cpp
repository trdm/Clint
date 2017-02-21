#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGui/QClipboard>
#include <QMimeData>
#include <QCloseEvent>
#include <QDebug>
#include <QSettings>
#include <QTimer>


MainWindow::MainWindow(unsigned short p, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    socket(0),
    port(p)
{
    ui->setupUi(this);

    // busted in Qt5
    //connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged()));
    connect(ui->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(textActivated()));

    currentText = ""; //QApplication::clipboard()->text();

    QTimer *timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer, SIGNAL(timeout()), this, SLOT(pollClipboard()));
    timer->start();

    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, port);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));

    QSettings s;
    if(s.contains("geom")) {
        QByteArray geom = s.value("geom").toByteArray();
        restoreGeometry(geom);
    }
    ui->listWidget->setMouseTracking(true);
    m_observClip = new QAction(QString::fromUtf8("Наблюдать"), this);
    ui->mainToolBar->addAction(m_observClip);
    m_observClip->setCheckable(true);
    m_observClip->setChecked(true);
    m_observClip->setShortcut(QKeySequence("Ctrl+O"));
    m_observClip->setToolTip(QString::fromUtf8("Наблюдать (Ctrl+O)"));

    m_alwaysOnTop = new QAction(QString::fromUtf8("Всегда наверху"), this); /// всегда наверху
    ui->mainToolBar->addAction(m_alwaysOnTop);
    m_alwaysOnTop->setCheckable(true); //   m_alwaysOnTop->setChecked(true);
    m_alwaysOnTop->setShortcut(QKeySequence("Ctrl+T"));
    m_alwaysOnTop->setToolTip(QString::fromUtf8("Всегда наверху (Ctrl+T)"));

    connect(m_alwaysOnTop, SIGNAL(toggled(bool)), this, SLOT(toggledAlwaysOnTop(bool)));

//    m_splitIndent = new QAction(QString::fromUtf8("BB->B b"), this); /// всегда наверху
//    ui->mainToolBar->addAction(m_splitIndent); //   m_splitIndent->setCheckable(true);

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

    QList<QListWidgetItem *> matches = ui->listWidget->findItems(text, Qt::MatchCaseSensitive);
    foreach(QListWidgetItem *i, matches) {
        delete i;
    }

    ui->listWidget->insertItem(0, text);
    QListWidgetItem *it = ui->listWidget->item(0);
//    it->setData(Qt::DisplayRole, text.mid(0, 300));
    it->setSizeHint(QSize(600,16));
//    it->setStatusTip(text);
    it->setToolTip(text);

    foreach(QHostAddress addy, nodes) {
        socket->writeDatagram(text.toUtf8(), addy, port);
    }
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
