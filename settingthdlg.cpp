#include "settingthdlg.h"
#include "ui_settingthdlg.h"
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

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
    ui->savedDataDir->setText(s.value("saveDataDir").toString());
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
    s.setValue("saveDataDir", ui->savedDataDir->text() );
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

void SettingthDlg::on_dirChoiceBtn_pressed()
{
    QString sDir = ui->savedDataDir->text();
    QString sCaption = QString::fromUtf8("Выберите директорию. (").append(sDir).append(")");
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this,
                                sCaption, //tr("QFileDialog::getExistingDirectory()"),
                                sDir,
                                options);
    if (!directory.isEmpty()){
        ui->savedDataDir->setText(directory);
    }
    //ui->dirChoiceBtn->released();
    //ui->dirChoiceBtn->setAutoRepeat(true);
}

void SettingthDlg::on_dirChoiceBtn_clicked()
{
    on_dirChoiceBtn_pressed();
}

void SettingthDlg::on_openDirBtn_clicked()
{
    QString sDir = ui->savedDataDir->text();
    if (sDir.isEmpty())
        return;
    QDesktopServices::openUrl(QUrl(sDir));
}
