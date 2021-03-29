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

char alarm_mood = 0;		   //�¶������ޱ���ģʽѡ��


float code PT100[12][2]={
{25,0.620},{30,0.89},{35,1.18},{40,1.46},{45,1.75},{50,2.01},{55,2.37},{60,2.66},{65,2.95},{70,3.335},{75,3.60},{80,3.90}
};//PT100��
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
	 return voltage_AD_avg; 
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

	  for(i=0; i<11; i++)													   //��ѯ�����
	  {
	     if(( voltage >= PT100[i][1]) && ( voltage < PT100[i+1][1]) )	  //|| (resistance>=NTC[i][1])
		   {
		   		temperature = PT100[i][0] + (voltage - PT100[i][1])/(PT100[i+1][1]-PT100[i][1])*5;break;	  //���Բ�ֵ��
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
 	

	    LED1 = 1;
		LED2 = 1;				  //ģʽ2��ȫ��
		voltage_2_5v_ad = Voltage_Measure2_5v_ad();//��2.5v��׼��ѹ
		temp_result = Temp_Measure_PT100(voltage_2_5v_ad);	//���¶�
		LED_Translate(temp_result);			//ת�����������ʾֵ
	 
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

void main(void)
{  
   WDT_CONTR =0x3E;
   Init_ADC();       //��ʼ��ADC 
   Timer0Init();     //����T0�����������ʾ��ʱ500usɨ�裬�ж�ģʽ
   ET0=1;            //����T0��ʱ�ж�
   INT_CLKO |= 0x10; //�����ⲿ�ж�2
   EA=1;			 //�ж���ʹ��
   Ledcount=0;      //LEDɨ����ʾ������
   P0M0=0xff;       //����������������������  
   KEY1 = 1;KEY2 = 1;KEY3 = 1;			 //������������	 �͵�ƽ��Ч
   TURN1 = 1;TURN2 = 1;TURN3 = 1;TURN4 = 1;		   //�������в��뿪�� �͵�ƽ��Ч
   	       
   while(1)
   {	
		
		sleep_check();			   //����ģʽ���
        key_check();			   //�����Ƿ����¼��
		mood_choose_work();		   //����ģʽѡ��work ��������

		Delay_MS(200);			   //��ʱ200ms
		WDT_CONTR =0x3E;	   	   //ι���Ź�
   }
}		 