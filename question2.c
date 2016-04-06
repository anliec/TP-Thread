#include <stdio.h>
#include <stdint.h>
#include <math.h>

void primeFactor(uint64_t n)
{
	uint64_t i,limit = sqrt(n);
	for(i=2 ; i<=limit ; i++)
	{
		if(n%i == 0)
		{
			//printf(" %ju",i);
			primeFactor(n/i);
			return;
		}
	}
	//printf(" %ju",n);
}

void print_prime_factors(uint64_t n)
{
	//printf("%ju :",n);
	primeFactor(n);
	//printf("\n");
    // your code goes here: print "n", a colon, and then the prime factors


}

int main(void)
{
    // your code goes  here: open the text file (e.g.  with fopen() ),
	FILE* fichier = fopen("question2_test.txt", "r");
    // then read each line (e.g. with fgets() ), turn it into a number
	
	uint64_t lecture = 0;

	while (fscanf(fichier, "%ju", &lecture) != EOF )
	{
		print_prime_factors(lecture);
	}
	fclose(fichier);
    // (e.g. with atoll() ) and then pass it to print_prime_factors.

    return 0;
}
