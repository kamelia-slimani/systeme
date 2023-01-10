#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "mes_semaphores.h"

/*typedef union semun {
    int val ;              //valeur pour SETVAL
    struct semid_ds *buf ; // buffer pour IPC_SET et IPC_STAT
    unsigned short *array ;// tableau pour GETALL et SETALL
    struct seminfo *__buf; //buffer pour IPC_INFO
} semval;*/

int sem_creation(int * semid, int nombre_semaphores) {
	*semid = semget(IPC_PRIVATE, nombre_semaphores, IPC_CREAT|0660|IPC_EXCL); //création de 4 sémaphores 
	if(*semid == -1){
    	perror("Erreur lors de la création des sémaphores");
    	return 1;
        }
    return 0;
}
//qui crée un ensemble de sémaphores (semid) de nombre_semaphores. Elle renvoie 0 si la création a bien fonctionné. Elle sera utilisée par le père.

int sem_initialisation(int semid, int num_semaphore, int nbr_jetons){
	
 	semctl(semid, 0, SETVAL, nbr_jetons);
 	return 0;

} 
//initialise le semaphore numéro num_semaphore de l'ensemble de sémaphore semid par la valeur nbr_jetons.
int P(int semid, int num_semaphore){
    struct sembuf smbf;
    smbf.sem_num = num_semaphore;
    smbf.sem_op = -1; 
    smbf.sem_flg = 0;
    if(semop(semid, &smbf, 1) ==-1){
        perror("erreur lors de l opération P()");
        return -1;
    }
    return 0;

} 
//Primitive P pour le sémaphore numéro num_semaphore. (pour cet exercice le numéro sera toujours 0 mais nous réutiliserons ces fonctions dans l'exercice suivant) avec sumop
int V(int semid, int num_semaphore){
     struct sembuf smbf;
    smbf.sem_num = num_semaphore;
    smbf.sem_op = 1; 
    smbf.sem_flg = 0;
    if(semop(semid, &smbf, 1) ==-1){
        perror("erreur lors de l opération V()");
        return -1;
    }
    return 0;
} //Primitive V pour le sémaphore numéro num_semaphore.
int sem_destruction(int semid){
    if(semctl(semid, IPC_RMID, 0)==-1){
        return -1;
    }
    return 0;
} //qui détruit un ensemble de sémaphores (semid). Elle sera utilisée par le père.

int sem_recup(int * semid, int nb_semaphores) {
    *semid = semget(IPC_PRIVATE, nb_semaphores, 0); 
    if(*semid == -1){
        perror("Erreur lors de la création des sémaphores");
        return 1;
        }
    return 0;
}
// qui récupère le numéro de l'ensemble de sémaphores (semid) (généralement utilisée par le processus le ls, mais ici nous ne l'utiliserons pas puisque nous n'utilisons pas execl...mais faites-là quand même :-) elle servira dans le prochain exercice). En entrée, il faut indiquer le nombre de sémaphore qu'il y a dans l'ensemble à récupérer. La fonction est identique à la création sauf pour l'option de semget qui doit être à 0.