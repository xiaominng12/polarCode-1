//======================================================================
//
//        Copyright (C) 2019-2022 ECIE of the China University of Geosciences
//        All rights reserved
//
//        filename :$encode$
//        description : implement the polar code encoder with c 
//
//        created by xiangxufu at  $2019.6.5$
//        
//        https://github.com/xiangxufu/polarCode.git
//======================================================================
//ʵ��polar�������볤�ı���
#include <stdio.h> //Cͷ�ļ�
#include <stdlib.h>
#include <math.h>
#define CODE_LENGTH 8 //�����볤Ϊ8
#define LEVEL  log(CODE_LENGTH)/log(2)//���뼶��Ϊ2��N����
static int level = LEVEL;//ÿ���һ�������ֵ�ݼ�����ֵΪ���뼶��
int polarCode[CODE_LENGTH];

int load()//����ԭʼ����
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/original.txt";
	FILE* fp;
	fp = fopen(filename, "r");
	for (i=0;i<CODE_LENGTH;i++)
    fscanf(fp,"%d", &polarCode[i]); // ѭ����
    fclose(fp);
	
    i = 0;
    printf("ԭʼ����Ϊ");
	while(i<CODE_LENGTH) 
	{
    printf("%d ",polarCode[i]);
    i++; 
    }
    return 0;
	
}
int write()//������������д���ļ� 
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/encode.txt";
	FILE* fp;
	fp = fopen(filename, "w+");
	for (i=0;i<CODE_LENGTH;i++)
	{
    fprintf(fp,"%d", polarCode[i]); // д����������� 
    fprintf(fp,"%s", " "); // д��ո� 
    }
    fclose(fp);
    
    printf("\n\n");
    i = 0;
    printf("��������������Ϊ");
	while(i<CODE_LENGTH) 
	{
    printf("%d ",polarCode[i]);
    i++; 
    }
	return 0;
}

int encode()//ִ��polar�����
{ 
	int i, j = 0;
	int per_level = (CODE_LENGTH >> (int)(LEVEL - level + 1));//ÿһ���������������
	int per_times = pow(2, LEVEL - level + 1);//ÿ��������������

	for (i = 0; i < per_level; i++)//ÿ�����㼶��Ϊ��һ����һ��
	{
	 for (j = 0;j < per_times/2; j++)//ǰһ������ͨ���������õ�
		polarCode[j + i*per_times] = polarCode[j + i*per_times] ^ polarCode[j + i*per_times + per_times / 2];
	 for (j = per_times/2;j < per_times; j++)//��һ������ֱ�Ӹ���
	    polarCode[j + i*per_times] = polarCode[j + i*per_times];
	}
	level--;
	if(level)encode();//�ݹ� 
	

	return 0;
}

int main(void)
{
	load();//����ԭʼ����
	encode();//ִ��polar�����
	write();//����ԭʼ����
}


