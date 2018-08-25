/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 22/8/2018
* Description        : MusicPlay 格式定义.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "music.h"
#include <stdio.h>
#include <string.h>
#include "cJSON/cJSON.h"

#include <QDebug>
#include "mydialog.h"

const uint32_t Music::buffer_size=BUFFER_SIZE;
/*
 * MusicPlay 格式歌曲示例
 */
const musical_t Music::musicals[musical_play_size] =
{
    {   //0 虫儿飞
        .rtttl     = "ChongErFei:d=4,o=5,b=103,c=F:", // 歌曲名:<ChongErFei>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 103, 调式: F调
        .melody    = "33345|3.22-|11123|3.77-|632-|632-|632.1|1---|00032|5--43|2--54|345.3|2--01|632.1|521--|43431-|43431.2|1---|0000|",
        .octave    = "-----|-----|-----|--..-|.---|.---|.----|----|-----|-----|-----|-----|-----|.----|.----|------|-------|----|----|",
        .durations = " --  |  -  | --  |  -  |    |    |    -|    |   --|   - |-  --| -   |   --|    -| --  |----  |----  -|    |    |",
    },
    {   //1 虫儿飞
        .rtttl     = "ChongErFei:d=4,o=5,b=103,c=F:", // 歌曲名:<ChongErFei>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 103, 调式: F调
        .melody    = "33345|3.22-|11123|3.77-|632-|632-|632.1|1---|33345|3.22-|11123|3.77-|632-|632-|632.1|1--32|5--43|2--54|345.3|2--01|632.1|521--|43431-|43431.2|1---|0000|",
        .octave    = "-----|-----|-----|--..-|.---|.---|.----|----|-----|-----|-----|--..-|.---|.---|.----|-----|-----|-----|-----|-----|.----|.----|------|-------|----|----|",
        .durations = " --  |  -  | --  |  -  |    |    |    -|    | --  |  -  | --  |  -  |    |    |    -|   --|   - |-  --| -   |   --|    -| --  |----  |----  -|    |    |",
    },
    {   // 2月亮代表我的心
        #if 0
        //.rtttl     = "YueLiangDaiBiaoWoDeXin:d=4,o=5,b=78,c=F#:", // 歌曲名:<YueLiangDaiBiaoWoDeXin>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 78, 调式: F升调
        .rtttl     = "YueLiangDaiBiaoWoDeXin:d=4,o=5,b=78,c=F#:",
        .melody    = "05|1.35.1|7.35.5|6.71.6|5--32|1.1132|1.1123|2.1623|2--05|1.35.1|7.35.5|6.71.6|5--32|1.1132|1.1123|2.6712|1--35|3.215|7--67|6.765|3--5|3.215|7--67|1.11.23|2--0|0000|",
        .octave    = "-.|------|..----|------|-----|------|------|---.--|----.|------|..----|---**-|-----|------|------|--..--|-----|-----|.....|.....|----|-----|.....|-------|----|----|",
        .durations = "--|  -  -|  -  -|  -  -|   --|  - --|  - --|  - --|   --|  -  -|  -  -|  -  -|   --|  - --|  - --|  - --|   --|  -  |   --|  -  |    |  -  |   --|  -  ==|    |    |"
        #else
        // http://www.qupu123.com/jipu/p288319.html
        //.rtttl     = "YueLiangDaiBiaoWoDeXin:d=4,o=5,b=78,c=F#:", // 歌曲名:<YueLiangDaiBiaoWoDeXin>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 78, 调式: F升调
        .rtttl     = "YueLiangDaiBiaoWoDeXin:d=4,o=5,b=78,c=F#:",
        .melody    = "05|1.35.1|7.35.5|6.71.6|5--32|1.1132|1.1123|2.1623|2--05|1.35.1|7.35.5|6.71.6|5--32|1.1132|1.1123|2.6712|1--35|3.215|7--67|6.765|3--5|3.215|7--67|1.11.23|2--05|1.35.1|7.35.5|6.71.6|5--32|1.1132|1.1123|2.6712|1--0|0000|",
        .octave    = "-.|------|..----|---**-|-----|------|------|---.--|----.|------|..----|---**-|-----|------|------|--..--|-----|-----|.....|.....|----|-----|.....|-------|----.|------|..----|---**-|-----|------|------|--..--|----|----|",
        .durations = "--|  -  -|  -  -|  -  -|   --|  - --|  - --|  - --|   --|  -  -|  -  -|  -  -|   --|  - --|  - --|  - --|   --|  -  |   --|  -  |    |  -  |   --|  -  ==|   --|  -  -|  -  -|  -  -|   --|  - --|  - --|  - --|    |    |"
        #endif
    },
    {  //3 故乡的原风景
       //.rtttl     = "GuXiangDeYuanFengJing:d=4,o=5,b=103,c=F:", // 歌曲名:<GuXiangDeYuanFengJing>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 103, 调式: F调
       .rtttl     = "GuXiangDeYuanFengJing:d=4,o=5,b=60,c=C:",
       .melody    = "5612----321.6|3--5612----|321553|3-5.612.----|32163---|5612----|3216---0|",
       .octave    = "..----------.|---..------|------|--...-------|---.----|..------|---....0|",
       .durations = "----    ==  =|   ----    |== - -|  --==--    |== -    |----    |== -    |",
    },
    {   //4 知道不知道
        .rtttl     = "ZhiDaoBuZhiDao:d=4,o=5,b=82,c=B#:", // 歌曲名:<ZhiDaoBuZhiDao>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 82, 调式: D调
        //.melody    = "33212|35323-|332153|32200|35165.6|16120|226165|5000|52-3|22165|332153|32200|35165.6|1612-|2.6165|5-00|",
        .melody    = "33212|35323-|332153|322.--0|35165.6|1612.-0|226165|5.---0|52-3|22165|332153|322--|35165.6|1612-|2.6165|5-00|",
        .octave    = "-----|------|------|------=|---....|-.----=|--.-..|.----=|----|---..|------|-----|---....|-.---|--.-..|.---|",
        .durations = " --  |----  | -- --|--     |----  -| --    | -- --|      |    | --  | -- --|--   |----  -| --  |  - --|    |",
    },
    {   // you yi tian chang di jiu 友谊天长地久
        .rtttl     = "YouYiDiJiuTianChang:d=2,o=5,b=50,c=F:", // 歌曲名:<YouYiDiJiuTianChang>, 2(d=2)/4拍, 音阶5, 每分钟节拍数 50, 调式: F调
        .melody    = "5|1.113|2.123|1.135|6.6|5.331|2.123|1.665|1.6|5.331|2.126|5.335|6.1|5.331|2.123|1.665|1.0|",
        .octave    = ".|-----|-----|-----|---|-----|-----|--...|--.|-----|-----|-----|--*|-----|-----|--...|---|",
        .durations = "-|--=--|--=--|--=--|  -|--=--|--=--|--=--|  -|--=--|--=--|--=--|  -|--=--|--=--|--=--|   |",
    },
    {
        // 天空之城
        .rtttl     = "TianKongZhiCheng:d=4,o=5,b=103,c=D:", // 歌曲名:<TianKongZhiCheng>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 103, 调式: D调
        .melody    = "67|1.713|7--3|6.561|5--3|4.3411|3--1|7.447|7-067|1.713|7--33|6.561|5--3|41771|231-0|17675|6-012|3.2345|2--5|21711123|3---|671712|1.55-|4321|3--67|1.713|7--3|6.561|5--3|41771|2311-|17675|6---|0000|",
        .octave    = "..|--.--|.--.|....-|.--.|....--|.---|.....|.--..|--.--|.--..|....-|.--.|.-..-|-----|-....|.----|------|---.|--.-----|----|..-.--|--..-|----|---..|--.--|.--.|....-|.--.|.-..-|-----|-....|....|----|",
        .durations = "--|  -  |    |  -  |    |  --- |    |  -  |   --|  -  |   --|  -  |    | --  | -   |--   |   --|     -|    |==== - -|    |-- -- |  -  |    |   --|  -  |    |  -  |    | --  | --  |--   |    |    |",
    },
    { // 7
      // 天空之城-赵海洋[质谱]
      // https://www.zhaogepu.com/jianpu/314882.html
      .rtttl     = "TianKongZhiCheng:d=4,o=5,b=100,c=D:", // 歌曲名:<TianKongZhiCheng>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 100, 调式: D调
      .melody    = "00067|1.713|7--33|6.561|5--3|4.341|3-0111|7.4#47|7-067|1.713|7--33|6.561|5--23|417711|22311-|176675#|6--12|3.235|2--55|1.713|3---|671722|1.55-|4321|3--3|6-55|321-01|21225|3--3|6-5-|321-01|21227|6--0|",
      .octave    = "-----|**-**|-----|----*|----|----*|---***|------|-----|**-**|-----|----*|-----|-*--**|******|*------|---**|*****|***--|**-**|****|--*-**|**---|****|****|****|****-*|*****|****|****|**** *|**** |----|",
      .durations = "   --|  -  |   --|  -  |    |  -  |  ----|  --  |   --|  -  |   --|  -  |   --| ---- |----  |----   |   --|  -  |   --|  -  |    |--  --|  -  |    |    |    |--  --| --  |    |    |--  --| --  |    |",
    },
    { // 8
      // 泪的物语
      // http://www.everyonepiano.cn/Number-2709.html
      //.rtttl     = "LeiDeWuYu:d=4,o=5,b=93,c=D:", // 歌曲名:<LeiDeWuYu>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 93, 调式: G调
      .rtttl     = "LeiDeWuYu:d=4,o=5,b=100,c=D:",
      //           (1)                        (5)                            (9)                    (13)                         (17)                      (21)                            (25)                  (29)                                   (33)                          (37)                            (41)                      (45)                        (49)                         (53)                      (57)
      .melody    = "171753|6545-|43236|4.#45#-|17175|6545.3|43213727|16146146|7---|37135|6545-|43236|4.#45#-|37135|6545.3|4321323|1---|135353|36313.3|43226|74#763-|135353|36313.3|43213232|65612|3513|2571|6---|6---|351351136136|2572572573571571|6--66|5351155#-|135353|36313.3|43236|4#445#-|135353|36313.3|43213232|1561235|3---|3--33|363-35|531175|2-3236|61335.|62.23.|1235612|1611111|371753|6545-|43236|4.#45#-|17175|6545.3|43213727|16146146|6---|0---|",
      .octave    = "H*H***|*****|*****|*******|H*H**|******|*******-|*-***HHH|HHHH|*-*--|-----|-----|-------|*-*--|------|-------|----|*-----|-.-----|-----|.....--|*-----|-.-----|--------|.--**|----|---*|----|----|-.-----..--.|-..--.------*--*|-----|---**----|*-----|-.-----|-----|-------|*-----|-.-----|--------|---****|****|*****|******|****--|--****|*HHH**|******|-------|-------|**H***|*****|*****|*******|H*H**|******|*******-|*-***HHH|H---|----|",
      .durations = " -- --| --  | --  |   -   | --  | --  -|--------|--------|    | --  | --  | --  |   -   | --  | --  -|---- --|    |----  |----  -| --  |-----  |----  |----  -|---- -==| --  |    |    |    |    |====--====--|================|   --|====--   |----  |----  -| --  |---    |----  |----   |---- -==| ------|    |   --|--  --|- -- -|  ----|- --  |-  -  | ------| ------| -- --| --  | --  |   -   | --  | --  -|--------|--------|    |    |",
    },
    {   //9
        // 雨的印记
        // http://www.jianpu.cn/pu/20/205626.htm
        #if 0
        .rtttl     = "YuDeYinJi:d=4,o=4,b=60,c=A#:", // 歌曲名:<YuDeYinJi>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 100, 调式: A调
        //.rtttl     = "YuDeYinJi:d=4,o=5,b=60,c=F:",
        .melody    = "0512|233-0123|2557-0567|71102321|7-0175|566-0654|45511-012|34444-54|3-2512|233-0123|255770567|71102321|7-0175|566-054|4551-012|34617|1-0135|6-0716|6550715|544332221|2330135|6-07765|50345443|4556.771|3.42512|3.22512|1-023|346171|1--0|0000|",
        .octave    = "-.--|--------|---..----|-**-****|---*--|--------|---------|--------|------|--------|---..----|-**-****|---*--|-------|--------|--.-.|------|---.--|----.--|---------|-------|-------|--------|-------*|****.--|****.--|-----|--.-.-|----|----|",
        .durations = "====|--  ====|---- ====|-- ---==|  ----|--  ====|---== -==|---== --|  ----|--  ====|---- ====|-- ---==|  ----|--  -==|---- -==|- -  |  ----|  ----|-- ----|-----==--|-- ----|  ---==| -==----| ----=--|  =----|  -----|   --|- - --|    |    |",
        #else
        //.rtttl     = "YuDeYinJi:d=4,o=5,b=60,c=A#:", // 歌曲名:<YuDeYinJi>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 100, 调式: A调
        .rtttl     = "YuDeYinJi:d=4,o=5,b=60,c=A#:",
        .melody    = "512|233-0123|2557-0567|71102321|7-0175|566-0654|45511-012|34444-54|3-2512|233-0123|2557-0567|71102321|7-0175|566-054|4551-012|34617|1-023|346171|1--0|0000|",
        .octave    = ".--|--------|---..----|-**-****|---*--|--------|---------|--------|------|--------|---..----|-**-****|---*--|-------|--------|--.-.|-----|--.-.-|----|----|",
        .durations = "===|--  ====|---- ====|-- ---==|  ----|--  ====|---== -==|---== --|  ----|--  ====|---- ====|-- ---==|  ----|--  -==|---- -==|- -  |   --|- - --|    |    |",
        #endif
    },
    {   //10 新鸳鸯蝴蝶梦
        // http://www.qupu123.com/jipu/p109532.html
        // http://www.jianpu.cn/pu/84/84532.htm
        .rtttl     = "XinYuanYangHuDieMeng:d=4,o=5,b=100,c=G:", // 歌曲名:<XinYuanYangHuDieMeng>, 4(d=4)/4拍, 音阶5, 每分钟节拍数 100, 调式: G调
        //            %3               %2                            %2              %3               %2                            %2                   %2                              %3                               %0
        //		.melody    = "35|6653321|23216-|23216123|2--35|6653321|23216-|2321656|1----0035|6653321|23216-|23216123|2--35|6653321|23216-|23216556|1--0|000321|2232.12|1656012|35565321|21612-|35563.2|1236.765|0535356|0161612|0323556|1--0|0000|",
        //		.octave    = "--|-------|----..|----.---|-----|-------|----..|----...|---------|-------|----..|----.---|-----|-------|----..|----....|----|------|-------|-...---|--------|--.---|-------|---.....|-......|--.-.--|-----..|----|----|",
        //		.durations = "--|---- --|----  |---- -==|   --|---- --|----  |-------|       --|---- --|----  |---- -==|   --|---- --|----  |--------|    |  ----|---  --|---- --|- ---==-|----  |- ==  -|- ---OO |------ |------ |---- --|    |    |",
        //		.melody    = "35|6653321|23216-|23216123|2--35|6653321|23216-|2321656|1----0035|6653321|23216-|23216123|2--35|6653321|23216-|2321656|1--0|000321|2232.12|1656012|35565321|21612-|35563.2|1236-|0535356|0161612|0323556|1--0|0000|",
        //		.octave    = "--|-------|----..|----.---|-----|-------|----..|----...|---------|-------|----..|----.---|-----|-------|----..|----...|----|------|-------|-...---|--------|--.---|-------|---..|-......|--.-.--|-----..|----|----|",
        //		.durations = "--|---- --|----  |---- -==|   --|---- --|----  |---- --|       --|---- --|----  |---- -==|   --|---- --|----  |---- --|    |  ----|---  --|---- --|- ---==-|----  |- ==  -|- -  |------ |------ |---- --|    |    |",
        .melody    = "35|6653321|23216-|2321613|2--35|6655321|23216-|2321656|1----0035|6655321|23216-|2321613|2--35|6653321|23216-|2321656|1---|000321|2232.12|1656-12|3566532|21612-|3563.2|1236765|0535356|0161612|0323556|1--0|0000|",
        .octave    = "--|-------|----..|----.--|-----|-------|----..|----...|---------|-------|----..|----.--|-----|-------|----..|----...|----|------|-------|-...---|-------|--.---|------|---...-|-......|--.-.--|-----..|----|----|",
        .durations = "--|---- --|----  |---- --|   --|---- --|----  |---- --|       --|---- --|----  |---- --|   --|---- --|----  |---- --|    |  ----|---  --|---- --|- -----|----  |- -  -|- --== |------ |------ |---- --|    |    |",
    },
};

Music::Music(QObject *parent) : QObject(parent)
{
    memset(_mode, 0, sizeof(_mode));
    memset(_name, 0, sizeof(_name));
    memset(_version, 0, sizeof(_version));
    memset(_rtttl_buf, 0, sizeof(_rtttl_buf));
    memset(_melody, 0, sizeof(_melody));
    memset(_octave, 0, sizeof(_octave));
    memset(_durations, 0, sizeof(_durations));
    _index=0;
}

/*
 * 打开 MusicPlay 歌曲文件,以 JSON 格式解析
*/
int Music::open_music(const char *path)
{
    size_t _size=0;
    size_t r_size=0;
    char* imagebyte=NULL;
    cJSON* root=NULL;
    cJSON* item=NULL;

    FILE *fp = NULL;
    qDebug() << "open_music" ;
    fp = fopen(path, "rb");
    if(NULL == fp)
    {
        qDebug() << "Unable to open" << QString::fromLocal8Bit(path);
        goto bad;
    }
    fseek(fp, 0, SEEK_END);
    _size = ftell(fp);
    if(0!=fseek(fp, 0, SEEK_SET))
    {
        qDebug() << "Unable to SET" << QString::fromLocal8Bit(path);
        goto bad;
    }
    qDebug() << "_size:" << _size;
    r_size = _size - _size%1024+2048;
    if(NULL == imagebyte) imagebyte = (char*)malloc(r_size);
    if(NULL == imagebyte)
    {
        qDebug() << "malloc space fail!";
        goto bad;
    }
    memset(imagebyte, 0x00, r_size);
    r_size = fread(imagebyte, 1, _size, fp);
    qDebug() << "r_size:" << r_size;
    if(_size != r_size)
    {
        qDebug() << "read music fail" ;
        goto bad;
    }
    fclose(fp);
    root = cJSON_Parse(imagebyte);
    if(NULL == root) goto bad;

    item = cJSON_GetObjectItem( root , "MusicPlay");
    //qDebug() << QString::fromLocal8Bit(item->valuestring) ;
    memcpy(_version, item->valuestring, strlen(item->valuestring));
    item = cJSON_GetObjectItem( root , "rtttl");
    //qDebug() << QString::fromLocal8Bit(item->valuestring) ;
    memcpy(_rtttl_buf, item->valuestring, strlen(item->valuestring));
    item = cJSON_GetObjectItem( root , "melody");
    //qDebug() << QString::fromLocal8Bit(item->valuestring) ;
    memcpy(_melody, item->valuestring, strlen(item->valuestring));
    item = cJSON_GetObjectItem( root , "octave");
    //qDebug() << QString::fromLocal8Bit(item->valuestring) ;
    memcpy(_octave, item->valuestring, strlen(item->valuestring));
    item = cJSON_GetObjectItem( root , "durations");
    //qDebug() << QString::fromLocal8Bit(item->valuestring) ;
    memcpy(_durations, item->valuestring, strlen(item->valuestring));

    if(NULL != root) cJSON_Delete(root);
    if(NULL == imagebyte) free(imagebyte);
    return 0;
bad:
    if(NULL != fp) fclose(fp);
    if(NULL != root) cJSON_Delete(root);
    if(NULL == imagebyte) free(imagebyte);
    return -1;
}
/*
 * 保存为 MusicPlay 歌曲文件,以 JSON 格式保存
*/
int Music::save_music(const char *path)
{
    size_t _size=0;
    size_t w_size=0;
    FILE* fp = NULL;
    cJSON* root =  cJSON_CreateObject();
    char* json_str;
    cJSON_AddItemToObject(root, "MusicPlay", cJSON_CreateString(MyDialog::version().toLocal8Bit().constData()));//根节点下添加
    cJSON_AddItemToObject(root, "rtttl", cJSON_CreateString(_rtttl_buf));
    cJSON_AddItemToObject(root, "melody", cJSON_CreateString(_melody));
    cJSON_AddItemToObject(root, "octave", cJSON_CreateString(_octave));
    cJSON_AddItemToObject(root, "durations", cJSON_CreateString(_durations));
    json_str = cJSON_Print(root);

    fp = fopen(path, "w+");
    if(NULL == fp)
    {
        qDebug() << "Unable to open" << QString::fromLocal8Bit(path);
        goto bad;
    }
    if(0!=fseek(fp, 0, SEEK_SET))
    {
        qDebug() << "Unable to SET" << QString::fromLocal8Bit(path);
        goto bad;
    }
    _size = strlen(json_str);
    qDebug() << "json size:" << _size;
    w_size = fwrite(json_str, 1, _size, fp);
    qDebug() << "w_size:" << w_size;
    if(_size != w_size)
    {
        qDebug() << "write music fail" ;
        goto bad;
    }
    fclose(fp);
    cJSON_Delete(root);
    free(json_str);
    return 0;

bad:
    if(NULL != fp) fclose(fp);
    cJSON_Delete(root);
    free(json_str);
    return -1;
}
/*
* 解析 RTTTL 格式获取歌曲信息
*/
uint16_t Music::rtttl(const char *_rtttl)
{
    uint16_t num;
    uint16_t pos=0;
    default_dur = 4;
    default_oct = 6;
    default_mode = NOTE_C; // 默认C调
    bpm = 63;
    memset(_name, 0, sizeof(_name));
    // "ChongErFei:d=4,o=5,b=103,c=F:"
    for(pos=0; ':'!=_rtttl[pos]; pos++)
    {
        _name[pos] = _rtttl[pos];
        if('\0'==_rtttl[pos]) return -1;
    }
    pos++;
    // 计算拍数
    if(_rtttl[pos] == 'd'){
        pos+=2;  // skip "d="
        num = 0;
        while(isdigit(_rtttl[pos])){
            num = (num * 10) + (_rtttl[pos++] - '0');
        }
        if(num > 0){
            default_dur = num;
        }
        pos++;                   // skip comma
    }

    // get default octave
    if(_rtttl[pos] == 'o')
    {
        pos+=2;              // skip "o="
        num = _rtttl[pos++] - '0';
        if(num >= 3 && num <=7){
            default_oct = num;
        }
        pos++;                   // skip comma
    }
    default_oct = default_oct-OCTAVE_START;

    // get BPM
    if(_rtttl[pos] == 'b'){
        pos+=2;              // skip "b="
        num = 0;
        while(isdigit(_rtttl[pos])){
            num = (num * 10) + (_rtttl[pos++] - '0');
        }
        bpm = num;
        pos++;                   // skip comma/colon
    }
    // get mode,获取调式
    if(_rtttl[pos] == 'c'){
        pos+=2;              // skip "c="
        memset(_mode, 0, sizeof(_mode));
        _mode[0] = _rtttl[pos];
        switch(_rtttl[pos])
        {
        case 'C':
            default_mode = NOTE_C;
            break;
        case 'D':
            default_mode = NOTE_D;
            break;
        case 'E':
            default_mode = NOTE_E;
            break;
        case 'F':
            default_mode = NOTE_F;
            break;
        case 'G':
            default_mode = NOTE_G;
            break;
        case 'A':
            default_mode = NOTE_A;
            break;
        case 'B':
            default_mode = NOTE_B;
            break;
        default:
            default_mode = NOTE_C;
            _mode[0] = 'C';
            break;
        }
        pos++;                   // skip colon
        if('#'==_rtttl[pos]) // 升调
        {
            default_mode++;
            _mode[1] = '#';
            pos++;                   // skip colon
        }
    }
    // BPM usually expresses the number of quarter notes per minute
    //wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)
    wholenote = (60 * 1000L / bpm);  // this is the time for whole note (in milliseconds)
    return pos;
}
