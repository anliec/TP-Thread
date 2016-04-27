#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

#define MAX_FACTORS 64

FILE* fichier;

void print_prime_factors(uint64_t n)
{
	uint64_t factors[MAX_FACTORS];
	
	int j,k;
	
	k=get_prime_factors(0,factors, n);
	
	printf("%ju: ",n);
	for(j=0; j<=k; j++)
	{
		printf("%ju ",factors[j]);
	}
	printf("\n");
}

int get_prime_factors(uint64_t n, uint64_t* dest, uint64_t number)
{
	uint64_t i,limit = sqrt(number);
	for(i=2 ; i<=limit ; i++)
	{
		if(number%i == 0)
		{
			dest[n] = i;
			return get_prime_factors(n+1, dest, number/i);
		}
	}
	dest[n] = number;
	
	return n;
}

void workEntry(pthread_mutex_t * mutex)
{
    uint64_t lecture = 0;
	int fin;
	while (1)
	{
		pthread_mutex_lock(mutex); // vérouiller mutex
		fin = fscanf(fichier, "%ju", &lecture);
		pthread_mutex_unlock(mutex); // dévérouiller mutex
		if(fin<=0)
		{
			break;
		}
		print_prime_factors(lecture);
    }
}



int main(void)
{
	int i;
	int numberOfThread = 0;
	int pos = 0;
	const int NB_THREAD = 2;

	uint64_t lecture = 0;
	pthread_t idThread[NB_THREAD];
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL); // initialiser mutex

    fichier = fopen("tricky.txt", "r");

	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_create(&idThread[i], NULL, workEntry,&mutex);
	}
	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_join(idThread[i], NULL);
	}
	pthread_mutex_destroy(&mutex); // supprimer mutex

    fclose(fichier);

	return 0;
}
