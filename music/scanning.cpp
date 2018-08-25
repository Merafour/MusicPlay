/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 22/8/2018
* Description        : MusicPlay 设备扫描子线程.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "scanning.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "mploader.h"
#include "user_port.h"
#include <QDateTime>

QStringList Scanning::port_list;
QStringList Scanning::pull_list;
QMutex Scanning::mutex;
uint8_t Scanning::_is_run = 0;
uint8_t Scanning::_overload = 0;
uint8_t Scanning::_stop = 0;
uint8_t Scanning::_scanning = 0;
uint8_t Scanning::_exit = 0;

Scanning::Scanning(QObject *parent) : QObject(parent)
{
}
/*
 * 释放设备资源函数
*/
void Scanning::Serial_Free(const QString COM)
{
    qDebug()<<"Free2 tmp ="<< COM;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        //qDebug()<<"Free tmp ="<< tmp;
        if(0==COM.compare(tmp))
        {
            port_list.removeOne(COM);
            break;
        }
    }
#if 0
    pull_list << COM; // 需等待设备拔出
#else
    pull_list.clear(); // 不等待设备拔出直接释放资源
#endif
    mutex.unlock();
}
/*
 * 设备扫描程序主循环
*/
void Scanning::doWork()
{
    int count;
    int i=0;
    QString port;
    QString pull;
    uint deadline =QDateTime::currentDateTime().toTime_t();
    uint time =QDateTime::currentDateTime().toTime_t();
    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    my_port = new user_port();
    pull_port = new user_port();
    // 清空 list
    port_list.clear();
    port_list << "NULL";
    pull_list.clear();
    //pull_list << "NULL";
    _is_run = 1;
    qDebug() << "_overload : " << _overload;
    // 扫描线程禁止多次创建
    if(_overload>0) return ;
    _overload = 1;
    count = 0;
    qDebug() << "new mploader ";
    up = new mploader(this);
    up->setting(my_port, "");
    deadline =QDateTime::currentDateTime().toTime_t();
    connect(pull_port, &user_port::errorOccurred, pull_port, &user_port::errorOccurred_slots);
    // 获取同步,扫描设备
    while (0==_stop)
    {
        count++;
        // 检测设备拔出
        mutex.lock();
        if(false == pull.isEmpty())
        {
            if(0!=pull_port->stop()) // 设备拔出
            {
                //emit
                emit sig_progress_update(pull, Scanning::Code::PULL);
                pull_list.removeOne(pull);
                pull="";
            }
        }
        //mutex.lock();
        //if(false == pull.isEmpty()) pull_list << pull;
        if(pull_list.size()>0)
        {
            pull = pull_list.at(0);
            // 移动 list,改变下一个扫描的设备
            pull_list.removeOne(pull);
            pull_list << pull;
            pull_port->OpenPortDefault(pull);
        }
        mutex.unlock();
        _is_run = 0;
        QThread::msleep(100); // 10Hz
        // 扫描
        _is_run = 1;
        if(1!=_scanning)
        {
            emit sig_progress_update(tr("watting... ")+QString::number(QDateTime::currentDateTime().toTime_t()-deadline), Scanning::Code::SCAN);
            time =QDateTime::currentDateTime().toTime_t();
            continue;
        }
        if(1==_exit) break;
        deadline =QDateTime::currentDateTime().toTime_t();
        // 后去系统设备列表逐个设备获取同步
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            port = info.portName();
            //if(0!=info.description().compare("USB-SERIAL CH340")) continue;
            if(0==port.compare("COM1")) continue;
            if(serach_port(port)) continue;
            if(0!=my_port->OpenPortDefault(port)) continue;
            connect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
            qDebug() << port << "Scanning...";
            emit sig_progress_update(tr("Scanning Device... ")+port, Scanning::Code::SCAN);
            for(i=0; i<5; i++)
            {
                if(0!=up->__scan()) break;
            }
            if(0!=up->__scan())
            {
                // 同步失败
                disconnect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
                my_port->close();
                continue;
            }
            disconnect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
            send_idle(port);
            my_port->close();
        }
    }
    disconnect(pull_port, &user_port::errorOccurred, pull_port, &user_port::errorOccurred_slots);
    if(1==_exit) exit(0);
}

void Scanning::recSerialData()
{
    char datas[128];
    qint64 len=0;
    qint64 count=0;
    //Worker_count++;
    len = my_port->read(datas, sizeof(datas));
    qDebug("recv[%3d]: ", len);
   for(count=0; count<len; count++)
   {
       qDebug("%02X ", datas[count]);
   }
   qDebug("\n");
}
/*
 * 释放设备资源函数,使用信号槽机制
*/
void Scanning::slots_Serial_Free(const QString COM)
{
    //qDebug()<<"Free tmp ="<< COM;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        //qDebug()<<"Free tmp ="<< tmp;
        if(0==COM.compare(tmp))
        {
            port_list.removeOne(COM);
            break;
        }
    }
    pull_list << COM;
    mutex.unlock();
}
/*
 * 发送一个扫描到的设备给主线程
*/
void Scanning::send_idle(QString port)
{
    int serach=0;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        //qDebug()<<"tmp ="<< tmp;
        if(0==port.compare(tmp))
        {
            serach = 1;
            break;
        }
    }
    if(0==serach)
    {
        port_list << port;
        emit sig_Serial_Idle(port);
    }
    mutex.unlock();
}
/*
 * 搜索当前 port是否被使用
*/
int Scanning::serach_port(QString &port)
{
    int serach=0;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        if(0==port.compare(tmp))
        {
            serach = 1;
            break;
        }
    }
    for(int i = 0; i< pull_list.size();++i)
    {
        QString tmp = pull_list.at(i);
        if(0==port.compare(tmp))
        {
            serach = 1;
            break;
        }
    }
    mutex.unlock();
    return serach;
}
