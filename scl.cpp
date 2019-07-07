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
/************************���˼·******************************************
1.֮ǰ��һϵ�в����������룬bpsk���ƣ�������������
2.������֮��������룬��������������code_med[CODE_LENGTH]�������Ϊ����һ����ִ��
3.code_med[]��������Ϊ1+1+2+4+8+16+32+64+...
4.ÿ�κ���ִ���껹��Ҫȷ������code_med[]��ƫ����
5.������8bit����Ϊ��������ִ�е������Լ�����code_med[]�е�λ�õ��������
   f(3��)-c(_bpsk/1��)-g(1��)-c(_bpsk/1��)-e(���bֵ/���)-g(1��)
*/
//*************************************************************�ײ㺯��*********************************************//
float bpsk(float *code_pre) //�����������ݽ���bpsk���� 
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
		memcpy(code_med[i], code_med[0], sizeof(code_med[0]));//�ڴ渴��
	}
	return 0;
*/
}
float f(float Lin1, float Lin2)//Lout = f(Lin1,Lin2)=sign(Lin1,Lin2)*min(|Lin1|,|Lin2|) //f���� 
{
	float Lout;
	Lout = ((Lin1*Lin2 > 0) ? 1 : -1)* ((fabsf(Lin1)>fabsf(Lin2)) ? fabsf(Lin2) : fabsf(Lin1));
	return Lout;
}

float g(float Lin1, float Lin2, char b)//Lout  = g(Lin1,Lin2,b) = (-1)^b*Lin1+Lin2 //g���� 
{
	float Lout;
	Lout = pow(-1, b)*Lin1 + Lin2;
	return Lout;
}

int e(int Lin1, int Lin2, char choose)//����õ�bֵ
{
	int Lout;
	Lout = (choose == 0) ? (Lin1^Lin2) : Lin2;
	return Lout;
}

char _bpsk(float bpsk)//bpsk�ķ����� 
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


//*************************************************************�ײ㺯��*********************************************//
//*************************************************************�����в㺯��*****************************************//
//ִ��f��������
void F(float (*code_med)[CODE_LENGTH * 2],int *location)
{
	//location[0]:����
	//location[1]:���һ��ֵ������
	//location[2]:��ģ��С
    //8bitʱ��һ�ε���ִ������f���㣬����ֱ�Ϊ[3,4,4][2,2,2][1,1,1]
	int i;
	int road;
	//���forѭ��ֻ���ÿ��ļ���������������������ٲ�(�������涥�����뺯��������)

	//����֮ǰ����һ������ת��
	//F����������ת����ʽ���£�
	location[0] = location[0] - 1;
	location[1] = location[1] - location[2]/2;
	location[2] = location[2]/2;

	for (road = 0; road < L; road++)
	{
		if (location[2] == 1)
		{
			code_med[road][1] = f(code_med[road][2], code_med[road][3]);//��code_med[1]����ʼ������ 
		}
		else
		{
			for (i = 0; i < location[2]; i++)
			{
				code_med[road][location[2] + i] = f(code_med[road][location[2] * 2 + i], code_med[road][location[2] * 3 + i]);
			}
			//location[2] = CODE_LENGTHʱ��1/2��ַ����ʼ�洢
			//location[2] = CODE_LENGTH/2ʱ��1/4��ַ����ʼ�洢 ...
		}
	}
}
//����c��������õ�ÿ��bit
void C(float (*code_med)[CODE_LENGTH * 2], int (*code_last)[CODE_LENGTH],int *location)
{
 	int i = 0, j = 0;
	
	int flag[L] = {0};//ǰLλ�ñ�����Ҫ���滻�����꣬��L��λ�ñ�����Ҫ���е�����
	int min = 0;//PM��Сֵ������
	int road = 0;
	//int location_replace;//��Ҫ�滻��·��������λ��
	//[1,1,1]->[1][2][1]->[1][3][1]->...->[1][8][1]
	//����֮ǰ����һ������ת��
	//C����������ת����ʽ���£�
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
	
	if(!Ac[location[1] - 1])//����bit��ȫ����Ϊ0��PMֵ���£�ͬʱ·��������
	{
		for(road = 0;road <L;road++)
		{
			code_last[road][location[1] - 1] = 0;//����bit��һ�����0
			if (_bpsk(code_med[road][1]))
			    {
				   //����bit����Ӧ�����1���0���1����PMֵ���� 
				   PM[road] = PM[road] - fabsf(code_med[road][1]);
				}
					
		}
			
	} 
    else
    { 
	    valid++; //��¼��Ϣbit�ĸ��� 
	   //·��С���趨·��ʱһֱ����
		if (pow(2, valid) <= L)//L =2 
		{
		    while (i < pow(2, valid))
			{
				for (j = 0; j < L / (int)pow(2,valid); j++)
				{
					road = i*L / (int)pow(2, valid) + j;
						//��Ϣλ��һ��·���������뷨һ��·�����շǳ����뷨
					code_last[road][location[1] - 1] = (i % 2) ? (!_bpsk(code_med[road][1])):_bpsk(code_med[road][1]);
					    //PM[i] = PM[i] -|LLR|	
						//���շǳ����뷨��ɵ�·��PMֵ��С 
					if (i % 2) { PM[road] = PM[road] - fabsf(code_med[i][1]); }
				}
				i++;
			} 
		}
		//���ﵽ�趨ֵ֮��ʼѡ��
		//1.�Ȼ�ȡPMֵ��С��·��
		//2.����һ��PM����Сֵ���з��ѣ����԰�����Ȼ˳�򣬲���Ҫ����
		//3.�������³��ֵ�·����PMֵ����СPMֵ·�����бȽϣ�С����������·������С·��(1)����������·������С·��(2)
		//4.(1)�²���Ҫ�ٴ�ȡ��Сֵ��2������Ҫ�ٴ�ȡ����Сֵ
	    //5.����4֮�󣬸�������ж��Ƿ���Ҫ�ٴλ�ȡ��Сֵ
		//6.�����ظ�2-5����(��Ҫע�����������ڣ�2������·���С·���ᷢ���仯)
		//PMֵ�����Լ�·���滻
		else if (pow(2, valid) > L)
		{
			
			//���������벻�漰·������ 
			for(road = 0;road<L;road++)
			{
		     	code_last[road][location[1] - 1] = _bpsk(code_med[road][1]); 
		     	
			} 
			//·��Ԥ���ɣ�PMֵ���ɣ� 
			for (i = 0; i < L; i++)
			{
				//�ҳ�·����PM��С��λ��
				for (j = 0; j < L; j++)
				{

					if (PM[min] > PM[j])
					{
						min = j;

					}
				}
				if(flag[i] == 0)//û���滻����λ�òſ���Ԥ���� 
				{
				//����PMֵԤ���Ѳ���Ԥ���Ѻ��PMֵ��������Сֵ���бȽ�
			    	PM[L + i] = PM[i] - fabsf(code_med[i][1]);
				    if (PM[L + i] > PM[min])//PMֵԤ���ѵ�ֵ�ϴ�������
				    { 
					//flag[0][min] = 1;//��¼��Щλ��PMֵ����������ȡ���ĵط���Ҫ�����ڴ����
					  flag[min] = i+1;//��¼��Щλ�÷��ѵ����ݿ�������,�����λ�ñ�ʾ�����滻�����滻ֵ���꼴Ϊ������ֵ��һ 
					  PM[min] = PM[L + i];//����PMֵ
				    }
			    }
				 
			}		
			//����·���滻 
			for (j = 0; j < L; j++)
			{
				if (flag[j])//Ϊ0��ʾ����Ҫ���滻�����ʾ��Ҫ���滻ͬʱ�����˷��ѵ����� 
				{
					//�Ȱ�֮ǰ�����ݸ��ƹ���
					memcpy(code_last[j], code_last[flag[j]-1], sizeof(code_last[j]));
					memcpy(code_med[j], code_med[flag[j]-1], sizeof(code_med[j]));
					memcpy(code_b[j], code_b[flag[j]-1], sizeof(code_b[j]));
					//�ٸ�������һλ����
					code_last[j][location[1] - 1] = !_bpsk(code_med[j][1]);
				}
			}

		}
    } 
}

//����g���㣬�Ƴ��������µ�ֵ
void G(int (*code_last)[CODE_LENGTH], float (*code_med)[CODE_LENGTH * 2],int (*code_b)[CODE_LENGTH], int *location)
{
	//����֮ǰ����һ������ת��
	//G����������ת����ʽ���£�
	int i;
	int road;
	location[0] = location[0];
	location[1] = location[1] + location[2];
	location[2] = location[2];

	for (road = 0; road < L; road++)//��ÿ��·����ִ�д�����
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

//����E���㣬�Ƴ�bֵ
void E(int (*code_last)[CODE_LENGTH], int (*code_b)[CODE_LENGTH], int *location)
{

	//����֮ǰ����һ������ת��
	//G����������ת����ʽ���£�
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
			for (j = 0; j < location[2] / 2; j++)//ǰһ������ͨ���������õ�
				code_b[road][location[1] - location[2] + j] = e(code_b[road][location[1] - location[2] + j], code_b[road][location[1] - location[2] / 2 + j], 0);
			for (j = 0; j < location[2] / 2; j++)//��һ������ֱ�Ӹ���
				code_b[road][location[1] - location[2] / 2 + j] = e(code_b[road][location[1] - location[2] + j], code_b[road][location[1] - location[2] / 2 + j], 1);
		}
	}
}
//*************************************************************�����в㺯��*****************************************//
//*************************************************************���붥�㺯��*****************************************//
//����������������
void decode()
{
	int i,j;
	for (i = 0; i < CODE_LENGTH / 2; i++)
	{
		j = 0; 
		//f(3��) - c(_bpsk / 1��) - g(1��) - c(_bpsk / 1��) - e(���bֵ / ���) - g(1��)
		while (location[2] > 1) { F(code_med, location); }
		C(code_med, code_last, location);//�����ֵ�����������code_last���� 
		G(code_last, code_med,code_b,location);//�Ƴ���һ��ʣ�����ֵ 
 		C(code_med, code_last, location);
		do
		{
			E(code_last, code_b, location);//����bֵ 
			j++;//ͳ�Ʊ������ 
		} while (!((location[1] >> j) & 1));
		G(code_last,code_med,code_b,location);//�Ƴ���lacation[0]��ʣ��� 
	}
	
}

//*************************************************************���붥�㺯��*****************************************//
//*************************************************************������Ƽ���ͳ��*****************************************//
//ACת���õ���������
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
		originalCode[i] = code_pre[i];//����ԭʼ���ݷ�������Ա���֤ 
	}

}

//������
float add_gassrand(float EbNo)
{

	int i;

	float Sigma2;//��������
	float Sigma;//������׼��
	float Rate = (CODE_LENGTH / 2) / (float)CODE_LENGTH;//���ݵĴ�������

	Sigma2 = (float)1 / (2 * Rate*pow(10, (EbNo / 10.0)));//�������ķ���
	Sigma = sqrtf(Sigma2);//�������ı�׼��

	for (i = 0; i < CODE_LENGTH; i++)
	{
		//printf("������֮ǰpolarCode[0][i] = %.2lf \t" ,polarCode[0][i]  ); 
		code_pre[i] = 2 * (code_pre[i] + gaussrand()*Sigma) / Sigma2;
		code_med[0][i + CODE_LENGTH] = code_pre[i];
		//printf("������֮��polarCode[0][i] = %.2lf \t" ,polarCode[0][i]  ); 
		//printf("\n");
	}
	for (i = 1; i < L; i++)
	{
		memcpy(code_med[i], code_med[0],sizeof(code_med[0]));//�ڴ渴��
	}

	return 0;
}

//����
void encode()//ִ��polar�����
{
	int level = LEVEL;
	while (level)
	{

		int i, j = 0;
		int per_level = (CODE_LENGTH >> (int)(LEVEL - level + 1));//ÿһ���������������
		int per_times = pow(2, LEVEL - level + 1);//ÿ��������������

		for (i = 0; i < per_level; i++)//ÿ�����㼶��Ϊ��һ����һ��
		{
			for (j = 0; j < per_times / 2; j++)//ǰһ������ͨ���������õ�
				code_pre[j + i*per_times] = (int)code_pre[j + i*per_times] ^ (int)code_pre[j + i*per_times + per_times / 2];
			for (j = per_times / 2; j < per_times; j++)//��һ������ֱ�Ӹ���
				code_pre[j + i*per_times] = code_pre[j + i*per_times];
		}
		level--;
		//	if (level)encode();//�ݹ� 
	}

}

int statistics()//ÿ��ִ�� 
{
	int i =0,min = 0;
	int error = 0;

	int LAST[CODE_LENGTH];
    

	//ѡ��PMֵ���(����ֵ��С)��һ��·����ԭʼ·�����жԱ�
	for (i = 0; i < L; i++)//L��·��������С�ģ���ʱ�γɵ�·������L���������� 
	{
		if (PM[min] < PM[i])
		{
			min = i;//��¼�ı��
		
		}
	}
	memcpy(LAST, code_last[min], sizeof(LAST));//PMֵ��С����Ϊ���ս�����
    if(min == 0) times_PM0++; 
  
	i = CODE_LENGTH;
	while (i--) {
		if (LAST[i] - originalCode[i]) error++;
	}
	return error;

}
//*************************************************************������Ƽ���ͳ��*****************************************//
//*************************************************************������***********************************************//
int main(/*int argc, char *argv[]*/)
{
	int i = 0;
	long sim_times;
	int BLER = 0;
	float ar_gv[3] = {2,2,10000};//(��������������ȣ����Դ���) 
	sim_times = (int)ar_gv[2];
	for (i; i<sim_times; i++)
	{
		memset(PM,0,sizeof(PM));
		
		location[0] = LEVEL;//��ʼ������ֵ
		location[1] = CODE_LENGTH;//��ʼ������ֵ
		location[2] = CODE_LENGTH;//��ʼ������ֵ
		
		valid = 0;//��Ϣbit�������� 
		
		AC();//�������������ͬʱ��������bitλ��
		encode();//ִ��polar�����
		bpsk(code_pre);//bpsk����
		add_gassrand(ar_gv[1]);//�Ӹ�˹������(�����)
		decode();//����
		if (statistics())BLER++;

		//  printf("�������Ϊsim_times=%d\n �������ΪBLER=%d",sim_times,BLER);

	}
	printf("�������Ϊsim_times=%d �������ΪBLER=%d ȡPM[0]�ĸ���Ϊ%d", sim_times, BLER,times_PM0);

	return 0;

}
//*************************************************************������***********************************************//
//*************************************************************��SC�������Ļ����Ͻ��иĽ����Ӷ�ʵ��SCL������**************************************//
/*���˼·
1.·����ÿ��bit����������ֵ�ʱ�����
2.PMֵ���������򣬶���ֱ�Ӳ����滻��Сֵ�ķ�����ѡȡ·��
3.·��������ѵ�˳��Ϊ���£����ѳ�2L->�滻����L��·��->

*/
//*************************************************************��SC�������Ļ����Ͻ��иĽ����Ӷ�ʵ��SCL������**************************************//
