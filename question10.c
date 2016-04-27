#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_FACTORS 64
#define MUTEX_FICHIER 0
#define MUTEX_ECRAN 1
#define MUTEX_L_MR 2
#define MUTEX_HT 3

pthread_mutex_t mutex[4];

#define LONGEUR 10000
#define PROFONDEUR 64

#define NB_THREAD 5

///Htab:
//HT_LONGUEUR est un nombre premier (c'est mieux)
#define HT_LONGUEUR 10007
#define HT_PROFONDEUR 10
#define HT_A 1
#define HT_B 0

struct keyValue
{
	uint64_t key;
	int value;
};

struct keyValue hashTable[HT_LONGUEUR][HT_PROFONDEUR];

#define FILE_READ "race2.txt"

uint64_t memoriseResultats[LONGEUR][PROFONDEUR];
int Longeur_MR = 0;

FILE* fichier;
uint64_t threadWork[NB_THREAD][2];

int get_prime_factors(uint64_t courantDestIndex, uint64_t *dest, uint64_t numberToFactorise, uint64_t beginLoopWith);

int getHashValue(uint64_t v)
{
    return ((v)*HT_A+HT_B)%HT_LONGUEUR;
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
	pthread_mutex_lock(&mutex[MUTEX_HT]);
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
			pthread_mutex_unlock(&mutex[MUTEX_HT]);
			return;
		}
	}
	pthread_mutex_unlock(&mutex[MUTEX_HT]);
    //printf("\n--------------------\nhash table overflow !!!\n-------------------\n");
}


void print_prime_factors(uint64_t n, int threadNumber)
{
	uint64_t factors[MAX_FACTORS];
    uint64_t deux = 2;

	unsigned int j,destSize;

    destSize=get_prime_factors(0,factors, n, deux);

	pthread_mutex_lock(&mutex[MUTEX_ECRAN]); // vérouiller mutexEcran
	for(unsigned i=0 ; i<threadWork[threadNumber][1] ; i++)
	{
        printf("%ju: ",n);
		for(j=0; j<=destSize; j++)
		{
			printf("%ju ",factors[j]);
		}
        printf("\n");
	}
    pthread_mutex_unlock(&mutex[MUTEX_ECRAN]); // dévérouiller mutexEcran

}

int addFactor(uint64_t courantDestIndex, uint64_t * dest, uint64_t numberToFactorise, uint64_t factor, int posMemorisation)
{
    dest[courantDestIndex] = factor;
    uint64_t factorised = (numberToFactorise) / (factor);

    int profondeur = get_prime_factors(courantDestIndex+1, dest, factorised, factor);

	for(int j=courantDestIndex ; j<=profondeur ; j++)
	{
		memoriseResultats[posMemorisation][j-courantDestIndex+1]=dest[j];
	}

	return profondeur;
}

int get_prime_factors(uint64_t courantDestIndex, uint64_t * dest, uint64_t numberToFactorise, uint64_t beginLoopWith)
{
    if(numberToFactorise == 1)
    {
        return courantDestIndex-1;
    }
	int found = getIndexOf(numberToFactorise);
	if(found != -1)
	{
		while(memoriseResultats[found][1] == 1)
		{
			sleep(1);
		}
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
		//get the pos for memorising the result
		pthread_mutex_lock(&mutex[MUTEX_L_MR]);
		int posMemorisation = Longeur_MR;
		Longeur_MR++;
		pthread_mutex_unlock(&mutex[MUTEX_L_MR]);
		//add the pos to hash tab
        addKeyValue(numberToFactorise,posMemorisation);
		//store the value on MR
		//memoriseResultats[posMemorisation][0]=numberToFactorise; //useless
		//signal that the value is beeing computed
		memoriseResultats[posMemorisation][1]=1;

        if(beginLoopWith == 2)
		{
            if((numberToFactorise)%2 == 0)
            {
                return addFactor(courantDestIndex,dest,numberToFactorise,2,posMemorisation);
			}
            (beginLoopWith)++;
		}
        if(beginLoopWith == 3)
        {
            if((numberToFactorise)%3 == 0)
            {
                return addFactor(courantDestIndex,dest,numberToFactorise,3,posMemorisation);
            }
            (beginLoopWith)+=2;
        }
        unsigned inc;
        if((beginLoopWith)%6 == 1)
        {
            inc = 4;
        }
        else if((beginLoopWith)%6 == 5)
        {
            inc = 2;
        }

        uint64_t i,limit = sqrt(numberToFactorise);
        for(i=(beginLoopWith) ; i<=limit ; i+=inc , inc=6-inc)
		{
            if((numberToFactorise)%i == 0)
			{
                return addFactor(courantDestIndex,dest,numberToFactorise,i,posMemorisation);
			}
		}
        dest[courantDestIndex] = numberToFactorise;
        memoriseResultats[posMemorisation][1]= numberToFactorise;
	}
	return courantDestIndex;
}

void workEntry(int threadNumber)
{
	uint64_t lecture = 0;
    int fin, compute;
	while (1)
	{
        compute=1;
		pthread_mutex_lock(&mutex[MUTEX_FICHIER]); // vérouiller mutexFichier
		fin = fscanf(fichier, "%ju", &lecture);
		pthread_mutex_unlock(&mutex[MUTEX_FICHIER]); // dévérouiller mutexFichier
		if(fin<=0)
		{
			break;
		}
		for(unsigned i=0 ; i<NB_THREAD ; i++)
		{
			if(threadWork[i][0]==lecture && i!=threadNumber)
			{
				threadWork[i][1]++;
                compute=0;
                break;
			}
		}
        if(compute)
        {
            threadWork[threadNumber][0]=lecture;
            threadWork[threadNumber][1]=1;
            print_prime_factors(lecture, threadNumber);
        }
	}
}

int main(void)
{
	unsigned int i;
	unsigned int j;

	for ( i = 0 ; i < LONGEUR ; i++)
	{
		for (j = 0 ; j < PROFONDEUR ; j++)
		{
			memoriseResultats[i][j] = 0;
		}
	}
	initHashTable();

	pthread_t idThread[NB_THREAD];

	pthread_mutex_init(&mutex[MUTEX_FICHIER], NULL); // initialiser mutexFichier
	pthread_mutex_init(&mutex[MUTEX_ECRAN], NULL);// initialiser mutexEcran
	pthread_mutex_init(&mutex[MUTEX_L_MR], NULL);
	pthread_mutex_init(&mutex[MUTEX_HT], NULL);

	fichier = fopen(FILE_READ, "r");

	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_create(&idThread[i], NULL, workEntry, i);
	}
	for(i=0 ; i < NB_THREAD ; i++)
	{
		pthread_join(idThread[i], NULL);
	}

	fclose(fichier);

	pthread_mutex_destroy(&mutex[MUTEX_FICHIER]); // supprimer mutexFichier
	pthread_mutex_destroy(&mutex[MUTEX_ECRAN]); // supprimer mutexEcran
	pthread_mutex_destroy(&mutex[MUTEX_L_MR]);
	pthread_mutex_destroy(&mutex[MUTEX_HT]);

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
