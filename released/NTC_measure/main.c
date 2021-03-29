#include "STC15F2K60S2.h"
#include "intrins.h"

#define STUDENT_NUMBER  3456  // 学号

#define ADC_POWER   0x80    
#define ADC_FLAG    0x10    
#define ADC_START   0x08    
#define ADC_SPEED0  0x00    //ADC转换速度 540个时钟
#define ADC_SPEED1  0x20    //ADC转换速度 360个时钟
#define ADC_SPEED2  0x40    //ADC转换速度 180个时钟
#define ADC_SPEED3  0x60    //ADC转换速度 90个时钟

float maxmum = 33;	  //上限设置值
float minmum = 25;    //下限设置值
   
float code NTC[146][2]={
{-40,340.9281},{-39,318.8772},{-38,298.3978},{-37,279.3683},{-36,261.6769},{-35,245.2212},{-34,229.9072},{-33,215.6488},{-32,202.3666},{-31,189.9878},
{-30,178.4456},{-29,167.6783},{-28,157.6292},{-27,148.246},{-26,139.4807},{-25,131.2888},{-24,123.6294},{-23,116.4648},{-22,109.76},{-21,103.4829},
{-20,97.6037},{-19,92.0947},{-18,86.9305},{-17,82.0877},{-16,77.5442},{-15,73.2798},{-14,69.2759},{-13,65.5149},{-12,61.9809},{-11,58.6587},
{-10,55.5345},{-9,52.5954},{-8,49.8294},{-7,47.2253},{-6,44.7727},{-5,42.462},{-4,40.2841},{-3,38.2307},{-2,36.294},{-1,34.4668},{0,32.7421},
{1,31.1138},{2,29.5759},{3,28.1229},{4,26.7496},{5,25.4513},{6,24.2234},{7,23.0618},{8,21.9625},{9,20.9218},{10,19.9364},
{11,19.0029},{12,18.1184},{13,17.28},{14,16.4852},{15,15.7313},{16,15.0161},{17,14.3375},{18,13.6932},{19,13.0815},{20,12.5005},
{21,11.9485},{22,11.4239},{23,10.9252},{24,10.451},{25,10},{26,9.5709},{27,9.1626},{28,8.7738},{29,8.4037},{30,8.0512},
{31,7.7154},{32,7.3954},{33,7.0904},{34,6.7996},{35,6.5223},{36,6.2577},{37,6.0053},{38,5.7645},{39,5.5345},{40,5.315},
{41,5.1053},{42,4.905},{43,4.7136},{44,4.5307},{45,4.3558},{46,4.1887},{47,4.0287},{48,3.8758},{49,3.7294},{50,3.5893},
{51,3.4553},{52,3.3269},{53,3.2039},{54,3.0862},{55,2.9733},{56,2.8652},{57,2.7616},{58,2.6622},{59,2.5669},{60,2.4755},
{61,2.3879},{62,2.3038},{63,2.2231},{64,2.1456},{65,2.0712},{66,1.9998},{67,1.9312},{68,1.8653},{69,1.8019},{70,1.7411},
{71,1.6826},{72,1.6264},{73,1.5723},{74,1.5203},{75,1.4703},{76,1.4222},{77,1.3759},{78,1.3313},{79,1.2884},{80,1.2471},
{81,1.2073},{82,1.169},{83,1.1321},{84,1.0965},{85,1.0623},{86,1.0293},{87,0.9974},{88,0.9667},{89,0.9372},{90,0.9086},
{91,0.8811},{92,0.8545},{93,0.8289},{94,0.8042},{95,0.7803},{96,0.7572},{97,0.735},{98,0.7135},{99,0.6927},{100,0.6727},
{101,0.6533},{102,0.6346},{103,0.6165},{104,0.599},{105,0.5821}
};

unsigned char code Table[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0x86,0x8E,0xBF,0xFF};// /*数码管段码表  低电平有效*/0-9,A-F GHPU ,-,灭
unsigned char Showtemp[4]={0,0,0,0};
unsigned char Ledcount = 0;   
unsigned char keycount = 0;   
unsigned char keystatus = 0;   
unsigned char key = 0; 
unsigned char key_final = 0;
char mood = 0;
char mood1= 0;     //上下限状态
sbit LED1 = P4^2;
sbit LED2 = P4^4;
sbit KEY3 = P3^4;
sbit KEY2 = P3^5;
sbit KEY1 = P3^6;

void Delay_MS(unsigned int ms)
{
    unsigned char i, j;
    _nop_();
    while(ms--)
    {
        i = 6;  j = 210;
        while (--i)
        while (--j);
    }
}
//AD初始化 
void Init_ADC(void)
{
    P1ASF = 0x40;       //打开P1.6口
    P1 &= 0xBF;         //设置P1.6口
    ADC_RES = 0;        //清掉ADC转换
    ADC_CONTR = ADC_POWER | ADC_SPEED3; //使能A/D
}

//AD值获取
unsigned int Get_AD(unsigned char channel)
{
    unsigned int result;
    ADC_RES = 0;    
    ADC_RESL= 0;//清掉ADC转换结果寄存器
    ADC_CONTR =ADC_POWER|ADC_SPEED0|ADC_START|channel;//配置ADC
    _nop_();    _nop_();
    _nop_();    _nop_();            //等待设置ADC_POWER完毕
    while (!(ADC_CONTR & ADC_FLAG));
    ADC_CONTR &= ~ADC_FLAG;         //清除标志位
    result = ADC_RES<<2|ADC_RESL;  
    return result;                  //ADC转换结果
}

float Voltage_Measure(void)
{	 
     float voltage = 0;
	 unsigned int voltage_std_AD = 0;
	 voltage_std_AD = Get_AD(7);
	 voltage = (1024/(float)voltage_std_AD )*2.5;
	 return voltage; 
}

float Temp_Measure()
{	  int   i = 0,j = 0;
      unsigned int voltage_AD = 0;
	  long int voltage_AD_avg = 0; 
	  float voltage = 0;
	  float resistance = 0;
	  float temperature = 0;

	  for(j=0;j<10;j++)
	  {
		  voltage_AD = Get_AD(6);
		  Delay_MS(1);
		  voltage_AD_avg += voltage_AD;
	  }
	  
	  voltage_AD_avg = voltage_AD_avg/10;
	  resistance = 10*(1024 - (float)voltage_AD_avg)/(float)voltage_AD_avg;
	  for(i=0; i<145; i++)
	  {
	     if((resistance<=NTC[i][1]) && (resistance>NTC[i+1][1]) )
		   {
		   	  temperature = NTC[i][0] + (NTC[i][1]-resistance)/(NTC[i][1]-NTC[i+1][1]);break;
		   }
		 else continue;
	  }		  
	  return temperature;
}

/*
数码管显示函数
*/
void LED_Translate(float dat)
{
    int data1 = 0;
	if ((dat<10)&&(dat>-10))
	{
		data1 = (int)(dat*100);
			if(dat >= 0)	
			{Showtemp[0] = 0xFF;}			          //第一位灭
			if(dat < 0)	
			{Showtemp[0] = 0xBF;
			data1 = 0-data1;}
		Showtemp[1] =  Table[(data1/100)]&0x7F;	      //个位数显示  加小数点						
		Showtemp[2] =  Table[data1%100/10];           //小数点后一位显示          
		Showtemp[3] =  Table[data1%10];   		      //小数点后两位显示
    }
	else if ((dat<100)&&(dat>-100))
	{
		data1 = (int)(dat*10);
			if(dat >= 0)	
			{Showtemp[0] = 0xFF;}			   
			if(dat < 0)	
			{Showtemp[0] = 0xBF;
			data1 = 0-data1;}
		Showtemp[1] =  Table[data1/100];	      	   //十位数显示					
		Showtemp[2] =  Table[data1%100/10]&0x7F;       //个位数显示  加小数点          
		Showtemp[3] =  Table[data1%10];   		       //小数点后一位显示
    }
	else if ((dat>=100)&&(dat<1000))
	{
		 data1 = (int)(dat*10);
		 Showtemp[0] =  Table[data1/1000];	      	  //bai位数显示
		 Showtemp[1] =  Table[data1/100];	      	  //十位数显示					
	 	 Showtemp[2] =  Table[data1%100/10]&0x7F;     //个位数显示  加小数点          
		 Showtemp[3] =  Table[data1%10];   		      //小数点后一位显示
	}
	else if (dat>=1000)
	{
		 Showtemp[0] =  Table[(int)dat/1000];	    //百位数显示
		 Showtemp[1] =  Table[(int)dat%1000/100];   //十位数显示					
	 	 Showtemp[2] =  Table[(int)dat%100/10];     //个位数显示
		 Showtemp[3] =  Table[(int)dat%10];   		     
	}					
}

void key_scan(void)
{
   	if (keystatus == 0)		  
		{		    
			if (KEY1 == 0)
			keystatus =1;
			if (KEY2 == 0)
			keystatus =2;
			if (KEY3 == 0)
			keystatus =3; 
		}
	else
	{	    
		keycount++;
		if(keycount==50)
		{
		   keycount = 0;
		   if(keystatus == 1)
			  {if (KEY1 == 0)
			  	  key = 1;}
		   if(keystatus == 2)
			  {if (KEY2 == 0)
			  	  key = 2;}
		   if(keystatus == 3)
			  {if (KEY3 == 0)
			  	  key = 3;}
		   keystatus =0;
		}
		
	}
   if(key == 1)
	  {if (KEY1 == 1)
	  	  {key_final = 1;key = 0;}}
   if(key == 2)
	  {if (KEY2 == 1)
	  	  {key_final = 2;key = 0;}}
   if(key == 3)
	  {if (KEY3 == 1)
	  	  {key_final = 3;key = 0;}}
}

void key_check(void)
{
	if(key_final == 1)
	{if(mood == 1) 
	      mood = 0;
	 else if(mood == 0) 
	      mood = 1;}
	if(key_final == 2)
	{
	if(mood1<2)
	mood1++;
	else mood1 = 0;
	}
	if(key_final == 3)
	{}
	key_final = 0;
}

void Timer0Init(void)		//500微秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xEA;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void Timer1Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x9A;		//设置定时初值
	TH1 = 0xA9;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
}

//Time0扫描LED显示驱动，计时,500us
void time0(void) interrupt 1
{
    if(4==++Ledcount)
    {Ledcount=0;
	}
    P2 = 0x0F;                      //关闭所有数码管选通
    P0 = Showtemp[Ledcount];        //送段码 低电平有效
    P2 = ~(0x01<<Ledcount);         //选择位选
    key_scan();				
}

void time1(void) interrupt 3
{
				
}

void main(void)
{
   float  VCC_measure  = 0;
   float temp_result = 0;
   
   Init_ADC();      //初始化ADC
   Timer0Init();    //设置T0用作数码管显示定时500us扫描，中断模式
   ET0=1;           //允许T0定时中断
   EA=1;
   Ledcount=0;      //LED扫描显示计数用
   P0M0=0xff;       //推挽输出驱动数码管  低电平有效 
   KEY1 = 1;
   KEY2 = 1;
   KEY3 = 1;

   LED_Translate(STUDENT_NUMBER);	

   Delay_MS(2000);
    
   while(1)
   {
        key_check();
		if( (mood == 0) && (mood1 == 0)	)
		{
			temp_result = Temp_Measure();
			LED_Translate(temp_result);	
			if(temp_result > maxmum )
			LED2 = 0;
			else LED2 = 1;
			if(temp_result < minmum )
			LED1 = 0;
			else LED1 = 1; 
		}   		
		if( (mood == 1) && (mood1 == 0))
		{
			VCC_measure = Voltage_Measure();
			LED_Translate(VCC_measure);
		}
		if (mood1 == 1)
		{
			LED_Translate(maxmum);
		}
		if (mood1 == 2)
		{
			LED_Translate(minmum);
		}
        

		Delay_MS(200);	   
   }
}		 