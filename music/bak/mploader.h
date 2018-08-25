#ifndef MPLOADER_H
#define MPLOADER_H

#include "user_port.h"
#include <QString>
#include <QObject>
#include <QThread>

//#include"Firmware.h"

/*
#define PROTO_INSYNC				0x12    // 'in sync' byte sent before status
#define PROTO_EOC					0x50    // end of command
// Reply bytes
#define PROTO_OK					0x10    // INSYNC/OK      - 'ok' response
#define PROTO_FAILED				0x11    // INSYNC/FAILED  - 'fail' response
#define PROTO_INVALID				0x13	// INSYNC/INVALID - 'invalid' response for bad commands
#define PROTO_BAD_SILICON_REV 		0x14 	// On the F4 series there is an issue with < Rev 3 silicon
// Command bytes
#define PROTO_GET_SYNC				0x21    // NOP for re-establishing sync
#define PROTO_DEBUG					0x31    // emit debug information - format not defined
#define PROTO_MUSIC 				0x60    // music data
//#define PROTO_RTTTL 				0x60    // rtttl
//#define PROTO_MELODY 				0x61    // melody
//#define PROTO_OCTAVE 				0x62    // octave
//#define PROTO_DURATIONS 			0x63    // durations
#define PROTO_PLAY   				0x64    // play
#define PROTO_PAUSE 				0x65    // pause
#define PROTO_NEXT   				0x66    // next play
#define PROTO_LAST   				0x67    // last play
*/
class mploader : public QObject
{
    Q_OBJECT

public:
    //mploader(){ self = this; }
    explicit mploader(QObject *parent = nullptr);
//    ~mploader();

//private slots:
public slots:

    virtual void recSerialData();
signals:
    void sig_progress_update(const QString &titel, int value);
public:
    enum Code
    {
        // response codes
        NOP = 0x00,
        OK = 0x10,
        FAILED = 0x11,
        INSYNC = 0x12,
        INVALID = 0x13,//	# rev3+

        // protocol commands
        EOC = 0x50,
        GET_SYNC = 0x21,
        GET_DEVICE = 0x22,	// returns DEVICE_ID and FREQ bytes
        CHIP_ERASE = 0x23,
        CHIP_VERIFY = 0x24,//# rev2 only
        PROG_MULTI = 0x27,
        READ_MULTI = 0x28,//# rev2 only
        GET_CRC = 0x29,//	# rev3+
        GET_OTP = 0x2a, // read a byte from OTP at the given address
        GET_SN = 0x2b,    // read a word from UDID area ( Serial)  at the given address
        GET_CHIP = 0x2c, // read chip version (MCU IDCODE)
//        PROTO_SET_DELAY	= 0x2d, // set minimum boot delay
//        REBOOT = 0x30,
//        STANDBY = 0x32,
        PROTO_MUSIC = 0x60,    // music data
        PROTO_PLAY  = 0x64,    // play
        PROTO_PAUSE = 0x65,    // pause
        PROTO_NEXT  = 0x66,    // next play
        PROTO_LAST  = 0x67,    // last play
    };

    enum Info {
        BL_REV = 1,//	# bootloader protocol revision
        BOARD_ID = 2,//	# board type
        BOARD_REV = 3,//	# board revision
        FLASH_SIZE = 4,//	# max firmware size in bytes
        VEC_AREA = 5
    };

    const uint8_t BL_REV_MIN = 2;//	# minimum supported bootloader protocol
    const uint8_t BL_REV_MAX = 5;//	# maximum supported bootloader protocol
//    const uint8_t PROG_MULTI_MAX = 60;//		# protocol max is 255, must be multiple of 4
//    const uint8_t READ_MULTI_MAX = 60;//		# protocol max is 255, something overflows with >= 64
//    const uint8_t PROG_MULTI_MAX = 200;//		# protocol max is 255, must be multiple of 4
//    const uint8_t READ_MULTI_MAX = 200;//		# protocol max is 255, something overflows with >= 64
    const uint8_t PROG_MULTI_MAX = 200;//		# protocol max is 255, must be multiple of 4
    const uint8_t READ_MULTI_MAX = 200;//		# protocol max is 255, something overflows with >= 64


public:
    int setting(user_port *_port, QString _filename);
    int identify(void);
    uint32_t crc32(const uint8_t *src, const uint32_t len);
//    int currentChecksum(Firmware *_fw);
//    int upload(const Firmware *_fw);
    int upload(const uint8_t *music, const uint16_t len);

    virtual int __recv(uint8_t buf[], const int count = 2)
    {
        int len=0;
        if(my_port->waitForReadyRead(1)) recSerialData();
        if(0==recv_rindex)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        if(recv_rindex>=recv_len)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        for(len=0; len<count; len++)
        {
            buf[len] = (uint8_t)recv_data[recv_rindex++];
        }
        return 0;
    }
    virtual uint8_t __recv(void)
    {
#if 0
        uint8_t c=0;
        __recv(&c);
        return c;
#else
        if(my_port->waitForReadyRead(1)) recSerialData();
        if(0==recv_rindex)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        if(recv_rindex>=recv_len)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        return (uint8_t)recv_data[recv_rindex++];
#endif
    }
//    int have_data(void)
//    {
//        //return recv_len;
//        while(!my_port->waitForReadyRead(1)) ;
//        recSerialData();
//        return my_port->size();
//    }
    int have_data(const uint32_t timeout)
    {
        //return recv_len;
        //while(!my_port->waitForReadyRead(10)) ;
        uint32_t _time;
#if 0
        for(_time=0; _time<timeout; _time+=10)
        {
            if(my_port->waitForReadyRead(10)) break;
        }
#else
        for(_time=0; _time<timeout; _time++)
        {
            if(my_port->waitForReadyRead(1)) break;
            if(0!=my_port->stop()) break;
        }
#endif
        recSerialData();
        return my_port->size();
    }
    virtual void clear(void)
    {
        my_port->clear();
        memset(recv_data, 0, sizeof(recv_data));
        recv_len = 0;
        recv_rindex = 0;
        //buffer.clear();
    }
    int __recv_int(void);
    int __getSync(void);
    int __sync(void);
    int __scan(void);
    int __getInfo(Info param);
    virtual void __program_multi(const uint8_t data[], const uint32_t len);
    void __program(const uint8_t *music, const uint16_t len);
    //void __send(const uint8_t c);
    virtual void __send(const uint8_t buf[], const uint32_t len);


    int board_type;
    int board_rev;
    int fw_maxsize;
    int bl_rev;
protected:
//private:
    //mploader *self;
    user_port *my_port;
    //user_port my_port;
    //buffer_size256 buffer;
    char recv_data[128];
    uint16_t recv_len;
    uint16_t recv_rindex;
    int __program_sync;
    //uint8_t music_buffer[1024*1024]; // 1MB
};

#endif // MPLOADER_H
