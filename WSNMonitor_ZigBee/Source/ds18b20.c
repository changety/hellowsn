#include "ds18b20.h"

unsigned char FRACTION_INDEX[16] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 7, 8, 9, 9 };//小数值查询表
unsigned char temp_data[2];
extern char ch[10]="asdf";



void Delay_temp(uint16 timeout)
{
    timeout>>= 1;
    while(timeout--)
    {
        
        
    }
}




void Init_DS18B20(void)
{ 
  Out_DQ;				//配置输出
  Low_DQ;				//数据线拉低
  Delay_temp(800);;			//480-960us
  Hign_DQ;			//拉高数据线
  Delay_temp(40);			//15-60us
  In_DQ;
  while(Read_DQ);		//读取数据线电平，检测初始化是否正常
  Out_DQ; 
  Hign_DQ;
  Delay_temp(800);
  Hign_DQ;
}



//****************************************************************************************
//*函数：char ReadOneChar(void)
//*功能：DS18B20读时间片函数
//***************************************************************************************/
char ReadOneChar(void)
{    
    char i;
	char dat = 0;
	
	for(i=8;i>0;i--)
	{   
	    Out_DQ;       //配置输出
        Hign_DQ;
		Delay_temp(1);     //5us
	    Low_DQ;       //置低电平
		Delay_temp(1);
		dat >>= 1;    //右移一位
		Hign_DQ;      //置高电平
		asm("nop");
		In_DQ;        //设置输入
		Delay_temp(1);
		if(Read_DQ)   //读取电平
                {
		dat |= 0x80;
                }
        Delay_temp(100);
	}

	return(dat);
}

//****************************************************************************************
//*函数：void WriteOneChar(char dat)
//*功能：DS18B20写时间片函数
//***************************************************************************************/
void WriteOneChar(char dat)
{   
    char i;
	Out_DQ;			//配置输出
	for(i=8;i>0;i--)
	{
	    Hign_DQ;
	    Low_DQ;		//置低电平
		Delay_temp(1);
		if(dat&0x01)//写电平
              {
        Hign_DQ;	//置高电平
		Delay_temp(80);
              }
        else
              {
         Low_DQ;	//置低电平
		 Delay_temp(80);
              }
		Hign_DQ;
		dat >>= 1;  //右移一位
		Delay_temp(1);
	}
	Hign_DQ;
}

//****************************************************************************************
//*函数：void ReadTemperature(void)
//*功能：读取温度
//***************************************************************************************/
void ReadTemperature(void)
{
        unsigned char tem_h,tem_l;
        unsigned char a,b;            //临时变量
        unsigned char flag;           //温度正负标记，正为0，负为1
	Init_DS18B20();					//初始化
	WriteOneChar(0xCC); 			// 跳过读序列号的操作
	WriteOneChar(0x44); 			// 启动温度转换
        Delay_temp(200);
	Delay_temp(200);
	Delay_temp(200);
	
	Init_DS18B20();					//再次初始化
	WriteOneChar(0xCC); 			//跳过读序列号的操作
	WriteOneChar(0xBE); 			//读取温度寄存器，前两个就是温度

	tem_l= ReadOneChar();   	//读取温度值低位LSB
	tem_h= ReadOneChar();    //读取温度值高位MSB
        
        if(tem_h & 0x80)
    {
        flag = 1;                 //温度为负
        a = (tem_l>>4);           //取温度低4位原码
        b = (tem_h<<4)& 0xf0;     //取温度高4位原码
        tem_h = ~(a|b) + 1;       //取整数部分数值，不符号位
        
        tem_l = ~(a&0x0f) + 1;    //取小数部分原值，不含符号位
    }
    else
    {
        flag = 0;                 //为正
        a = tem_h<<4;
        a += (tem_l&0xf0)>>4;     //得到整数部分值 
        b = tem_l&0x0f;           //得出小数部分值
        tem_h = a;                //整数部分
        tem_l = b&0xff;           //小数部分
    }
    
    temp_data[0] = FRACTION_INDEX[tem_l]; //查表得小数值
    temp_data[1] = tem_h| (flag<<7);      //整数部分，包括符号位
}






void gettempstr(void)
{   
    
    unsigned char temh,teml;
    teml = temp_data[0];
    temh = temp_data[1];
    ch[0] = ' ';
    ch[1] = ' ';
    
    if(temh & 0x80)            //判断正负温度
    {
        ch[2]='-';              //最高位为正
    }
    else ch[2]='+';
    if(temh/100==0)
        ch[3]=' ';
    else ch[3]=temh/100+0x30;      //+0x30 为变 0~9 ASCII码
    if((temh/10%10==0)&&(temh/100==0))
        ch[4]=' ';
    else ch[4]=temh/10%10+0x30;
    ch[5]=temh%10+0x30;
    ch[6]='.';
    ch[7]=teml+0x30;// 小数部分
}