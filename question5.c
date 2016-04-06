#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

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
	int i;
	int numberOfThread = 0;
	int pos = 0;
	const int NB_THREAD = 2;
	
	FILE* fichier = fopen("question3_test.txt", "r");
	uint64_t lecture = 0;
	pthread_t idThread[NB_THREAD];
	
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL); // initialisation mutex
	int fin;

	while (fin > 0 )
	{
		if (numberOfThread >= NB_THREAD )
		{
			pthread_join(idThread[pos], NULL);
			numberOfThread--;
		}
		pthread_mutex_lock(&mutex); // vérouiller mutex
		fin = fscanf(fichier, "%ju", &lecture);
		pthread_mutex_unlock(&mutex); // dévérouiller mutex

		if(fin<=0)
		{
			break;
		}
					
		pthread_create(&idThread[pos], NULL, print_prime_factors,lecture);
		numberOfThread++;
		pos = (pos+1)%NB_THREAD;
	}
	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_join(idThread[i], NULL);
	}
	pthread_mutex_destroy(&mutex); // supprimer mutex
	
	fclose(fichier);
	return 0;
}
    
