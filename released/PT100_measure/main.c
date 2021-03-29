#include "STC15F2K60S2.h"
#include "intrins.h"

#define ADC_POWER   0x80    //ADC电源控制位
#define ADC_FLAG    0x10    //ADC完成标志位
#define ADC_START   0x08    //ADC启动控制位
#define ADC_SPEED0  0x00    //ADC转换速度，一次转换需要540个时钟
#define ADC_SPEED1  0x20    //ADC转换速度，一次转换需要360个时钟
#define ADC_SPEED2  0x40    //ADC转换速度，一次转换需要180个时钟
#define ADC_SPEED3  0x60    //ADC转换速度，一次转换需要90个时钟
  
/*数码管段码表  低电平有效*///0    1 	2	 3	 4	   5	6	7  	 8	  9	   	A	 B    E	  F 	-    灭
unsigned char code Table[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0x86,0x8E,0xBF,0xFF};// 数码管转换字表 0-9,A-F GHPU ,-,灭
unsigned char Showtemp[4]={0,0,0,0};    //数码管显示显示数组
unsigned char MAXMINtemp[3]={0,0,0};    //最大值最小值设定暂存数组

char loca_count =0;	 //
char signal = 0;	 //符号变量
float maxmum = 40; 	 //温度最大值
float minmum = 20;	 //温度最小值
int maxmum1 = 0; 	 //最大值暂存
int minmum1 = 0;     //最小值暂存
char flag1 = 0;		  
char error = 0;	     //错误状态检测

float  VCC_measure  = 0;    //电压测量值变量
float temp_result = 0;		//温度测量值变量
int voltage_2_5v_ad = 0;
unsigned char Ledcount = 0;    //数码管显示计数
unsigned char keycount = 0;    //摁键消抖定时20ms计数
unsigned char keystatus = 0;   //摁键摁下状态变量 
unsigned char key = 0; 		   // 摁键循环变量
unsigned char key_final = 0;   //摁键抬起后最终执行变量
char mood = 0;                 //系统工作模式    0测温   1电压
char sleep_flag = 0;           //休眠标志
char sleep_flag1 = 0;
char turn_start =1;			   //

char alarm_mood = 0;		   //温度上下限报警模式选择


float code PT100[12][2]={
{25,0.620},{30,0.89},{35,1.18},{40,1.46},{45,1.75},{50,2.01},{55,2.37},{60,2.66},{65,2.95},{70,3.335},{75,3.60},{80,3.90}
};//PT100表
sbit LED1 = P4^2;
sbit LED2 = P4^4;
sbit KEY3 = P3^4;
sbit KEY2 = P3^5;
sbit KEY1 = P3^6;
sbit TURN1 = P2^7;
sbit TURN2 = P2^6;
sbit TURN3 = P2^5;
sbit TURN4 = P2^4;

/******************************
函数说明：延时函数  1-65535毫秒
入口参数：ms=延时毫秒数
出口参数：无
******************************/
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
 
/******************************
函数说明：初始化ADC寄存器，设置P1.6为ADC输入功能
入口参数：无
出口参数：无
******************************/
void Init_ADC(void)
{
    P1M1 = 0xFF;
	P1M0 = 0x00;
    P1ASF = 0x40;       //打开P1.6口的ADC功能
    //P1 &= 0xBF;         //设置P1.6口输出低电平确保能采集到外部电平信号
	//P1 = 0x37;
    ADC_RES = 0;        //清掉ADC转换结果寄存器
    ADC_CONTR = ADC_POWER | ADC_SPEED3; //使能A/D供电，设置转换速度90T
}

/******************************
函数说明：查询方式读取ADC转换结果
入口参数：channel  ADC采样通道
出口参数：result   ADC_RES ADC转换结果
******************************/
unsigned int Get_AD(unsigned char channel)
{
    unsigned int result;
    ADC_RES = 0;    
    ADC_RESL= 0;//清掉ADC转换结果寄存器
    ADC_CONTR =ADC_POWER|ADC_SPEED0|ADC_START|channel;//配置ADC，设置转换通道，启动转换
    _nop_();    _nop_();
    _nop_();    _nop_();            //等待设置ADC_POWER完毕
    while (!(ADC_CONTR & ADC_FLAG));//读取转换完毕标志位ADC_FLAG
    ADC_CONTR &= ~ADC_FLAG;         //清除ADC_FLAG标志位
    result = ADC_RES<<2|ADC_RESL;   //读取10位转换结果保存到result
    return result;                  //返回ADC转换结果10位
}

/******************************
函数说明：电压值测量函数
入口参数：无
出口参数：voltage 计算后得到的电压值
******************************/
float Voltage_Measure(void)
{	 int   i = 0,j = 0;
     float voltage = 0;
	 int voltage_AD_avg = 0; 	  //AD采样平均值
	 unsigned int voltage_std_AD = 0;
	 for(j=0;j<10;j++)				 //10次求平均 采样间隔1ms
	  {
		  voltage_std_AD = Get_AD(7);
		  Delay_MS(1);							   //采样间隔1ms
		  voltage_AD_avg += voltage_std_AD;
	  }
	 voltage_AD_avg = voltage_AD_avg/10;    //  除10求平均
	 //voltage = (1024/(float)voltage_std_AD )*2.5;
	 voltage = (1067/(float)voltage_AD_avg )*2.5;
	 return voltage; 
}

float Voltage_Measure2_5v_ad(void)
{	 int   i = 0,j = 0;
     float voltage = 0;
	 int voltage_AD_avg = 0; 	  //AD采样平均值
	 unsigned int voltage_std_AD = 0;
	 for(j=0;j<10;j++)				 //10次求平均 采样间隔1ms
	  {
		  voltage_std_AD = Get_AD(7);
		  Delay_MS(1);							   //采样间隔1ms
		  voltage_AD_avg += voltage_std_AD;
	  }
	 voltage_AD_avg = voltage_AD_avg/10;    //  除10求平均
	 return voltage_AD_avg; 
}

/******************************
函数说明：温度值测量函数    使用PT100！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
入口参数：无
出口参数：temperature 计算后得到的温度值
******************************/
float Temp_Measure_PT100(int voltage_std_ad)
{	  int   i = 0,j = 0;
      unsigned int voltage_AD = 0;	  //本次采样AD值
	  long int voltage_AD_avg = 0; 	  //AD采样平均值
	  float voltage = 0;              //根据ad求出的电压值 不利用电源电压的求法
	  float temperature = 0;		  //温度值
	  long float resistance = 0;
	  for(j=0;j<10;j++)				 //10次求平均 采样间隔1ms
	  {
		  voltage_AD = Get_AD(3);    //PT100连接通道2
		  Delay_MS(1);							   //采样间隔1ms
		  voltage_AD_avg += voltage_AD;
	  }
	  
	  voltage_AD_avg = voltage_AD_avg/10;    //  除10求平均 
	  if (voltage_AD_avg <5)				 //传感器未连接判断  如果没电压则没有连接
	  {error = 1;}
	  else
	  {error = 0;}
	  voltage = ((float)voltage_AD_avg/(float)voltage_std_ad)*2.5;
//	  resistance = (6200*(long float)voltage_AD_avg+10057565.6)/(121177.812-(long float)voltage_AD_avg);	   //计算电阻值 根据满电压ad为1024

	  for(i=0; i<11; i++)													   //轮询法查表
	  {
	     if(( voltage >= PT100[i][1]) && ( voltage < PT100[i+1][1]) )	  //|| (resistance>=NTC[i][1])
		   {
		   		temperature = PT100[i][0] + (voltage - PT100[i][1])/(PT100[i+1][1]-PT100[i][1])*5;break;	  //线性插值法
		   }
		 else continue;
	  }		  
	  return temperature;
//	  return voltage_AD_avg;
//	  return voltage;                   
}

/******************************
函数说明：数转换为4位数码管显示码
入口参数：dat 需转化的数字
出口参数：无 操作对象为Showtemp[4]
******************************/
void LED_Translate(float dat)
{
    int data1 = 0;
	
	if ((dat<10)&&(dat>-10))
	{
		data1 = (int)(dat*100);
			if(dat >= 0)	
			{Showtemp[0] = 0xFF;}			   //数码管第1位显示 符号位
			if(dat < 0)	
			{Showtemp[0] = 0xBF;
			data1 = 0-data1;}
		Showtemp[1] =  Table[(data1/100)]&0x7F;	      //个位数显示  加小数点						
		Showtemp[2] =  Table[data1%100/10];         //小数点后一位显示          
		Showtemp[3] =  Table[data1%10];   		  //小数点后两位显示
    }
	else if ((dat<100)&&(dat>-100))
	{
		data1 = (int)(dat*10);
			if(dat >= 0)	
			{Showtemp[0] = 0xFF;}			        //数码管第1位显示	符号位
			if(dat < 0)	
			{Showtemp[0] = 0xBF;
			data1 = 0-data1;}
		Showtemp[1] =  Table[data1/100];	      	 //十位数显示					
		Showtemp[2] =  Table[data1%100/10]&0x7F;     //个位数显示  加小数点          
		Showtemp[3] =  Table[data1%10];   		     //小数点后一位显示
    }
	else if ((dat>=100)&&(dat<1000))
	{
		 data1 = (int)(dat*10);
		 Showtemp[0] =  Table[data1/1000];	      	  //百位数显示
		 Showtemp[1] =  Table[data1%1000/100];	      	  //十位数显示					
	 	 Showtemp[2] =  Table[data1%100/10]&0x7F;     //个位数显示  加小数点          
		 Showtemp[3] =  Table[data1%10];   		      //小数点后一位显示
	}
	else if (dat>=1000)
	{
		 Showtemp[0] =  Table[(int)dat/1000];	   //千位数显示
		 Showtemp[1] =  Table[(int)dat%1000/100];   //百位数显示					
	 	 Showtemp[2] =  Table[(int)dat%100/10];     //十位数显示
		 Showtemp[3] =  Table[(int)dat%10];			//个
	}						
}


/******************************
函数说明：数码管全灭 无需进入中断执行 全部 四个位 拉高
入口参数：无
出口参数：无 
******************************/
void LED_Clean(void)	         //LED灯全灭函数
{	int ii = 0;
	Showtemp[0] = 0xFF;	
	Showtemp[1] = 0xFF; 
	Showtemp[2] = 0xFF; 
	Showtemp[3] = 0xFF;
	for(ii=0; ii<4; ii++)
	{
	    P2 = 0x0F;               //关闭所有数码管选通
        P0 = Showtemp[ii];       //送段码 低电平有效
        P2 = ~(0x01<<ii);    }   //选择位选
}
/******************************
函数说明：摁键扫描函数 中断执行 
入口参数：无   全局变量   keystatus  keycount  key_final
出口参数：无 
******************************/
void key_scan(void)
{
   	if (keystatus == 0)		  	 //摁键摁下
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
		if(keycount==50)			//消抖检测
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
   if(key == 1)						//摁键抬起确认 	key_final置位代表一次摁键物理意义完成
	  {if (KEY1 == 1)
	  	  {key_final = 1;key = 0;}}
   if(key == 2)
	  {if (KEY2 == 1)
	  	  {key_final = 2;key = 0;}}
   if(key == 3)
	  {if (KEY3 == 1)
	  	  {key_final = 3;key = 0;}}
}

/******************************
函数说明：定时器0初始化  16位定时器自动重装载模式
入口参数：无   
出口参数：无 
******************************/
void Timer0Init(void)		//500微秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式

	TL0 = 0x66;		    //设置定时初值
	TH0 = 0xEA;	   	    //设置定时初值
	TF0 = 0;	     	//清除TF0标志
	TR0 = 1;		    //定时器0开始计时
}

/******************************
函数说明：外部中断2中断服务函数  休眠标志置位和取消
入口参数：无   
出口参数：无   修改全局变量sleep_flag
******************************/
void input2_ISR (void) interrupt 10	              //外部中断2
{
     Delay_MS(25);
	 if (KEY1 == 0)	          
	 {
	 if(sleep_flag == 0)
		{sleep_flag = 1;}
	 else if(sleep_flag == 1)
	    sleep_flag = 0;
	 } 
}

/******************************
函数说明：定时器0中断服务函数  数码管扫描  摁键扫描
入口参数：无   
出口参数：无   修改全局变量sleep_flag
******************************/
//Time0扫描LED显示驱动，计时,500us
void time0(void) interrupt 1
{
    if(4==++Ledcount)		   //数码管扫描
    {Ledcount=0;
	}
    P2 = 0x0F;                 //关闭所有数码管选通
    P0 = Showtemp[Ledcount];       //送段码 低电平有效
    P2 = ~(0x01<<Ledcount);       //选择位选

    key_scan();				   //摁键扫描
}
/******************************
函数说明：工作模式选择 0测温度 1测电压
入口参数：无   
出口参数：无  
******************************/
void mood_choose_work(void)
{
 	

	    LED1 = 1;
		LED2 = 1;				  //模式2灯全灭
		voltage_2_5v_ad = Voltage_Measure2_5v_ad();//测2.5v标准电压
		temp_result = Temp_Measure_PT100(voltage_2_5v_ad);	//测温度
		LED_Translate(temp_result);			//转换出数码管显示值
	 
}

/******************************
函数说明：休眠状态检测 主循环调用 
入口参数：无   
出口参数：无  
******************************/
void sleep_check(void)
{
    if((sleep_flag == 1)||(sleep_flag1 == 1))	 //sleep_flag 摁键进入	 sleep_flag1 拨码开关进入
	{
		if(sleep_flag1 == 1)
		{turn_start = 0;sleep_flag1 = 0;}		
		LED_Clean();	   //关闭数码管
		LED2 = 1;LED1 = 0;Delay_MS(200);LED1 = 1;Delay_MS(200);LED1 = 0;Delay_MS(200);LED1 = 1;		   //休眠前红灯闪烁	
		PCON = 0x02;					   //进入休眠  程序停在此处
		_nop_();_nop_();_nop_();		   //唤醒后执行的空指令
		LED2 = 0;Delay_MS(200);LED2 = 1;Delay_MS(200);LED2 = 0;Delay_MS(200);LED2 = 1;Delay_MS(200);LED2 = 0;  //唤醒后绿灯闪烁
	}
}

/******************************
函数说明：摁键状态检测 主循环调用 	摁键正常工作摁下执行程序在此处
入口参数：无   
出口参数：无  
******************************/
void key_check(void)
{
	if(key_final == 1)   //	摁键1摁下执行程序位置
	{}
	if(key_final == 2)	 // 摁键2摁下执行程序位置
	{if(mood < 2) 
	      {mood ++;}	     
	 else{mood = 0;}
	 }
	if(key_final == 3)	 // 摁键3摁下执行程序位置
	{}
	key_final = 0;
}

/******************************
函数说明：摁键设置参数 处理程序	设置最大值最小值
入口参数：无   全局变量 MAXMINtemp[]	   loca_count   signal
出口参数： numresult 最大值最小值计算值
******************************/
float set_num_calculate()
{
	float numresult = 0;
	numresult = 10*(float)MAXMINtemp[0] + (float)MAXMINtemp[1] + 0.1*(float)MAXMINtemp[2];
	if (signal == 1)
	numresult = 0 - numresult;
	return numresult;
}

void main(void)
{  
   WDT_CONTR =0x3E;
   Init_ADC();       //初始化ADC 
   Timer0Init();     //设置T0用作数码管显示定时500us扫描，中断模式
   ET0=1;            //允许T0定时中断
   INT_CLKO |= 0x10; //允许外部中断2
   EA=1;			 //中断总使能
   Ledcount=0;      //LED扫描显示计数用
   P0M0=0xff;       //推挽输出驱动共阴极数码管  
   KEY1 = 1;KEY2 = 1;KEY3 = 1;			 //拉高所有摁键	 低电平有效
   TURN1 = 1;TURN2 = 1;TURN3 = 1;TURN4 = 1;		   //拉高所有拨码开关 低电平有效
   	       
   while(1)
   {	
		
		sleep_check();			   //休眠模式检测
        key_check();			   //摁键是否摁下检测
		mood_choose_work();		   //工作模式选择并work （测量）

		Delay_MS(200);			   //延时200ms
		WDT_CONTR =0x3E;	   	   //喂看门狗
   }
}		 