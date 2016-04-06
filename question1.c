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
    // your code goes here: print "n", a colon, and then the prime factors


}



int main(void)
{
    print_prime_factors(77); // expected result:   77: 7 11
    print_prime_factors(84); // expected result:   84: 2 2 3 7

    // expected result:   429496729675917: 3 18229 7853726291
    print_prime_factors(429496729675917);


    return 0;
}
