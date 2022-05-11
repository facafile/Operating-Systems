#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BR_DRETVI 15

pthread_mutex_t m;
pthread_cond_t red[2];
char *prog[]={"Microsoft","Linux"};

int trenutni_prog=0;
int broj_u_rest=0;


void *programer(void *vr){
     //udji(vrsta);
     //obavi;
     //izadji(vrsta);
     int vrsta_prog = *((int *)vr);


    //obavi ulazak u restoran 
     pthread_mutex_lock(&m);
     while(vrsta_prog!=trenutni_prog && broj_u_rest!=0){
         pthread_cond_wait(&red[vrsta_prog],&m);
     }
     broj_u_rest++;
     trenutni_prog=vrsta_prog;
     printf("programer za %s sjeo\n",prog[vrsta_prog]);

     pthread_mutex_unlock(&m);

    //cekaj 2 sekunde
    sleep(2);

    //obavi izlazak iz restorana
    pthread_mutex_lock(&m);

        //smanji broj ljudi u restoranu za 1
        broj_u_rest--;
        //ako je taj broj veci od 0 propusti sljedecu dretvu u redu cekanja istih programera kao i ovoga sto upravo izlazi ako uopce neko ceka tamo
        if(broj_u_rest > 0){
            pthread_cond_signal(&red[vrsta_prog]);
        }
        //u suprotnom propusti sve dretve programera suprotne orjentacije od programera koji upravo izlazi da obavljaju svoj posao
        else{
            pthread_cond_broadcast(&red[1-vrsta_prog]);
        }
        printf("programer za %s izasao iz restorana\n",prog[vrsta_prog]);
    pthread_mutex_unlock(&m);



}

int main(void){

    //inicijaliziraj monitor i 2 reda cekanja za odredeni uvjet
    pthread_mutex_init(&m,NULL);
    pthread_cond_init(&red[0],NULL);
    pthread_cond_init(&red[1],NULL);

    //napravi onoliko dretvi koliko ce pocetno biti programera
    //i odredi nasumicno hoce li biti za Microsoft ili Linux
    pthread_t programeri[BR_DRETVI]; 
    int svi_prog[BR_DRETVI];
    srand((int) time(NULL));

    for(int i=0;i<BR_DRETVI;i++){
        svi_prog[i]=rand()%2;
    }

    //napravi dretve ili dojavi pogresku 
    //nakon stvaranja svake dretve cekaj 1 sekundu
    for(int i =0;i<BR_DRETVI;i++){
        if (pthread_create(&programeri[i], NULL, programer, &svi_prog[i])) {
			printf("Ne mogu napraviti thread programer - %d\n",i);
			exit(0);
	    }  
	    sleep(1); 
    }

    //cekaj na zavrsetak svih dretvi programer
    for(int i =0;i<BR_DRETVI;i++){
    	pthread_join(programeri[i],NULL);
    }

    //pobrisi monitor i redove cekanja koje smo inicijalizirali
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&red[0]);
    pthread_cond_destroy(&red[1]);

    return 0;
}