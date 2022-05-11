#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
# include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>

#define N 5
#define BR_LJUDI 25

sem_t *vrtuljak2,*binarni,*sidi,*izadi;

void posjetitelj() {

   sem_wait(vrtuljak2);

   //sjedi;
   printf("sjedi\n");

    sem_post(sidi);
    sem_wait(binarni);
   

   //ustani;
   printf("Izadi\n");

   sem_post(izadi);
   

}


 
void vrtuljak() {

   while(1) {
    
       for(int i =0; i<N;i++){

           sem_post(vrtuljak2);
       }
       for(int i =0;i<N;i++){
           sem_wait(sidi);
       }
      
      //pokreni vrtuljak;
      //cekaj 1 sekundu
      //zaustavi vrtuljak;
      printf("pokreni vrtuljak\n");
      sleep(1);
      printf("zaustavi vrtuljak\n");

      for(int i=0;i<N;i++){
          sem_post(binarni);
      }
      for(int i=0;i<N;i++){
          sem_wait(izadi);
      }

   }
}

int main(void){
    int ID;
    pid_t pid,pid2;
    //osiguraj odreden memorijski prostor i stavi ga na mjesto po zelji u memoriji
    //4 semafora koja imamo stavi u taj prostor jedan za drugim 
    ID=shmget(IPC_PRIVATE,sizeof(sem_t)*4,0600);
    vrtuljak2 = shmat(ID,NULL,0);
    binarni = (sem_t *)(vrtuljak2+1);
    sidi= (sem_t *)(vrtuljak2+2);
    izadi= (sem_t *)(vrtuljak2+3);
    
    //odredi sto ce se dogadati pri oslobadanju memorije ??
    shmctl(ID,IPC_RMID,NULL); 

    //inicijaliziraj semafore i odredi da su za sinkronizaciju dretvi razlicitih procesa i stavi im svima pocetnu vrijednost na 0
    sem_init(vrtuljak2, 1, 0);
    sem_init(binarni, 1, 0);
    sem_init(sidi, 1, 0);
    sem_init(izadi, 1, 0);
    
    //stvori proces koji saljemo u funkciju vrtuljak ili dojavi gresku ako se proces ne moze stvoriti
    pid=fork();
    if(pid==0){
        vrtuljak();
    }
    else if(pid==-1){
    	printf("ne mogu stvorit proces");
    	exit(1);
    }


    //napravi odreden broj procesa koje saljemo u funkciju posjetitelj 
    //ako se neki proces ne moze stvoritit to dojavi
    for(int i =0;i < BR_LJUDI;i++){
        pid2=fork();
        if (pid2==0) {
		posjetitelj();
		exit(0);
	    }
	else if(pid==-1){
    	printf("ne mogu stvorit proces");
    	exit(1);
    	}
    }
    //cekaj na sve procese posjetitelj
    for(int i =0;i<BR_LJUDI;i++){
    	wait(NULL);
    }
    //ubij proces vrtuljak
    kill(pid,SIGTERM);

    //pobrisi semafore i oslobodi zajednicku memoriju u kojoj su bili
    sem_destroy(vrtuljak2);
    sem_destroy(binarni);
    sem_destroy(sidi);
    sem_destroy(izadi);
    shmdt(vrtuljak2);
    

    return 0;

}

