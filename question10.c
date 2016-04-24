#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

#define MAX_FACTORS 64
#define MUTEX_FICHIER 0
#define MUTEX_ECRAN 1

#define LONGEUR 10000
#define PROFONDEUR 64

///Htab:
//HT_LONGUEUR est un nombre premier (c'est mieux)
#define HT_LONGUEUR 4001
#define HT_PROFONDEUR 10
#define HT_A 1
#define HT_B 0

struct keyValue
{
	uint64_t key;
	int value;
};

struct keyValue hashTable[HT_LONGUEUR][HT_PROFONDEUR];

#define FILE_READ "input.txt"

uint64_t memoriseResultats[LONGEUR][PROFONDEUR];
int Longeur_MR = 0;

FILE* fichier;

int get_prime_factors(uint64_t courantDestIndex, uint64_t* dest, uint64_t numberToFactorise, uint64_t beginLoopWith);

int getHashValue(uint64_t v)
{
	return (v*HT_A+HT_B)%HT_LONGUEUR;
}

int getIndexOf(uint64_t v)
{
	int hashPos = getHashValue(v);
	for(unsigned i=0 ; i<HT_PROFONDEUR ; i++)
	{
		if(hashTable[hashPos][i].key == v)
		{
			return hashTable[hashPos][i].value;
		}
		else if(hashTable[hashPos][i].value == -1)//si il n'y a pas d'autres valeurs à parcourrir
		{
			return -1;
		}
	}
	return -1;
}

void initHashTable()
{
	for(unsigned i=0 ; i<HT_LONGUEUR ; i++)
	{
		for(unsigned j=0 ; j<HT_PROFONDEUR ; j++)
		{
			hashTable[i][j].key = 0;
			hashTable[i][j].value = -1;
		}
	}
}

void addKeyValue(uint64_t key, int value)
{
	int hashPos = getHashValue(key);
	for(unsigned i=0 ; i<HT_PROFONDEUR ; i++)
	{
		//inutile
		/*if(hashTable[hashPos][i].key == key)
        {
            return;
        }
        else */if(hashTable[hashPos][i].value == -1)
		{
			hashTable[hashPos][i].value = value;
			hashTable[hashPos][i].key = key;
			return;
		}
	}
	printf("\n--------------------\nhash table overflow !!!\n-------------------\n");
}


void print_prime_factors(uint64_t n, pthread_mutex_t * mutexEcran)
{
	uint64_t factors[MAX_FACTORS];

	int j,destSize;

	destSize=get_prime_factors(0,factors, n, 2);

	pthread_mutex_lock(mutexEcran); // vérouiller mutexEcran
	printf("%ju: ",n);
	for(j=0; j<=destSize; j++)
	{
		printf("%ju ",factors[j]);
	}
	printf("\n");
	pthread_mutex_unlock(mutexEcran); // dévérouiller mutexEcran
}

int addFactor(uint64_t courantDestIndex, uint64_t* dest, uint64_t numberToFactorise, uint64_t factor)
{
	dest[courantDestIndex] = factor;
	uint64_t factorised = numberToFactorise/factor;
	int profondeur = get_prime_factors(courantDestIndex+1, dest, factorised, factor);
	//a protéger avec un mutex ?!!
	int posMemorisation = Longeur_MR;
	Longeur_MR++;
	//fin de protection
	memoriseResultats[posMemorisation][0]=numberToFactorise;

	for(int j=courantDestIndex ; j<=profondeur ; j++)
	{
		memoriseResultats[posMemorisation][j-courantDestIndex+1]=dest[j];
	}
	addKeyValue(numberToFactorise,posMemorisation);

	return profondeur;
}

int get_prime_factors(uint64_t courantDestIndex, uint64_t* dest, uint64_t numberToFactorise, uint64_t beginLoopWith)
{
	int found = getIndexOf(numberToFactorise);
	if(found != -1)
	{
		//printf("\nfactor found ! \n");
		int j = 1;
		while(memoriseResultats[found][j]!=0 && j<PROFONDEUR)
		{
			dest[courantDestIndex]=memoriseResultats[found][j];
			j++;
			courantDestIndex++;
		}
		return courantDestIndex-1;
	}
	else
	{
		if(beginLoopWith == 2)
		{
			if(numberToFactorise%2 == 0)
			{
				return addFactor(courantDestIndex,dest,numberToFactorise,2);
			}
			beginLoopWith++;
		}
		/*if(beginLoopWith == 3)
        {
            if(numberToFactorise%3 == 0)
            {
                return addFactor(courantDestIndex,dest,numberToFactorise,3);
            }
            beginLoopWith+=2;
        }*/
		uint64_t i,limit = sqrt(numberToFactorise);
		for(i=beginLoopWith ; i<=limit ; i+=2)
		{
			if(numberToFactorise%i == 0)
			{
				return addFactor(courantDestIndex,dest,numberToFactorise,i);
			}
		}
		dest[courantDestIndex] = numberToFactorise;
	}
	return courantDestIndex;
}

void workEntry(pthread_mutex_t * mutex)
{
	//FILE*
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
	unsigned int i;
	unsigned int j = 0;
	const int NB_THREAD = 4;

	for ( i = 0 ; i < LONGEUR ; i++)
	{
		for (j = 0 ; j < PROFONDEUR ; j++)
		{
			memoriseResultats[i][j] = 0;
		}
	}
	initHashTable();

	pthread_t idThread[NB_THREAD];

	pthread_mutex_t mutex[2];
	pthread_mutex_init(&mutex[MUTEX_FICHIER], NULL); // initialiser mutexFichier
	pthread_mutex_init(&mutex[MUTEX_ECRAN], NULL);// initialiser mutexEcran

	fichier = fopen(FILE_READ, "r");

	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_create(&idThread[i], NULL, workEntry, &mutex);
	}
	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_join(idThread[i], NULL);
	}

	fclose(fichier);

	pthread_mutex_destroy(&mutex[MUTEX_FICHIER]); // supprimer mutexFichier
	pthread_mutex_destroy(&mutex[MUTEX_ECRAN]); // supprimer mutexEcran

//    printf("\n\ntableau:\n");

//    for (j = 0 ; j < 10 ; j++)
//    {

//        for ( i = 0 ; i<6 && memoriseResultats[i][0]!=0 ; i++)
//        {
//            printf("%5ju ",memoriseResultats[i][j]);
//        }
//        printf("\n ");
//    }


	return 0;
}
