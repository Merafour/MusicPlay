/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 22/8/2018
* Description        : MusicPlay 设备通讯类.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "mploader.h"
#include <QDebug>
#include <QThread>
#include <stdio.h>
#include "process.h"

mploader::mploader(QObject *parent) : QObject(parent)
{
    board_type = 0;
    board_rev = 0;
    bl_rev = 0;
    fw_maxsize = 0;
}

void mploader::recSerialData()
{
   QByteArray ba;
   ba = my_port->_readAll();
   uint32_t count=0;
   const char* datas = ba.constData();
   int len = ba.size();
   for(count=0; count<len; count++)
   {
       recv_data[recv_len+count] = datas[count];
       //qDebug("%02X ", datas[count]);
   }
   recv_len += count;
}

int mploader::setting(user_port *_port, QString _filename)
{
    my_port = _port;
    //connect(my_port, SIGNAL(readyRead()), this, SLOT(recSerialData())); // 连接串口收到数据事件与读取数据函数
    return 0;
}

int mploader::identify()
{
    //if(0==self->__sync()) return 0;
    if(0!=__sync())
    {
        return -1;
    }
    bl_rev=__getInfo(Info::BL_REV);

     board_type = __getInfo(Info::BOARD_ID);
     board_rev = __getInfo(Info::BOARD_REV);
     fw_maxsize = __getInfo(Info::FLASH_SIZE);
    return 0;
}

int mploader::currentChecksum(Firmware *_fw)
{
    char str[128];
    uint8_t buf[2];
    buf[0] = Code::GET_CRC;
    buf[1] = Code::EOC;
    //int expect_crc = _fw->crc(fw_maxsize);
    int expect_crc = _fw->crc32(fw_maxsize);
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(3000); // 3s
    int report_crc = __recv_int();
    __getSync();

    qDebug("FW File 0x%X, %d" , expect_crc , expect_crc);
    qDebug("Current 0x%X, %d" , report_crc , report_crc);
    memset(str, 0, sizeof(str));
    snprintf(str, sizeof(str)-1, "FW File 0x%X, %d Current 0x%X, %d" , expect_crc , expect_crc , report_crc , report_crc);
    emit sig_progress_update(str, 0);
    qDebug(str);
    if (expect_crc == report_crc)
    {
         qDebug("Same Firmware. Not uploading");
         return 1;
    }
    return 0;
}

int mploader::upload(const Firmware *_fw)
{
    if (fw_maxsize < _fw->image_size())
    {
        //throw new Exception("Firmware image is too large for this board");
        emit sig_progress_update(tr("Firmware image is too large for this board"), 0);
        return -1;
    }

    qDebug("erase...");
#if 0
    __erase();
#else
    if(0!=__erase())
    {
        emit sig_progress_update(tr("ERASE FAIL!"), 0);
        emit sig_progress_update("erase", Process::Code::ERR);
        return -1;
    }
#endif
    qDebug("program...");
    __program(_fw);
    if(0!=__program_sync)
    {
        emit sig_progress_update("program", Process::Code::ERR);
        return -1;
    }

    qDebug("verify...");
    //currentChecksum((Firmware *)_fw);
    if(0!=__verify_v3((Firmware *)_fw))
    {
        emit sig_progress_update("verify", Process::Code::ERR);
        return -1;
    }

    //__standby();
    return 0;
}

int mploader::music()
{
    uint8_t buf[2];
    buf[0] = Code::MUSIC;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(100);
    return __getSync();
}

int mploader::play()
{
    uint8_t buf[2];
    buf[0] = Code::PLAY;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(100);
    return __getSync();
}

int mploader::pause()
{
    uint8_t buf[2];
    buf[0] = Code::PAUSE;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(100);
    return __getSync();
}

int mploader::next()
{
    uint8_t buf[2];
    buf[0] = Code::NEXT;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(100);
    return __getSync();
}

int mploader::last()
{
    uint8_t buf[2];
    buf[0] = Code::LAST;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(100);
    return __getSync();
}

int mploader::set_volume(uint8_t _vol)
{
    uint8_t buf[3];
    buf[0] = Code::VOL_SET;
    buf[1] = _vol;
    buf[2] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, sizeof(buf));
    have_data(100);
    return __getSync();
}

int mploader::get_volume(uint8_t &_vol)
{
    uint8_t buf[2];
    buf[0] = Code::VOL_GET;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(100);
    _vol = __recv();
    return __getSync();
}

int mploader::__recv_int()
{
#if 0
    uint8_t raw[4];
    memset(raw, 0, sizeof(raw));
    __recv(raw, 4);
    int val = (raw[0]&0xFF) | (raw[1]&0xFF<<8) | (raw[2]&0xFF<<16) | (raw[3]&0xFF<<24);
    return val;
#else
    uint8_t raw[4];
    memset(raw, 0, sizeof(raw));
    __recv(raw, 4);
    //int val = (raw[0]&0xFF) | (raw[1]&0xFF<<8) | (raw[2]&0xFF<<16) | (raw[3]&0xFF<<24);
    uint32_t val=0;
    val = raw[3]&0xFF;
    val <<= 8; val |= raw[2]&0xFF;
    val <<= 8; val |= raw[1]&0xFF;
    val <<= 8; val |= raw[0]&0xFF;
    return (int)val;
#endif
}

int mploader::__getSync()
{
    //qDebug("Sync[%3d]: %02X %02X %02X %02X %02X %02X", recv_len, recv_data[0], recv_data[1], recv_data[2], recv_data[3], recv_data[4], recv_data[5]);
    uint8_t c = __recv();
    if (c != (uint8_t)Code::INSYNC)
    {
        qDebug("unexpected {%0X} instead of INSYNC", c);
        return -1;
    }
    c = __recv();
    if (c == (uint8_t)Code::INVALID)
    {
        qDebug("bootloader reports INVALID OPERATION:%0X", c);
        return -1;
    }
    if (c == (uint8_t)Code::FAILED)
    {
        qDebug("bootloader reports OPERATION FAILED:%0X", c);
        return -1;
    }
    if (c != (uint8_t)Code::OK)
    {
        qDebug("unexpected {%0X} instead of OK", c);
        return -1;
    }
    return 0;
}

int mploader::__sync()
{
    uint8_t buf[2];
    buf[0] = Code::GET_SYNC;
    //buf[0] = Code::GET_CRC;
    buf[1] = Code::EOC;
    //my_port->clear();
//    buffer.clear();
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //QThread::msleep(1);
    have_data(100);
    //have_data(1);
    return __getSync();
}

int mploader::__scan()
{
    uint8_t buf[2];
    buf[0] = Code::GET_SYNC;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //QThread::msleep(1);
    //have_data(100);
    have_data(10);
    uint8_t insync = __recv();
    uint8_t ok = __recv();
    //return __getSync();
    if(((uint8_t)Code::INSYNC == insync) && ((uint8_t)Code::OK == ok)) return 0;
    return -1;
}

int mploader::__getInfo(mploader::Info param)
{
    uint8_t buf[3];
    buf[0] = Code::GET_DEVICE;
    buf[1] = param;
    buf[2] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 3);
    //QThread::msleep(1500);
    int info = __recv_int();
    __getSync();
    qDebug("info:%0X %d", info, info);
    return info;
}
#include <QDateTime>

int mploader::__erase()
{
    uint32_t count=0;
    //QDateTime current_date_time =QDateTime::currentDateTime();
    uint deadline =QDateTime::currentDateTime().toTime_t()+20;
    uint8_t buf[2];
    buf[0] = Code::CHIP_ERASE;
    buf[1] = Code::EOC;
    __sync();

    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //DateTime deadline = DateTime.Now.AddSeconds(20);
    //while (DateTime.Now < deadline)
    count=0;
    deadline =QDateTime::currentDateTime().toTime_t()+20;
    while (QDateTime::currentDateTime().toTime_t() < deadline)
    {
        count++;
        //emit sig_progress_update("erase..."+count, 0);
#if 0
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str)-1, "erase... %d", count);
        emit sig_progress_update(str, 0);
#else
        emit sig_progress_update(tr("erase...")+QString::number(count), 0);
#endif
        QThread::msleep(100);
        if (my_port->waitForReadyRead(10))
        {
            qDebug("__erase btr %d", my_port->size());
            break;
        }
        if(0!=my_port->stop()) break;
        //qDebug("Time: %d", QDateTime::currentDateTime().toTime_t());
    }
    recSerialData();
    return __getSync();
}

void mploader::__program_multi(const uint8_t data[], const uint32_t len)
{
#if 0
    uint8_t buf[2];
    buf[0] = Code::PROG_MULTI;
    buf[1] = len;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    __send(data, len);
    __send(Code::EOC);
#else
    uint8_t buf[PROG_MULTI_MAX+10];
    memset(buf, 0, sizeof(buf));
    buf[0] = Code::PROG_MULTI;
    buf[1] = len;
    memcpy(&buf[2], data,  len);
    buf[2+len] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2+len+1);
#endif
    have_data(100); // 100ms
    have_data(10);  // delay 10ms, 纯延时,减慢下载速度,使人可以看清楚下载进度的更新
    __program_sync = __getSync();
}

void mploader::__program(const Firmware *fw)
{
    const uint8_t* code = fw->image();
    const uint32_t groups=fw->image_size()/PROG_MULTI_MAX + 1;
    uint32_t _size=0;
    uint32_t count=0;
    _size=fw->image_size();
    count=0;
    __program_sync = 0;
    qDebug("Program {%d}/{%d} %d", count, groups, __program_sync);
    for(count=0; count<groups; count++)
    {
        //emit sig_progress_update("program..."+QString::number(__program_sync), ((count)*100)/groups);
        emit sig_progress_update(tr("program... ")+QString::number(((count)*100)/groups)+"%", ((count)*100)/groups);
        _size -= PROG_MULTI_MAX;
        if(_size>PROG_MULTI_MAX) __program_multi(&code[count*PROG_MULTI_MAX], PROG_MULTI_MAX);
        else __program_multi(&code[count*PROG_MULTI_MAX], _size);

        //qDebug("Program {%d}/{%d} %d", count, groups, __program_sync);
        if(0!=__program_sync) return;
        QThread::msleep(1);
    }
    emit sig_progress_update(tr("Download done ")+QString::number(((count)*100)/groups)+"%", ((count)*100)/groups);
}

int mploader::__verify_v3(Firmware *_fw)
{
    uint8_t buf[2];
    buf[0] = Code::GET_CRC;
    buf[1] = Code::EOC;
    int expect_crc = _fw->crc(fw_maxsize);
    QThread::msleep(1);
    __send(buf, 2);
    have_data(3000); // 3s
    int report_crc = __recv_int();
    __getSync();

    qDebug() << "Expected 0x" << QString::number(expect_crc, 16) << expect_crc;
    qDebug() << "Got      0x" << QString::number(report_crc, 16) << report_crc;
    if (report_crc != expect_crc)
    {
        //throw new Exception("Program CRC failed");
        emit sig_progress_update("Program CRC failed", 100);
        return -1;
    }
    return 0;
}

void mploader::__reboot()
{
    uint8_t buf[2];
    buf[0] = Code::REBOOT;
    buf[1] = Code::EOC;
    __send(buf, 2);
    my_port->flush();
    //QThread::msleep(10);
    have_data(300); // 300ms
    __getSync();
}

void mploader::__standby()
{
    uint8_t buf[2];
    buf[0] = Code::STANDBY;
    buf[1] = Code::EOC;
    __send(buf, 2);
    my_port->flush();
    //QThread::msleep(10);
    have_data(300); // 300ms
    __getSync();
}

//void mploader::__send(const uint8_t c)
//{
//    my_port->_write((const char *)&c, 1);
//}

void mploader::__send(const uint8_t buf[], const uint32_t len)
{
    my_port->_write((const char *)buf, len);
}


