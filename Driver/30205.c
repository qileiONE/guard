#include "stdio.h"
#include "30205.h" 
#include "delay.h" 

extern   float tmpCaliK;
extern   float tmpCaliB;
//float temperature=0;
 void shutdown(void)
 {
 
 
 }
 void max_init (void)
 {
	 IIC_Init();

   	I2CwriteByte(MAX30205_ADDRESS, MAX30205_CONFIGURATION, 0x00);
	I2CwriteByte(MAX30205_ADDRESS, MAX30205_THYST ,        0x00);
  	I2CwriteByte(MAX30205_ADDRESS, MAX30205_TOS, 0x00);
 } 
// float GetTemperature(void)
// {  
//	  float temperature1=0.0;
//    u8 readRaw[2]={0,0};
//	  u16 raw=0;
//	  u16 raw1=0;
//	  I2CreadBytes(MAX30205_ADDRESS,MAX30205_TEMPERATURE, &readRaw[0] ,2);
//	  raw=raw|readRaw[0];
//    raw1=(raw<<8)|readRaw[1];	 
//    temperature1 = raw1  * 0.00390625+2.0;     // convert to temperature
//	  return  temperature1;
// 
// }
 
 float GetTemperature(void)
	{
		float temperature= 0.0f;
		int16_t raw = 0;
		uint8_t readRaw[2];
		I2CreadBytes(MAX30205_ADDRESS,MAX30205_TEMPERATURE, readRaw ,2); // read two bytes
		//printf("readRaw[0];:%d\r\n",readRaw[0]);
		//printf("readRaw[1];:%d\r\n",readRaw[1]);
		raw = ((uint16_t) readRaw[0] << 8) | readRaw[1];  //combine two bytes
		temperature = raw  * 0.00390625f;     // convert to temperature
		//printf("raw:%d\r\n",raw);
		return  temperature;
	}
	
	void MAX_shutdown(void){
  uint8_t reg = I2CreadByte(MAX30205_ADDRESS, MAX30205_CONFIGURATION);  // Get the current register
  I2CwriteByte(MAX30205_ADDRESS, MAX30205_CONFIGURATION, reg | 0x80);
}
	void MAX_begin(void){
  I2CwriteByte(MAX30205_ADDRESS, MAX30205_CONFIGURATION, 0x00); //mode config
  I2CwriteByte(MAX30205_ADDRESS, MAX30205_THYST , 		 0x00); // set threshold
  I2CwriteByte(MAX30205_ADDRESS, MAX30205_TOS, 			 0x00); //
}

//void MAX30205::printRegisters(void){
//  Serial.println(I2CreadByte(MAX30205_ADDRESS, MAX30205_TEMPERATURE),  BIN);
//  Serial.println(I2CreadByte(MAX30205_ADDRESS, MAX30205_CONFIGURATION),  BIN);
//  Serial.println(I2CreadByte(MAX30205_ADDRESS, MAX30205_THYST), BIN);
//  Serial.println(I2CreadByte(MAX30205_ADDRESS, MAX30205_TOS), BIN);

//}
 //写入指定地址一个字节数据
 void I2CwriteByte(u8 address,u8 subAddress,u8 data)  //
 {
     //IIC_Start();                                       //产生开始信号
	   I2Cwriteaddr(address|IIC_WRITE,subAddress);  //对器件地址写入命令信号 然后寄存器地址
	   IIC_Send_Byte(data);                               //对数据进行写进去
	   IIC_Wait_Ack();                                    //等待回应
	   IIC_Stop();	                         //产生停止信号
}
 //I2C读一个字节操作
 u8 I2CreadByte(u8 address,u8 subAddress)
 {   
	   u8 data;
    // IIC_Start();                           //开始
	   I2Cwriteaddr(address|IIC_WRITE, subAddress); //对器件地址发送写入命令 写入寄存器地址
     IIC_Start(); 	                                      //开始
	   IIC_Send_Byte(address|IIC_READ);                //发送读取命令
	 
	   IIC_Wait_Ack();                                    //等待回应
	   data=IIC_Read_Byte(0);
	   IIC_Stop();	                         //产生停止信号
	   return data;	   
}
 
 void I2CreadBytes(u8 address,u8 subAddress,u8*dest,u8 count)
 {
    int i;
	  for(i=0;i<count;i++)
		{
			dest[i]=I2CreadByte(address,subAddress);
			subAddress+=1;
		}
 
}
 

void I2Cwriteaddr(u8 address,u8 subAddress)//写地址和寄存器操作
{
     IIC_Start();                         //产生开始条件
	   IIC_Send_Byte(address);              //写入器件地址
	   IIC_Wait_Ack();                      //等待回应
	   IIC_Send_Byte(subAddress);           //写入寄存器地址
	   IIC_Wait_Ack();                       //等待回应
	   //IIC_Stop();	                         //产生停止信号
}
int16_t Read_max16(u8 address,u8 subAddress)
{
			int16_t data;
			u8 instant;
		// IIC_Start();                           //开始
			I2Cwriteaddr(address|IIC_WRITE, subAddress); //对器件地址发送写入命令 写入寄存器地址
			IIC_Start(); 	                                      //开始
			IIC_Send_Byte(address|IIC_READ);                //发送读取命令
		 
			IIC_Wait_Ack();                                    //等待回应
			data=IIC_Read_Byte(1);
			IIC_Wait_Ack(); 
			instant = IIC_Read_Byte(0);
			IIC_Stop();	                         //产生停止信号
			data = (data<<8)|instant;
			return data;	
}	

float wendu[106][2]={//?????????????
0,3274,//0 32.74k
1,3111,//1 31.11k
2,2957,//2 29.57k
3,2812,//
4,2674,//
5,2545,
6,2422,
7,2306,
8,2196,
9,2092,
10,1993,
11,1900,
12,1811,
13,1728,
14,1648,
15,1573,
16,1501,
17,1433,
18,1369,
19,1308,
20,1250,
21,1194,
22,1142,
23,1092,
24,1045,
25,1000,//25 10k                
26,957,//26 9.57k
27,916,
28,877,
29,840,
30,805,
31,771,
32,739,
33,709,
34,679,
35,652,
36,625,
37,600,
38,576,
39,553,
40,531,
41,510,
42,490,
43,471,
44,453,
45,435,
46,418,
47,402,
48,387,
49,372,
50,358,
51,345,
52,332,
53,320,
54,308,
55,297,
56,286,
57,276,
58,266,
59,256,
60,247,
61,238, 
62,230,
63,222,
64,214,
65,207,
66,199,
67,193,
68,186,
69,180,
70,174,
71,168,
72,162,
73,157,
74,152,
75,147,
76,142,
77,137,
78,133,
79,128,
80,124,
81,120,
82,116,
83,113,
84,109,
85,106,
86,102,//86 1.02k
87,99,//87 0.99k
88,96,
89,93,
90,90,
91,88,
92,85,
93,82,
94,80,
95,78,
96,75,
97,73,
98,71,
99,69,
100,67,
101,65,
102,63,
103,61, 
104,59,
105,58//105 0.58k
          

}; 

float GetNTCTemperature(void)
{
	float tmpf2,r_value;
	uint32_t AD_value,ii;
	//tmpf1 = GetTemperature1();
            AD_value  = 3300000/4096*ADC_ConvertedValue/1000;
			r_value = (AD_value*1000)/(3300-AD_value);
            tmpf2 = 0;
            for(ii=0;ii<106;ii++)  
			{
				if((r_value <= wendu[ii][1]) && (r_value > wendu[ii+1][1]))
				{ //
					//dat=(wendu[ii][0]);//dat2
					tmpf2 = (float)((wendu[ii+1][0]-wendu[ii][0])/(wendu[ii][1]-wendu[ii+1][1]));
					tmpf2 = tmpf2*(wendu[ii][1]-r_value);
					tmpf2 = tmpf2 + wendu[ii][0];
					ii=0;
					break;
				}//else{dat=0;}
			}
			return tmpf2;
}

float GetTemperature1(void)
	{
		float temperature= 0.0;
		float temperatureNTC= 0.0;
		float temperatureMAX= 0.0;
		uint16_t temp;
//		int16_t i;
//		MAX_begin();
		temp = Read_max16(MAX30205_ADDRESS,MAX30205_TEMPERATURE);
//		for(i=0;i<temp;i++)
//		{
//			temperature += 0.00390625;
//		}
        
//        if(temp > 0x3000)
//            temp = 0x3000;
        temperatureMAX = temp * 0.00390625f ;//* tmpCaliK + tmpCaliB;
		//temperature += TempBase;//tmpCaliK
	//	temperature = temp*39.06/10000;// + TempBase;//;*0.00390625;
	//	MAX_shutdown();
		temperatureNTC = GetNTCTemperature() + 1.65;
		
		if(temperatureNTC > temperatureMAX)
		{
			
			if((temperatureNTC - temperatureMAX) > 1.10f)
			{
				temperature = (temperatureMAX + temperatureNTC)/2;
			}
			else if((temperatureNTC - temperatureMAX) > 0.50f)
			{
				temperature = temperatureNTC;
				tempDevice = 1;
			}
//			{
//				temperature = temperatureMAX;
//				tempDevice = 0;
//			}
//			else
//			{
//				temperature = temperatureNTC;
//				tempDevice = 1;
//			}
		}
		else if(temperatureNTC < temperatureMAX)
		{
			if((temperatureMAX - temperatureNTC) > 0.50f)
			{
				temperature = temperatureNTC;
				tempDevice = 1;
			}
			else if(((temperatureMAX - temperatureNTC) > 0.45f)&&((temperatureMAX - temperatureNTC) < 0.50f))
			{
				temperature = temperatureNTC + 0.25f;
				tempDevice = 1;
			}
			else if(((temperatureMAX - temperatureNTC) > 0.25f)&&((temperatureMAX - temperatureNTC) < 0.45f))
			{
				temperature = temperatureMAX - 0.25f;
				tempDevice = 0;
			}
			else 
			{
				temperature = temperatureMAX;
				tempDevice = 0;
			}
		}
		else 
		{
			temperature = temperatureMAX;
			tempDevice = 0;
		}
//		temperature = temperatureNTC;
		temperature = temperature * tmpCaliK + tmpCaliB;
		return  temperature;
	}








