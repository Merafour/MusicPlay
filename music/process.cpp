/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 22/8/2018
* Description        : MusicPlay 设备操作子线程.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "Process.h"
#include "mploader.h"
#include "music/music.h"
#include <QDateTime>

uint8_t Process::_exit = 0;

Process::Process(QObject *parent) : QObject(parent)
{

}
/*
 * 该接口不使用,忽略
*/
void Process::doWork(const QString parameter, const Firmware *fw)
{
    uint8_t _down=0;
    QString port;
    QString str;
    _port = new user_port();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");

    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    qDebug("Loaded firmware for %d,%d waiting for the bootloader...", fw->board_id, fw->board_revision);
    qDebug() << current_date ;
    up = new mploader(this);
    up->setting(_port, "");
    connect(up, &mploader::sig_progress_update, this, &Process::slots_progress_update);
    //do_count = 0;
    uint time_start =QDateTime::currentDateTime().toTime_t();
    uint timeout =QDateTime::currentDateTime().toTime_t()+30;
    _down=0;
    while(QDateTime::currentDateTime().toTime_t()<timeout)
    {
        QThread::msleep(50);
        //do_count++;
        if(1==_exit) break;
        {
            port = parameter;
            current_date_time =QDateTime::currentDateTime();
            current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
            qDebug() << current_date << " Watting connect " << port << "sec:" << (QDateTime::currentDateTime().toTime_t()-time_start);
            str = " Watting connect " + port + "sec:" + QString::number(QDateTime::currentDateTime().toTime_t()-time_start);
            //emit sig_progress_update("Trying Port "+port, 0);
            emit sig_progress_update(str, 0);
            if(0!=_port->OpenPortDefault(port)) continue;
            connect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            if(0!=up->identify())
            {
                current_date_time =QDateTime::currentDateTime();
                current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
                qDebug() << current_date << "Not There..";
                if(0!=_port->stop()) break;
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                continue;
            }
            if(0!=_port->stop()) break;
            qDebug("Found board type %d boardrev %d bl rev %d fwmax %d on %s", up->board_type, up->board_rev,up->bl_rev,up->fw_maxsize, port.toLocal8Bit().constData());
            if(0!=up->currentChecksum((Firmware *)fw))
            {
                qDebug("No need to upload. already on the board");
                emit sig_progress_update(tr("No need to upload. already on the board"), 0);
                //up->__reboot();
                QThread::msleep(2);
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                //continue;
                //goto ret;
                //do_count = timeout+100;
                _down = 1;
                break;
            }
            up->upload(fw);
            up->music();
            up->play();
            disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            _port->close();
            //goto ret;
            //do_count = timeout+100;
            _down = 1;
            break;
        }
    }
    sig_progress_update(port, Code::DOWN);
    //if((timeout<=do_count) && (do_count<(timeout+100))) emit sig_progress_update("Device connect timeout", 0);
    if(0==_down) emit sig_progress_update("Device connect timeout", 0);
    delete up;
    delete _port;
    uint time_end =QDateTime::currentDateTime().toTime_t();
    uint _time = time_end-time_start;
    sig_progress_update(" Time:"+QString::number(_time)+" s", Code::TIME);
    if(1==_exit) exit(0);
}

/*
 * 处理与设备通讯的子线程
 * parameter:串口号
 * fw:歌曲数据
 * cmd:操作指令
*/
void Process::slots_doWork(const QString parameter, const Firmware *fw, int cmd)
{
    uint8_t _down=0;
    QString port;
    QString str;
    _port = new user_port();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");

    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    qDebug("Loaded firmware for %d,%d waiting for the bootloader...", fw->board_id, fw->board_revision);
    qDebug() << current_date ;
    up = new mploader(this);
    up->setting(_port, "");
    connect(up, &mploader::sig_progress_update, this, &Process::slots_progress_update);
    //do_count = 0;
    uint time_start =QDateTime::currentDateTime().toTime_t();
    uint timeout =QDateTime::currentDateTime().toTime_t()+30;
    _down=0;
    // 循环扫描设备
    while(QDateTime::currentDateTime().toTime_t()<timeout)
    {
        QThread::msleep(50);
        if(1==_exit) break;
        {
            // 打开设备
            port = parameter;
            current_date_time =QDateTime::currentDateTime();
            current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
            qDebug() << current_date << " Watting connect " << port << "sec:" << (QDateTime::currentDateTime().toTime_t()-time_start);
            str = " Watting connect " + port + "sec:" + QString::number(QDateTime::currentDateTime().toTime_t()-time_start);
            //emit sig_progress_update("Trying Port "+port, 0);
            emit sig_progress_update(str, 0);
            if(0!=_port->OpenPortDefault(port)) continue;
            connect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            // 检测设备
            if(0!=up->identify())
            {
                current_date_time =QDateTime::currentDateTime();
                current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
                qDebug() << current_date << "Not There..";
                if(0!=_port->stop()) break;
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                continue;
            }
            if(0!=_port->stop()) break;
            // 操作设备
            switch (cmd) {
            case Process::Code::PLAY:
                qDebug("Found board type %d boardrev %d bl rev %d fwmax %d on %s", up->board_type, up->board_rev,up->bl_rev,up->fw_maxsize, port.toLocal8Bit().constData());
                if(0!=up->currentChecksum((Firmware *)fw))
                {
                    qDebug("No need to upload. already on the board");
                    emit sig_progress_update(tr("No need to upload. already on the board"), 0);
                    up->music();
                    up->play();
                    QThread::msleep(2);
                    disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                    _port->close();
                    _down = 1;
                    break;
                }
                up->upload(fw);
                up->music();
                up->play();
                break;
            case Process::Code::PAUSE:
                up->pause();
                break;
            case Process::Code::VOL_GET:
                if(0==up->get_volume(Music::g_volume))
                {
                    sig_progress_update(port, Code::VOL_GET);
                }
                break;
            case Process::Code::VOL_SET:
                up->set_volume(Music::g_volume);
                break;
            default:
                break;
            }
            disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            _port->close();
            //goto ret;
            //do_count = timeout+100;
            _down = 1;
            // 操作完成,结束循环
            break;
        }
    }
    // 发送操作结束信号
    sig_progress_update(port, Code::DOWN);
    //if((timeout<=do_count) && (do_count<(timeout+100))) emit sig_progress_update("Device connect timeout", 0);
    if(0==_down) emit sig_progress_update("Device connect timeout", 0);
    delete up;
    delete _port;
    uint time_end =QDateTime::currentDateTime().toTime_t();
    uint _time = time_end-time_start;
    sig_progress_update(" Time:"+QString::number(_time)+" s", Code::TIME);
    if(1==_exit) exit(0);
}
/*
 * 接收串口数据,不使用该接口,直接使用设备操作类 mploader
*/
void Process::recSerialData()
{
    char datas[128];
    qint64 len=0;
    qint64 count=0;
    Worker_count++;
    len = _port->read(datas, sizeof(datas));
    qDebug("recv[%3d]: ", Worker_count);
   for(count=0; count<len; count++)
   {
       qDebug("%02X ", datas[count]);
   }
   qDebug("\n");
}
