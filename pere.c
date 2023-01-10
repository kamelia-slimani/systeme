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

#define key_num 1
#define TAILLE 1024
#define TAILLE_NB_LETTRE 26
/* 
 - user :  correspond au temps que met le processus utilisateur.
 - system : correspond au temps que met le processus kernel .
 - total : correspond au temps en temps réel que met le processus.
 - Lorsque qu'on a beaucoup de fils, le temps est long car notre machine suppose qu'un seul coeur peut suffir, tant qu'elle n'est pas débordé elle restera sur se mode de travail. 
 - De plus les semaphores combinés a un grand nombre de processus ne fait que ralentir encore plus la machine. */


int main(int argc, char const *argv[]){
    char lettre ;
    char ligneTraite[15];
    char ligneATraiteC[15];
    int nbfils = atoi(argv[2]);
    int deb[nbfils];
    int ligneAtraite[nbfils];
    int nbligne= 0;
    int nbLignesFichier = 0;
    int * pVariablePartagee ;
    char chaine[TAILLE] ;
   
    //Recuperer nb ligne 
    FILE*  fichierCorpus = fopen(argv[1], "r") ;
    if (!fichierCorpus) {
        printf("erreur a l'ouverture du fichier %s\n", argv[1]) ;
        exit(2) ;
    }

    while (fgets(chaine, TAILLE, fichierCorpus)) {
        nbLignesFichier++;
    }
    
    key_t key = ftok("pere.c", key_num);
    //creation memoire partagé
    int mem_id = shmget(key, 1*sizeof(int), IPC_CREAT|0660);
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
    pid_t ret[nbfils];
    int reste = nbLignesFichier%nbfils;
    for(int j = 0; j <nbfils; j ++){
        if(reste == 0){
            deb[j] = ((nbLignesFichier/nbfils) *j); 
            ligneAtraite[j] = nbLignesFichier/nbfils ;
        }
        else{

            ligneAtraite[j] =  nbLignesFichier/nbfils + 1;
            deb[j] = ((nbLignesFichier/nbfils)*j) +1;
            reste --;
            }
            deb[0]=0;
        }
        
    
    for(int i =0 ; i< nbfils; i++){
        ret[i] = fork();
        if (ret[i]==-1){
            perror("création impossible");
            exit(1);
            }
        else if (ret[i] == 0){
               
            sprintf(ligneATraiteC, "%d", deb[i]);
            sprintf(ligneTraite, "%d", ligneAtraite[i]);
            execl("./LectureCorpusLettre","LectureCorpusLettre.c", argv[1],"test.txt",ligneATraiteC,ligneTraite, NULL);
            wait(&ret[i]);
            exit(0);
            }  
    }
    for(int j =0; j<nbfils; j++){
        wait(&ret[j]);
    }
   
    lettre='a';
    for(int i=0; i<TAILLE_NB_LETTRE; i++){
        
        nbligne+= pVariablePartagee[i];
        
    }
    double res = 0;
    for(int i=0; i<TAILLE_NB_LETTRE; i++){
    res +=(double)(pVariablePartagee[i]*100)/nbligne;
    printf("%c : %f %%\n", lettre++, (double)(pVariablePartagee[i]*100)/nbligne );
    // printf("%c : %f \n", lettre++, (double)(pVariablePartagee[i]) );
            
  }

  printf("Total : %f%%\n",res);
  return 0;
}
