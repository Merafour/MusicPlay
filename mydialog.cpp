/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 22/8/2018
* Description        : 关于MusicPlay.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "mydialog.h"
#include "ui_mydialog.h"

#define _BL_REVR							0x02
#define _BL_REVX							0x00
#define _BL_REVY							0x0A

MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    char time[256];
    ui->setupUi(this);
    //ui->buttonBox->setVisible(false);
    //setWindowTitle("About");
    setWindowTitle("MusicPlay");
    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    setWindowFlags(Qt::WindowCloseButtonHint);
    //ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    //ui->groupBox_version->setTitle("MusicPlay");
    ui->groupBox_version->setTitle("merafour.blog.163.com");
    //ui->label_version->setText("  Version: V1.12.0");
    //ui->label_version->setText("  Version: V "+QString::number(_BL_REVR) +"."+QString::number(_BL_REVX) +"."+QString::number(_BL_REVY) +" ");
    ui->label_version->setText(version());
    memset(time, 0, sizeof(time));
    snprintf(time, sizeof(time)-1, "  %s %s", __DATE__, __TIME__);
    ui->label_time->setText(time);
    ui->textBrowser_version->setEnabled(false);
    ui->textBrowser_version->setText("支持功能：");
    ui->textBrowser_version->append("1.中英文切换；");
    ui->textBrowser_version->append("2.歌曲编辑；");
    ui->textBrowser_version->append("3.通过串口(USB串口)发送歌曲到 MusicPlay设备；");
    adjustSize();
    setFixedSize(this->width(), this->height());
}

MyDialog::~MyDialog()
{
    delete ui;
}

QString MyDialog::version()
{
    QString _version = "  Version: V "+QString::number(_BL_REVR) +"."+QString::number(_BL_REVX) +"."+QString::number(_BL_REVY) +" ";
    return _version;
}
