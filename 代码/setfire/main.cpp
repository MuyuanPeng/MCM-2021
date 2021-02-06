#include <stdio.h>
#include <iostream>
#include "16807.h"
using namespace std;


// 人数/火点的总数
constexpr auto PEOPLE = 20;
constexpr auto FIRE = 100;

// RANGE表示起火的范围（单位：km）
constexpr auto RANGE_L = 1;
constexpr auto RANGE_H = 10;



struct region
{
	double x;
	double y;
	double w;//weight（sum=1）
	double rho;//半径
	int num_of_fireman;
	int size;
};

region fire_region[FIRE];
double fire[FIRE][2];//火点x,y
double fireman[PEOPLE][2];//消防员x,y



void setfire_start(double lambda_num, double lambda_size);
void recalc();
void setfire_firemen();
void setfire_write_file();
int Possion_sample(double lambda);
int JieCheng(int i);
double Exp_sample(double lambda);
double random();
int Discreate_sample(double weight[], int num);
double Norm_sample(double x, double sigma);

int main()
{
	// 1.在半径为10km的范围内抽样，得到起火点
	// 2.对于每个起火点，抽样得到起火大小
	// 3.根据起火大小，抽样得到消防人员的位置
	setfire_start(3,10);//para1:火区的lambda(不应该太大)；para2:火点的lambda(注：火点数目均值为para1*para2)
	recalc();//根据现有的火点，重新计算火区的中心坐标和大小
	setfire_firemen();
	setfire_write_file();

	return 0;
}

void setfire_start(double lambda_region, double lambda_size)
{
	// 第一步:找出火区数目
	int fire_region_num;
	int fire_spot_num;
	for (int i=1;;++i)
	{
		fire_region_num = Possion_sample(lambda_region);
		if (fire_region_num >= FIRE)
		{
			printf("\n\n\n\t\t\tERROR!!!There are %d fire spots!\n\n\n", fire_region_num);
			continue;
		}
		if (fire_region_num)break;
		cout <<i<< "-th trial:"<<"Warning: fire_region_num=0, another initiation process will be done."<< endl;
	} 

	// 第二步:找出火区位置
	for (int i = 0; i < fire_region_num; ++i)
	{
		double r, theta;
		for (;;)
		{
			r = RANGE_H * sqrt(random());
			if (r > RANGE_L)break;
		}
		theta = 2 * PI * random();
		// 抽样得到火区的x,y坐标
		fire_region[i].x = r*cos(theta);
		fire_region[i].y = abs(r * sin(theta));
		fire_region[i].w = Exp_sample(lambda_size);
	}
	// 计算得到火区的权重
	double temp_count = 0;
	for (int i = 0; i < fire_region_num; ++i)
	{
		temp_count += fire_region[i].w;
	}
	for (int i = 0; i < fire_region_num; ++i)
	{
		fire_region[i].w = fire_region[i].w / temp_count;
		fire_region[i].size++;
	}
	// 由火区的权重进行抽样，得到火区的size（也就是每个火区有多少个火点）
	// 先计算火点的数目（火区数目越多，火点数目倾向于越多）
	fire_spot_num = Possion_sample(fire_region_num * lambda_size);
	double weight[FIRE];
	for (int i = 0; i < fire_region_num; ++i)
	{
		weight[i]= fire_region[i].w;
	}
	for (int i = 0; i < fire_spot_num; ++i)
	{
		int temp = Discreate_sample(weight, fire_region_num);
		fire_region[temp - 1].size++;
	}

	// 第三步:抽样得到火点的x,y坐标
	int fire_spot_count=0;
	for (int i = 0; i < fire_region_num; ++i)
	{// 对于每个火区来说
		for (int j = 0; j < fire_region[i].size; ++j)
		{// 对于这个火区的每个火点来说
			// 确定火点的位置
			double r, theta;
			double rho = Norm_sample(pow(fire_region[i].size,0.3),0.1);// 火点分布的半径取决于火点数目，数目越多，半径越大。
			fire_region[i].rho = rho;
			r = rho * sqrt(random());
			if (r > rho) { cout << "ERROR!!!!!" << endl; getchar(); getchar(); }
			theta = 2 * PI * random();
			// 抽样得到火区的x,y坐标
			fire[fire_spot_count][0] = r * cos(theta) + fire_region[i].x;
			fire[fire_spot_count][1] = r * sin(theta) + fire_region[i].y;
			fire_spot_count++;
		}
	}
}

void setfire_firemen()
{
	// 第一步：根据火区的大小确定各个火区指派消防员的个数
	double region_radii_weight[FIRE];
	double region_radii_weight_sum = 0;
	for (int i = 0; i < FIRE; ++i)
	{
		region_radii_weight[i] = fire_region[i].size;
		region_radii_weight_sum += region_radii_weight[i];
	}
	for (int i = 0; i < FIRE; ++i)
	{
		region_radii_weight[i] = region_radii_weight[i] / region_radii_weight_sum;
	}
	int region_num = 0;// 有多少个火区
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size != 0)
		{
			region_num++;
			fire_region[i].num_of_fireman++;
		}
	}
	if (region_num > PEOPLE) { cout << "ERROR!!! region number is bigger than total people" << endl; }
	for (int i = 0; i < PEOPLE - region_num; ++i)
	{
		int man_in_region = Discreate_sample(region_radii_weight,FIRE);
		fire_region[man_in_region-1].num_of_fireman++;
	}

	// 第二步：指派消防员的位置
	int man_count = 0;
	for (int i = 0; i < FIRE; i++)
	{
		double x = fire_region[i].x;
		double y = fire_region[i].y;
		double theta0 = atan(y/x);
		if (x > 0)theta0 += PI;
		double range_of_theta = 2;
		double r, theta;
		double rho = Norm_sample(pow(fire_region[i].size, 0.3), 0.1);// 火点分布的半径取决于火点数目，数目越多，半径越大。
		

		for (int j = 0; j < fire_region[i].num_of_fireman; j++)
		{
			r = sqrt(random());
			r = fire_region[i].rho + 0.1 * r * fire_region[i].rho;
			//r = 0;
			theta = (theta0 + range_of_theta * (random() - 0.5));
			fireman[man_count][0] = x + r * cos(theta);
			fireman[man_count][1] = y + r * sin(theta);
			man_count++;
		}
	}
	// cout << man_count << endl;
	// getchar();
}
void setfire_write_file()
{
	FILE* f_fire = fopen("fire.dat", "w");
	FILE* f_fireman = fopen("fireman.dat", "w");
	FILE* f_region = fopen("fire_region.dat", "w");
	FILE* f_for_zw = fopen("fire_zw.dat", "w");

	int region_num = 0;// 有多少个火区
	int fire_num = 0;// 有多少个火点
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size != 0)
		{
			region_num++;
			fire_num += fire_region[i].size;
		}
	}

	
	for (int i = 0; i < fire_num; ++i)
	{
		fprintf(f_fire, "%6f\t%6f\n", fire[i][0], fire[i][1]);
	}
	for (int i = 0; i < PEOPLE; ++i)
	{
		fprintf(f_fireman, "%6f\t%6f\n", fireman[i][0], fireman[i][1]);
	}
	for (int i = 0; i < FIRE; ++i)
	{
		fprintf(f_region, "%6f\t%6f\t%6f\n", fire_region[i].x, fire_region[i].y, fire_region[i].rho);
	}
	int spot_count = 0;
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size == 0)continue;
		fprintf(f_for_zw,"%6f\t%6f\t%6d\n", fire_region[i].x, fire_region[i].y, fire_region[i].size);
		// fprintf(f_for_zw, "%6f\t%6f\t%6d\t%d\n", fire_region[i].x, fire_region[i].y, fire_region[i].size, fire_region[i].num_of_fireman);
		for (int j = 0; j < fire_region[i].size; ++j)
		{
			fprintf(f_for_zw, "%6f\t%6f\n", fire[spot_count][0], fire[spot_count][1]);
			spot_count++;
		}
	}
	fclose(f_fire);
	fclose(f_fireman);
}

int Possion_sample(double lambda)
// 抽样，得到泊松分布
{
	// p是0-1随机数
	double p = random();
	int k = 0;
	double count = 0;
	for (count = 0; ; k++)
	{
		count += pow(lambda, k) / JieCheng(k) * exp(-lambda);
		if (count > p)break;
	}
	return k;
}

double Exp_sample(double lambda)
// 抽样，得到指数分布
{
	double xi = random();
	return -lambda * log(xi);
}

int JieCheng(int i)
// 返回i的阶乘
{
	int result = 1;
	if (i < 0) { printf("ERROR in JieCheng();(阶乘计算有误)！\n"); getchar(); return -1; }
	if (i == 0)return 1;
	for (;; i--)
	{
		result *= i;
		if (i == 1)break;
	}
	return result;
}

double random()
{
	return random_16807();
}

int Discreate_sample(double weight[],int num)
// 根据weight()权函数，抽取离散变量。
// 返回值是抽取出的变量（也就是在第几个类别中）
// num是weight的数组长度
{
	double count=0;
	double x = random();
	for (int i=0;i<num;++i)
	{
		if (x > count)count += weight[i];
		else return i;
	}
	return num;
}

double Norm_sample(double x, double sigma)
{
	double xi1 = random();
	double xi2 = random();//xi1 xi2：uniform distribution
	double X = pow(-2 * log(xi1), 0.5) * cos(2 * PI * xi2);
	return X * sigma + x;
}

void recalc()
{// 根据现有的火点，重新计算火区的中心坐标和大小
	int fire_spot_count = 0;
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size == 0)continue;
		// 中心坐标
		double x_sum = 0;
		double y_sum = 0;
		int temp_spot_count = fire_spot_count;
		for (int j = 0; j < fire_region[i].size; j++)
		{
			x_sum += fire[temp_spot_count][0];
			y_sum += fire[temp_spot_count][1];
			temp_spot_count++;
		}
		for (int j = 0; j < fire_region[i].size; j++)
		{
			fire_region[i].x = x_sum / fire_region[i].size;
			fire_region[i].y = y_sum / fire_region[i].size;
		}
		// 大小
		double max = 0;
		for (int j = 0; j < fire_region[i].size; j++)
		{
			double temp = sqrt((fire[fire_spot_count][0] - fire_region[i].x) *
				(fire[fire_spot_count][0] - fire_region[i].x) +
				(fire[fire_spot_count][1] - fire_region[i].y) *
				(fire[fire_spot_count][1] - fire_region[i].y));
			if (temp > max)max = temp;
			fire_spot_count++;
		}
		fire_region[i].rho = max;
	}
}






