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
//实现polar码任意码长的编码
#include <stdio.h> //C头文件
#include <stdlib.h>
#include <math.h>
#define CODE_LENGTH 8 //定义码长为8
#define LEVEL  log(CODE_LENGTH)/log(2)//编码级数为2的N次幂
static int level = LEVEL;//每完成一级编码此值递减，初值为编码级数
int polarCode[CODE_LENGTH];

int load()//加载原始数据
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/original.txt";
	FILE* fp;
	fp = fopen(filename, "r");
	for (i=0;i<CODE_LENGTH;i++)
    fscanf(fp,"%d", &polarCode[i]); // 循环读
    fclose(fp);
	
    i = 0;
    printf("原始数据为");
	while(i<CODE_LENGTH) 
	{
    printf("%d ",polarCode[i]);
    i++; 
    }
    return 0;
	
}
int write()//将编码后的数据写入文件 
{
	int i;
	const char* filename = "C:/disk/004_polarCode/code/c/encode.txt";
	FILE* fp;
	fp = fopen(filename, "w+");
	for (i=0;i<CODE_LENGTH;i++)
	{
    fprintf(fp,"%d", polarCode[i]); // 写入编码后的数据 
    fprintf(fp,"%s", " "); // 写入空格 
    }
    fclose(fp);
    
    printf("\n\n");
    i = 0;
    printf("经过编码后的数据为");
	while(i<CODE_LENGTH) 
	{
    printf("%d ",polarCode[i]);
    i++; 
    }
	return 0;
}

int encode()//执行polar码编码
{ 
	int i, j = 0;
	int per_level = (CODE_LENGTH >> (int)(LEVEL - level + 1));//每一级分组运算的组数
	int per_times = pow(2, LEVEL - level + 1);//每组包含的运算次数

	for (i = 0; i < per_level; i++)//每级运算级数为上一级的一半
	{
	 for (j = 0;j < per_times/2; j++)//前一半数据通过异或运算得到
		polarCode[j + i*per_times] = polarCode[j + i*per_times] ^ polarCode[j + i*per_times + per_times / 2];
	 for (j = per_times/2;j < per_times; j++)//后一半数据直接跟随
	    polarCode[j + i*per_times] = polarCode[j + i*per_times];
	}
	level--;
	if(level)encode();//递归 
	

	return 0;
}

int main(void)
{
	load();//加载原始数据
	encode();//执行polar码编码
	write();//加载原始数据
}


