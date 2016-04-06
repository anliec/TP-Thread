#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

#define MAX_FACTORS 64
#define MUTEX_FICHIER 0
#define MUTEX_ECRAN 1

#define LONGEUR 1000
#define PROFONDEUR 64

uint64_t memoriseResultats[1000][64];
int longeurMax = 0;

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
	int trouver = 0;
	for(int i=0 ; i<longeurMax ; i++)
	{
		if(number==memoriseResultats[i][0])
		{
			trouver = 1;
			int j = 1;
			while(memoriseResultats[i][j]!=0 && j<PROFONDEUR)
			{
				dest[n++]=memoriseResultats[i][j];
				j++;
			}
		}
	}
	if(!trouver)
	{
		uint64_t i,limit = sqrt(number);
		for(i=2 ; i<=limit ; i++)
		{
			if(number%i == 0)
			{
				dest[n] = i;
				uint64_t factor = number/i;
				int profondeur = get_prime_factors(n+1, dest, factor);
				int pos=longeurMax++;
				memoriseResultats[pos][0]=number;
				for(int j=n ; j<=profondeur ; j++)
				{
					memoriseResultats[pos][j-n+1]=dest[j];
				}
				return profondeur;
			}
		}
		dest[n] = number;
	}
	return n;
}

void workEntry(pthread_mutex_t * mutex)
{
	FILE* fichier = fopen("large.txt", "r");
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
	fclose(fichier);
}

int main(void)
{
	int i;
	int j = 0;
	const int NB_THREAD = 1;

	for ( i = 0 ; i < LONGEUR ; i++)
	{
		for (j = 0 ; j < PROFONDEUR ; j++)
		{
			memoriseResultats[i][j] = 0;
		}
	}
	
	pthread_t idThread[NB_THREAD];

	pthread_mutex_t mutex[2];
	pthread_mutex_init(&mutex[MUTEX_FICHIER], NULL); // initialiser mutexFichier
	pthread_mutex_init(&mutex[MUTEX_ECRAN], NULL);// initialiser mutexEcran

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

	printf("\n\ntableau:\n");

	for (j = 0 ; j < 10 ; j++)
	{
		
		for ( i = 0 ; i<8 && memoriseResultats[i][0]!=0 ; i++)
		{
			printf("%20ju ",memoriseResultats[i][j]);
		}
		printf("\n ");
	}
	

	return 0;
}