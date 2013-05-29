#include <ioCC2530.h>

#ifndef DS18B20_H_
#define DS18B20_H_


#define SEARCH_ROM      0xF0              //搜索ROM
#define READ_ROM        0x33              //读ROM
#define MATCH_ROM       0x55              //匹配ROM(挂多个DS18B20时使用)
#define SKIP_ROM        0xCC              //跳过匹配ROM(单个DS18B20时跳过)
#define ALARM_SEARCH    0xEC              //警报搜索

#define CONVERT_T       0x44              //开始转换温度
#define WR_SCRATCHPAD   0x4E              //写便笺
#define RD_SCRATCHPAD   0xBE              //读便笺
#define CPY_CCTATCHPAD  0x48              //复制便笺
#define RECALL_EE       0xB8              //未启用
#define RD_PWR_SUPPLY   0xB4              //读电源供应



#define BT(n)    1<<(n)
#define DQIO_PORT  1
#define DQIO_PIN   0 
#define Out_DQ  P1DIR |= BT(DQIO_PIN)
#define In_DQ   P1DIR &=~BT(DQIO_PIN)
#define Low_DQ  P1_0 = 0
#define Hign_DQ P1_0 = 1
#define Read_DQ P1_0



extern void Delay_temp(uint16 timeout);
extern void Init_DS18B20(void);
extern char ReadOneChar(void);
extern void WriteOneChar(char dat);
extern void ReadTemperature(void);
extern void gettempstr(void);

#endif