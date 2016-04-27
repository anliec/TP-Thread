#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

#define MAX_FACTORS 64
#define MUTEX_FICHIER 0
#define MUTEX_ECRAN 1

FILE* fichier;

void print_prime_factors(uint64_t n, pthread_mutex_t * mutexEcran)
{
	uint64_t factors[MAX_FACTORS];
	
	int j,k;
	
	k=get_prime_factors(0,factors, n);
	
	pthread_mutex_lock(mutexEcran); // vérouiller mutexEcran
	printf("%ju: ",n);
	for(j=0; j<=k; j++)
	{
		printf("%ju ",factors[j]);
	}
	printf("\n");
	pthread_mutex_unlock(mutexEcran); // dévérouiller mutexEcran
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
		pthread_mutex_lock(&mutex[MUTEX_FICHIER]); // vérouiller mutexFichier
		fin = fscanf(fichier, "%ju", &lecture);
		pthread_mutex_unlock(&mutex[MUTEX_FICHIER]); // dévérouiller mutexFichier
		if(fin<=0)
		{
			break;
		}
		print_prime_factors(lecture, &mutex[MUTEX_ECRAN]);
	}
}



int main(void)
{
	int i;
	const int NB_THREAD = 2;

	pthread_t idThread[NB_THREAD];

	pthread_mutex_t mutex[2];
	pthread_mutex_init(&mutex[MUTEX_FICHIER], NULL); // initialiser mutexFichier
	pthread_mutex_init(&mutex[MUTEX_ECRAN], NULL);// initialiser mutexEcran

    fichier = fopen("nombre premier.txt", "r");

	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_create(&idThread[i], NULL, workEntry, &mutex);
	}
	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_join(idThread[i], NULL);
	}
	pthread_mutex_destroy(&mutex[MUTEX_FICHIER]); // supprimer mutexFichier
	pthread_mutex_destroy(&mutex[MUTEX_ECRAN]); // supprimer mutexEcran

    fclose(fichier);

	return 0;
}
