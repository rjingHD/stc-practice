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
unsigned char code Table1[]={'0','1','2','3','4','5','6','7','8','9','-'};//字符串转换字表0-9,-,灭
unsigned char Sendtemp[6]={' ',' ',' ',' ',' ',' '};    //串口传输数组
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
char PT100_error_flag = 0;
char alarm_mood = 0;		   //温度上下限报警模式选择

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
};//NTC表
float code PT100[14][2]={
//{25,0.620},{30,0.89},{35,1.18},{40,1.46},{45,1.75},{50,2.01},{55,2.37},{60,2.66},{65,2.95},{70,3.335},{75,3.60},{80,3.90}
{25,0.83},{30,1.10},{35,1.38},{40,1.65},{45,1.946},{50,2.248},{55,2.548},{60,2.84},{61,2.90},{62,2.95},
{63,3.02},{64,3.09},{65,3.20},{70,3.50}
};//NTC表
sbit LED1 = P4^2;
sbit LED2 = P4^4;
sbit KEY3 = P3^4;
sbit KEY2 = P3^5;
sbit KEY1 = P3^6;
sbit TURN1 = P2^7;
sbit TURN2 = P2^6;
sbit TURN3 = P2^5;
sbit TURN4 = P2^4;
sbit FAN = 	P3^2;			 
sbit BUZZ = P3^3;

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
	 //voltage = (1024/(float)voltage_std_AD )*2.5;
	 //voltage = (1067/(float)voltage_AD_avg )*2.5;
	 return voltage_AD_avg; 
}

/******************************
函数说明：温度值测量函数
入口参数：无
出口参数：temperature 计算后得到的温度值
******************************/
float Temp_Measure()
{	  int   i = 0,j = 0;
      unsigned int voltage_AD = 0;	  //本次采样AD值
	  long int voltage_AD_avg = 0; 	  //AD采样平均值
	  float voltage = 0;              //根据ad求出的电压值 不利用电源电压的求法
	  float resistance = 0;	          //根据电压算出的传感器阻值
	  float temperature = 0;		  //温度值

	  for(j=0;j<10;j++)				 //10次求平均 采样间隔1ms
	  {
		  voltage_AD = Get_AD(6);
		  Delay_MS(1);							   //采样间隔1ms
		  voltage_AD_avg += voltage_AD;
	  }
	  
	  voltage_AD_avg = voltage_AD_avg/10;    //  除10求平均 
	  if (voltage_AD_avg <5	)				 //传感器未连接判断  如果没电压则没有连接
	  {error = 1;}
	  else
	  {error = 0;}
	  resistance = 10*(1024 - (float)voltage_AD_avg)/(float)voltage_AD_avg;	   //计算电阻值 根据满电压ad为1024
	  for(i=0; i<145; i++)													   //轮询法查表
	  {
	     if((resistance<=NTC[i][1]) && (resistance>NTC[i+1][1]) )	  //|| (resistance>=NTC[i][1])
		   {
		   		temperature = NTC[i][0] + (NTC[i][1]-resistance)/(NTC[i][1]-NTC[i+1][1]);break;	  //线性插值法
		   }
		 else continue;
	  }		  
	  return temperature;
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
	  PT100_error_flag = 1;
	  for(i=0; i<13; i++)													   //轮询法查表	   n-1
	  {
	     if(( voltage >= PT100[i][1]) && ( voltage < PT100[i+1][1]) )	  //|| (resistance>=NTC[i][1])
		   {
		   		temperature = PT100[i][0] + (voltage - PT100[i][1])/(PT100[i+1][1]-PT100[i][1])*(PT100[i+1][0]-PT100[i][0]);	  //线性插值法
				PT100_error_flag = 0;
				break;
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
函数说明：数转换为6位字符串 最后一位为空格
入口参数：dat 需转化的数字
出口参数：无 操作对象为Sendtemp[6]
******************************/
void STR_Translate(float dat)
{
    int data1 = 0;
	Sendtemp[0] =  ' ';	      //个位数显示  加小数点						
	Sendtemp[1] =  ' ';         //小数点后一位显示          
	Sendtemp[2] =  ' ';   		  //小数点后两位显示
	Sendtemp[3] =  ' ';	      //个位数显示  加小数点						
	Sendtemp[4] =  ' ';         //小数点后一位显示
	Sendtemp[5] =  ' ';         //小数点后一位显示          
	if ((dat<10)&&(dat>-10))
	{
		data1 = (int)(dat*100);
			if(dat >= 0)	
			{Sendtemp[0] = ' ';}			   //数码管第1位显示
			if(dat < 0)	
			{Sendtemp[0] = '-';
			data1 = 0-data1;}
		Sendtemp[1] =  Table1[(data1/100)];	      //个位数显示  加小数点
		Sendtemp[2] =  '.';						
		Sendtemp[3] =  Table1[data1%100/10];         //小数点后一位显示          
		Sendtemp[4] =  Table1[data1%10];   		  //小数点后两位显示
    }
	else if ((dat<100)&&(dat>-100))
	{
		data1 = (int)(dat*10);
			if(dat >= 0)	
			{Sendtemp[0] = ' ';}			   //数码管第1位显示
			if(dat < 0)	
			{Sendtemp[0] = '-';
			data1 = 0-data1;}
		Sendtemp[1] =  Table1[data1/100];	      	  //十位数显示					
		Sendtemp[2] =  Table1[data1%100/10];
		Sendtemp[3] =  '.';                       //个位数显示  加小数点          
		Sendtemp[4] =  Table1[data1%10];   		  //小数点后一位显示
    }
	else if ((dat>=100)&&(dat<1000))
	{
		 data1 = (int)(dat*10);
		 Sendtemp[0] =  Table1[data1/1000];	      	  //bai位数显示
		 Sendtemp[1] =  Table1[data1/100];	      	  //十位数显示					
	 	 Sendtemp[2] =  Table1[data1%100/10];     //个位数显示  加小数点
		 Sendtemp[3] =  '.';         
		 Sendtemp[4] =  Table1[data1%10];   		  //小数点后一位显示
	}
	else if (dat>=1000)
	{
		 Sendtemp[0] =  Table1[(int)dat/1000];	   //bai位数显示
		 Sendtemp[1] =  Table1[(int)dat%1000/100];   //十位数显示					
	 	 Sendtemp[2] =  Table1[(int)dat%100/10];     //个位数显示
		 Sendtemp[3] =  Table1[(int)dat%10];
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
 	if(mood == 0)
	{

		temp_result = Temp_Measure();	//测温度
		LED_Translate(temp_result);		//转换出数码管显示值
		STR_Translate(temp_result);		//转换出串口传输值
		
		if(alarm_mood == 1)								   //报警模式
		{
			if((temp_result>maxmum)||(temp_result<minmum))
			{
				if(temp_result>maxmum)
					{LED2 = ~LED2;		  //超最大值绿灯闪	红灯常灭
					LED1 = 1;}
				if(temp_result<minmum)
				    {LED1 = ~LED1;		   //低于最小值红灯闪 绿灯长灭
					LED2 = 1;}
			}
			else						  //报警模式正常下 确保绿灯亮红灯灭
			{	    
			LED1 = 1;
			LED2 = 0;}				
		}
		else if(mood != 2)											  //非报警模式 确保绿灯亮红灯灭
		{
			LED1 = 1;
			LED2 = 0;
		}
	}   		
	 if(mood == 1)
	{
	    LED1 = 0;
		LED2 = 1;				  //模式1红灯亮
		VCC_measure = Voltage_Measure();	//测电压
		LED_Translate(VCC_measure);			//转换出数码管显示值
	    STR_Translate(VCC_measure);			//转换出串口传输值
	}
	if(mood == 2)
	{
	    LED1 = 1;
		LED2 = 1;				  //模式2灯全灭
		voltage_2_5v_ad = Voltage_Measure2_5v_ad();//测2.5v标准电压
		temp_result = Temp_Measure_PT100(voltage_2_5v_ad);	//测温度
		LED_Translate(temp_result);			//转换出数码管显示值
	    STR_Translate(temp_result);			//转换出串口传输值
	}
	 
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
入口参数：无   
出口参数：无  	影响全局变量 MAXMINtemp[]	   loca_count   signal
******************************/
void key_check_setnum(void)
{	
    if(loca_count<3)	 //个位十位 小数点后一位 处理
	{
		if(key_final == 1)   //	摁键1摁下执行程序位置  当前位加1
		{if(MAXMINtemp[loca_count]<9)
				MAXMINtemp[loca_count]++;
			else
				MAXMINtemp[loca_count] = 0;}
		if(key_final == 2)	 // 摁键2摁下执行程序位置  当前位减1
		{
		  if(MAXMINtemp[loca_count]>0)
				MAXMINtemp[loca_count]--;
			else
				MAXMINtemp[loca_count] = 9;
		}
	}

	if(loca_count == 3)	     //符号位处理
	{
		if(key_final == 1)   //	摁键1摁下执行程序位置  取反
		{if(signal == 1) 
	      signal = 0;
	 else if(signal == 0)  
	      signal = 1;}
		if(key_final == 2)   //	摁键2摁下执行程序位置  取反
		{if(signal == 1) 
	      signal = 0;
	 else if(signal == 0)  
	      signal = 1;}
	}
	if(key_final == 3)	     // 摁键3摁下执行程序位置
	{
		if(loca_count<3)
			loca_count++;
		else
			loca_count = 0;
	}
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

/******************************
函数说明：拨码开关状态检测 主循环调用 	拨码开关正常工作摁下执行程序在此处
入口参数：无   
出口参数：无  
******************************/
void turn_check(void)
{
	while((TURN1 == 0)&& (TURN2 != 0))	   //拨码1为on 与2互锁
	{	LED1 = 0;
		LED2 = 0;
		INT_CLKO = 0x00; //禁用外部中断2
		if(flag1==0)
		{
			flag1 = 1;		
			if ((maxmum<100)&&(maxmum>-100))	  //这一部分防止设置完min 设置max时 显示的设置初始值是min的
			{
				maxmum1 = (int)(maxmum*10);
					if(maxmum >= 0)	
					{signal = 0;}			   
					if(maxmum < 0)	
					{signal = 1;
					maxmum1 = 0-maxmum1;}
				MAXMINtemp[0] =  maxmum1/100;	  					
				MAXMINtemp[1] =  maxmum1%100/10;              
				MAXMINtemp[2] =  maxmum1%10;   		  
		    }
		}
		key_check_setnum();
		maxmum = set_num_calculate();
		LED_Translate(maxmum);
		WDT_CONTR =0x3E;			//喂看门狗
	}
	INT_CLKO |= 0x10; //允许外部中断2

	while((TURN2 == 0)&& (TURN1 != 0))	   //拨码2为on 与1互锁
	{
	    LED1 = 0;
		LED2 = 0;
		INT_CLKO = 0x00; //禁用外部中断2
		if(flag1==0)
	    {
			flag1 = 1;			
			if ((minmum<100)&&(minmum>-100))     //这一部分防止设置完max 设置min时 显示的设置初始值是max的
			{
				minmum1 = (int)(minmum*10);
				if(minmum >= 0)	
				{signal = 0;}			   //数码管第1位显示
				if(minmum < 0)	
				{signal = 1;
				minmum1 = 0-minmum1;}
				MAXMINtemp[0] =  minmum1/100;	 					
				MAXMINtemp[1] =  minmum1%100/10;              
				MAXMINtemp[2] =  minmum1%10;   		  
		    }
		}
		key_check_setnum();				 
		minmum = set_num_calculate();	 //	 赋值
		LED_Translate(minmum);			 //显示最小值
		WDT_CONTR =0x3E;			 //喂看门狗
	}
	INT_CLKO |= 0x10; //允许外部中断2
	flag1 = 0;
	  if(TURN3 == 0)					   //拨码3选择是否进入上下限报警报警
	  {
	  		alarm_mood = 1;
	  }
	  else
	  {
		  alarm_mood = 0;
		  LED2 = 0;
	  }
//     if(TURN4 == 0)						//拨码4进入休眠模式
//	{
//		Delay_MS(50);
//	    if(TURN4 == 0)
//		{
//			if((sleep_flag1 == 0)&&(turn_start == 1))
//			{sleep_flag1 = 1;}
//		}
//	}
//	else if	(turn_start == 0)
//	{turn_start = 1;}
}
/******************************
函数说明：串口1初始化程序 9600bps
入口参数：无   
出口参数：无  
******************************/
void UartInit(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xE0;		//设定定时初值
	TH1 = 0xFE;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}
/******************************
函数说明：串口1中断服务程序 发送数据到串口
入口参数：无   	发送全局变量 Sendtemp[] 一次函数发全部数据
出口参数：无  
******************************/
void UART1_ISR() interrupt 4
{   
	if (RI)					 //如果收到请求信息
	{   
	    SBUF =Sendtemp[0] ;	  //发送整个数组    
		Delay_MS(2);						
		SBUF =Sendtemp[1] ;            
		Delay_MS(2);       
		SBUF =Sendtemp[2] ;   	
		Delay_MS(2);
		SBUF =Sendtemp[3];	      
		Delay_MS(2);						
		SBUF =Sendtemp[4];        
		Delay_MS(2);
		SBUF =Sendtemp[5];       
		Delay_MS(2);	   
	 
	   RI = 0;
	}
	else
	   TI = 0;				 //清发送标志
}
void main(void)
{  
   WDT_CONTR =0x3E;
   Init_ADC();       //初始化ADC
   UartInit();
   ES = 1; 
   Timer0Init();     //设置T0用作数码管显示定时500us扫描，中断模式
   ET0=1;            //允许T0定时中断
   INT_CLKO |= 0x10; //允许外部中断2
   EA=1;			 //中断总使能
   Ledcount=0;      //LED扫描显示计数用
   P0M0=0xff;       //推挽输出驱动共阴极数码管  
   KEY1 = 1;KEY2 = 1;KEY3 = 1;			 //拉高所有摁键	 低电平有效
   TURN1 = 1;TURN2 = 1;TURN3 = 1;TURN4 = 1;		   //拉高所有拨码开关 低电平有效
   	    
   Showtemp[0]=Table[1];
   Showtemp[1]=Table[0];
   Showtemp[2]=Table[2];
   Showtemp[3]=Table[7];

   Delay_MS(2000);
   
   
   while(1)
   {	
		turn_check();			   //拨码开关检测
		sleep_check();			   //休眠模式检测
        key_check();			   //摁键是否摁下检测
		mood_choose_work();		   //工作模式选择并work （测量）
		if(maxmum<minmum)		   //两种error
		error = 2;
		if(error == 1)
		{
			Showtemp[0]=0xff;
   			Showtemp[1]=0xff;
   			Showtemp[2]=0x86;   //E
   			Showtemp[3]=Table[1];	//显示E1
		}
		if (error == 2)
		{
			Showtemp[0]=0xff;
   			Showtemp[1]=0xff;
   			Showtemp[2]=0x86; //E
   			Showtemp[3]=Table[2];	//显示E2
		}
		if(PT100_error_flag == 1)
		{
			Showtemp[0]=0xff;
   			Showtemp[1]=0xff;
   			Showtemp[2]=0x86; //E
   			Showtemp[3]=Table[3];	//显示E3
			PT100_error_flag = 0;
		}
		Delay_MS(200);			   //延时200ms
		WDT_CONTR =0x3E;	   	   //喂看门狗
   }
}		 