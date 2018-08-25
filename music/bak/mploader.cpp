#include "mploader.h"
#include <QDebug>
#include <QThread>
#include <stdio.h>
#include "program.h"

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
//    qDebug()<<"mploader recSerialData";
//    qDebug()<<"recSerialData:" << ba.data();
//    qDebug("recSerialData: %d", ba.size());
   //qDebug("recSerialData: %s", ba.constData());
   uint32_t count=0;
   const char* datas = ba.constData();
   int len = ba.size();
   //memset(recv_data, 0, sizeof(recv_data));
   for(count=0; count<len; count++)
   {
       recv_data[recv_len+count] = datas[count];
       //qDebug("%02X ", datas[count]);
   }
   //qDebug("recv[%3d]: %02X %02X %02X %02X %02X %02X", recv_len, recv_data[recv_len+0], recv_data[recv_len+1], recv_data[recv_len+2], recv_data[recv_len+3]\
           , recv_data[recv_len+4], recv_data[recv_len+5]);
   recv_len += count;
   //recv_rindex = 0;
#if 0
   qDebug("recv[%3d]: ", ba.size());
   for(count=0; count<len; count++)
   {
       qDebug("%02X ", (uint8_t)datas[count]);
   }
   qDebug("\n");
#endif

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
//    bl_rev=__getInfo(Info::BL_REV);
    // Console.WriteLine("2 " + DateTime.Now.Millisecond);
//     if ((bl_rev < (int)BL_REV_MIN) || (bl_rev > (int)BL_REV_MAX))
//     {
//         throw new Exception("Bootloader protocol mismatch");
//     }

     board_type = __getInfo(Info::BOARD_ID);
     board_rev = __getInfo(Info::BOARD_REV);
     fw_maxsize = __getInfo(Info::FLASH_SIZE);
    return 0;
}

uint32_t mploader::crc32(const uint8_t *src, const uint32_t len)
{
    uint32_t sum=0;
    uint32_t data=0;
    uint32_t i=0;
    const uint8_t left[4]={0, 8, 16, 24};

    sum=0;
    for (unsigned i = 0; i < len; i++) {
        data = src[i]<<left[i&0x3];
        sum += data;
    }

    return sum;
}

//int mploader::currentChecksum(Firmware *_fw)
//{
//    return 0;
//}

//int mploader::upload(const Firmware *_fw)
//{
//    return 0;
//}
// command:		PROTO_MUSIC/<crc32:4>/<len:2>/<data:len>/EOC
// success reply:	INSYNC/OK
// invalid reply:	INSYNC/INVALID
// readback failure:	INSYNC/FAILURE
int mploader::upload(const uint8_t *music, const uint16_t len)
{
//    uint32_t crc=0;
//    uint8_t head[7];
//    uint8_t eoc=0;
//    uint16_t pos=0;
//    crc=crc32(music, len);
//    memset(head, 0, sizeof(head));
//    //memset(music_buffer, 0, sizeof(music_buffer));
//    head[0] = PROTO_MUSIC;
//    // <crc32:4>
//    head[1] = (crc>>0)&0xFF;
//    head[2] = (crc>>8)&0xFF;
//    head[3] = (crc>>16)&0xFF;
//    head[4] = (crc>>24)&0xFF;
//    // <len:2>
//    head[5] = (len>>0)&0xFF;
//    head[6] = (len>>8)&0xFF;
//    eoc=EOC;
//    __send(head, sizeof(head));
//    for(pos=0; (pos+64)<len; pos+=64)
//    {
//        __send(&music[pos], 64);
//        emit sig_progress_update(tr("program... ")+QString::number(((pos)*100)/len)+"%", ((pos)*100)/len);
//        QThread::msleep(500);
//    }
//    if(pos<len)
//    {
//        __send(&music[pos], len-pos);
//    }
//    pos = len;
//    emit sig_progress_update(tr("program... ")+QString::number(((pos)*100)/len)+"%", ((pos)*100)/len);
//    __send(&eoc, sizeof(eoc));
//    __getSync();
    __program(music, len);
    return 0;
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
    //val = (val<<8) | raw[2]&0xFF;
//    val = (val<<8) | raw[1]&0xFF;
//    val = (val<<8) | raw[0]&0xFF;
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
    __program_sync = __getSync();
}

void mploader::__program(const uint8_t *music, const uint16_t len)
{
    const uint8_t* code = music;
    const uint32_t groups=len/PROG_MULTI_MAX + 1;
    uint32_t _size=0;
    uint32_t count=0;
    _size=len;
    count=0;
    __program_sync = 0;
    qDebug("Program {%d}/{%d} %d", count, groups, __program_sync);
    for(count=0; count<groups; count++)
    //for(count=0; count<10; count++)
    {
        //emit sig_progress_update("program..."+QString::number(__program_sync), ((count)*100)/groups);
        emit sig_progress_update(tr("program... ")+QString::number(((count)*100)/groups)+"%", ((count)*100)/groups);
        _size -= PROG_MULTI_MAX;
        if(_size>PROG_MULTI_MAX) __program_multi(&code[count*PROG_MULTI_MAX], PROG_MULTI_MAX);
        else __program_multi(&code[count*PROG_MULTI_MAX], _size);

        //qDebug("Program {%d}/{%d} %d", count, groups, __program_sync);
        if(0!=__program_sync) return;
        QThread::msleep(1000);
    }
    //emit sig_progress_update("Download done", ((count)*100)/groups);
    emit sig_progress_update(tr("Download done ")+QString::number(((count)*100)/groups)+"%", ((count)*100)/groups);
//    List<byte[]> groups = self.__split_len(code, PROG_MULTI_MAX);
//    Console.WriteLine("Programing packet total: "+groups.Count);
//    int a = 1;
//    foreach (Byte[] bytes in groups)
//    {
//        self.__program_multi(bytes);

//        Console.WriteLine("Program {0}/{1}",a, groups.Count);

//        a++;
//        if (ProgressEvent != null)
//            ProgressEvent((a / (float)groups.Count) * 100.0);
    //    }
}

//void mploader::__send(const uint8_t c)
//{
//    my_port->_write((const char *)&c, 1);
//}

void mploader::__send(const uint8_t buf[], const uint32_t len)
{
    my_port->_write((const char *)buf, len);
}









