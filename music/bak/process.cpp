#include "process.h"
#include "uploader.h"

#include <QDateTime>

//uint8_t Process::_mavlink = 0;
uint8_t Process::_exit = 0;
//enum Process::Bl_Mode Process::_bl_mode = Process::Bl_Mode::USB;

Process::Process(QObject *parent) : QObject(parent)
{

}
static uint8_t music_buffer[1024*1024]; // 1MB
void Process::doWork(const QString parameter, const QString rtttl, const QString melody, const QString octave, const QString durations)
{
    //QByteArray tempdata;
//    user_port *_port;
    //char data[16];
    //const uint32_t timeout = 20*30; // 30s
    //uint32_t do_count;
    uint8_t _down=0;
    uint32_t len=0;
    uint32_t pos=0;
    const char* text=NULL;
    QString port;
    QString str;
    _port = new user_port();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");

    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    qDebug() << current_date ;
    qDebug() << "rtttl     : " << rtttl;
    qDebug() << "melody    : " << melody;
    qDebug() << "octave    : " << octave;
    qDebug() << "durations : " << durations;
    up = new mploader(this);
    up->setting(_port, "");
    connect(up, SIGNAL(sig_progress_update(QString,int)), this, SLOT(slots_progress_update(QString,int)));
    //do_count = 0;
    uint time_start =QDateTime::currentDateTime().toTime_t();
    uint timeout =QDateTime::currentDateTime().toTime_t()+30;
    _down=0;
    while(QDateTime::currentDateTime().toTime_t()<timeout)
    {
        QThread::msleep(50);
        //do_count++;
        qDebug() << "connect device";
        if(1==_exit) break;
        {
            qDebug() << "open device" << parameter;
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
            qDebug("Found board type %d boardrev %d bl rev %d fwmax %d on %s", up->board_type, up->board_rev,up->bl_rev,up->fw_maxsize, port.toLocal8Bit().constData());
            if(0!=_port->stop()) break;
            //up->upload(fw);
            memset(music_buffer, 0, sizeof(music_buffer));
            pos=0;
            text = rtttl.toLocal8Bit().constData();
            len = strlen(text);
            memcpy(&music_buffer[pos], text, len);
            pos += len;
            music_buffer[pos++] = '\n';
            text = melody.toLocal8Bit().constData();
            len = strlen(text);
            memcpy(&music_buffer[pos], text, len);
            pos += len;
            music_buffer[pos++] = '\n';
            text = octave.toLocal8Bit().constData();
            len = strlen(text);
            memcpy(&music_buffer[pos], text, len);
            pos += len;
            music_buffer[pos++] = '\n';
            text = durations.toLocal8Bit().constData();
            len = strlen(text);
            memcpy(&music_buffer[pos], text, len);
            pos += len;
            music_buffer[pos++] = '\n';
            up->upload(music_buffer, pos);
            disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            _port->close();
            //goto ret;
            //do_count = timeout+100;
            _down = 1;
            break;
        }
    }
    //QThread::msleep(3000);
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
void Process::recSerialData()
{
#if 0
    QByteArray ba;
    ba = my_port->readAll();
    qDebug()<<"recSerialData";
    qDebug()<<"recSerialData:" << ba.data();
    qDebug("recSerialData: %d", ba.size());
    //qDebug("recSerialData: %s", ba.constData());
#else
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
#endif
}
