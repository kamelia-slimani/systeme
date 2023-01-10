#ifndef MES_SEMAPHORES_H
#define MES_SEMAPHORES_H
/**
 * Fonction qui crée un ensemble de sémaphores (semid) de nombre_semaphores. 
 * Elle renvoie 0 si la création a bien fonc-tionné. 
 * Elle sera utilisée par le père.
*/
int sem_creation(int * semid, int nombre_semaphores);
/**
 * initialise le semaphore numéro num_semaphore de l'ensemble de sémaphore semid par la valeur nbr_jetons.
*/
int sem_initialisation(int semid, int num_semaphore, int nbr_jetons);
/**
 * Primitive P pour le sémaphore numéro num_semaphore.
*/
int P(int semid, int num_semaphore);
/**
 * Primitive V pour le sémaphore numéro num_semaphore.
*/
int V(int semid, int num_semaphore);
/**
 * Fonction qui détruit un ensemble de sémaphores (semid). 
 * Elle sera utilisée par le père.
*/
int sem_destruction(int semid);
/**
 * Fonction qui récupère le numéro de l'ensemble de sémaphores (semid) 
 * (généralement utilisée par le processus le fils, mais ici nous ne l'utiliserons pas puisque nous n'utilisons pas execl...mais faites-là quand même :-) elle servira dans le prochain exercice). 
 * En entrée, il faut indiquer le nombre de sémaphore qu'il y a dans l'ensemble à récupérer. 
 * La fonction est identique à la création sauf pour l'option de semget qui doit être à 0.
*/
int sem_recup(int * semid, int nb_semaphores);

int mem_creation(int *memid);
int mem_attachement(int* pVariablePartagee, int memid);
#endif 