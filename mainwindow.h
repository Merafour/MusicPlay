/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 22/8/2018
* Description        : MusicPlay UI.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QTime>
#include "music/music.h"
#include "music/scanning.h"
#include "music/process.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    // 菜单
    void actionLanguage_triggered();
    void actionOperate_triggered();
    void actionHelp_triggered();
    // 控件
    void on_pushButton_Load_clicked();

    void on_pushButton_Save_clicked();

    void on_pushButton_Send_clicked();

    void on_pushButton_Play_clicked();

    void on_pushButton_Pause_clicked();


    // 设备操作
    void slots_progress_update(const QString &titel, int value);
    void slots_scanning_update(const QString &titel, int value);
    void slots_Serial_Idle(const QString COM);

private:
    // 绘制菜单即操作函数
    void create_menu(void);
    void Language_cutover(void);

    void open_music(void);
    void save_music(void);
    void Analyze(void);
    int get_random_number(void)
    {
        int number = 0;
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        number = qrand();
        return number;
    }

    uint8_t busy(void)
    {
        return _busy;
    }
    int idle(QString COM);
    void send(QString COM);

    Ui::MainWindow *ui;

    // 定义菜单
    QMenu *Menu_Operate;
    QAction *actionSend;
    QAction *actionAnalyze;
    QAction *actionNext;
    QAction *actionLast;
    QMenu *Menu_File;
    QAction *actionNew;
    QAction *actionSave;
    QAction *actionLoad;
    QMenu *Menu_Help;
    QAction *actionAbout;

    // 状态栏控件
    QLabel* msgLabel;
    QLabel* permanent;
    QProgressBar* progress;

    // setting
#define Setting_Language     "Language"

    // 数据
    QString fileName;
    Music _music;
    int _width;
    int _height;
    int _send;
    uint8_t _busy;
    QString _COM;
    QString qss_default;

    Firmware *fw;
    ControllerScanning *_scanning;
    ControllerProcess *_process;
};

#endif // MAINWINDOW_H
