#include <signal.h>  
#include <stdio.h>  
#include <string.h>  
#include <sys/time.h>  

void timer_handler(int);
void print_t1(int);
void print_t2(int);
void print_t3(int);
void timer_init();
  
int main ()  
{
  	timer_init();
  	while(1);
}

void timer_init(void){
  	struct sigaction sa;  
  	struct itimerval timer;  
  	/* Install timer_handler as the signal handler for SIGVTALRM.  */  
  	memset (&sa, 0, sizeof (sa));  
  	sa.sa_handler = &timer_handler;  
  	sigaction (SIGVTALRM, &sa, NULL);  
  	/* Configure the timer to expire after 250 msec...  */  
  	timer.it_value.tv_sec = 0;  
  	timer.it_value.tv_usec = 25000;  
  	/* ... and every 250 msec after that.  */  
  	timer.it_interval.tv_sec = 0;  
  	timer.it_interval.tv_usec = 25000;  
  	/* Start a virtual timer. It counts down whenever this process is  
     	executing.  */  
  	setitimer (ITIMER_VIRTUAL, &timer, NULL);  
  	/* Do busy work.  */  
}  

void timer_handler (int signum)
{
        static int count = 0;
        count+=10;
        static int t1=600;
        t1-=10;
        static int t2=350;
        t2-=10;
        static int t3=250;
        t3-=10;
        if(t3<10){
                t3+=250;
        }
        if(t2<10){
                t2+=350;
        }
        if(t1<10){
                t1+=600;
        }
        printf ("\n%d (%4d %4d %4d) : ", count, t1, t2, t3);
        if(t1==10){
                void(*print)(int);
		print=&print_t1;
		(*print)(1);
        }
        if(t2==10){
                void(*print)(int);
		print=&print_t2;
		(*print)(2);
        }
        if(t3==10){
                void(*print)(int);
		print=&print_t3;
		(*print)(3);
        }
}

void print_t1(int i){
        printf("Task %d ", i);
}

void print_t2(int i){
        printf("Task %d ", i);
}

void print_t3(int i){
        printf("Task %d ", i);
}



