#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

#define FILE_OPEN_NAME "nombre premier.txt"

FILE* fichier;

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
	pthread_mutex_init(&mutex, NULL); // initialisation mutex

    fichier = fopen("tricky.txt", "r");

	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_create(&idThread[i], NULL, workEntry,&mutex);
	}
	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_join(idThread[i], NULL);
	}

    fclose(fichier);

	pthread_mutex_destroy(&mutex); // supprimer mutex

	return 0;
}

