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
//ʵ��16bit���ݵ�sl������ 
#include<stdio.h>
#include <math.h>
#define LEVEL 6 //��������Ϊ�볤������һ
#define CODE_LENGTH 32 
float polarCode[LEVEL][CODE_LENGTH]; //�ô����ŵĶ�ά����������м����� 

int load()//����ԭʼ����
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/32bit_2019_6_9/encode.txt";
	FILE* fp;
	fp = fopen(filename, "r");
	for (i=0;i<CODE_LENGTH;i++)
    fscanf(fp,"%f", &polarCode[0][i]); // ѭ����
    fclose(fp);
      
    i = 0;
    printf("�����������%dbit����Ϊ",CODE_LENGTH);
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
	const char* filename = "C:/disk/004_polarCode/code/c/32bit_2019_6_9/decode.txt";
	FILE* fp;
	fp = fopen(filename, "w+");
	for (i=0;i<CODE_LENGTH;i++)
	{ 
    fprintf(fp,"%d", (int)polarCode[LEVEL-1][i]); // ѭ��д 
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
    printf("����BPSK���ƺ��%dbit����Ϊ",CODE_LENGTH);
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
//����ԭ��ʲôʱ�����һ����bֵ�Ƴ���ʲôʱ��Ϳ��Լ��������Ʒ��߻ص���һ������
//����·�ߣ����bֵ������
//�����ṹ�����������
//*************************************************************************
//��1���������һ����1,2��bit(*)********************2bit-----------ȫ׷��

//��2�������ݵ��ڶ�������ڶ���1,2��bit(bֵ)position = 0
//��3��: ����ڶ�����3,4��bit(2)position = 0
//��4���������һ����3,4��bit(*)********************4bit   lever = 1
//��5�������ݵ��ڶ�������ڶ���3,4��bit(bֵ)position = 1
//��6�������ݵ������������1,2,3,4��bit(bֵ)position = 0

//��7�������ݵ������������5,6,7,8��bit
//*************************************************************************

//��8�����Ƴ���һ����5,6��bit(*)

//��9�������ݵ��ڶ�������ڶ���5,6��bit(bֵ)
//��10��: ����ڶ�����7,8��bit
//��11���������һ����7,8��bit(*)********************8bit
//��12�������ݵ��ڶ�������ڶ���7,8��bit(bֵ)
//��13�������ݵ������������5,6,7,8��bit(bֵ)

//��14�������ݵ����ļ������1,2,3,4,5,6,7,8��bit(bֵ)
//��15�������ݵ����ļ������9,10,11,12,13,14,15,16��bit------------׷��8bit

//*************************************************************************
//��16���������һ����9,10��bit(*)

//��17�������ݵ��ڶ�������ڶ���9,10��bit(bֵ)
//��18��: ����ڶ�����11,12��bit
//��19���������һ����11,12��bit(*)
//��20�������ݵ��ڶ�������ڶ���11,12��bit(bֵ)
//��21�������ݵ������������9,10,11,12��bit(bֵ)

//��22�������ݵ������������13,14,15,16��bit
//*************************************************************************
//��23�����Ƴ���һ����13,14��bit(*)

//��24�������ݵ��ڶ�������ڶ���13,14��bit(bֵ)
//��25��: ����ڶ�����15,16��bit
//��26���������һ����15,16��bit(*)**************************16bit
//��27�������ݵ��ڶ�������ڶ���15,16��bit(bֵ)
//��28�������ݵ������������13,14,15,16��bit(bֵ)

//��29�������ݵ����ļ������9,10,11,12,13,14,15,16��bit(bֵ)
//��30�������ݵ����弶�����1-16��bit(bֵ)
//��31�������ݵ����弶�����17-32��bit-----------׷��16bit
//*************************************************************************

//��32���������һ����17,18��bit(*)

//��33�������ݵ��ڶ�������ڶ���17,18��bit(bֵ)
//��34��: ����ڶ�����19,20��bit
//��35���������һ����19,20��bit(*)
//��36�������ݵ��ڶ�������ڶ���19,20��bit(bֵ)
//��37�������ݵ������������17,18,19,20��bit(bֵ)

//��38�������ݵ������������21,22,23,24��bit
//*************************************************************************
//��39�����Ƴ���һ����21,22��bit(*)

//��40�������ݵ��ڶ�������ڶ���21,22��bit(bֵ)
//��41��: ����ڶ�����23,24��bit
//��42���������һ����23,24��bit(*)
//��43�������ݵ��ڶ�������ڶ���23,24��bit(bֵ)
//��44�������ݵ������������21,22,23,24��bit(bֵ)

//��45�������ݵ����ļ������17,18,19,20,21,22,23,24��bit(bֵ)
//��46�������ݵ����ļ������25,26,27,28,29,30,31,32��bit
//*************************************************************************

//��47���������һ����25,26��bit(*)

//��48�������ݵ��ڶ�������ڶ���25,26��bit(bֵ)
//��49��: ����ڶ�����27,28��bit
//��50���������һ����27,28��bit(*)
//��51�������ݵ��ڶ�������ڶ���27,28��bit(bֵ)
//��52�������ݵ������������25,26,27,28��bit(bֵ)

//��53�������ݵ������������29,30,31,32��bit
//*************************************************************************

//��54�����Ƴ���һ����29,30��bit(*)

//��55�������ݵ��ڶ�������ڶ���29,30��bit(bֵ)
//��56��: ����ڶ�����31,32��bit
//��57���������һ����31,32��bit(*)**************************32bit
//��58�������ݵ��ڶ�������ڶ���31,32��bit(bֵ)
//��59�������ݵ������������29,30,31,32��bit(bֵ)

//��60�������ݵ����ļ������25,26,27,28,29,30,31,32��bit(bֵ)
//��61�������ݵ����弶�����17-32��bit(bֵ)

//��62�������ݵ������������1-32��bit(bֵ)
//��63�������ݵ������������33-64��bit-----------׷��32bit
//*************************************************************************

//ר��дһ��׷�ݺ���(����)
//�趨ȫ׷��Ϊ0��2bitΪ1,4bitΪ2��8bitΪ3,16bitΪ4,32bitΪ5

int get_bit1_2(int position ,int lever)//����ÿ��׷�ݵõ�ƫ�ƺ�ĵ�һ�ڶ���ֵ
{
    //��һ�����裺�����һ����1,2��bit(*)
	int i = 0,j = 0;
	
	int i_start = (lever == 0)?0:(LEVEL-lever-1);//�뼶���Լ��й� 
	int j_start = (lever == 0)?0:(pow(2,lever)+position*pow(2,lever+1));//�뼶����ƫ�����й� 
	
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

//����
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
//����bֵ
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

int decode()//�������
{
	
	int i ;
	
	get_bit1_2(0,0);//ȫ׷��      **********[1]
	get_remain_b(0,2);//���Ƶڶ���b  **********[2]
	get_remain(0,2);//���Ƶڶ���     **********[3]
	
	get_bit1_2(0,1);//׷��2bit       **********[4]
	get_remain_b(1,2);//���Ƶڶ���b  **********[5]
	get_remain_b(0,3);//���Ƶ�����b  **********[6]
	get_remain(0,3);//���Ƶ�����     **********[7]
	
	get_bit1_2(0,2);//׷��4bit       **********[8]
	get_remain_b(2,2);//���Ƶڶ���b  **********[9]
	get_remain(1,2);//���Ƶڶ���     **********[10]
	
	get_bit1_2(1,1);//׷��2bit       **********[11]
	get_remain_b(3,2);//���Ƶڶ���b  **********[12]
	get_remain_b(1,3);//���Ƶ�����b  **********[13]
	get_remain_b(0,4);//���Ƶ��ļ�b  **********[14]
	get_remain(0,4);//���Ƶ��ļ�     **********[15]
	
	get_bit1_2(0,3);//׷��8bit       **********[16]
	get_remain_b(4,2);//���Ƶڶ���b  **********[17]
	get_remain(2,2);//���Ƶڶ���     **********[18]
	
	get_bit1_2(2,1);//׷��2bit       **********[19]
	get_remain_b(5,2);//���Ƶڶ���b  **********[20]
	get_remain_b(2,3);//���Ƶ�����b  **********[21]
	get_remain(1,3);//���Ƶ�����     **********[22]
	
	get_bit1_2(1,2);//׷��4bit       **********[23]
	get_remain_b(6,2);//���Ƶڶ���b  **********[24]
	get_remain(3,2);//���Ƶڶ���     **********[25]
	
	get_bit1_2(3,1);//׷��2bit       **********[26]
	
	
	get_remain_b(7,2);//���Ƶڶ���b  **********[27]
	get_remain_b(3,3);//���Ƶ�����b  **********[28]
	get_remain_b(1,4);//���Ƶ��ļ�b  **********[29]
	get_remain_b(0,5);//���Ƶ��弶b  **********[30]
	get_remain(0,5);//���Ƶ��弶     **********[31]
	
    get_bit1_2(0,4);//׷��16bit      **********[32]
	get_remain_b(8,2);//���Ƶڶ���b  **********[33]
	get_remain(4,2);//���Ƶڶ���     **********[34]
	
	get_bit1_2(4,1);//׷��2bit       **********[35]
	get_remain_b(9,2);//���Ƶڶ���b  **********[36]
	get_remain_b(4,3);//���Ƶ�����b  **********[37]
	get_remain(2,3);//���Ƶ�����     **********[38]
	
	get_bit1_2(2,2);//׷��4bit       **********[39]
	get_remain_b(10,2);//���Ƶڶ���b  **********[40]
	get_remain(5,2);//���Ƶڶ���     **********[41]
	
	get_bit1_2(5,1);//׷��2bit       **********[42]
	get_remain_b(11,2);//���Ƶڶ���b **********[43]
	get_remain_b(5,3);//���Ƶ�����b  **********[44]
	get_remain_b(2,4);//���Ƶ��ļ�b  **********[45]
	get_remain(1,4);//���Ƶ��ļ�     **********[46]
	
	get_bit1_2(1,3);//׷��8bit       **********[47]
	get_remain_b(12,2);//���Ƶڶ���b **********[48]
	get_remain(6,2);//���Ƶڶ���     **********[49]
	
	get_bit1_2(6,1);//׷��2bit       **********[50]
	get_remain_b(13,2);//���Ƶڶ���b **********[51]
	get_remain_b(6,3);//���Ƶ�����b  **********[52]
	get_remain(3,3);//���Ƶ�����     **********[53]
	
	get_bit1_2(3,2);//׷��4bit       **********[54]
	get_remain_b(14,2);//���Ƶڶ���b **********[55]
	get_remain(7,2);//���Ƶڶ���     **********[56]
	
	get_bit1_2(7,1);//׷��2bit       **********[57]finish
	get_remain_b(15,2);//���Ƶڶ���b **********[58]
	get_remain_b(7,3);//���Ƶ�����b  **********[59]
	get_remain_b(3,4);//���Ƶ��ļ�b  **********[60]
	get_remain_b(1,5);//���Ƶ��弶b  **********[61]
	get_remain_b(0,6);//���Ƶ�����b  **********[62]
	get_remain(0,6);//���Ƶ�����     **********[63]
	
	
	//��ӡ�������һ������
	i = 0;
	printf("\n\n");
    printf("����������%dbit����Ϊ",CODE_LENGTH);
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
