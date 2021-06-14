/*
 * The Game of Life
 *
 * a cell is born, if it has exactly three neighbours 
 * a cell dies of loneliness, if it has less than two neighbours 
 * a cell dies of overcrowding, if it has more than three neighbours 
 * a cell survives to the next generation, if it does not die of loneliness 
 * or overcrowding 
 *
 * In my version, a 2D array of ints is used.  A 1 cell is on, a 0 cell is off.
 * The game plays 100 rounds, printing to the screen each time.  'x' printed
 * means on, space means 0.
 *
 */
#include<errno.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>    
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include<string.h>
#include<signal.h>

/* dimensions of the screen */

#define BOARD_WIDTH	79
#define BOARD_HEIGHT	24
#define TOTAL_THREADS	4

/*semaphores declarations*/
sem_t turnstile;
//sem_t turnstile2;
sem_t mutex;
pthread_mutex_t lock;

/*global variables*/
int gcount=0;
int totalPthreads=0;
int giterator=0;
int giterator1=0;

/*required structs*/
struct c1
{
int** stboard;
int identifier;
};


/* set everthing to zero */

void initialize_board (int** board) {
	int	i, j;

	for (i=0; i<BOARD_WIDTH; i++) for (j=0; j<BOARD_HEIGHT; j++) 
		board[i][j] = 0;
}

/* add to a width index, wrapping around like a cylinder */

int xadd (int i, int a) {
	i += a;
	while (i < 0) i += BOARD_WIDTH;
	while (i >= BOARD_WIDTH) i -= BOARD_WIDTH;
	return i;
}

/* add to a height index, wrapping around */

int yadd (int i, int a) {
	i += a;
	while (i < 0) i += BOARD_HEIGHT;
	while (i >= BOARD_HEIGHT) i -= BOARD_HEIGHT;
	return i;
}

/* return the number of on cells adjacent to the i,j cell */

int adjacent_to (int** board, int i, int j) {
	int	k, l, count;

	count = 0;

	/* go around the cell */

	for (k=-1; k<=1; k++) for (l=-1; l<=1; l++)

		/* only count if at least one of k,l isn't zero */

		if (k || l)
			if (board[xadd(i,k)][yadd(j,l)]) count++;
	return count;
}


 int barrier()
{
	sem_wait(&mutex);
	gcount=gcount+1;
	sem_post(&mutex);
	if(gcount==totalPthreads)
	{
			sem_post(&turnstile);						
	}
	sem_wait(&turnstile);
	sem_post(&turnstile);
	return 0;
} 
void* play (void* param) /*int board[][BOARD_HEIGHT], int tDecider*/
{
/*
	(copied this from some web page, hence the English spellings...)

	1.STASIS : If, for a given cell, the number of on neighbours is 
	exactly two, the cell maintains its status quo into the next 
	generation. If the cell is on, it stays on, if it is off, it stays off.

	2.GROWTH : If the number of on neighbours is exactly three, the cell 
	will be on in the next generation. This is regardless of the cell's 		current state.

	3.DEATH : If the number of on neighbours is 0, 1, 4-8, the cell will 
	be off in the next generation.
*/
struct c1 obj = *(struct c1*)param;

int	i, j, a, newboard[BOARD_WIDTH][BOARD_HEIGHT];
int start= (obj.identifier)*(BOARD_WIDTH/TOTAL_THREADS);
int end=((obj.identifier)+1)*(BOARD_WIDTH/TOTAL_THREADS);

	if(end>BOARD_WIDTH)
	{
		end=BOARD_WIDTH;
	}

	/* for each cell, apply the rules of Life */
	
	for (i=start; i<end; i++) for (j=0; j<BOARD_HEIGHT; j++) {
		a = adjacent_to (obj.stboard, i, j);
		
		
		barrier();
		/*CRITICAL SECTION STARTS */
		
		if (a == 2) newboard[i][j] = obj.stboard[i][j];
		if (a == 3) newboard[i][j] = 1;
		if (a < 2) newboard[i][j] = 0;
		if (a > 3) newboard[i][j] = 0;
	}


	/* copy the new board back into the old board */

	for (i=start; i<end; i++) for (j=0; j<BOARD_HEIGHT; j++) {
		obj.stboard[i][j] = newboard[i][j];
	}
	
		/*CRITICAL SECTION ENDS */
		
	
	pthread_exit(NULL);
}


/* print the life board */

void print (int** board) {
	int	i, j;

	/* for each row */

	for (j=0; j<BOARD_HEIGHT; j++) {

		/* print each column position... */

		for (i=0; i<BOARD_WIDTH; i++) {
			printf ("%c", board[i][j] ? 'x' : ' ');
		}

		/* followed by a carriage return */

		printf ("\n");
	}
}

/* read a file into the life board */

void read_file (int** board, char *name) {
	FILE	*f;
	int	i, j;
	char	s[100];

	f = fopen (name, "r");
	for (j=0; j<BOARD_HEIGHT; j++) {

		/* get a string */

		fgets (s, 100, f);

		/* copy the string to the life board */

		for (i=0; i<BOARD_WIDTH; i++) {
			board[i][j] = s[i] == 'x';
		}
	}
	fclose (f);
}

/* main program */

int main (int argc, char *argv[]) {
	int**	board=0;
		int i, j;
		board = malloc(BOARD_WIDTH * sizeof(int*));
	for(i=0;i<BOARD_WIDTH;i++)
	{
		board[i] = malloc(BOARD_HEIGHT * sizeof(int));
	}		
	struct c1 obj;
	obj.stboard=board;

	initialize_board (board);
	read_file (board, argv[1]);
	totalPthreads=TOTAL_THREADS;
	if( (BOARD_WIDTH % TOTAL_THREADS) > 0) totalPthreads++;
	pthread_t tid[totalPthreads];
	
	/*semaphores initilization used in BARRIERS*/
	

	/* play game of life 100 times */
	int k;
	for (i=0; i<100; i++)
	{
		sem_init(&turnstile, 0, 1);
		//sem_init(&turnstile2, 0, 1);
		sem_init(&mutex, 0, 1);
		if (pthread_mutex_init(&lock, NULL) != 0) 
		{ 
			printf("\n mutex init has failed\n"); 
			return 1; 
		} 
			print (board);
			printf("totalPthreads:%d",totalPthreads);
			for(k=0;k<totalPthreads;k++)
			{	
				obj.identifier=k;
				if(pthread_create((tid+k),0,&play,&obj)<0)
					{
						printf("Thread creation failed\n in %dth thread",k);
						exit(1);	
					}
				pthread_join(tid[k],0);
			}

			/* clear the screen using VT100 escape codes */
		
			sleep(1);//for displaying each level out of 100 
			puts ("\033[H\033[J");
			sem_destroy(&mutex);
		//sem_destroy(&turnstile2);
		sem_destroy(&turnstile);
	 pthread_mutex_destroy(&lock); 
	}
	/* for(i=0;i<BOARD_WIDTH;i++)
	{
		free board[i];

	}	
		free board; */
		
	return 0;
}
