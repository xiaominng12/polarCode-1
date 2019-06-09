//======================================================================
//
//        Copyright (C) 2019-2022 ECIE of the China University of Geosciences
//        All rights reserved
//
//        filename :$sl$
//        description : implement the sl decoder with c 
//
//        created by xiangxufu at  $2019.6.9$
//        
//        https://github.com/xiangxufu/polarCode.git
//======================================================================
//实现16bit数据的sl译码器 
#include<stdio.h>
#include <math.h>
#define LEVEL 6 //数组行数为码长开方加一
#define CODE_LENGTH 32 
float polarCode[LEVEL][CODE_LENGTH]; //用带符号的二维数组来存放中间数据 

int load()//加载原始数据
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/32bit_2019_6_9/encode.txt";
	FILE* fp;
	fp = fopen(filename, "r");
	for (i=0;i<CODE_LENGTH;i++)
    fscanf(fp,"%f", &polarCode[0][i]); // 循环读
    fclose(fp);
      
    i = 0;
    printf("译码器输入的%dbit数据为",CODE_LENGTH);
    while(i<CODE_LENGTH) 
	{
      printf("%d ",(int)polarCode[0][i]);
      i++; 
    }
    
	return 0;
}
int write()//将编码后的数据写入文件 
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/32bit_2019_6_9/decode.txt";
	FILE* fp;
	fp = fopen(filename, "w+");
	for (i=0;i<CODE_LENGTH;i++)
	{ 
    fprintf(fp,"%d", (int)polarCode[LEVEL-1][i]); // 循环写 
    fprintf(fp,"%s", " "); // 写入空格 
    }
	 
    fclose(fp);
    
	return 0;
}


float bpsk() //将编码后的数据进行bpsk调制 
{
  int i;
  for (i=0;i<CODE_LENGTH;i++)
      polarCode[0][i] = (polarCode[0][i] == 0)?1:-1;
      
    i = 0;
    printf("\n\n");
    printf("经过BPSK调制后的%dbit数据为",CODE_LENGTH);
    while(i<CODE_LENGTH) 
	{
      printf("%d ",(int)polarCode[0][i]);
      i++; 
    }
    
	return 0;
       
      
} 

float f(float Lin1,float Lin2)//Lout = f(Lin1,Lin2)=sign(Lin1,Lin2)*min(|Lin1|,|Lin2|) //f函数 
{
	float Lout;
	Lout = ((Lin1*Lin2 > 0)? 1:-1)* ((abs(Lin1)>abs(Lin2))?abs(Lin2):abs(Lin1));
	return Lout;
 } 

float g(float Lin1,float Lin2,short b)//Lout  = g(Lin1,Lin2,b) = (-1)^b*Lin1+Lin2 //g函数 
{
	float Lout;
	Lout = pow(-1, b)*Lin1+Lin2;
	return Lout;
}

float _bpsk(float bpsk)//bpsk的反过程 
{
	float reverse_bpsk;
	reverse_bpsk   = (bpsk<0)?1:0;
	return reverse_bpsk;
	
 } 
//基本原则：什么时候把上一级的b值推出来什么时候就可以继续往下推否者回到第一级正推
//基本路线：求出b值往下推
//基本结构：锯齿形爬行
//*************************************************************************
//第1步：求出第一级第1,2个bit(*)********************2bit-----------全追溯

//第2步：上溯到第二级求出第二级1,2个bit(b值)position = 0
//第3步: 求出第二级第3,4个bit(2)position = 0
//第4步：求出第一级第3,4个bit(*)********************4bit   lever = 1
//第5步：上溯到第二级求出第二级3,4个bit(b值)position = 1
//第6步：上溯到第三级求出第1,2,3,4个bit(b值)position = 0

//第7步：上溯到第三级求出第5,6,7,8个bit
//*************************************************************************

//第8步：推出第一级第5,6个bit(*)

//第9步：上溯到第二级求出第二级5,6个bit(b值)
//第10步: 求出第二级第7,8个bit
//第11步：求出第一级第7,8个bit(*)********************8bit
//第12步：上溯到第二级求出第二级7,8个bit(b值)
//第13步：上溯到第三级求出第5,6,7,8个bit(b值)

//第14步：上溯到第四级求出第1,2,3,4,5,6,7,8个bit(b值)
//第15步：上溯到第四级求出第9,10,11,12,13,14,15,16个bit------------追溯8bit

//*************************************************************************
//第16步：求出第一级第9,10个bit(*)

//第17步：上溯到第二级求出第二级9,10个bit(b值)
//第18步: 求出第二级第11,12个bit
//第19步：求出第一级第11,12个bit(*)
//第20步：上溯到第二级求出第二级11,12个bit(b值)
//第21步：上溯到第三级求出第9,10,11,12个bit(b值)

//第22步：上溯到第三级求出第13,14,15,16个bit
//*************************************************************************
//第23步：推出第一级第13,14个bit(*)

//第24步：上溯到第二级求出第二级13,14个bit(b值)
//第25步: 求出第二级第15,16个bit
//第26步：求出第一级第15,16个bit(*)**************************16bit
//第27步：上溯到第二级求出第二级15,16个bit(b值)
//第28步：上溯到第三级求出第13,14,15,16个bit(b值)

//第29步：上溯到第四级求出第9,10,11,12,13,14,15,16个bit(b值)
//第30步：上溯到第五级求出第1-16个bit(b值)
//第31步：上溯到第五级求出第17-32个bit-----------追溯16bit
//*************************************************************************

//第32步：求出第一级第17,18个bit(*)

//第33步：上溯到第二级求出第二级17,18个bit(b值)
//第34步: 求出第二级第19,20个bit
//第35步：求出第一级第19,20个bit(*)
//第36步：上溯到第二级求出第二级19,20个bit(b值)
//第37步：上溯到第三级求出第17,18,19,20个bit(b值)

//第38步：上溯到第三级求出第21,22,23,24个bit
//*************************************************************************
//第39步：推出第一级第21,22个bit(*)

//第40步：上溯到第二级求出第二级21,22个bit(b值)
//第41步: 求出第二级第23,24个bit
//第42步：求出第一级第23,24个bit(*)
//第43步：上溯到第二级求出第二级23,24个bit(b值)
//第44步：上溯到第三级求出第21,22,23,24个bit(b值)

//第45步：上溯到第四级求出第17,18,19,20,21,22,23,24个bit(b值)
//第46步：上溯到第四级求出第25,26,27,28,29,30,31,32个bit
//*************************************************************************

//第47步：求出第一级第25,26个bit(*)

//第48步：上溯到第二级求出第二级25,26个bit(b值)
//第49步: 求出第二级第27,28个bit
//第50步：求出第一级第27,28个bit(*)
//第51步：上溯到第二级求出第二级27,28个bit(b值)
//第52步：上溯到第三级求出第25,26,27,28个bit(b值)

//第53步：上溯到第三级求出第29,30,31,32个bit
//*************************************************************************

//第54步：推出第一级第29,30个bit(*)

//第55步：上溯到第二级求出第二级29,30个bit(b值)
//第56步: 求出第二级第31,32个bit
//第57步：求出第一级第31,32个bit(*)**************************32bit
//第58步：上溯到第二级求出第二级31,32个bit(b值)
//第59步：上溯到第三级求出第29,30,31,32个bit(b值)

//第60步：上溯到第四级求出第25,26,27,28,29,30,31,32个bit(b值)
//第61步：上溯到第五级求出第17-32个bit(b值)

//第62步：上溯到第六级求出第1-32个bit(b值)
//第63步：上溯到第六级求出第33-64个bit-----------追溯32bit
//*************************************************************************

//专门写一个追溯函数(反推)
//设定全追溯为0，2bit为1,4bit为2，8bit为3,16bit为4,32bit为5

int get_bit1_2(int position ,int lever)//用于每次追溯得到偏移后的第一第二个值
{
    //第一个步骤：求出第一级第1,2个bit(*)
	int i = 0,j = 0;
	
	int i_start = (lever == 0)?0:(LEVEL-lever-1);//与级数以及有关 
	int j_start = (lever == 0)?0:(pow(2,lever)+position*pow(2,lever+1));//与级数与偏移量有关 
	
	int location_bit1 = ((lever == 0)?0:(int)pow(2,lever))+position*(int)pow(2,lever+1);
	int location_bit2 = ((lever == 0)?0:(int)pow(2,lever))+position*(int)pow(2,lever+1)+1;
		
	for(i = i_start; i < LEVEL-1; i++)
	  for(j = j_start; j < (j_start+ pow(2,LEVEL-i-2));j++) 
	     polarCode[i+1][j]  = f(polarCode[i][j],polarCode[i][j+(int)pow(2,((lever == 0)?(LEVEL-2):(lever-1))-(i-i_start))]); 

	 polarCode[LEVEL-1][location_bit1]  =  _bpsk(polarCode[LEVEL-1][location_bit1]);
	 polarCode[LEVEL-1][location_bit2]  =  _bpsk(g(polarCode[LEVEL-2][location_bit1],
	                                               polarCode[LEVEL-2][location_bit2],
	                                               polarCode[LEVEL-1][location_bit1]));	
	                                                                         
     return 0; 
}

//正推
int get_remain(int position,int lever)
{
   int i;
   int location_A = position*(int)pow(2,lever);
   int location_B = position*(int)pow(2,lever)+(int)pow(2,lever-1);
   
   for(i=0;i<pow(2,lever-1);i++)
   polarCode[LEVEL-lever][location_B+i] = 
 g(polarCode[LEVEL-lever-1][location_A+i],
   polarCode[LEVEL-lever-1][location_B+i],																	   
   polarCode[LEVEL-lever  ][location_A+i]);
   
   return 0; 
   
}	 
//正推b值
int get_remain_b(int position,int lever)		 
{
   int i;
   for(i=0;i<pow(2,lever-2);i++)
   {
      polarCode[LEVEL-lever][position*(int)pow(2,lever-1)+i] = 
      (int)polarCode[LEVEL-lever+1][position*(int)pow(2,lever-1)+i]^
      (int)polarCode[LEVEL-lever+1][position*(int)pow(2,lever-1)+(int)pow(2,lever-2)+i]; 
      
       polarCode[LEVEL-lever][position*(int)pow(2,lever-1)+(int)pow(2,lever-2)+i] =  
       polarCode[LEVEL-lever+1][position*(int)pow(2,lever-1)+(int)pow(2,lever-2)+i];
   }
 
 return 0; 
}	 

int decode()//解码过程
{
	
	int i ;
	
	get_bit1_2(0,0);//全追溯      **********[1]
	get_remain_b(0,2);//正推第二级b  **********[2]
	get_remain(0,2);//正推第二级     **********[3]
	
	get_bit1_2(0,1);//追溯2bit       **********[4]
	get_remain_b(1,2);//正推第二级b  **********[5]
	get_remain_b(0,3);//正推第三级b  **********[6]
	get_remain(0,3);//正推第三级     **********[7]
	
	get_bit1_2(0,2);//追溯4bit       **********[8]
	get_remain_b(2,2);//正推第二级b  **********[9]
	get_remain(1,2);//正推第二级     **********[10]
	
	get_bit1_2(1,1);//追溯2bit       **********[11]
	get_remain_b(3,2);//正推第二级b  **********[12]
	get_remain_b(1,3);//正推第三级b  **********[13]
	get_remain_b(0,4);//正推第四级b  **********[14]
	get_remain(0,4);//正推第四级     **********[15]
	
	get_bit1_2(0,3);//追溯8bit       **********[16]
	get_remain_b(4,2);//正推第二级b  **********[17]
	get_remain(2,2);//正推第二级     **********[18]
	
	get_bit1_2(2,1);//追溯2bit       **********[19]
	get_remain_b(5,2);//正推第二级b  **********[20]
	get_remain_b(2,3);//正推第三级b  **********[21]
	get_remain(1,3);//正推第三级     **********[22]
	
	get_bit1_2(1,2);//追溯4bit       **********[23]
	get_remain_b(6,2);//正推第二级b  **********[24]
	get_remain(3,2);//正推第二级     **********[25]
	
	get_bit1_2(3,1);//追溯2bit       **********[26]
	
	
	get_remain_b(7,2);//正推第二级b  **********[27]
	get_remain_b(3,3);//正推第三级b  **********[28]
	get_remain_b(1,4);//正推第四级b  **********[29]
	get_remain_b(0,5);//正推第五级b  **********[30]
	get_remain(0,5);//正推第五级     **********[31]
	
    get_bit1_2(0,4);//追溯16bit      **********[32]
	get_remain_b(8,2);//正推第二级b  **********[33]
	get_remain(4,2);//正推第二级     **********[34]
	
	get_bit1_2(4,1);//追溯2bit       **********[35]
	get_remain_b(9,2);//正推第二级b  **********[36]
	get_remain_b(4,3);//正推第三级b  **********[37]
	get_remain(2,3);//正推第三级     **********[38]
	
	get_bit1_2(2,2);//追溯4bit       **********[39]
	get_remain_b(10,2);//正推第二级b  **********[40]
	get_remain(5,2);//正推第二级     **********[41]
	
	get_bit1_2(5,1);//追溯2bit       **********[42]
	get_remain_b(11,2);//正推第二级b **********[43]
	get_remain_b(5,3);//正推第三级b  **********[44]
	get_remain_b(2,4);//正推第四级b  **********[45]
	get_remain(1,4);//正推第四级     **********[46]
	
	get_bit1_2(1,3);//追溯8bit       **********[47]
	get_remain_b(12,2);//正推第二级b **********[48]
	get_remain(6,2);//正推第二级     **********[49]
	
	get_bit1_2(6,1);//追溯2bit       **********[50]
	get_remain_b(13,2);//正推第二级b **********[51]
	get_remain_b(6,3);//正推第三级b  **********[52]
	get_remain(3,3);//正推第三级     **********[53]
	
	get_bit1_2(3,2);//追溯4bit       **********[54]
	get_remain_b(14,2);//正推第二级b **********[55]
	get_remain(7,2);//正推第二级     **********[56]
	
	get_bit1_2(7,1);//追溯2bit       **********[57]finish
	get_remain_b(15,2);//正推第二级b **********[58]
	get_remain_b(7,3);//正推第三级b  **********[59]
	get_remain_b(3,4);//正推第四级b  **********[60]
	get_remain_b(1,5);//正推第五级b  **********[61]
	get_remain_b(0,6);//正推第六级b  **********[62]
	get_remain(0,6);//正推第六级     **********[63]
	
	
	//打印数组最后一行数据
	i = 0;
	printf("\n\n");
    printf("经过译码后的%dbit数据为",CODE_LENGTH);
    while(i<CODE_LENGTH) 
	{
      printf("%d ",(int)polarCode[LEVEL-1][i]);
      i++; 
    }
    
	return 0;
	
}

int main(void)
{

	load();
	bpsk();
	decode();
	write(); 
	
	
}
