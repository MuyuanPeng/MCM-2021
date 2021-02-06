#include<stdio.h>
#include <iostream>
using namespace std;

#define PEOPLE 20

void setfire_start();
void setfire_size();
void setfire_firemen();
void setfire_write_file();
int Possion_sample(double lambda);
int JieCheng(int i);

int main()
{
	// 1.在半径为10km的范围内抽样，得到起火点
	// 2.对于每个起火点，抽样得到起火大小
	// 3.根据起火大小，抽样得到消防人员的位置
	setfire_start();
	setfire_size();
	setfire_firemen();
	setfire_write_file();
	for(int i=0;i<10;++1)
	{
		cout<<i<<"!="<<JieCheng(i)<<endl;
		getchar();
	}
	return 0;
}

void setfire_start()
{
	;
}
void setfire_size()
{
	;
}
void setfire_firemen()
{
	;
}
void setfire_write_file()
{
	;
}

int Possion_sample(double lambda)
// 抽样，得到泊松分布
{
	// p是0-1随机数
	double p=(rand()%1000)/1000.;
	int k=0;
	double count=0;
	for(;count=0;k++)
	{
		count += pow(lambda,k)/JieCheng(k)*exp(-lambda);
		if(count > p)break;
	}
	return k;
}

int JieCheng(int i)
// 返回i的阶乘
{
	int result=1;
	if(i<0){printf("ERROR in JieCheng();(阶乘计算有误)！\n");getchar();return -1;}
	if(i==0)return 1;
	for(;;i--)
	{
		result*=i;
		if(i==1)break;
	}
	return result;
}