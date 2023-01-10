#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "mes_semaphores.h"
#define key_num 1

#define TAILLE 1024
#define TAILLE_NB_LETTRE 26
#define nb_sem 1
#define num_sem 0

/*
 - gcc -Wall pere.c -o pere
 - gcc -Wall LectureCorpusLettre.c -o LectureCorpusLettre
 - gcc -Wall -c mes_semaphores.c -o mes_semaphores
 - gcc -Wall mes_semaphores LectureCorpusLettre.c -o LectureCorpusLettre
 
 Fonction d'Ecritue de l'histogramme dans un fichier */
void ecrireFichier(char *nomFichier, long int cpt, int h[]) {

  FILE *fichOut ;
  int j ;

  
  fichOut = fopen(nomFichier, "w") ;
  if (!fichOut) {
    printf("erreur à l'ouverture du fichier %s\n", nomFichier) ;
    exit(2) ;
  }
  
  //printf("Ecriture du fichier %s\n", nomFichier) ;
  fprintf(fichOut, "nombre de caractères lus : %ld\n", cpt) ;
  for (j = 0 ; j < 26 ; j++) {
    fprintf(fichOut, "%c : %d\n", j + 'a', h[j]) ;
  }
  fclose(fichOut) ;
}

/********************
* 
* Calcul de la fréquence d'apparition des lettres dans un fichier texte.
* (génération de fichiers de progression)
*
******************/


int main(int argc, char* argv[]) 
{
  FILE *fichierCorpus ;
  char nomFichierGenere[100] ;
  int i, car, nbLignes, cptFichier ;
  long int nbCar ;
  char chaine[TAILLE] ;
  int nbLignesFichier=0;
  int numLigne_deb = 0;
  int nbLigneTraite = 0;
  int * pVariablePartagee ;
  int sem_id;
  sem_creation(&sem_id, nb_sem);
  sem_initialisation(sem_id, num_sem, 1);
  key_t key = ftok("pere.c", key_num);

  int mem_id = shmget(key, 1*sizeof(int), 0660);
	if (mem_id==-1){
                perror("Erreur creation segment memoire partagé");
                exit(1);
         }
/* Attachement du segment pour pouvoir l'utiliser */
    pVariablePartagee = (int*) shmat(mem_id, NULL, 0);
    if(pVariablePartagee == NULL){
        perror("Erreur d'attachement au segment memoire partagé");
        exit(2);
         }
  /* Tableau pour l'histogramme*/
  int histo[TAILLE_NB_LETTRE] = {0} ;
  
  
  if (argc != 5) {
    printf("usage : %s <fichier a traiter> <nom generique fichiers resultats> <Le numéro de ligne à partir de laquelle le traitement doit commencer> < le nombre de lignes à traiter>\n", argv[0]) ;
    exit(1) ;
  }

  fichierCorpus = fopen(argv[1], "r") ;
  if (!fichierCorpus) {
    printf("erreur a l'ouverture du fichier %s\n", argv[1]) ;
    exit(2) ;
  }

  while (fgets(chaine, TAILLE, fichierCorpus)) {
    nbLignesFichier++;
  } 
 
  fseek(fichierCorpus, 0, SEEK_SET); //on se replace au début du fichier
  
  nbCar = 0 ;
  nbLignes = 0 ;
  cptFichier = 0 ;

  numLigne_deb = atoi(argv[3]);
  nbLigneTraite = atoi(argv[4]);
  
  /* Traitement ligne par ligne */
  while (fgets(chaine, TAILLE, fichierCorpus)) {
    if(((numLigne_deb <= nbLignes) && (nbLignes<= numLigne_deb+nbLigneTraite)) || (nbLigneTraite==0) ){
    
	/* Décompte des lettres (minuscules)*/
     
	    for (i = 0 ; i < strlen(chaine) ; i++) {
        
            car = chaine[i] - 'a' ;
            if ((car >= 0) && (car < 26)) {
                histo[car] ++ ;
                nbCar++;
               
            }
        }
	}
    
	/* Génération du fichier de décompte */
	nbLignes++ ;
	if (nbLignes % 10000 == 0) {
		sprintf(nomFichierGenere, "%s_%d_%d.txt", argv[2], getpid(), cptFichier) ;
		ecrireFichier(nomFichierGenere, nbCar, histo) ;	
		printf(".");fflush(stdout);
		cptFichier++ ;

	}
  }
  printf("\n");
  
  /* Ecriture dans le dernier fichier */
  P(sem_id, num_sem);
  for(int i=0; i<sizeof(histo); i++){
      pVariablePartagee[i] = histo[i];
    }
  V(sem_id, num_sem);

  sprintf(nomFichierGenere, "%s_%d_final.txt", argv[2], getpid()) ;
  ecrireFichier(nomFichierGenere, nbCar, histo) ;
  printf("Dernier fichier généré: %s [nombre de caractères traités : %ld, nb lignes traitées : %d sur %d]\n", nomFichierGenere, nbCar, nbLigneTraite, nbLignesFichier) ;

  fclose(fichierCorpus) ;
  return(0) ;
}