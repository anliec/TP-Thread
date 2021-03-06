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
            printf(" %ju",i);
			primeFactor(n/i);
			return;
		}
    }
    printf(" %ju",n);
}

void print_prime_factors(uint64_t n)
{
    printf("%ju :",n);
    primeFactor(n);
    printf("\n");
}

int main(void)
{
    FILE* fichier = fopen("tricky.txt", "r");
	
	uint64_t lecture = 0;

	while (fscanf(fichier, "%ju", &lecture) != EOF )
	{
		print_prime_factors(lecture);
	}
    fclose(fichier);

    return 0;
}
