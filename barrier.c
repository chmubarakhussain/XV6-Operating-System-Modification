#include<stdio.h>
#include"barrier.h"
#include<semaphore.h>

int init_barrier(int total)
{
	count=0;
	n=total;
	sem_init(&turnstile,0,0);
	sem_init(&turnstile2,0,1);
	sem_init(&mutex,0,1);
	
}

int barrier_point()
{
mutex.wait()
	count+=1
	if count==n;
	turnstile2.wait()
	turnstile.signal()
mutex_signal()

turnstile.wait()
turnstile.signal()

mutex.wait()
	count -= 1
	if count == 0
	turnstile.wait()
	turnstile2.signal()
mutex.signal()

turnstile2.wait()
turnstile2.signal()
	
}
