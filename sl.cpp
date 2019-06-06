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
//ʵ��16bit���ݵ�sl������ 
#include<stdio.h>
#include <math.h>
#define LEVEL 4
#define CODE_LENGTH 8 
float polarCode[LEVEL][CODE_LENGTH]; //�ô����ŵĶ�ά����������м����� 

int load()//����ԭʼ����
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/encode.txt";
	FILE* fp;
	fp = fopen(filename, "r");
	for (i=0;i<CODE_LENGTH;i++)
    fscanf(fp,"%f", &polarCode[0][i]); // ѭ����
    fclose(fp);
      
    i = 0;
    printf("�����������8bit����Ϊ");
    while(i<CODE_LENGTH) 
	{
      printf("%d ",(int)polarCode[0][i]);
      i++; 
    }
    
	return 0;
}
int write()//������������д���ļ� 
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/decode.txt";
	FILE* fp;
	fp = fopen(filename, "w+");
	for (i=0;i<CODE_LENGTH;i++)
	{ 
    fprintf(fp,"%d", (int)polarCode[3][i]); // ѭ��д 
    fprintf(fp,"%s", " "); // д��ո� 
    }
	 
    fclose(fp);
    
	return 0;
}


float bpsk() //�����������ݽ���bpsk���� 
{
  int i;
  for (i=0;i<CODE_LENGTH;i++)
      polarCode[0][i] = (polarCode[0][i] == 0)?1:-1;
      
    i = 0;
    printf("\n\n");
    printf("����BPSK���ƺ��8bit����Ϊ");
    while(i<CODE_LENGTH) 
	{
      printf("%d ",(int)polarCode[0][i]);
      i++; 
    }
    
	return 0;
       
      
} 

float f(float Lin1,float Lin2)//Lout = f(Lin1,Lin2)=sign(Lin1,Lin2)*min(|Lin1|,|Lin2|) //f���� 
{
	float Lout;
	Lout = ((Lin1*Lin2 > 0)? 1:-1)* ((abs(Lin1)>abs(Lin2))?abs(Lin2):abs(Lin1));
	return Lout;
 } 

float g(float Lin1,float Lin2,short b)//Lout  = g(Lin1,Lin2,b) = (-1)^b*Lin1+Lin2 //g���� 
{
	float Lout;
	Lout = pow(-1, b)*Lin1+Lin2;
	return Lout;
}

float _bpsk(float bpsk)//bpsk�ķ����� 
{
	float reverse_bpsk;
	reverse_bpsk   = (bpsk<0)?1:0;
	return reverse_bpsk;
	
 } 

int decode(void)//sl������� 
{
	//��һ�����裺��������һ���Ŀɿ�����
	//����һ���ֳ������飬ǰCODE_LENGTH/2�Լ���CODE_LENGTH/2��
	int i,j;
	for(i = 0 ; i < LEVEL ; i++)
	  for(j = 0 ; j < pow(2,LEVEL-i-2) ;j++) 
	     polarCode[i+1][j]  = f(polarCode[i][j],polarCode[i][j+(int)pow(2,LEVEL-i-2)]);//�ӵڶ�����ʼ���� 
	
	
	//�ڶ������裺�õ������ĵ�һ������ ��bit1�� 
	 polarCode[LEVEL-1][0] =  _bpsk( polarCode[LEVEL-1][0]); 
	
	//���������裺ͨ��g�����õ������ĵڶ������ݣ�bit2�� 
	 polarCode[LEVEL-1][1] = _bpsk(g( polarCode[LEVEL-2][0], polarCode[LEVEL-2][1],polarCode[LEVEL-1][0])) ;
	
	//���ĸ����裺�ϵ�һ���Ƴ������ڶ�����ֵ
	     polarCode[LEVEL-2][0] =  (int)polarCode[LEVEL-1][0]^(int)polarCode[LEVEL-1][1]; //ǰ����ֵͨ�����һ���Ƴ�����Ϊ������ֵ��bֵ 
	     polarCode[LEVEL-2][1] =  polarCode[LEVEL-1][1]; //���� 
	     
	     polarCode[LEVEL-2][2] =  g(polarCode[LEVEL-3][0],polarCode[LEVEL-3][2],polarCode[LEVEL-2][0]);
	     polarCode[LEVEL-2][3] =  g(polarCode[LEVEL-3][1],polarCode[LEVEL-3][3],polarCode[LEVEL-2][1]);
	     
	//��������裺�ֱ�ͨ��f������g�����õ��������ĸ����ݣ�bit3��bit4��
	polarCode[LEVEL-1][2] = _bpsk(f( polarCode[LEVEL-2][2], polarCode[LEVEL-2][3]));
	polarCode[LEVEL-1][3] = _bpsk(g( polarCode[LEVEL-2][2], polarCode[LEVEL-2][3],polarCode[LEVEL-1][2]));
	
	//���������� ���ϵ�һ���Ƴ��ڶ�����ֵ 
	     polarCode[LEVEL-2][2] =  (int)polarCode[LEVEL-1][2]^(int)polarCode[LEVEL-1][3];
	     polarCode[LEVEL-2][3] =  polarCode[LEVEL-1][3];
	     
	//���߸����裺���ϵ�һ���Ƴ���������ֵ
	     polarCode[LEVEL-3][0] =  (int)polarCode[LEVEL-2][0]^(int)polarCode[LEVEL-2][2];  
	     polarCode[LEVEL-3][1] =  (int)polarCode[LEVEL-2][1]^(int)polarCode[LEVEL-2][3]; 
	     polarCode[LEVEL-3][2] =  polarCode[LEVEL-2][2];
	     polarCode[LEVEL-3][3] =  polarCode[LEVEL-2][2];//����
	     
     //�ڰ˸����� ��ͨ��g�����Ƴ����������ĸ���
	     polarCode[LEVEL-3][4] =  g( polarCode[LEVEL-4][0], polarCode[LEVEL-4][4],polarCode[LEVEL-3][0]); 
	     polarCode[LEVEL-3][5] =  g( polarCode[LEVEL-4][1], polarCode[LEVEL-4][5],polarCode[LEVEL-3][1]); 
	     polarCode[LEVEL-3][6] =  g( polarCode[LEVEL-4][2], polarCode[LEVEL-4][6],polarCode[LEVEL-3][2]); 
	     polarCode[LEVEL-3][7] =  g( polarCode[LEVEL-4][3], polarCode[LEVEL-4][7],polarCode[LEVEL-3][3]); 
	     
	 //�ھŸ����裺ͨ�����������ĸ����õ��ڶ��������������
	     polarCode[LEVEL-2][4] =  f( polarCode[LEVEL-3][4],polarCode[LEVEL-3][6]); 
	     polarCode[LEVEL-2][5] =  f( polarCode[LEVEL-3][5],polarCode[LEVEL-3][7]); 
	     
     //��ʮ�����裺ͨ���ڶ���������������Ƴ��������������(bit5��bit6) 
    polarCode[LEVEL-1][4] = _bpsk(f( polarCode[LEVEL-2][4], polarCode[LEVEL-2][5]));
	polarCode[LEVEL-1][5] = _bpsk(g( polarCode[LEVEL-2][4], polarCode[LEVEL-2][5],polarCode[LEVEL-1][4]));
	
	//��ʮһ������ ���ϵ�һ���Ƴ��ڶ�����ֵ 
	     polarCode[LEVEL-2][4] =  (int)polarCode[LEVEL-1][4]^(int)polarCode[LEVEL-1][5];
	     polarCode[LEVEL-2][5] =  polarCode[LEVEL-1][5];
	     
	  //��ʮ�������� ��ͨ��g�����Ƴ��ڶ������������
	     polarCode[LEVEL-2][6] =  g( polarCode[LEVEL-3][4], polarCode[LEVEL-3][6],polarCode[LEVEL-2][4]); 
	     polarCode[LEVEL-2][7] =  g( polarCode[LEVEL-3][5], polarCode[LEVEL-4][7],polarCode[LEVEL-2][5]);    
	     
	  //��ʮ�������裺ͨ���ڶ���������������Ƴ��������������(bit7��bit8) 
	polarCode[LEVEL-1][6] = _bpsk(f( polarCode[LEVEL-2][6], polarCode[LEVEL-2][7]));
	polarCode[LEVEL-1][7] = _bpsk(g( polarCode[LEVEL-2][6], polarCode[LEVEL-2][7],polarCode[LEVEL-1][6]));
	
	
	i = 0;
	printf("\n\n");
    printf("����������8bit����Ϊ");
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
