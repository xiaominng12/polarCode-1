//======================================================================
//
//        Copyright (C) 2019-2022 Lab of the China University of Geosciences
//        All rights reserved
//
//        filename :$sc$
//        description : implement the polar code encoder with c 
//
//        created by xiangxufu at  $2019.7.4$
//        
//        https://github.com/xiangxufu/polarCode.git
//======================================================================

#include<C:\disk\004_polarCode\code\c\sc_1019_7_3\sc\sc\sc.h>
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
		
		code_med[i + CODE_LENGTH] = code_pre[i];
		
	}

	return 0;
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


/*
//����
void e(int *encode_pre,int *encode_post,int *location)//ִ��polar�����
{
	int level = location[0];
	while (level)
	{

		int i, j = 0;
		int per_level = (location[2] >> (int)(location[0] - level + 1));//ÿһ���������������
		int per_times = pow(2, location[0] - level + 1);//ÿ��������������

		for (i = 0; i < per_level; i++)//ÿ�����㼶��Ϊ��һ����һ��
		{
			for (j = 0; j < per_times / 2; j++)//ǰһ������ͨ���������õ�
				encode_pre[j + i*per_times] = (int)encode_pre[j + i*per_times] ^ (int)encode_pre[j + i*per_times + per_times / 2];
			for (j = per_times / 2; j < per_times; j++)//��һ������ֱ�Ӹ���
				encode_pre[j + i*per_times] = encode_pre[j + i*per_times];
		}
		level--;
	}
	memcpy(encode_post, encode_pre, sizeof(encode_post));
}
*/

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
void F(float *code_pre, float *code_med,int *location)
{
	//location[0]:����
	//location[1]:���һ��ֵ������
	//location[2]:��ģ��С
    //8bitʱ��һ�ε���ִ������f���㣬����ֱ�Ϊ[3,4,4][2,2,2][1,1,1]
	int i;
	//���forѭ��ֻ���ÿ��ļ���������������������ٲ�(�������涥�����뺯��������)

	//����֮ǰ����һ������ת��
	//F����������ת����ʽ���£�
	location[0] = location[0] - 1;
	location[1] = location[1] - location[2]/2;
	location[2] = location[2]/2;


		if (location[2] == 1)
		{
			code_med[1] = f(code_med[2], code_med[3]);//��code_med[1]����ʼ������ 
		}
		else 
		{
			for (i = 0; i < location[2]; i++)
			{
				code_med[location[2] + i] = f(code_med[location[2] * 2 + i], code_med[location[2] * 3 + i]);
			}
			//location[2] = CODE_LENGTHʱ��1/2��ַ����ʼ�洢
		    //location[2] = CODE_LENGTH/2ʱ��1/4��ַ����ʼ�洢 ...
		}
}
//����c��������õ�ÿ��bit
void C(float *code_med, int *code_last,int *location)
{
	//[1,1,1]->[1][2][1]->[1][3][1]->...->[1][8][1]
	//����֮ǰ����һ������ת��
	//C����������ת����ʽ���£�
	location[0] = location[0];
	location[1] = location[1];
	location[2] = location[2];
 
	code_last[location[1]-1] = Ac[location[1]-1]&&_bpsk(code_med[1]);
}

//����g���㣬�Ƴ��������µ�ֵ
void G(int *code_last, float *code_med,int *code_b, int *location)
{
	//����֮ǰ����һ������ת��
	//G����������ת����ʽ���£�
	int i;
	location[0] = location[0];
	location[1] = location[1] + location[2];
	location[2] = location[2];

	if (location[2] == 1)
	{
		code_med[1] = g(code_med[2], code_med[3], code_last[location[1]-2]);
	}
	else
	for (i = 0; i < location[2]; i++)
	{
		code_med[location[2] + i] = g(code_med[location[2] * 2 + i], code_med[location[2] * 3 + i], code_b[location[1]-location[2]*2 + i]);
	}
}

//����E���㣬�Ƴ�bֵ
void E(int *code_last, int *code_b, int *location)
{

	//����֮ǰ����һ������ת��
	//G����������ת����ʽ���£�
	int i,j;
	location[0] = location[0]+1;
	location[1] = location[1];
	location[2] = location[2]*2;

	if (location[2] == 2)
	{
		code_b[location[1] - 2] = e(code_last[location[1]-2],code_last[location[1]-1],0);
		code_b[location[1] - 1] = e(code_last[location[1]-2],code_last[location[1]-1],1);
	}
	else
	{
			for (j = 0; j < location[2] / 2; j++)//ǰһ������ͨ���������õ�
				code_b[location[1] - location[2] +j] = e(code_b[location[1] - location[2] + j], code_b[location[1] - location[2]/2 + j], 0);
			for (j = 0; j < location[2]/2; j++)//��һ������ֱ�Ӹ���
				code_b[location[1] - location[2]/2 +j] = e(code_b[location[1] - location[2] + j], code_b[location[1] - location[2]/2 + j], 1);
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
		while (location[2] > 1) { F(code_pre, code_med, location); }
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
		originalCode[i] = code_pre[i];
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
		code_med[i + CODE_LENGTH] = code_pre[i];
		//printf("������֮��polarCode[0][i] = %.2lf \t" ,polarCode[0][i]  ); 
		//printf("\n");

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
	int i = CODE_LENGTH;
	int error = 0;
	while (i--) {
		if (code_last[i] - originalCode[i]) error++;
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
	float ar_gv[3] = {2,3,10000};//(��������������ȣ����Դ���) 
	sim_times = (int)ar_gv[2];
	for (i; i<sim_times; i++)
	{
		location[0] = LEVEL;//�������ֵ
		location[1] = CODE_LENGTH;//�������ֵ
		location[2] = CODE_LENGTH;//�������ֵ
		
		AC();//�������������ͬʱ��������bitλ��
		encode();//ִ��polar�����
		bpsk(code_pre);//bpsk����
		add_gassrand(ar_gv[1]);//�Ӹ�˹������(�����)
		decode();//����
		if (statistics())BLER++;

		//  printf("�������Ϊsim_times=%d\n �������ΪBLER=%d",sim_times,BLER);

	}
	printf("�������Ϊsim_times=%d �������ΪBLER=%d", sim_times, BLER);

	return 0;

}
//*************************************************************������***********************************************//
