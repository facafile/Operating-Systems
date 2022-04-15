#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <values.h>

int zajednicka_var_1=0;
int *zajednicka_var_2;
int Id;

void brisi(int sig){
    (void) shmdt((char *) zajednicka_var_2);
   (void) shmctl(Id, IPC_RMID, NULL);
   exit(0);
}

void *ulazna_dretva(void *arg){
    printf("pokrenuta ULAZNA DRETVA\n");
    int pom;
    int i;
    int j=*((int*)arg);
    srand(time(0));
    for(int x=0;x<j;x++){
        while(zajednicka_var_1 != 0){
            continue;
        }
        sleep(1);
        pom = (rand() % 100) + 1;
        printf("\nULAZNA DRETVA : broj %d\n",pom);
        zajednicka_var_1=pom;
    }
    while(zajednicka_var_1!=0)continue;
    printf("zavrsila ULAZNA DRETVA\n");
}

int main(int argc, char *argv[]){
    
    printf("Pokrenuta RADNA DRETVA\n");
    int i= atoi(argv[1]);


    struct sigaction act;
    pthread_t thr_id;

    act.sa_handler = brisi;
    sigaction(SIGINT, &act, NULL);

    Id = shmget(IPC_PRIVATE, sizeof(int), 0600);
 
   if (Id == -1)
      exit(1);  /* greška - nema zajedničke memorije */
 
   zajednicka_var_2 = (int *) shmat(Id, NULL, 0);
   *zajednicka_var_2 = 0 ;

    if (pthread_create(&thr_id, NULL, ulazna_dretva, &i) != 0) {
        printf("Greska pri stvaranju dretve!\n");
        exit(1);
   }

    if(fork()==0){
        printf("Pokrenut IZLAZNI PROCES\n");

        FILE *fptr1;
        fptr1=fopen("drugi_lab.txt","w");

        for(int l=0;l<i;l++){
            while(*zajednicka_var_2==0){
              continue;
            }
            fprintf(fptr1,"%d\n",*zajednicka_var_2);
            printf("IZLAZNI PROCES : broj upisan u datoteku %d\n",*zajednicka_var_2);

            *zajednicka_var_2=0;
        }

        fclose(fptr1);
        printf("Zavrsio IZLAZNI PROCES\n");
        exit(0);
    }

    for(int x=i;x>0;x--){
        while(zajednicka_var_1 == 0){
            continue;
        }
        
        printf("RADNA DRETVA : procitan broj %d i uvecan na broj %d\n", zajednicka_var_1, zajednicka_var_1 + 1);
        *zajednicka_var_2 = zajednicka_var_1 + 1;

        while(*zajednicka_var_2 != 0){
            continue;
        }
        zajednicka_var_1 = 0;
    }

    printf("\nzavrsila RADNA DRETVA\n");

    (void) wait(NULL);
    pthread_join( thr_id, NULL);
    brisi(0);
    
    return 0;
}