#include "16807.h"
#include <time.h>
#include "my_time.h"

/*
	Description:
	This is a 16807 random number generator
	using Schrage method
*/

// parameters in 16807 method
// b=0, therefore we don't set this virable
int random_16807_a = 18607;
int random_16807_m = 2147483647;

// generated random numbers(range:0~m-1)
int random_16807_I;
// index of I
int random_16807_k = 0;


double random_16807()
{
	int* I = &random_16807_I;
	int a = random_16807_a;
	int m = random_16807_m;

	int* k = &random_16807_k;


	if (*k == 0)// initialization
	{
		int seed = seed_generater(m);// set seed to an integer according to system time
		seed = 971;
		*I = seed;
		cout << "random_16807:seed: " << seed << endl;
	}

	int q = (m - m % a) / a;
	int r = m % a;
	*I = (a * ((*I) % q) - r * (((*I) - (*I) % q) / q)) % m;
	if ((*I) < 0)*I += m;

	(*k)++;
	return (*I) / (double)m;
}

int seed_generater(int T)
{
	/*
		T:period, in this case, T=m=2147483647
	*/
	struct timeval tv;
	gettimeofday(&tv, NULL);
	// tv.tv_usec: millisecond
	// 0-999999
	int temp = tv.tv_usec/1000 - 1000 % T;
	return ((int)tv.tv_usec/1000) % T;
}