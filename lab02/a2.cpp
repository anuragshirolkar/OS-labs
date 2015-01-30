#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <signal.h>
#include <wait.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;



static void hdl (int sig, siginfo_t *siginfo, void *context)
{
	cout<<"HERE"<<endl;
}


int main(){

struct sigaction act;
		memset (&act, '\0', sizeof(act));
		act.sa_sigaction = &hdl;
		act.sa_flags = SA_SIGINFO;
		cout<<sigaction(SIGTERM, &act, NULL)<<endl;
	int shmid, pid;
	int *sh;
	shmid =  shmget(IPC_PRIVATE, 4*sizeof(int), IPC_CREAT | 0666);
	//cout<<shmid<<endl;
	printf("%d\n", shmid );

	sh = (int *) shmat(shmid, NULL, 0);
	
	//cout<< &sh<<endl;
	sh[0]=123234;
	//cout<<sh[0]<<endl;

	pid = fork();
	if (pid == 0){
		printf("%s\n", "Hello World");
		printf("%d\n", sh[0] );
		sh[0] = sh[0] + 1;
		while(1) sleep(10);
	}
	else{
		cout<<pid<<endl;

		while(1){

		}
	}
	return 0;

}