#include <stdio.h>
#include<time.h>
#include<stdlib.h>
#include <unistd.h>
int main()
{
	int q = fork();
	int r = fork();
	int s = fork();
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = 500000;  
  FILE* f;    
  long int i,j,k;
	long long x = 0;
  for (j=1;j<999999999/12;j++)
  {
    for (i=0;i<100000000;i++)
    {
      x++;
    }
    if(nanosleep(&tim , &tim2) < 0 )  
    {
      printf("failed \n");
      return -1;
    }
// nanosleep(tim); //50 millis
  }
}
