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
    };
//    enum Bl_Mode{
//        USB = 0x00,
//        WIFI = 0x01,
//        MSC = 0x02,
//        Serial = 0x03,
//    };
    explicit Process(QObject *parent = nullptr);
//    static void set_bl_mode(enum Bl_Mode mode)
//    {
//        _bl_mode = mode;
//    }
//    static enum Bl_Mode get_bl_mode(void)
//    {
//        return _bl_mode;
//    }
    static void exit_thread(void) { _exit = 1;}

public slots:
    void doWork(const QString parameter, const QString rtttl, const QString melody, const QString octave, const QString durations) ;
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
//    static enum Bl_Mode _bl_mode;
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
        connect(worker, &Process::resultReady, this, &ControllerProcess::handleResults);
        connect(worker, &Process::sig_progress_update, this, &ControllerProcess::slots_progress_update);
        workerThread.start();
    }
    ~ControllerProcess() {
        workerThread.quit();
        workerThread.wait();
    }
    void start(const QString COM, const QString rtttl, const QString melody, const QString octave, const QString durations)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        operate(COM, rtttl, melody, octave, durations);
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
    void operate(const QString, const QString rtttl, const QString melody, const QString octave, const QString durations);
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
};

#endif // PROCESS_H
