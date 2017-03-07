#include "settingthdlg.h"
#include "ui_settingthdlg.h"
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QDir>

SettingthDlg::SettingthDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingthDlg)
{
    ui->setupUi(this);
    QSettings s;
    bool st = s.value("startWithOS").toBool();
    ui->startWithOS->setChecked(st);
    st = s.value("saveData").toBool();
    ui->saveData->setChecked(st);
    ui->savedDataDir->setText(s.value("savedDataDir").toString());
}

SettingthDlg::~SettingthDlg()
{
    delete ui;
}

void SettingthDlg::on_buttonBox_accepted()
{
    QSettings s;
    s.setValue("saveData", ui->saveData->isChecked() );
    s.setValue("startWithOS", ui->startWithOS->isChecked() );
    s.setValue("savedDataDir", ui->savedDataDir->text() );
    #ifdef Q_OS_WIN32
        QString appName = qApp->applicationName();
        //qDebug() << appName;
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        if (ui->startWithOS->isChecked()) {
            settings.setValue(appName, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
            settings.sync();
        } else {
            settings.remove(appName);
        }
    #endif


}
