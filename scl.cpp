//======================================================================
//
//        Copyright (C) 2019-2022 Lab of the China University of Geosciences
//        All rights reserved
//
//        filename :$scl$
//        description : implement the polar code decoder with c 
//
//        created by xiangxufu at  $2019.7.4$
//        
//        https://github.com/xiangxufu/polarCode.git
//======================================================================

#include<D:\004_polarCode\scl_2019_7_7\scl\scl\scl.h>
/************************设计思路******************************************
1.之前的一系列操作包括编码，bpsk调制，加噪声都不变
2.加噪声之后进行译码，译码过程中以填充code_med[CODE_LENGTH]这个数组为线索一步步执行
3.code_med[]将被划分为1+1+2+4+8+16+32+64+...
4.每次函数执行完还需要确定存入code_med[]的偏移量
5.首先以8bit数据为例，函数执行的数据以及存入code_med[]中的位置的情况如下
   f(3次)-c(_bpsk/1次)-g(1次)-c(_bpsk/1次)-e(求出b值/多次)-g(1次)
*/
//*************************************************************底层函数*********************************************//
float bpsk(float *code_pre) //将编码后的数据进行bpsk调制 
{
	int i;
	for (i = 0; i < CODE_LENGTH; i++)
	{
		code_pre[i] = (code_pre[i] == 0) ? 1 : -1;
		
	//	code_med[0][i + CODE_LENGTH] = code_pre[i];
	}
/* 
	for (i = 1; i < L; i++)
	{
		memcpy(code_med[i], code_med[0], sizeof(code_med[0]));//内存复制
	}
	return 0;
*/
}
float f(float Lin1, float Lin2)//Lout = f(Lin1,Lin2)=sign(Lin1,Lin2)*min(|Lin1|,|Lin2|) //f函数 
{
	float Lout;
	Lout = ((Lin1*Lin2 > 0) ? 1 : -1)* ((fabsf(Lin1)>fabsf(Lin2)) ? fabsf(Lin2) : fabsf(Lin1));
	return Lout;
}

float g(float Lin1, float Lin2, char b)//Lout  = g(Lin1,Lin2,b) = (-1)^b*Lin1+Lin2 //g函数 
{
	float Lout;
	Lout = pow(-1, b)*Lin1 + Lin2;
	return Lout;
}

int e(int Lin1, int Lin2, char choose)//编码得到b值
{
	int Lout;
	Lout = (choose == 0) ? (Lin1^Lin2) : Lin2;
	return Lout;
}

char _bpsk(float bpsk)//bpsk的反过程 
{
	char reverse_bpsk;
	reverse_bpsk = (bpsk<0) ? 1 : 0;
	return reverse_bpsk;
}

double gaussrand()
{
	static double V1, V2, S;
	static int phase = 0;
	double X;
	if (phase == 0)
	{
		do
		{
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;
			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);
		X = V1 * sqrt(-2 * log(S) / S);
	}
	else
		X = V2 * sqrt(-2 * log(S) / S);
	phase = 1 - phase;
	return X;
}


//*************************************************************底层函数*********************************************//
//*************************************************************译码中层函数*****************************************//
//执行f运算流程
void F(float (*code_med)[CODE_LENGTH * 2],int *location)
{
	//location[0]:层数
	//location[1]:最后一个值的坐标
	//location[2]:规模大小
    //8bit时第一次调用执行三次f运算，坐标分别为[3,4,4][2,2,2][1,1,1]
	int i;
	int road;
	//这个for循环只需管每层的计算次数而不用在意计算多少层(留给外面顶层译码函数来控制)

	//运算之前进行一个坐标转化
	//F函数的坐标转化公式如下：
	location[0] = location[0] - 1;
	location[1] = location[1] - location[2]/2;
	location[2] = location[2]/2;

	for (road = 0; road < L; road++)
	{
		if (location[2] == 1)
		{
			code_med[road][1] = f(code_med[road][2], code_med[road][3]);//从code_med[1]处开始存数据 
		}
		else
		{
			for (i = 0; i < location[2]; i++)
			{
				code_med[road][location[2] + i] = f(code_med[road][location[2] * 2 + i], code_med[road][location[2] * 3 + i]);
			}
			//location[2] = CODE_LENGTH时从1/2地址处开始存储
			//location[2] = CODE_LENGTH/2时从1/4地址处开始存储 ...
		}
	}
}
//进行c运算译码得到每个bit
void C(float (*code_med)[CODE_LENGTH * 2], int (*code_last)[CODE_LENGTH],int *location)
{
 	int i = 0, j = 0;
	
	int flag[L] = {0};//前L位置保存需要被替换的坐标，后L个位置保存需要入列的坐标
	int min = 0;//PM最小值的坐标
	int road = 0;
	//int location_replace;//需要替换的路径的坐标位置
	//[1,1,1]->[1][2][1]->[1][3][1]->...->[1][8][1]
	//运算之前进行一个坐标转化
	//C函数的坐标转化公式如下：
	location[0] = location[0];
	location[1] = location[1];
	location[2] = location[2];
 
	/*
	L = 8
	bit1		    bit2		    bit3
	i=0(T)	j = 0	i=0(T)	j = 0	i=0(T)	j = 0
	        j = 1		    j = 1	i=1(F)	j = 0
	        j = 2	i=1(F)	j = 0	i=2(T)	j = 0
	        j = 3		    j = 1	i=3(F)	j = 0
	i=1(F)	j = 0	i=2(T)	j = 0	i=4(T)	j = 0
	        j = 1		    j = 1	i=5(F)	j = 0
	        j = 2	i=3(F)	j = 0	i=6(T)	j = 0
	        j = 3		    j = 1	i=7(F)	j = 0
	*/
	/*
	L = 4
	bit1		    bit2		    
	i=0(T)	j = 0	i=0(T)	j = 0
		    j = 1	i=1(F)	j = 0
	i=1(F)	j = 0	i=2(T)	j = 0
		    j = 1	i=3(F)	j = 0
	*/	
		/*
	L = 2
	bit1		   		    
	i=0(T)	j = 0
    i=1(F)	j = 0	
   */
	
	if(!Ac[location[1] - 1])//冻结bit处全部译为0，PM值更新，同时路径不分裂
	{
		for(road = 0;road <L;road++)
		{
			code_last[road][location[1] - 1] = 0;//冻结bit处一律译成0
			if (_bpsk(code_med[road][1]))
			    {
				   //冻结bit处本应该译成1结果0译成1导致PM值减少 
				   PM[road] = PM[road] - fabsf(code_med[road][1]);
				}
					
		}
			
	} 
    else
    { 
	    valid++; //记录信息bit的个数 
	   //路径小于设定路径时一直分裂
		if (pow(2, valid) <= L)//L =2 
		{
		    while (i < pow(2, valid))
			{
				for (j = 0; j < L / (int)pow(2,valid); j++)
				{
					road = i*L / (int)pow(2, valid) + j;
						//信息位处一半路径按常规译法一半路径按照非常规译法
					code_last[road][location[1] - 1] = (i % 2) ? (!_bpsk(code_med[road][1])):_bpsk(code_med[road][1]);
					    //PM[i] = PM[i] -|LLR|	
						//按照非常规译法完成的路径PM值减小 
					if (i % 2) { PM[road] = PM[road] - fabsf(code_med[i][1]); }
				}
				i++;
			} 
		}
		//当达到设定值之后开始选择
		//1.先获取PM值最小的路径
		//2.再找一条PM非最小值进行分裂（可以按照自然顺序，不需要排序）
		//3.将分裂新出现的路径的PM值与最小PM值路径进行比较，小于则舍弃新路径保留小路径(1)大于则保留新路径舍弃小路径(2)
		//4.(1)下不需要再次取最小值（2）下需要再次取得最小值
	    //5.经过4之后，根据情况判断是否需要再次获取最小值
		//6.接着重复2-5过程(需要注意的是如果处于（2）情况下非最小路径会发生变化)
		//PM值排序以及路径替换
		else if (pow(2, valid) > L)
		{
			
			//先正常译码不涉及路径分裂 
			for(road = 0;road<L;road++)
			{
		     	code_last[road][location[1] - 1] = _bpsk(code_med[road][1]); 
		     	
			} 
			//路径预生成（PM值生成） 
			for (i = 0; i < L; i++)
			{
				//找出路径中PM最小的位置
				for (j = 0; j < L; j++)
				{

					if (PM[min] > PM[j])
					{
						min = j;

					}
				}
				if(flag[i] == 0)//没有替换过的位置才可以预分裂 
				{
				//进行PM值预分裂并将预分裂后的PM值与上述最小值进行比较
			    	PM[L + i] = PM[i] - fabsf(code_med[i][1]);
				    if (PM[L + i] > PM[min])//PM值预分裂的值较大则入列
				    { 
					//flag[0][min] = 1;//记录哪些位置PM值被舍弃，被取代的地方需要进行内存替代
					  flag[min] = i+1;//记录哪些位置分裂的数据可以入列,非零的位置表示可以替换，其替换值坐标即为其数组值减一 
					  PM[min] = PM[L + i];//更新PM值
				    }
			    }
				 
			}		
			//进行路径替换 
			for (j = 0; j < L; j++)
			{
				if (flag[j])//为0表示不需要被替换非零表示需要被替换同时给出了分裂的坐标 
				{
					//先把之前的数据复制过来
					memcpy(code_last[j], code_last[flag[j]-1], sizeof(code_last[j]));
					memcpy(code_med[j], code_med[flag[j]-1], sizeof(code_med[j]));
					memcpy(code_b[j], code_b[flag[j]-1], sizeof(code_b[j]));
					//再更新最新一位数据
					code_last[j][location[1] - 1] = !_bpsk(code_med[j][1]);
				}
			}

		}
    } 
}

//进行g运算，推出本级留下的值
void G(int (*code_last)[CODE_LENGTH], float (*code_med)[CODE_LENGTH * 2],int (*code_b)[CODE_LENGTH], int *location)
{
	//运算之前进行一个坐标转化
	//G函数的坐标转化公式如下：
	int i;
	int road;
	location[0] = location[0];
	location[1] = location[1] + location[2];
	location[2] = location[2];

	for (road = 0; road < L; road++)//对每条路径都执行此运算
	{
		if (location[2] == 1)
		{
			code_med[road][1] = g(code_med[road][2], code_med[road][3], code_last[road][location[1] - 2]);
		}
		else
			for (i = 0; i < location[2]; i++)
			{
				code_med[road][location[2] + i] = g(code_med[road][location[2] * 2 + i], code_med[road][location[2] * 3 + i], code_b[road][location[1] - location[2] * 2 + i]);
			}
	}

}

//进行E运算，推出b值
void E(int (*code_last)[CODE_LENGTH], int (*code_b)[CODE_LENGTH], int *location)
{

	//运算之前进行一个坐标转化
	//G函数的坐标转化公式如下：
	int i,j;
	int road;
	location[0] = location[0]+1;
	location[1] = location[1];
	location[2] = location[2]*2;

	for (road = 0; road < L; road++)
	{
		if (location[2] == 2)
		{
			code_b[road][location[1] - 2] = e(code_last[road][location[1] - 2], code_last[road][location[1] - 1], 0);
			code_b[road][location[1] - 1] = e(code_last[road][location[1] - 2], code_last[road][location[1] - 1], 1);
		}
		else
		{
			for (j = 0; j < location[2] / 2; j++)//前一半数据通过异或运算得到
				code_b[road][location[1] - location[2] + j] = e(code_b[road][location[1] - location[2] + j], code_b[road][location[1] - location[2] / 2 + j], 0);
			for (j = 0; j < location[2] / 2; j++)//后一半数据直接跟随
				code_b[road][location[1] - location[2] / 2 + j] = e(code_b[road][location[1] - location[2] + j], code_b[road][location[1] - location[2] / 2 + j], 1);
		}
	}
}
//*************************************************************译码中层函数*****************************************//
//*************************************************************译码顶层函数*****************************************//
//控制整个译码流程
void decode()
{
	int i,j;
	for (i = 0; i < CODE_LENGTH / 2; i++)
	{
		j = 0; 
		//f(3次) - c(_bpsk / 1次) - g(1次) - c(_bpsk / 1次) - e(求出b值 / 多次) - g(1次)
		while (location[2] > 1) { F(code_med, location); }
		C(code_med, code_last, location);//译出码值，并将其存入code_last数组 
		G(code_last, code_med,code_b,location);//推出第一级剩余的数值 
 		C(code_med, code_last, location);
		do
		{
			E(code_last, code_b, location);//正推b值 
			j++;//统计编码次数 
		} while (!((location[1] >> j) & 1));
		G(code_last,code_med,code_b,location);//推出第lacation[0]级剩余的 
	}
	
}

//*************************************************************译码顶层函数*****************************************//
//*************************************************************编码调制加噪统计*****************************************//
//AC转换得到编码数据
void AC()
{
	int i;
	switch (CODE_LENGTH)
	{
	case 4:memcpy(Ac, Ac_4, sizeof(Ac)); break;
	case 8:memcpy(Ac, Ac_8, sizeof(Ac)); break;
	case 16:memcpy(Ac, Ac_16, sizeof(Ac)); break;
	case 32:memcpy(Ac, Ac_32, sizeof(Ac)); break;
	case 64:memcpy(Ac, Ac_64, sizeof(Ac)); break;
	case 128:memcpy(Ac, Ac_128, sizeof(Ac)); break;
	case 256:memcpy(Ac, Ac_256, sizeof(Ac)); break;
	case 512:memcpy(Ac, Ac_512, sizeof(Ac)); break;
	case 1024:memcpy(Ac, Ac_1024, sizeof(Ac)); break;
	case 2048:memcpy(Ac, Ac_2048, sizeof(Ac)); break;
	case 4096:memcpy(Ac, Ac_4096, sizeof(Ac)); break;

	default:memcpy(Ac, Ac_8, sizeof(Ac));
	}

	for (i = 0; i < CODE_LENGTH; i++)
	{
		code_pre[i] = (Ac[i]) && (rand() % 2); 
		originalCode[i] = code_pre[i];//保存原始数据方便后续对比验证 
	}

}

//加噪声
float add_gassrand(float EbNo)
{

	int i;

	float Sigma2;//噪声方差
	float Sigma;//噪声标准差
	float Rate = (CODE_LENGTH / 2) / (float)CODE_LENGTH;//数据的传输速率

	Sigma2 = (float)1 / (2 * Rate*pow(10, (EbNo / 10.0)));//白噪声的方差
	Sigma = sqrtf(Sigma2);//白噪声的标准差

	for (i = 0; i < CODE_LENGTH; i++)
	{
		//printf("加噪声之前polarCode[0][i] = %.2lf \t" ,polarCode[0][i]  ); 
		code_pre[i] = 2 * (code_pre[i] + gaussrand()*Sigma) / Sigma2;
		code_med[0][i + CODE_LENGTH] = code_pre[i];
		//printf("加噪声之后polarCode[0][i] = %.2lf \t" ,polarCode[0][i]  ); 
		//printf("\n");
	}
	for (i = 1; i < L; i++)
	{
		memcpy(code_med[i], code_med[0],sizeof(code_med[0]));//内存复制
	}

	return 0;
}

//编码
void encode()//执行polar码编码
{
	int level = LEVEL;
	while (level)
	{

		int i, j = 0;
		int per_level = (CODE_LENGTH >> (int)(LEVEL - level + 1));//每一级分组运算的组数
		int per_times = pow(2, LEVEL - level + 1);//每组包含的运算次数

		for (i = 0; i < per_level; i++)//每级运算级数为上一级的一半
		{
			for (j = 0; j < per_times / 2; j++)//前一半数据通过异或运算得到
				code_pre[j + i*per_times] = (int)code_pre[j + i*per_times] ^ (int)code_pre[j + i*per_times + per_times / 2];
			for (j = per_times / 2; j < per_times; j++)//后一半数据直接跟随
				code_pre[j + i*per_times] = code_pre[j + i*per_times];
		}
		level--;
		//	if (level)encode();//递归 
	}

}

int statistics()//每次执行 
{
	int i =0,min = 0;
	int error = 0;

	int LAST[CODE_LENGTH];
    

	//选择PM值最大(绝对值最小)的一条路径与原始路径进行对比
	for (i = 0; i < L; i++)//L条路径中找最小的，暂时形成的路径（后L条）不考虑 
	{
		if (PM[min] < PM[i])
		{
			min = i;//记录的编号
		
		}
	}
	memcpy(LAST, code_last[min], sizeof(LAST));//PM值最小的作为最终结果输出
    if(min == 0) times_PM0++; 
  
	i = CODE_LENGTH;
	while (i--) {
		if (LAST[i] - originalCode[i]) error++;
	}
	return error;

}
//*************************************************************编码调制加噪统计*****************************************//
//*************************************************************主函数***********************************************//
int main(/*int argc, char *argv[]*/)
{
	int i = 0;
	long sim_times;
	int BLER = 0;
	float ar_gv[3] = {2,2,10000};//(参数个数，信噪比，测试次数) 
	sim_times = (int)ar_gv[2];
	for (i; i<sim_times; i++)
	{
		memset(PM,0,sizeof(PM));
		
		location[0] = LEVEL;//初始化坐标值
		location[1] = CODE_LENGTH;//初始化坐标值
		location[2] = CODE_LENGTH;//初始化坐标值
		
		valid = 0;//信息bit个数置零 
		
		AC();//生成随机数序列同时给定冻结bit位置
		encode();//执行polar码编码
		bpsk(code_pre);//bpsk调制
		add_gassrand(ar_gv[1]);//加高斯白噪声(信噪比)
		decode();//译码
		if (statistics())BLER++;

		//  printf("仿真次数为sim_times=%d\n 错误次数为BLER=%d",sim_times,BLER);

	}
	printf("仿真次数为sim_times=%d 错误次数为BLER=%d 取PM[0]的个数为%d", sim_times, BLER,times_PM0);

	return 0;

}
//*************************************************************主函数***********************************************//
//*************************************************************在SC译码器的基础上进行改进，从而实现SCL译码器**************************************//
/*设计思路
1.路径在每个bit数据译码出现的时候分裂
2.PM值不进行排序，而是直接采用替换最小值的方法来选取路径
3.路径排序分裂的顺序为以下：分裂成2L->替换保留L条路径->

*/
//*************************************************************在SC译码器的基础上进行改进，从而实现SCL译码器**************************************//
