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
#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "user_port.h"
#include "mploader.h"

class Process : public QObject
{
    Q_OBJECT
public:
    enum Code
    {
        // response codes
        DOWN = 0x0180,
        CRC  = 0x0181,
        SCAN = 0x0182,
        TIME = 0x0183,
        ERR  = 0x0184,
        PULL = 0x0185,
        PLAY  = 0x0186,
        PAUSE = 0x0187,
        NEXT  = 0x0188,
        LAST  = 0x0189,
    };
    explicit Process(QObject *parent = nullptr);
    static void exit_thread(void) { _exit = 1;}

public slots:
    void doWork(const QString parameter, const Firmware *fw) ;
    void slots_doWork(const QString parameter, const Firmware *fw, int cmd) ;
    void recSerialData();
    void slots_progress_update(const QString &titel, int value)
    {
        emit sig_progress_update(titel, value);
    }

signals:
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
private:
    user_port *_port;
    uint32_t Worker_count;
    mploader *up;
    static uint8_t _exit;
};

class ControllerProcess : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    ControllerProcess() {
        Process *worker = new Process;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &ControllerProcess::operate, worker, &Process::doWork);
        connect(this, SIGNAL(sig_operate(QString,const Firmware*,int)), worker, SLOT(slots_doWork(QString,const Firmware*,int)) );
        connect(worker, &Process::resultReady, this, &ControllerProcess::handleResults);
        connect(worker, &Process::sig_progress_update, this, &ControllerProcess::slots_progress_update);
        workerThread.start();
    }
    ~ControllerProcess() {
        workerThread.quit();
        workerThread.wait();
    }
    void start(const QString COM, const Firmware *fw)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        operate(COM, fw);
    }
    void start(const QString COM, const Firmware *fw, int cmd)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        sig_operate(COM, fw, cmd);
    }
public slots:
    void handleResults(int value)
    {
        //qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        //qDebug() << "handleResults : " << value;
        emit resultReady(value);
    }
    void slots_progress_update(const QString &titel, int value)
    {
        emit sig_progress_update(titel, value);
    }
signals:
    void operate(const QString, const Firmware *fw);
    void sig_operate(const QString, const Firmware *fw, int cmd);
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
};

#endif // PROCESS_H
