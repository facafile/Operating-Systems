#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

void obradi_sigusr1(int sig);
void obradi_sigterm(int sig);

FILE *fptr1;

int value;
int state;

int main(void){

    struct sigaction act;
    int pom;
    

    act.sa_handler = obradi_sigusr1;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM);
    act.sa_flags = 0; 
    sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = obradi_sigterm;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);
    
    printf("Program s PID=%ld krenuo s radom\n", (long) getpid());

    fptr1=fopen("status.txt","r");
    fscanf(fptr1,"%d",&state);
    fclose(fptr1);

    if(state==0){
        fptr1=fopen("obrada.txt","r");
        fscanf(fptr1,"%d",&pom);
        while(!feof(fptr1)){
            state=pom;
            fscanf(fptr1,"\n%d",&pom);
        }
        state = sqrt(state);
        fclose(fptr1);
    }

    value=state;
    fptr1=fopen("status.txt","w");
    fprintf(fptr1,"%d\n",0);
    fclose(fptr1);
	
	while(1){
	printf(".\n");
        value++;
        pom=(int) pow((double)value,2);
        fptr1=fopen("obrada.txt","a");
        fprintf(fptr1,"%d\n",pom);
        fclose(fptr1);
        for(int i =0;i<5;i++)
		sleep(1);
	}

    return 0;
}

void obradi_sigusr1(int sig){
        printf("%d\n",value);
    }

void obradi_sigterm(int sig){
        fptr1= fopen("status.txt","w");
        fprintf(fptr1,"%d\n",value);
        fclose(fptr1);
        exit(1);
    }