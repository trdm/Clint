#ifndef SETTINGTHDLG_H
#define SETTINGTHDLG_H

#include <QDialog>

namespace Ui {
class SettingthDlg;
}

class SettingthDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SettingthDlg(QWidget *parent = 0);
    ~SettingthDlg();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SettingthDlg *ui;
};

#endif // SETTINGTHDLG_H
