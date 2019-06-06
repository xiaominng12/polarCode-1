//======================================================================
//
//        Copyright (C) 2019-2022 ECIE of the China University of Geosciences
//        All rights reserved
//
//        filename :$sl$
//        description : implement the sl decoder with c 
//
//        created by xiangxufu at  $2019.6.6$
//        
//        https://github.com/xiangxufu/polarCode.git
//======================================================================
//实现16bit数据的sl译码器 
#include<stdio.h>
#include <math.h>
#define LEVEL 4
#define CODE_LENGTH 8 
float polarCode[LEVEL][CODE_LENGTH]; //用带符号的二维数组来存放中间数据 

int load()//加载原始数据
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/encode.txt";
	FILE* fp;
	fp = fopen(filename, "r");
	for (i=0;i<CODE_LENGTH;i++)
    fscanf(fp,"%f", &polarCode[0][i]); // 循环读
    fclose(fp);
      
    i = 0;
    printf("译码器输入的8bit数据为");
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
	const char* filename = "C:/disk/004_polarCode/code/c/decode.txt";
	FILE* fp;
	fp = fopen(filename, "w+");
	for (i=0;i<CODE_LENGTH;i++)
	{ 
    fprintf(fp,"%d", (int)polarCode[3][i]); // 循环写 
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
    printf("经过BPSK调制后的8bit数据为");
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

int decode(void)//sl解码过程 
{
	//第一个步骤：计算出最后一级的可靠数据
	//将第一级分成两个组，前CODE_LENGTH/2以及后CODE_LENGTH/2个
	int i,j;
	for(i = 0 ; i < LEVEL ; i++)
	  for(j = 0 ; j < pow(2,LEVEL-i-2) ;j++) 
	     polarCode[i+1][j]  = f(polarCode[i][j],polarCode[i][j+(int)pow(2,LEVEL-i-2)]);//从第二级开始计算 
	
	
	//第二个步骤：得到译码后的第一个数据 （bit1） 
	 polarCode[LEVEL-1][0] =  _bpsk( polarCode[LEVEL-1][0]); 
	
	//第三个步骤：通过g函数得到译码后的第二个数据（bit2） 
	 polarCode[LEVEL-1][1] = _bpsk(g( polarCode[LEVEL-2][0], polarCode[LEVEL-2][1],polarCode[LEVEL-1][0])) ;
	
	//第四个步骤：上弹一级推出倒数第二级数值
	     polarCode[LEVEL-2][0] =  (int)polarCode[LEVEL-1][0]^(int)polarCode[LEVEL-1][1]; //前两个值通过最后一级推出，作为后两个值的b值 
	     polarCode[LEVEL-2][1] =  polarCode[LEVEL-1][1]; //正推 
	     
	     polarCode[LEVEL-2][2] =  g(polarCode[LEVEL-3][0],polarCode[LEVEL-3][2],polarCode[LEVEL-2][0]);
	     polarCode[LEVEL-2][3] =  g(polarCode[LEVEL-3][1],polarCode[LEVEL-3][3],polarCode[LEVEL-2][1]);
	     
	//第五个步骤：分别通过f函数和g函数得到第三第四个数据（bit3，bit4）
	polarCode[LEVEL-1][2] = _bpsk(f( polarCode[LEVEL-2][2], polarCode[LEVEL-2][3]));
	polarCode[LEVEL-1][3] = _bpsk(g( polarCode[LEVEL-2][2], polarCode[LEVEL-2][3],polarCode[LEVEL-1][2]));
	
	//第六个步骤 ：上弹一级推出第二级数值 
	     polarCode[LEVEL-2][2] =  (int)polarCode[LEVEL-1][2]^(int)polarCode[LEVEL-1][3];
	     polarCode[LEVEL-2][3] =  polarCode[LEVEL-1][3];
	     
	//第七个步骤：再上弹一级推出第三级数值
	     polarCode[LEVEL-3][0] =  (int)polarCode[LEVEL-2][0]^(int)polarCode[LEVEL-2][2];  
	     polarCode[LEVEL-3][1] =  (int)polarCode[LEVEL-2][1]^(int)polarCode[LEVEL-2][3]; 
	     polarCode[LEVEL-3][2] =  polarCode[LEVEL-2][2];
	     polarCode[LEVEL-3][3] =  polarCode[LEVEL-2][2];//正推
	     
     //第八个步骤 ：通过g函数推出第三级后四个数
	     polarCode[LEVEL-3][4] =  g( polarCode[LEVEL-4][0], polarCode[LEVEL-4][4],polarCode[LEVEL-3][0]); 
	     polarCode[LEVEL-3][5] =  g( polarCode[LEVEL-4][1], polarCode[LEVEL-4][5],polarCode[LEVEL-3][1]); 
	     polarCode[LEVEL-3][6] =  g( polarCode[LEVEL-4][2], polarCode[LEVEL-4][6],polarCode[LEVEL-3][2]); 
	     polarCode[LEVEL-3][7] =  g( polarCode[LEVEL-4][3], polarCode[LEVEL-4][7],polarCode[LEVEL-3][3]); 
	     
	 //第九个步骤：通过第三级后四个数得到第二级第五第六个数
	     polarCode[LEVEL-2][4] =  f( polarCode[LEVEL-3][4],polarCode[LEVEL-3][6]); 
	     polarCode[LEVEL-2][5] =  f( polarCode[LEVEL-3][5],polarCode[LEVEL-3][7]); 
	     
     //第十个步骤：通过第二级第五第六个数推出第五第六个数据(bit5，bit6) 
    polarCode[LEVEL-1][4] = _bpsk(f( polarCode[LEVEL-2][4], polarCode[LEVEL-2][5]));
	polarCode[LEVEL-1][5] = _bpsk(g( polarCode[LEVEL-2][4], polarCode[LEVEL-2][5],polarCode[LEVEL-1][4]));
	
	//第十一个步骤 ：上弹一级推出第二级数值 
	     polarCode[LEVEL-2][4] =  (int)polarCode[LEVEL-1][4]^(int)polarCode[LEVEL-1][5];
	     polarCode[LEVEL-2][5] =  polarCode[LEVEL-1][5];
	     
	  //第十二个步骤 ：通过g函数推出第二级最后两个数
	     polarCode[LEVEL-2][6] =  g( polarCode[LEVEL-3][4], polarCode[LEVEL-3][6],polarCode[LEVEL-2][4]); 
	     polarCode[LEVEL-2][7] =  g( polarCode[LEVEL-3][5], polarCode[LEVEL-4][7],polarCode[LEVEL-2][5]);    
	     
	  //第十三个步骤：通过第二级第五第六个数推出第五第六个数据(bit7，bit8) 
	polarCode[LEVEL-1][6] = _bpsk(f( polarCode[LEVEL-2][6], polarCode[LEVEL-2][7]));
	polarCode[LEVEL-1][7] = _bpsk(g( polarCode[LEVEL-2][6], polarCode[LEVEL-2][7],polarCode[LEVEL-1][6]));
	
	
	i = 0;
	printf("\n\n");
    printf("经过译码后的8bit数据为");
    while(i<CODE_LENGTH) 
	{
      printf("%d ",(int)polarCode[3][i]);
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
