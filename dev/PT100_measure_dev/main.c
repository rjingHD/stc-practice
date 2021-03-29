#include "STC15F2K60S2.h"
#include "intrins.h"

#define ADC_POWER   0x80    //ADC��Դ����λ
#define ADC_FLAG    0x10    //ADC��ɱ�־λ
#define ADC_START   0x08    //ADC��������λ
#define ADC_SPEED0  0x00    //ADCת���ٶȣ�һ��ת����Ҫ540��ʱ��
#define ADC_SPEED1  0x20    //ADCת���ٶȣ�һ��ת����Ҫ360��ʱ��
#define ADC_SPEED2  0x40    //ADCת���ٶȣ�һ��ת����Ҫ180��ʱ��
#define ADC_SPEED3  0x60    //ADCת���ٶȣ�һ��ת����Ҫ90��ʱ��
  
/*����ܶ����  �͵�ƽ��Ч*///0    1 	2	 3	 4	   5	6	7  	 8	  9	   	A	 B    E	  F 	-    ��
unsigned char code Table[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0x86,0x8E,0xBF,0xFF};// �����ת���ֱ� 0-9,A-F GHPU ,-,��
unsigned char code Table1[]={'0','1','2','3','4','5','6','7','8','9','-'};//�ַ���ת���ֱ�0-9,-,��
unsigned char Sendtemp[6]={' ',' ',' ',' ',' ',' '};    //���ڴ�������
unsigned char Showtemp[4]={0,0,0,0};    //�������ʾ��ʾ����
unsigned char MAXMINtemp[3]={0,0,0};    //���ֵ��Сֵ�趨�ݴ�����

char loca_count =0;	 //
char signal = 0;	 //���ű���
float maxmum = 40; 	 //�¶����ֵ
float minmum = 20;	 //�¶���Сֵ
int maxmum1 = 0; 	 //���ֵ�ݴ�
int minmum1 = 0;     //��Сֵ�ݴ�
char flag1 = 0;		  
char error = 0;	     //����״̬���

float  VCC_measure  = 0;    //��ѹ����ֵ����
float temp_result = 0;		//�¶Ȳ���ֵ����
int voltage_2_5v_ad = 0;
unsigned char Ledcount = 0;    //�������ʾ����
unsigned char keycount = 0;    //����������ʱ20ms����
unsigned char keystatus = 0;   //��������״̬���� 
unsigned char key = 0; 		   // ����ѭ������
unsigned char key_final = 0;   //����̧�������ִ�б���
char mood = 0;                 //ϵͳ����ģʽ    0����   1��ѹ
char sleep_flag = 0;           //���߱�־
char sleep_flag1 = 0;
char turn_start =1;			   //
char PT100_error_flag = 0;
char alarm_mood = 0;		   //�¶������ޱ���ģʽѡ��

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
};//NTC��
float code PT100[14][2]={
//{25,0.620},{30,0.89},{35,1.18},{40,1.46},{45,1.75},{50,2.01},{55,2.37},{60,2.66},{65,2.95},{70,3.335},{75,3.60},{80,3.90}
{25,0.83},{30,1.10},{35,1.38},{40,1.65},{45,1.946},{50,2.248},{55,2.548},{60,2.84},{61,2.90},{62,2.95},
{63,3.02},{64,3.09},{65,3.20},{70,3.50}
};//NTC��
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
����˵������ʱ����  1-65535����
��ڲ�����ms=��ʱ������
���ڲ�������
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
����˵������ʼ��ADC�Ĵ���������P1.6ΪADC���빦��
��ڲ�������
���ڲ�������
******************************/
void Init_ADC(void)
{
    P1M1 = 0xFF;
	P1M0 = 0x00;
    P1ASF = 0x40;       //��P1.6�ڵ�ADC����
    //P1 &= 0xBF;         //����P1.6������͵�ƽȷ���ܲɼ����ⲿ��ƽ�ź�
	//P1 = 0x37;
    ADC_RES = 0;        //���ADCת������Ĵ���
    ADC_CONTR = ADC_POWER | ADC_SPEED3; //ʹ��A/D���磬����ת���ٶ�90T
}

/******************************
����˵������ѯ��ʽ��ȡADCת�����
��ڲ�����channel  ADC����ͨ��
���ڲ�����result   ADC_RES ADCת�����
******************************/
unsigned int Get_AD(unsigned char channel)
{
    unsigned int result;
    ADC_RES = 0;    
    ADC_RESL= 0;//���ADCת������Ĵ���
    ADC_CONTR =ADC_POWER|ADC_SPEED0|ADC_START|channel;//����ADC������ת��ͨ��������ת��
    _nop_();    _nop_();
    _nop_();    _nop_();            //�ȴ�����ADC_POWER���
    while (!(ADC_CONTR & ADC_FLAG));//��ȡת����ϱ�־λADC_FLAG
    ADC_CONTR &= ~ADC_FLAG;         //���ADC_FLAG��־λ
    result = ADC_RES<<2|ADC_RESL;   //��ȡ10λת��������浽result
    return result;                  //����ADCת�����10λ
}

/******************************
����˵������ѹֵ��������
��ڲ�������
���ڲ�����voltage �����õ��ĵ�ѹֵ
******************************/
float Voltage_Measure(void)
{	 int   i = 0,j = 0;
     float voltage = 0;
	 int voltage_AD_avg = 0; 	  //AD����ƽ��ֵ
	 unsigned int voltage_std_AD = 0;
	 for(j=0;j<10;j++)				 //10����ƽ�� �������1ms
	  {
		  voltage_std_AD = Get_AD(7);
		  Delay_MS(1);							   //�������1ms
		  voltage_AD_avg += voltage_std_AD;
	  }
	 voltage_AD_avg = voltage_AD_avg/10;    //  ��10��ƽ��
	 //voltage = (1024/(float)voltage_std_AD )*2.5;
	 voltage = (1067/(float)voltage_AD_avg )*2.5;
	 return voltage; 
}

float Voltage_Measure2_5v_ad(void)
{	 int   i = 0,j = 0;
     float voltage = 0;
	 int voltage_AD_avg = 0; 	  //AD����ƽ��ֵ
	 unsigned int voltage_std_AD = 0;
	 for(j=0;j<10;j++)				 //10����ƽ�� �������1ms
	  {
		  voltage_std_AD = Get_AD(7);
		  Delay_MS(1);							   //�������1ms
		  voltage_AD_avg += voltage_std_AD;
	  }
	 voltage_AD_avg = voltage_AD_avg/10;    //  ��10��ƽ��
	 //voltage = (1024/(float)voltage_std_AD )*2.5;
	 //voltage = (1067/(float)voltage_AD_avg )*2.5;
	 return voltage_AD_avg; 
}

/******************************
����˵�����¶�ֵ��������
��ڲ�������
���ڲ�����temperature �����õ����¶�ֵ
******************************/
float Temp_Measure()
{	  int   i = 0,j = 0;
      unsigned int voltage_AD = 0;	  //���β���ADֵ
	  long int voltage_AD_avg = 0; 	  //AD����ƽ��ֵ
	  float voltage = 0;              //����ad����ĵ�ѹֵ �����õ�Դ��ѹ����
	  float resistance = 0;	          //���ݵ�ѹ����Ĵ�������ֵ
	  float temperature = 0;		  //�¶�ֵ

	  for(j=0;j<10;j++)				 //10����ƽ�� �������1ms
	  {
		  voltage_AD = Get_AD(6);
		  Delay_MS(1);							   //�������1ms
		  voltage_AD_avg += voltage_AD;
	  }
	  
	  voltage_AD_avg = voltage_AD_avg/10;    //  ��10��ƽ�� 
	  if (voltage_AD_avg <5	)				 //������δ�����ж�  ���û��ѹ��û������
	  {error = 1;}
	  else
	  {error = 0;}
	  resistance = 10*(1024 - (float)voltage_AD_avg)/(float)voltage_AD_avg;	   //�������ֵ ��������ѹadΪ1024
	  for(i=0; i<145; i++)													   //��ѯ�����
	  {
	     if((resistance<=NTC[i][1]) && (resistance>NTC[i+1][1]) )	  //|| (resistance>=NTC[i][1])
		   {
		   		temperature = NTC[i][0] + (NTC[i][1]-resistance)/(NTC[i][1]-NTC[i+1][1]);break;	  //���Բ�ֵ��
		   }
		 else continue;
	  }		  
	  return temperature;
}

/******************************
����˵�����¶�ֵ��������    ʹ��PT100����������������������������������������������������������������������������������������
��ڲ�������
���ڲ�����temperature �����õ����¶�ֵ
******************************/
float Temp_Measure_PT100(int voltage_std_ad)
{	  int   i = 0,j = 0;
      unsigned int voltage_AD = 0;	  //���β���ADֵ
	  long int voltage_AD_avg = 0; 	  //AD����ƽ��ֵ
	  float voltage = 0;              //����ad����ĵ�ѹֵ �����õ�Դ��ѹ����
	  float temperature = 0;		  //�¶�ֵ
	  long float resistance = 0;
	  for(j=0;j<10;j++)				 //10����ƽ�� �������1ms
	  {
		  voltage_AD = Get_AD(3);    //PT100����ͨ��2
		  Delay_MS(1);							   //�������1ms
		  voltage_AD_avg += voltage_AD;
	  }
	  
	  voltage_AD_avg = voltage_AD_avg/10;    //  ��10��ƽ�� 
	  if (voltage_AD_avg <5)				 //������δ�����ж�  ���û��ѹ��û������
	  {error = 1;}
	  else
	  {error = 0;}
	  voltage = ((float)voltage_AD_avg/(float)voltage_std_ad)*2.5;
//	  resistance = (6200*(long float)voltage_AD_avg+10057565.6)/(121177.812-(long float)voltage_AD_avg);	   //�������ֵ ��������ѹadΪ1024
	  PT100_error_flag = 1;
	  for(i=0; i<13; i++)													   //��ѯ�����	   n-1
	  {
	     if(( voltage >= PT100[i][1]) && ( voltage < PT100[i+1][1]) )	  //|| (resistance>=NTC[i][1])
		   {
		   		temperature = PT100[i][0] + (voltage - PT100[i][1])/(PT100[i+1][1]-PT100[i][1])*(PT100[i+1][0]-PT100[i][0]);	  //���Բ�ֵ��
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
����˵������ת��Ϊ4λ�������ʾ��
��ڲ�����dat ��ת��������
���ڲ������� ��������ΪShowtemp[4]
******************************/
void LED_Translate(float dat)
{
    int data1 = 0;
	
	if ((dat<10)&&(dat>-10))
	{
		data1 = (int)(dat*100);
			if(dat >= 0)	
			{Showtemp[0] = 0xFF;}			   //����ܵ�1λ��ʾ ����λ
			if(dat < 0)	
			{Showtemp[0] = 0xBF;
			data1 = 0-data1;}
		Showtemp[1] =  Table[(data1/100)]&0x7F;	      //��λ����ʾ  ��С����						
		Showtemp[2] =  Table[data1%100/10];         //С�����һλ��ʾ          
		Showtemp[3] =  Table[data1%10];   		  //С�������λ��ʾ
    }
	else if ((dat<100)&&(dat>-100))
	{
		data1 = (int)(dat*10);
			if(dat >= 0)	
			{Showtemp[0] = 0xFF;}			        //����ܵ�1λ��ʾ	����λ
			if(dat < 0)	
			{Showtemp[0] = 0xBF;
			data1 = 0-data1;}
		Showtemp[1] =  Table[data1/100];	      	 //ʮλ����ʾ					
		Showtemp[2] =  Table[data1%100/10]&0x7F;     //��λ����ʾ  ��С����          
		Showtemp[3] =  Table[data1%10];   		     //С�����һλ��ʾ
    }
	else if ((dat>=100)&&(dat<1000))
	{
		 data1 = (int)(dat*10);
		 Showtemp[0] =  Table[data1/1000];	      	  //��λ����ʾ
		 Showtemp[1] =  Table[data1%1000/100];	      	  //ʮλ����ʾ					
	 	 Showtemp[2] =  Table[data1%100/10]&0x7F;     //��λ����ʾ  ��С����          
		 Showtemp[3] =  Table[data1%10];   		      //С�����һλ��ʾ
	}
	else if (dat>=1000)
	{
		 Showtemp[0] =  Table[(int)dat/1000];	   //ǧλ����ʾ
		 Showtemp[1] =  Table[(int)dat%1000/100];   //��λ����ʾ					
	 	 Showtemp[2] =  Table[(int)dat%100/10];     //ʮλ����ʾ
		 Showtemp[3] =  Table[(int)dat%10];			//��
	}						
}
/******************************
����˵������ת��Ϊ6λ�ַ��� ���һλΪ�ո�
��ڲ�����dat ��ת��������
���ڲ������� ��������ΪSendtemp[6]
******************************/
void STR_Translate(float dat)
{
    int data1 = 0;
	Sendtemp[0] =  ' ';	      //��λ����ʾ  ��С����						
	Sendtemp[1] =  ' ';         //С�����һλ��ʾ          
	Sendtemp[2] =  ' ';   		  //С�������λ��ʾ
	Sendtemp[3] =  ' ';	      //��λ����ʾ  ��С����						
	Sendtemp[4] =  ' ';         //С�����һλ��ʾ
	Sendtemp[5] =  ' ';         //С�����һλ��ʾ          
	if ((dat<10)&&(dat>-10))
	{
		data1 = (int)(dat*100);
			if(dat >= 0)	
			{Sendtemp[0] = ' ';}			   //����ܵ�1λ��ʾ
			if(dat < 0)	
			{Sendtemp[0] = '-';
			data1 = 0-data1;}
		Sendtemp[1] =  Table1[(data1/100)];	      //��λ����ʾ  ��С����
		Sendtemp[2] =  '.';						
		Sendtemp[3] =  Table1[data1%100/10];         //С�����һλ��ʾ          
		Sendtemp[4] =  Table1[data1%10];   		  //С�������λ��ʾ
    }
	else if ((dat<100)&&(dat>-100))
	{
		data1 = (int)(dat*10);
			if(dat >= 0)	
			{Sendtemp[0] = ' ';}			   //����ܵ�1λ��ʾ
			if(dat < 0)	
			{Sendtemp[0] = '-';
			data1 = 0-data1;}
		Sendtemp[1] =  Table1[data1/100];	      	  //ʮλ����ʾ					
		Sendtemp[2] =  Table1[data1%100/10];
		Sendtemp[3] =  '.';                       //��λ����ʾ  ��С����          
		Sendtemp[4] =  Table1[data1%10];   		  //С�����һλ��ʾ
    }
	else if ((dat>=100)&&(dat<1000))
	{
		 data1 = (int)(dat*10);
		 Sendtemp[0] =  Table1[data1/1000];	      	  //baiλ����ʾ
		 Sendtemp[1] =  Table1[data1/100];	      	  //ʮλ����ʾ					
	 	 Sendtemp[2] =  Table1[data1%100/10];     //��λ����ʾ  ��С����
		 Sendtemp[3] =  '.';         
		 Sendtemp[4] =  Table1[data1%10];   		  //С�����һλ��ʾ
	}
	else if (dat>=1000)
	{
		 Sendtemp[0] =  Table1[(int)dat/1000];	   //baiλ����ʾ
		 Sendtemp[1] =  Table1[(int)dat%1000/100];   //ʮλ����ʾ					
	 	 Sendtemp[2] =  Table1[(int)dat%100/10];     //��λ����ʾ
		 Sendtemp[3] =  Table1[(int)dat%10];
	}						
}

/******************************
����˵���������ȫ�� ��������ж�ִ�� ȫ�� �ĸ�λ ����
��ڲ�������
���ڲ������� 
******************************/
void LED_Clean(void)	         //LED��ȫ����
{	int ii = 0;
	Showtemp[0] = 0xFF;	
	Showtemp[1] = 0xFF; 
	Showtemp[2] = 0xFF; 
	Showtemp[3] = 0xFF;
	for(ii=0; ii<4; ii++)
	{
	    P2 = 0x0F;               //�ر����������ѡͨ
        P0 = Showtemp[ii];       //�Ͷ��� �͵�ƽ��Ч
        P2 = ~(0x01<<ii);    }   //ѡ��λѡ
}
/******************************
����˵��������ɨ�躯�� �ж�ִ�� 
��ڲ�������   ȫ�ֱ���   keystatus  keycount  key_final
���ڲ������� 
******************************/
void key_scan(void)
{
   	if (keystatus == 0)		  	 //��������
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
		if(keycount==50)			//�������
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
   if(key == 1)						//����̧��ȷ�� 	key_final��λ����һ�����������������
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
����˵������ʱ��0��ʼ��  16λ��ʱ���Զ���װ��ģʽ
��ڲ�������   
���ڲ������� 
******************************/
void Timer0Init(void)		//500΢��@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ

	TL0 = 0x66;		    //���ö�ʱ��ֵ
	TH0 = 0xEA;	   	    //���ö�ʱ��ֵ
	TF0 = 0;	     	//���TF0��־
	TR0 = 1;		    //��ʱ��0��ʼ��ʱ
}

/******************************
����˵�����ⲿ�ж�2�жϷ�����  ���߱�־��λ��ȡ��
��ڲ�������   
���ڲ�������   �޸�ȫ�ֱ���sleep_flag
******************************/
void input2_ISR (void) interrupt 10	              //�ⲿ�ж�2
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
����˵������ʱ��0�жϷ�����  �����ɨ��  ����ɨ��
��ڲ�������   
���ڲ�������   �޸�ȫ�ֱ���sleep_flag
******************************/
//Time0ɨ��LED��ʾ��������ʱ,500us
void time0(void) interrupt 1
{
    if(4==++Ledcount)		   //�����ɨ��
    {Ledcount=0;
	}
    P2 = 0x0F;                 //�ر����������ѡͨ
    P0 = Showtemp[Ledcount];       //�Ͷ��� �͵�ƽ��Ч
    P2 = ~(0x01<<Ledcount);       //ѡ��λѡ

    key_scan();				   //����ɨ��
}
/******************************
����˵��������ģʽѡ�� 0���¶� 1���ѹ
��ڲ�������   
���ڲ�������  
******************************/
void mood_choose_work(void)
{
 	if(mood == 0)
	{

		temp_result = Temp_Measure();	//���¶�
		LED_Translate(temp_result);		//ת�����������ʾֵ
		STR_Translate(temp_result);		//ת�������ڴ���ֵ
		
		if(alarm_mood == 1)								   //����ģʽ
		{
			if((temp_result>maxmum)||(temp_result<minmum))
			{
				if(temp_result>maxmum)
					{LED2 = ~LED2;		  //�����ֵ�̵���	��Ƴ���
					LED1 = 1;}
				if(temp_result<minmum)
				    {LED1 = ~LED1;		   //������Сֵ����� �̵Ƴ���
					LED2 = 1;}
			}
			else						  //����ģʽ������ ȷ���̵��������
			{	    
			LED1 = 1;
			LED2 = 0;}				
		}
		else if(mood != 2)											  //�Ǳ���ģʽ ȷ���̵��������
		{
			LED1 = 1;
			LED2 = 0;
		}
	}   		
	 if(mood == 1)
	{
	    LED1 = 0;
		LED2 = 1;				  //ģʽ1�����
		VCC_measure = Voltage_Measure();	//���ѹ
		LED_Translate(VCC_measure);			//ת�����������ʾֵ
	    STR_Translate(VCC_measure);			//ת�������ڴ���ֵ
	}
	if(mood == 2)
	{
	    LED1 = 1;
		LED2 = 1;				  //ģʽ2��ȫ��
		voltage_2_5v_ad = Voltage_Measure2_5v_ad();//��2.5v��׼��ѹ
		temp_result = Temp_Measure_PT100(voltage_2_5v_ad);	//���¶�
		LED_Translate(temp_result);			//ת�����������ʾֵ
	    STR_Translate(temp_result);			//ת�������ڴ���ֵ
	}
	 
}

/******************************
����˵��������״̬��� ��ѭ������ 
��ڲ�������   
���ڲ�������  
******************************/
void sleep_check(void)
{
    if((sleep_flag == 1)||(sleep_flag1 == 1))	 //sleep_flag ��������	 sleep_flag1 ���뿪�ؽ���
	{
		if(sleep_flag1 == 1)
		{turn_start = 0;sleep_flag1 = 0;}		
		LED_Clean();	   //�ر������
		LED2 = 1;LED1 = 0;Delay_MS(200);LED1 = 1;Delay_MS(200);LED1 = 0;Delay_MS(200);LED1 = 1;		   //����ǰ�����˸	
		PCON = 0x02;					   //��������  ����ͣ�ڴ˴�
		_nop_();_nop_();_nop_();		   //���Ѻ�ִ�еĿ�ָ��
		LED2 = 0;Delay_MS(200);LED2 = 1;Delay_MS(200);LED2 = 0;Delay_MS(200);LED2 = 1;Delay_MS(200);LED2 = 0;  //���Ѻ��̵���˸
	}
}

/******************************
����˵��������״̬��� ��ѭ������ 	����������������ִ�г����ڴ˴�
��ڲ�������   
���ڲ�������  
******************************/
void key_check(void)
{
	if(key_final == 1)   //	����1����ִ�г���λ��
	{}
	if(key_final == 2)	 // ����2����ִ�г���λ��
	{if(mood < 2) 
	      {mood ++;}	     
	 else{mood = 0;}
	 }
	if(key_final == 3)	 // ����3����ִ�г���λ��
	{}
	key_final = 0;
}
/******************************
����˵�����������ò��� �������	�������ֵ��Сֵ
��ڲ�������   
���ڲ�������  	Ӱ��ȫ�ֱ��� MAXMINtemp[]	   loca_count   signal
******************************/
void key_check_setnum(void)
{	
    if(loca_count<3)	 //��λʮλ С�����һλ ����
	{
		if(key_final == 1)   //	����1����ִ�г���λ��  ��ǰλ��1
		{if(MAXMINtemp[loca_count]<9)
				MAXMINtemp[loca_count]++;
			else
				MAXMINtemp[loca_count] = 0;}
		if(key_final == 2)	 // ����2����ִ�г���λ��  ��ǰλ��1
		{
		  if(MAXMINtemp[loca_count]>0)
				MAXMINtemp[loca_count]--;
			else
				MAXMINtemp[loca_count] = 9;
		}
	}

	if(loca_count == 3)	     //����λ����
	{
		if(key_final == 1)   //	����1����ִ�г���λ��  ȡ��
		{if(signal == 1) 
	      signal = 0;
	 else if(signal == 0)  
	      signal = 1;}
		if(key_final == 2)   //	����2����ִ�г���λ��  ȡ��
		{if(signal == 1) 
	      signal = 0;
	 else if(signal == 0)  
	      signal = 1;}
	}
	if(key_final == 3)	     // ����3����ִ�г���λ��
	{
		if(loca_count<3)
			loca_count++;
		else
			loca_count = 0;
	}
	key_final = 0;
}
/******************************
����˵�����������ò��� �������	�������ֵ��Сֵ
��ڲ�������   ȫ�ֱ��� MAXMINtemp[]	   loca_count   signal
���ڲ����� numresult ���ֵ��Сֵ����ֵ
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
����˵�������뿪��״̬��� ��ѭ������ 	���뿪��������������ִ�г����ڴ˴�
��ڲ�������   
���ڲ�������  
******************************/
void turn_check(void)
{
	while((TURN1 == 0)&& (TURN2 != 0))	   //����1Ϊon ��2����
	{	LED1 = 0;
		LED2 = 0;
		INT_CLKO = 0x00; //�����ⲿ�ж�2
		if(flag1==0)
		{
			flag1 = 1;		
			if ((maxmum<100)&&(maxmum>-100))	  //��һ���ַ�ֹ������min ����maxʱ ��ʾ�����ó�ʼֵ��min��
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
		WDT_CONTR =0x3E;			//ι���Ź�
	}
	INT_CLKO |= 0x10; //�����ⲿ�ж�2

	while((TURN2 == 0)&& (TURN1 != 0))	   //����2Ϊon ��1����
	{
	    LED1 = 0;
		LED2 = 0;
		INT_CLKO = 0x00; //�����ⲿ�ж�2
		if(flag1==0)
	    {
			flag1 = 1;			
			if ((minmum<100)&&(minmum>-100))     //��һ���ַ�ֹ������max ����minʱ ��ʾ�����ó�ʼֵ��max��
			{
				minmum1 = (int)(minmum*10);
				if(minmum >= 0)	
				{signal = 0;}			   //����ܵ�1λ��ʾ
				if(minmum < 0)	
				{signal = 1;
				minmum1 = 0-minmum1;}
				MAXMINtemp[0] =  minmum1/100;	 					
				MAXMINtemp[1] =  minmum1%100/10;              
				MAXMINtemp[2] =  minmum1%10;   		  
		    }
		}
		key_check_setnum();				 
		minmum = set_num_calculate();	 //	 ��ֵ
		LED_Translate(minmum);			 //��ʾ��Сֵ
		WDT_CONTR =0x3E;			 //ι���Ź�
	}
	INT_CLKO |= 0x10; //�����ⲿ�ж�2
	flag1 = 0;
	  if(TURN3 == 0)					   //����3ѡ���Ƿ���������ޱ�������
	  {
	  		alarm_mood = 1;
	  }
	  else
	  {
		  alarm_mood = 0;
		  LED2 = 0;
	  }
//     if(TURN4 == 0)						//����4��������ģʽ
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
����˵��������1��ʼ������ 9600bps
��ڲ�������   
���ڲ�������  
******************************/
void UartInit(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xE0;		//�趨��ʱ��ֵ
	TH1 = 0xFE;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
}
/******************************
����˵��������1�жϷ������ �������ݵ�����
��ڲ�������   	����ȫ�ֱ��� Sendtemp[] һ�κ�����ȫ������
���ڲ�������  
******************************/
void UART1_ISR() interrupt 4
{   
	if (RI)					 //����յ�������Ϣ
	{   
	    SBUF =Sendtemp[0] ;	  //������������    
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
	   TI = 0;				 //�巢�ͱ�־
}
void main(void)
{  
   WDT_CONTR =0x3E;
   Init_ADC();       //��ʼ��ADC
   UartInit();
   ES = 1; 
   Timer0Init();     //����T0�����������ʾ��ʱ500usɨ�裬�ж�ģʽ
   ET0=1;            //����T0��ʱ�ж�
   INT_CLKO |= 0x10; //�����ⲿ�ж�2
   EA=1;			 //�ж���ʹ��
   Ledcount=0;      //LEDɨ����ʾ������
   P0M0=0xff;       //����������������������  
   KEY1 = 1;KEY2 = 1;KEY3 = 1;			 //������������	 �͵�ƽ��Ч
   TURN1 = 1;TURN2 = 1;TURN3 = 1;TURN4 = 1;		   //�������в��뿪�� �͵�ƽ��Ч
   	    
   Showtemp[0]=Table[1];
   Showtemp[1]=Table[0];
   Showtemp[2]=Table[2];
   Showtemp[3]=Table[7];

   Delay_MS(2000);
   
   
   while(1)
   {	
		turn_check();			   //���뿪�ؼ��
		sleep_check();			   //����ģʽ���
        key_check();			   //�����Ƿ����¼��
		mood_choose_work();		   //����ģʽѡ��work ��������
		if(maxmum<minmum)		   //����error
		error = 2;
		if(error == 1)
		{
			Showtemp[0]=0xff;
   			Showtemp[1]=0xff;
   			Showtemp[2]=0x86;   //E
   			Showtemp[3]=Table[1];	//��ʾE1
		}
		if (error == 2)
		{
			Showtemp[0]=0xff;
   			Showtemp[1]=0xff;
   			Showtemp[2]=0x86; //E
   			Showtemp[3]=Table[2];	//��ʾE2
		}
		if(PT100_error_flag == 1)
		{
			Showtemp[0]=0xff;
   			Showtemp[1]=0xff;
   			Showtemp[2]=0x86; //E
   			Showtemp[3]=Table[3];	//��ʾE3
			PT100_error_flag = 0;
		}
		Delay_MS(200);			   //��ʱ200ms
		WDT_CONTR =0x3E;	   	   //ι���Ź�
   }
}		 