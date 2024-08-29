//*********************************************************
/*  文件名：main.c
*        功能：    触摸
*   IC:        FT60F123RB 16MHZ 2T模式
*   晶振：   16M/2T                    
*/
 //好久没有发帖子了，感谢平台一直帮助我，
 //我今天分享一种彩灯WS2811幻彩芯片的程序，.
 //里面有各种模式 渐变、跳变、闪烁、跑马、追逐等模式，
 //还可以写更多的，主控采用目前比较流行且有性价的辉芒微
 //FT60F211RB芯片SOP-8,FLASH的，才2毛多，在实际应用中绝对实用，
 //可以大量投产。附件中有PCB原代码，及图片视频等资料，可以参考
//*********************************************************
#include "FMD.h"
#include        "SYSCFG.h";
//**********************************************************
//***********************宏定义*****************************
#define  uchar   unsigned char
#define  uint    unsigned int
#define  ulong   unsigned long

#define RGB_count  16
#define RGB_speed  90

uint        shudu=0;

#define DIN RA4 //IO输出

uchar        mod;

//RGB 色光控制数组序列(根据芯片定义脉冲接口数量修改)
//  延时间隔 需根据电路调整 
//                      0    1    2    3    4     5    6    7  
const uchar table[] = { 255, 0,   0,   0,   255,  0,   0,   0, 
                        255, 255, 255, 0,   255,  0,   255, 0, 
						255, 255};
						
const uchar table1[]= { 255, 0,   0,   255, 0,    0,   255, 0,
                        0,   0,   255, 0,   0,    255, 0,   0,
						255, 0,   0,   0,   255,  0,   0,   255,
						0,   0,   255, 255, 255,  0,   255, 255,
						0,   255, 255, 0,   255,  0,   255, 255,
						0,   255, 255, 0,   255,  0,   255, 255,
						0,   255, 255, 0,   255,  255};

//读取EEPROM的接口函数  
uchar n, i, num;
uchar EEPROMread( uchar EEAddr )  
{
        uchar ReEEPROMread;
		
		//读写寄存器地址赋值 
        EEADR = EEAddr;            //将地址写入EEADR寄存器   
		//使能读
        RD = 1;
		
		//4周期指令延时 等待读取完成
        NOP();
        NOP();
        NOP();
        NOP();
		
		//开始取出数据
        ReEEPROMread = EEDAT;      //EEPROM的读数据 ReEEPROMread = EEDATA;
		
		//禁用读
        RD = 0;
		
        return ReEEPROMread;
}

//********************************************
//写EEPROM的接口函数  
//函数名称：EEPROMwrite
//功能：    写数据到EEPROM
//输入参数：EEAddr  需要写入数据的地址
//          Data    需要写入的数据
//返回参数：无
//********************************************
void EEPROMwrite(uchar EEAddr,uchar Data)
{
	//关总中断 
    GIE = 0;               //写数据必须关闭中断
		
    while(GIE);            //等待GIE为0
    EEADR = EEAddr;        //EEPROM的地址   写入目标地址
    EEDAT = Data;          //EEPROM的写数据  EEDATA = Data;   写入目标数据  
    EEIF = 0;              //中断标志位清0
    EECON1 |= 0x34;        //置位WREN1,WREN2,WREN3三个变量.
		
    WR = 1;                //置位WR启动编程
    NOP();
    NOP();
    NOP();
    NOP();
		
    while(WR);             //等待EE写入完成
    GIE = 1;
}

/***************EEPROM初始化***************************************/   
/* 写初始化 */
void EEPROM_init()
{   
    EEPROMwrite( 0x55, 0xaa );              
    EEPROMwrite( 0x55, 0xaa );
}

/* 读写使能标志位? */
/*************0码*******************/
void DIN_bit_0()
{
    DIN=1;
        
    DIN=0;        //1+3+4*7
        
}

/*************1码*******************/
void DIN_bit_1()
{
        DIN=1;       //1+3+4*7+1
		
        NOP(); 
		NOP(); 
		NOP(); 
		NOP(); 
		NOP(); 
		NOP();
		
        DIN=0;
        
}
/* 读写使能标志位? */

/*************reset码*******************/
void reset( void )
{
    uchar a,b;
    DIN=0;
	
    for( b=1; b>0; b-- )
    for( a=1; a>0; a-- );

    NOP();  //if Keil,require use intrins.h
}

/*************数据传输******************/
void send_dat( uchar dat )
{ 
    //高位先传
    uchar i;
    for(i=8;i>0;i--)
    {
        if(dat & 0x80) 
           DIN_bit_1();
        else
           DIN_bit_0();
        
        dat <<=1;
    }	
}

void send_rgb(uchar g, uchar r, uchar b)
{
    send_dat(g);
    send_dat(r);
    send_dat(b);
}

/***************延时******************/
void delay(uint t)
{
        unsigned  j;
        while(t--)
        for(j=0;j<100;j++);
}

/***************延时******************/
void delay1(uchar t)
{
        unsigned  j;
        while(t--)
        for(j=0;j<10;j++);
}

/* 三色控制接口 */
void send_rgb_cor( uchar rc, uchar gc, uchar bc )
{
	//
    send_dat( rc );
    send_dat( gc );
    send_dat( bc );	
	
 //	
        //RG渐灭  功能接口
//        for(i=255;i>0;i--)  
 //       {
 //               for(n=RGB_count;n>0;n--)
 //               {
 //                       send_dat(i);
 //                       send_dat(i);
 //                       send_dat(0);
 //               }
 //               reset();
 //               delay1(sudu);
 //       }	
}

/*-------------------------------------------------
*  函数名：POWER_INITIAL
*        功能：  上电系统初始化
*  输入：  无
*  输出：  无
--------------------------------------------------*/        
void SYS_INITIAL (void)
{       
    OSCCON = 0B01110000;//WDT 32KHZ IRCF=110=16MHZ/4=4MHZ,0.25US/T
                        //Bit0=1,系统时钟为内部振荡器
                        //Bit0=0,时钟源由FOSC<2：0>决定即编译选项时选择
    OPTION = 0B00000000;    //Bit3 PSA   预分频器分配位 0= 预分频器分配给Timer0模块 1：分配给WDT
                            //Bit2：0：预分频比选择位 000: 1:2                           
    INTCON = 0;             //暂禁止所有中断
    PORTA =0B00000000;
        
    TRISA =  0B00000100;    // 0-输出 1-输入  
        
    WPUA =  0B00000100;     //PA端口上拉控制 1-开上拉 0-关上拉               
}

void jiabian1(uchar sudu)//渐变
{
    uchar i,n;
		
//    send_dat(255);//先赋值给R
//    send_dat(0);
//    send_dat(0);
	send_rgb_cor( 255, 0, 0 );
		
    //R渐亮//G渐灭
    for( i=0; i<255; i++ )
    {
        for( n=RGB_count; n>0; n-- )
        {
//            send_dat(255-i);
//            send_dat(i);
//            send_dat(0);
	       send_rgb_cor( 255 - i, i, 0 );
        }
   
        reset();
        delay1(sudu);
    }
 
    delay1(250);
		
    //G渐亮//R渐灭
    for(i=0;i<255;i++)
    {
        for(n=RGB_count;n>0;n--)
        {
         //   send_dat(0);
         //   send_dat(255-i);
         //   send_dat(i);
	       send_rgb_cor( 0, 255 - i, i );			
        }
    
	    reset();
        delay1(sudu); 
	}
    delay1(250);
		
     //RGB渐亮
     for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(i);
                        send_dat(255);
                }
                reset();
                delay1(sudu);
        }
        delay1(250);
		
        //B不变//RG渐灭 while()
        for(i=0;i<255;i++)
        {
            for(n=RGB_count;n>0;n--)
            {
                send_dat(255);
                send_dat(255-i);
                send_dat(255-i);
            }
			
            reset();
            delay1(sudu);
        }
		
        delay1(250);
}

void huxi(uchar sudu)//呼吸
{
        uchar i,n;
		
        //R渐亮
        for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(0);
                        send_dat(0);
                }
                reset();
                delay1(sudu);
        }
		
        //R渐灭
        for(i=255;i>0;i--)  
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(0);
                        send_dat(0);
                }
                reset();
                delay1(sudu);
        }
		
        //G渐亮
        for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(0);
                        send_dat(i);
                        send_dat(0);
                }
                reset();
                delay1(sudu);
        }
		
        //G渐灭
        for(i=255;i>0;i--)  
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(0);
                        send_dat(i);
                        send_dat(0);
                }
                reset();
                delay1(sudu);
        }        
		
        //B渐亮
        for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(0);
                        send_dat(0);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }
		
        //B渐灭
        for(i=255;i>0;i--)  
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(0);
                        send_dat(0);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }
		
        //RG渐亮
        for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(i);
                        send_dat(0);
                }
                reset();
                delay1(sudu);
        }
		
        //RG渐灭
        for(i=255;i>0;i--)  
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(i);
                        send_dat(0);
                }
                reset();
                delay1(sudu);
        }
		
        //BR渐亮
        for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(0);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }
		
        //        BR渐灭
        for(i=255;i>0;i--)  
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(0);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }
		
    //        BG渐亮
        for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(0);
                        send_dat(i);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }
		
                //        BG渐灭
        for(i=255;i>0;i--)  
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(0);
                        send_dat(i);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }
		
                //        BGR渐亮
        for(i=0;i<255;i++)
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(i);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }
		
                //        RBG渐灭
        for(i=255;i>0;i--)  
        {
                for(n=RGB_count;n>0;n--)
                {
                        send_dat(i);
                        send_dat(i);
                        send_dat(i);
                }
                reset();
                delay1(sudu);
        }        
}
void paoma1(uint sudu)
{
        uchar i,num;
		
        //R跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(0);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
        //G跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(255);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
        //B跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(0);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
        //RG跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(255);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
        //RB跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(0);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
        //BG跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(255);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
        //RGB跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(255);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
        /********************反向*****************/

}

void paoma2(uchar sudu)
{
    uchar i,num;
        //R跑马灯增加*******************
         for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(0);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
        for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
         //G跑马灯增加*******************
         for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(255);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
        for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
        //B跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(0);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
      //RG跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(255);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
       //RB跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(0);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
       //BG跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(255);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
       //RGB跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(255);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
}

void paoma3(uchar sudu)
{
    uchar i,n,num,t;
        //R跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(0);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
      //G跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(255);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
      //B跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(0);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
      //RG跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(0);
                        send_dat(255);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
       //RB跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(0);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
       //BG跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(255);
                        send_dat(0);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
		
       //RGB跑马灯增加*******************
        for(i=0;i<=RGB_count;i++)
         {
                for(num=0;num<i;num++)
                 {
                        send_dat(255);
                        send_dat(255);
                        send_dat(255);
                }
                reset();
                delay(sudu);
        }
		
      for(num=RGB_count;num>0;num--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
}

//流水灯
void liushui(uchar sudu)
{
                send_dat(255);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
                reset();
                delay(sudu);//1250
        
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(255);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                reset();
                delay(sudu);

            send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(255);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
                reset();
                delay(sudu);
        
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(255);
                send_dat(0);
                send_dat(0);
               
                reset();
                delay(sudu);   
				
            /*********R***************/
            send_dat(0);
                send_dat(255);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
                reset();
                delay(sudu);
        
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                 send_dat(0);
                send_dat(255);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                reset();
                delay(sudu);

            send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                 send_dat(0);
                send_dat(255);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
                reset();
                delay(sudu);
        
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                 send_dat(0);
                send_dat(255);
                send_dat(0);
               
                reset();
                delay(sudu);   
            /*********R***************/
            send_dat(0);
                send_dat(0);
                send_dat(255);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
                reset();
                delay(sudu);
        
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(255);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                reset();
                delay(sudu);

            send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(255);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
                reset();
                delay(sudu);
        
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(0);
               
                send_dat(0);
                send_dat(0);
                send_dat(255);
               
                reset();
                delay(sudu);   
            /*********R***************/
}

//转到数组发送
//while ( arr{n, n-1} -- ) 
// send_rgb(arr(n))
void paoma5(uint sudu)
{
        send_dat(255);
        send_dat(0);
        send_dat(0);
        
        send_dat(0);
        send_dat(255);
        send_dat(0);
        
        send_dat(0);
        send_dat(0);
        send_dat(255);
        
        send_dat(0);
        send_dat(255);
        send_dat(255);

        send_dat(255);
        send_dat(0);
        send_dat(255);

        send_dat(255);
        send_dat(255);
        send_dat(0);
        reset();
        delay(sudu);//1250

        send_dat(0);
        send_dat(255);
        send_dat(0);
        
        send_dat(0);
        send_dat(0);
        send_dat(255);
        
        send_dat(0);
        send_dat(255);
        send_dat(255);
        
        send_dat(255);
        send_dat(0);
        send_dat(255);

        send_dat(255);
        send_dat(255);
        send_dat(0);

        send_dat(255);
        send_dat(0);
        send_dat(2);
        
        reset();
        delay(sudu);   

}

void shanshuo(uint sudu)
{
        /**************R***********/
        uchar n;
        for(n=RGB_count;n>0;n--)
        {
                send_dat(255);
                send_dat(0);
                send_dat(0);
        }
        reset();
        delay(sudu);
		
        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
        delay(sudu);
        /**************G***********/

        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(255);
                send_dat(0);
        }
        reset();
        delay(sudu);
        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
        delay(sudu);
        /**************B***********/

        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(255);
        }
        reset();
        delay(sudu);
        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
        delay(sudu);
        

        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(255);
                send_dat(255);
        }
        reset();
        delay(sudu);
        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
        delay(sudu);
        

        for(n=RGB_count;n>0;n--)
        {
                send_dat(255);
                send_dat(0);
                send_dat(255);
        }
        reset();
        delay(sudu);
        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
        delay(sudu);
        

        /*for(n=RGB_count;n>0;n--)
        {
                send_dat(255);
                send_dat(255);
                send_dat(0);
        }
        reset();
        delay(sudu);
        for(n=RGB_count;n>0;n--)
        {
                send_dat(0);
                send_dat(0);
                send_dat(0);
        }
        reset();
        delay(sudu); */        
}

void zondong(uint sudu)//多个灯为例 1个灯一组 流动
{
        uchar n,b,a;
        for(n=0;n<RGB_count;n++)
        {
                 b=3*n;
                 for(a=0;a<(RGB_count*3);a++)//先扫描出整排灯，静态的初始状态
                 {
                        send_dat(table[b]);//先取出表格内的3位数 然后通过移位个数*3 因为我们是3位数相加而来
                        b++;
                        if(b>=(RGB_count*3))//这里面是取数据到最后一位了，要直接清0，从而实现移位
                        b=0;
                 }
                         reset();
                delay(sudu);//1250
        }

}

void zondong2(uint sudu)//6个灯为例 3个灯一组 流动
{
        uchar n,b,a;
        for(n=0;n<6;n++)
        {
                 b=9*n;//取样9个
                 for(a=0;a<(6*9);a++)//先扫描出整排灯，静态的初始状态
                 {
                        send_dat(table1[b]);//先取出表格内的3位数 然后通过移位个数*3 因为我们是3位数相加而来
                        b++;
                        if(b>=(6*9))//这里面是取数据到最后一位了，要直接清0，从而实现移位
                        b=0;
                 }
                         reset();
                delay(sudu);//1250
        }
}

/*-------------------------------------------------
*  函数名:  main
*        功能：  主函数
*  输入：  无
*  输出：  无
--------------------------------------------------*/
void main()
{  
    //系统初始化
    SYS_INITIAL();
		
	//当前保存运行模式获取
    mod=EEPROMread(0x01);
    mod++;
    if(mod>3)mod=0;
	
    //切换执行	
    EEPROMwrite(0x01,mod);
    switch (mod)
    {
        case 0:
           for(n=0;n<=RGB_count;n++)
           {
              send_dat(0);
              send_dat(0);
              send_dat(0);
           }
				  
           reset();
           delay1(100);
           shudu=50;
        break;
				
        case 1:
            shudu=30;
        break;
				
        case 2:
            shudu=1500;
        break;
				
        case 3:
            shudu=600;
        break;

    }
		
    while(1)
    {
	  //状态机---遍历-- 增加-- 
			
	  //获取当前控制状态
			
	  //切换到控制状态 -- 
      switch (mod)
      {
        case 0:		
           if( (shudu>0) && (shudu<20) )
		   {
               shudu+=2;							  
		   }else if( (shudu>20) && (shudu<100) )
		   {
               shudu+=10;							  
		   }else if( (shudu>100) && (shudu<2000) )
						  {
                            shudu+=1000;							  
						  }

                          if(shudu>2000)shudu=1;
						  
                          paoma1(shudu);
						  
                        break;

                        case 1:
                          //jiabian1(shudu);
                          zondong2(3000);
                        break;

                        case 2:
                          shudu+=200;
                          if(shudu>2000)shudu=200;
						  
                          huxi(shudu);
                          //zondong(shudu);
                        break;

                        case 3:
                          shudu+=50;
                          if(shudu>1000)shudu=50;
                          shanshuo(shudu);
                        break;
                        /*case 8:
                        for(n=0;n<=RGB_count;n++)
                        {
                                send_dat(0);
                                send_dat(255);
                                send_dat(255);
                        }
                        reset();
                        //delay1(100);
                        break;
                        case 9:
                        for(n=0;n<=RGB_count;n++)
                        {
                                send_dat(255);
                                send_dat(0);
                                send_dat(255);
                        }
                        reset();
                        delay1(100);
                        break;
                        case 10:
                        for(n=0;n<=RGB_count;n++)
                        {
                                send_dat(255);
                                send_dat(255);
                                send_dat(255);
                        }
                        reset();
                        delay1(100);
                        break;*/
                }
        }                  
}