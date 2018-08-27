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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydialog.h"
#include "music/process.h"

#include <QSettings>
#include <QDebug>
#include <QTranslator>
#include <QFileDialog>
#include <QDesktopWidget>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    uint32_t count;
    const musical_t *musical;
    bool ok;
    count = setting.value("count").toInt(&ok);
    qDebug() << "count:" << count;
    setting.setValue("count", QString::number(++count));
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->setupUi(this);
    setWindowTitle("MusicPlay");
    ui->pushButton_Send->setEnabled(false);
    ui->pushButton_Send->setVisible(false);
    _send = 0;
    _busy = 0;
    fw = new Firmware;
    ui->verticalSlider_volume->setValue(20);
    _send = Process::Code::VOL_GET;

    // 语言转换与菜单
    Language_cutover();
    create_menu();
    // 设置窗体大小
    ui->lineEdit_rtttl->setMinimumWidth(700);
    this->adjustSize();
    _width = this->width();
    _height = this->height();
    setFixedSize(this->width(), this->height());
    //ui->groupBox_download->setFixedSize(ui->groupBox_download->width(), ui->groupBox_download->height());
    //setFixedSize(this->width(), this->height());
    setWindowFlags(windowFlags() &~ Qt::WindowMaximizeButtonHint);
    // 使用随机数显示默认歌曲
    //musical = _music.get_musicals(7);
    musical = _music.get_musicals(get_random_number());
    //musical = _music.get_musicals(0);
    ui->lineEdit_rtttl->setText(QString::fromLocal8Bit(musical->rtttl));
    ui->lineEdit_melody->setText(QString::fromLocal8Bit(musical->melody));
    ui->lineEdit_octave->setText(QString::fromLocal8Bit(musical->octave));
    ui->lineEdit_durations->setText(QString::fromLocal8Bit(musical->durations));
    ui->label_file->setText(tr("default"));
    ui->label_info->setText("info");
    Analyze();
    // 移动窗口位置
    //const QRect deskRect = QApplication::desktop()->availableGeometry();
    const QRect deskRect = QApplication::desktop()->availableGeometry(this);
    //this->move(deskRect.left(), deskRect.top());
    this->move((deskRect.width()-_width)/2, (deskRect.height()-_height)/2);

    // 创建子线程
    _process = new ControllerProcess();
    connect(_process, SIGNAL(sig_progress_update(const QString &, int)), this, SLOT(slots_progress_update(const QString &, int)));
    _scanning = new ControllerScanning();
    connect(_scanning, SIGNAL(sig_Serial_Idle(const QString)), this, SLOT(slots_Serial_Idle(const QString)));
    //connect(_scanning, SIGNAL(sig_progress_update(const QString &, int)), this, SLOT(slots_progress_update(const QString &, int)));
    connect(_scanning, SIGNAL(sig_progress_update(const QString &, int)), this, SLOT(slots_scanning_update(const QString &, int)));
    _scanning->Serial_Free("COM6");
    _scanning->start();
    // 设置状态栏
    permanent = new QLabel(this);
    //permanent->setFrameShape(QFrame::WinPanel);
    //permanent->setFrameShadow(QFrame::Sunken);
    //permanent->setFrameStyle(QFrame::Box | QFrame::Sunken);
    permanent->setText(tr("<a href=\"http://github.com/Merafour\">github.com/Merafour</a>"));
    permanent->setTextFormat(Qt::RichText);
    permanent->setOpenExternalLinks(true);
    ui->statusBar->addPermanentWidget(permanent);
//    msgLabel = new QLabel(this);
//    ui->statusBar->addWidget(msgLabel);
    progress = new QProgressBar(this);
    progress->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 对齐方式
    qss_default = progress->styleSheet(); // 保存样式
    progress->setMinimumWidth(_width-permanent->width()-30);
    progress->setRange(0, 100);
    progress->setValue(0);
    ui->statusBar->addWidget(progress);
    statusBar()->setStyleSheet(QString("QStatusBar::item{border:0px}"));
    //statusBar()->setSizeGripEnabled(false);

    //msgLabel->setText("--MusicPlay--");
    //ui->statusBar->show();
    ui->statusBar->showMessage("MusicPlay", 3000);
    Scanning::scanning(1);
    progress->setFormat("ready");//
}

MainWindow::~MainWindow()
{
    Scanning::exit_thread();
    QThread::msleep(100);
    delete ui;
}

void MainWindow::actionLanguage_triggered()
{
    // 切换语言
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QAction *action = (QAction*)sender();
    QString actionText = action->text();
    setting.setValue(Setting_Language, actionText);
    qApp->exit(7714); // main 函数通过 7714 判断程序需要重启
}
// 菜单
void MainWindow::actionOperate_triggered()
{
    QAction *action = (QAction*)sender();
    QString objectName = action->objectName();
    qDebug()<<"objectName:"<<objectName<<endl;
    //if(0==objectName.compare("actionAbout"))
    if(0==objectName.compare(actionLoad->objectName()))
    {
        // load file
        open_music();
    }
    if(0==objectName.compare(actionSend->objectName()))
    {
        // send file
    }
    if(0==objectName.compare(actionNew->objectName()))
    {
        // new file
        ui->lineEdit_rtttl->setText("");
        ui->lineEdit_melody->setText("");
        ui->lineEdit_octave->setText("");
        ui->lineEdit_durations->setText("");
        ui->label_file->setText(tr(""));
        ui->label_info->setText("");
    }
    if(0==objectName.compare(actionSave->objectName()))
    {
        // save file
        save_music();
    }
    if(0==objectName.compare(actionAnalyze->objectName()))
    {
        // Analyze file
        Analyze();
    }
    if(0==objectName.compare(actionNext->objectName()))
    {
        const musical_t *musical;
        musical = _music.get_musicals_next();
        ui->lineEdit_rtttl->setText(QString::fromLocal8Bit(musical->rtttl));
        ui->lineEdit_melody->setText(QString::fromLocal8Bit(musical->melody));
        ui->lineEdit_octave->setText(QString::fromLocal8Bit(musical->octave));
        ui->lineEdit_durations->setText(QString::fromLocal8Bit(musical->durations));
        ui->label_file->setText(tr("default"));
        ui->label_info->setText("info");
        Analyze();
    }
    if(0==objectName.compare(actionLast->objectName()))
    {
        const musical_t *musical;
        musical = _music.get_musicals_last();
        ui->lineEdit_rtttl->setText(QString::fromLocal8Bit(musical->rtttl));
        ui->lineEdit_melody->setText(QString::fromLocal8Bit(musical->melody));
        ui->lineEdit_octave->setText(QString::fromLocal8Bit(musical->octave));
        ui->lineEdit_durations->setText(QString::fromLocal8Bit(musical->durations));
        ui->label_file->setText(tr("default"));
        ui->label_info->setText("info");
        Analyze();
    }
}
// 菜单
void MainWindow::actionHelp_triggered()
{
    QAction *action = (QAction*)sender();
    QString objectName = action->objectName();
    qDebug()<<"objectName:"<<objectName<<endl;
    //if(0==objectName.compare("actionAbout"))
    if(0==objectName.compare(actionAbout->objectName()))
    {
        MyDialog _about;
        _about.setWindowModality(Qt::ApplicationModal);
        //_about.show();
        _about.exec();
    }
}
// 创建菜单
void MainWindow::create_menu()
{
    ui->actionEnglish->setStatusTip(tr("Switch to English"));
    ui->actionSimplified_Chinese->setStatusTip(tr("Switch to Simplified Chinese"));
//    ui->actionEnglish->setShortcut(Qt::SHIFT | Qt::Key_E);
//    ui->actionSimplified_Chinese->setShortcut(Qt::SHIFT | Qt::Key_C);
    connect(ui->actionEnglish, SIGNAL(triggered()), this, SLOT(actionLanguage_triggered()));
    connect(ui->actionSimplified_Chinese, SIGNAL(triggered()), this, SLOT(actionLanguage_triggered()));
    // Menu_File
    actionNew = new QAction(tr("New"));
    actionNew->setObjectName(QStringLiteral("actionNew"));
    actionNew->setCheckable(false);
    actionNew->setStatusTip(tr("New MusicPlay file"));
    actionSave = new QAction(tr("Save"));
    actionSave->setObjectName(QStringLiteral("actionSave"));
    actionSave->setCheckable(false);
    actionSave->setStatusTip(tr("Save music to MusicPlay file"));
    actionLoad = new QAction(tr("Load"));
    actionLoad->setObjectName(QStringLiteral("actionLoad"));
    actionLoad->setCheckable(false);
    actionLoad->setStatusTip(tr("Load music file"));
    Menu_File = new QMenu(tr("File"));
    Menu_File->setObjectName(QStringLiteral("actionFile"));
    actionNew->setShortcuts(QKeySequence::New);
    actionSave->setShortcuts(QKeySequence::Save);
    actionLoad->setShortcuts(QKeySequence::Open);
    Menu_File->addAction(actionNew);
    Menu_File->addAction(actionSave);
    //Menu_File->addSeparator();
    Menu_File->addAction(actionLoad);
    ui->menuBar->addAction(Menu_File->menuAction());
    connect(actionNew, SIGNAL(triggered()), this, SLOT(actionOperate_triggered()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(actionOperate_triggered()));
    connect(actionLoad, SIGNAL(triggered()), this, SLOT(actionOperate_triggered()));
    // Menu_Operate
    actionAnalyze = new QAction(tr("Analyze"));
    actionAnalyze->setObjectName(QStringLiteral("actionAnalyze"));
    actionAnalyze->setCheckable(false);
    actionAnalyze->setStatusTip(tr("Analyze MusicPlay file"));
    actionSend = new QAction(tr("Send"));
    actionSend->setObjectName(QStringLiteral("actionSend"));
    actionSend->setCheckable(false);
    actionSend->setStatusTip(tr("Send music to MusicPlay"));
    actionNext = new QAction(tr("Next"));
    actionNext->setObjectName(QStringLiteral("actionNext"));
    actionNext->setCheckable(false);
    actionNext->setStatusTip(tr("Next example"));
    actionLast = new QAction(tr("Last"));
    actionLast->setObjectName(QStringLiteral("actionLast"));
    actionLast->setCheckable(false);
    actionLast->setStatusTip(tr("Last example"));
    Menu_Operate = new QMenu(tr("Operate"));
    Menu_Operate->setObjectName(QStringLiteral("actionOperate"));
    actionAnalyze->setShortcut(Qt::CTRL | Qt::Key_D);
    //actionLoad->setShortcut(Qt::ALT | Qt::Key_L);
    actionSend->setShortcut(Qt::ALT | Qt::Key_S);
    actionNext->setShortcut(Qt::ALT | Qt::Key_N);
    actionLast->setShortcut(Qt::ALT | Qt::Key_L);
    Menu_Operate->addAction(actionAnalyze);
    Menu_Operate->addAction(actionSend);
    Menu_Operate->addAction(actionNext);
    Menu_Operate->addAction(actionLast);
    ui->menuBar->addAction(Menu_Operate->menuAction());
    connect(actionAnalyze, SIGNAL(triggered()), this, SLOT(actionOperate_triggered()));
    connect(actionSend, SIGNAL(triggered()), this, SLOT(actionOperate_triggered()));
    connect(actionNext, SIGNAL(triggered()), this, SLOT(actionOperate_triggered()));
    connect(actionLast, SIGNAL(triggered()), this, SLOT(actionOperate_triggered()));
    // Menu_Help
    actionAbout = new QAction(tr("About"));
    actionAbout->setObjectName(QStringLiteral("actionAbout"));
    actionAbout->setCheckable(false);
    //actionAbout->setShortcuts(QKeySequence::AddTab);
    actionAbout->setShortcut(Qt::ALT | Qt::Key_A);
    actionAbout->setStatusTip(tr("About MusicPlay"));
    Menu_Help = new QMenu(tr("Help"));
    Menu_Help->setObjectName(QStringLiteral("actionHelp"));
    Menu_Help->addAction(actionAbout);
    ui->menuBar->addAction(Menu_Help->menuAction());
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(actionHelp_triggered()));
}
// 转换语言,需要生成语言转换文件 *.qm
void MainWindow::Language_cutover()
{
    QTranslator translator;
    bool load=false;
    (void)load;
    QString _language;
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    _language = setting.value(Setting_Language).toString();
    qDebug() << "Language: " << _language;
    if(_language.isEmpty())
    {
        //_language = "English";
        _language = ui->actionEnglish->text();
        setting.setValue(Setting_Language, _language);
    }
    //if(0==_language.compare("Simplified Chinese"))
    if(0==_language.compare(ui->actionSimplified_Chinese->text()))
    {
        load=translator.load("cn.qm");
        ui->actionEnglish->setChecked(false);
        ui->actionSimplified_Chinese->setChecked(true);
    }
    //else if (0==_language.compare("English"))
    else if (0==_language.compare(ui->actionEnglish->text()))
    {
        load=translator.load("en.qm");
        ui->actionEnglish->setChecked(true);
        ui->actionSimplified_Chinese->setChecked(false);
    }
    else // English
    {
        load=translator.load("en.qm");
        ui->actionEnglish->setChecked(true);
        ui->actionSimplified_Chinese->setChecked(false);
        _language = ui->actionEnglish->text();
        setting.setValue(Setting_Language, _language);
    }
    ui->menuLanguage->setEnabled(true);
    ui->menuLanguage->setVisible(true);
}

void MainWindow::open_music()
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
    QString lastPath = setting.value("LastFilePath").toString();
    //打开传输的文件
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),lastPath,tr("MusicPlay File(*.mp)"));
    if (fileName.isEmpty())
        return;
    setting.setValue("LastFilePath", fileName);
    ui->label_file->setText(fileName);
    _music.open_music(fileName.toLocal8Bit().constData());
    ui->lineEdit_rtttl->setText(QString::fromLocal8Bit(_music.wrtttl()));
    ui->lineEdit_melody->setText(QString::fromLocal8Bit(_music.wmelody()));
    ui->lineEdit_octave->setText(QString::fromLocal8Bit(_music.woctave()));
    ui->lineEdit_durations->setText(QString::fromLocal8Bit(_music.wdurations()));
    Analyze();
}

void MainWindow::save_music()
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
    QString lastPath = setting.value("LastFilePath").toString();
    //打开传输的文件
    fileName = QFileDialog::getSaveFileName(this, tr("Open File"),lastPath,tr("MusicPlay File(*.mp)"));
    if (fileName.isEmpty())
        return;
    setting.setValue("LastFilePath", fileName);
    ui->label_file->setText(fileName);

    _music.wrtttl(ui->lineEdit_rtttl->text().toLocal8Bit().constData());
    _music.wmelody(ui->lineEdit_melody->text().toLocal8Bit().constData());
    _music.woctave(ui->lineEdit_octave->text().toLocal8Bit().constData());
    _music.wdurations(ui->lineEdit_durations->text().toLocal8Bit().constData());
    _music.save_music(fileName.toLocal8Bit().constData());
}
// 解析 RTTTL 信息
void MainWindow::Analyze(void)
{
    uint8_t _oct;
    uint8_t _dur;
    uint16_t _bpm;
    uint16_t _wholenote;
    uint8_t _mode;
    const char* _rtttl;
    QString Text;
    _rtttl = ui->lineEdit_rtttl->text().toLocal8Bit().constData();
    _music.rtttl(_rtttl);
    _music.read_info(_oct, _dur, _bpm, _wholenote, _mode);
#if 1
    Text  = "Name:"+ QString::fromLocal8Bit(_music.name());
    Text += " octave:" + QString::number(_oct);
    Text += " durations:" + QString::number(_dur);
    Text += " bpm:" + QString::number(_bpm);
    Text += " wholenote:" + QString::number(_wholenote) + "ms";
    Text += " Mode:" + QString::fromLocal8Bit(_music.mode());
#else
    Text  = "歌名:"+ QString::fromLocal8Bit(_music.name());
    Text += " 音阶:" + QString::number(_oct);
    Text += " 一小jie节拍数:" + QString::number(_dur);
    Text += " 节拍数:" + QString::number(_bpm);
    Text += " 一节拍时长:" + QString::number(_wholenote) + "ms";
    Text += " 调式:" + QString::fromLocal8Bit(_music.mode());
#endif
    ui->label_info->setText(Text);
}
// 释放资源
int MainWindow::idle(QString COM)
{
    if(0==_COM.compare(COM))
    {
        progress->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 0);\n" "border-color: rgb(255, 255, 255);"));
        progress->setValue(0);
        return 0;
    }
    return -1;
}
// 操作设备
void MainWindow::send(QString COM)
{
    //_program->start(COM, fw);
    //_program->start("COM13");
    //_process->start(COM, ui->lineEdit_rtttl->text(), ui->lineEdit_melody->text(), ui->lineEdit_octave->text(), ui->lineEdit_durations->text());
    fw->ProcessFirmware(ui->lineEdit_rtttl->text(), ui->lineEdit_melody->text(), ui->lineEdit_octave->text(), ui->lineEdit_durations->text());
    //_process->start(COM, fw);
    _process->start(COM, fw, _send);
    qDebug()<<QThread::currentThreadId();
    //ui->groupBox_download->setTitle(COM+_Title);
    progress->setStyleSheet(qss_default);
    _busy = 1;
    _COM = COM;
}
// 控件操作
void MainWindow::on_pushButton_Load_clicked()
{
    open_music();
}

void MainWindow::on_pushButton_Save_clicked()
{
    save_music();
}
// 操作设备子线程消息处理
void MainWindow::slots_progress_update(const QString &titel, int value)
{
    switch (value) {
    case Process::Code::ERR:
        progress->setValue(0);
        progress->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n" "border-color: rgb(255, 255, 255);"));

        break;
    case Process::Code::DOWN:
        Scanning::Serial_Free(titel); // 释放串口资源
        _busy = 0;
        ui->pushButton_Send->setEnabled(true);
        ui->pushButton_Load->setEnabled(true);
        ui->pushButton_Save->setEnabled(true);
        ui->pushButton_Play->setEnabled(true);
        ui->pushButton_Pause->setEnabled(true);
        ui->menuLanguage->setEnabled(true);
        ui->menuBar->setEnabled(true);
        _send = 0;
        //Scanning::scanning(0);
        return;
        break;
    case Process::Code::TIME:
        progress->setFormat(progress->text()+titel);
        break;
    case Process::Code::VOL_GET:
        ui->label_volume->setText(QString::number(Music::g_volume));
        ui->verticalSlider_volume->setValue(Music::g_volume);
        break;
    default:
        //this->ui->label_watting->setText(titel);
        progress->setValue(value);
        //QString str = titel+QString::number(value)+"%";
        //ui->m_progressBar->setFormat(str);
        progress->setFormat(titel);
        break;
    }
}
// 设备扫描子线程消息处理
void MainWindow::slots_scanning_update(const QString &titel, int value)
{
    switch (value) {
    case Scanning::Code::PULL:
        qDebug()<< "pull out:" << titel;
//        progress->setStyleSheet(qss_default);
//        progress->setFormat(titel);
        idle(titel);
        break;
    case Scanning::Code::SCAN:
        progress->setFormat(titel);
        break;
    default:
        break;
    }
}
// 接收一个扫描到的设备并处理
void MainWindow::slots_Serial_Idle(const QString COM)
{
    //qDebug() << "Serial Idle: " << COM << "_send:" << QString::number(_send) << "busy:" << QString::number(busy());
    if(_send) // 需要对设备进行操作
    {
        if(0==busy())
        {
            send(COM);
            return;
        }
        _scanning->Serial_Free(COM);
    }
    else // 直接释放资源
    {
        _scanning->Serial_Free(COM);
    }
}
// UI 控件
void MainWindow::on_pushButton_Send_clicked()
{
#if 0
    if(_send)
    {
        //ui->pushButton_Send->setText(tr("Send"));
        ui->pushButton_Send->setEnabled(true);
        ui->pushButton_Load->setEnabled(true);
        ui->menuLanguage->setEnabled(true);
        _send = 0;
        Scanning::scanning(0);
        return;
    }
#endif
    //ui->pushButton_Send->setText(tr("STOP"));
    ui->pushButton_Send->setEnabled(false);
    ui->pushButton_Load->setEnabled(false);
    ui->pushButton_Save->setEnabled(false);
    ui->pushButton_Play->setEnabled(false);
    ui->pushButton_Pause->setEnabled(false);
    ui->menuLanguage->setEnabled(false);
    ui->menuBar->setEnabled(false);
    _send = 1;
    //Scanning::scanning(1);
    qDebug()<<QThread::currentThreadId();
}

void MainWindow::on_pushButton_Play_clicked()
{
    ui->pushButton_Send->setEnabled(false);
    ui->pushButton_Load->setEnabled(false);
    ui->pushButton_Save->setEnabled(false);
    ui->pushButton_Play->setEnabled(false);
    ui->pushButton_Pause->setEnabled(false);
    ui->menuLanguage->setEnabled(false);
    ui->menuBar->setEnabled(false);
    _send = Process::Code::PLAY;
    Scanning::scanning(1);
    qDebug()<<QThread::currentThreadId();
}

void MainWindow::on_pushButton_Pause_clicked()
{
    ui->pushButton_Send->setEnabled(false);
    ui->pushButton_Load->setEnabled(false);
    ui->pushButton_Save->setEnabled(false);
    ui->pushButton_Play->setEnabled(false);
    ui->pushButton_Pause->setEnabled(false);
    ui->menuLanguage->setEnabled(false);
    ui->menuBar->setEnabled(false);
    _send = Process::Code::PAUSE;
    Scanning::scanning(1);
    qDebug()<<QThread::currentThreadId();
}

void MainWindow::on_verticalSlider_volume_valueChanged(int value)
{
    //ui->label_volume->setText(QString::number(value));
}

void MainWindow::on_verticalSlider_volume_sliderMoved(int position)
{
    Music::g_volume = position;
    ui->label_volume->setText(QString::number(Music::g_volume));
    //_send = Process::Code::VOL_SET;
}

void MainWindow::on_verticalSlider_volume_sliderReleased()
{
    Music::g_volume = ui->verticalSlider_volume->value();
    ui->label_volume->setText(QString::number(Music::g_volume));
    _send = Process::Code::VOL_SET;
}
