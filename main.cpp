#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define M 200
#define K 200
#define N 200
#define NUM_THREADS 100

int A [M][K];
int B [K][N];
int C [M][N];
int goldenC [M][N];
// Adjusted struct to hold ranges or chunks
struct v {
   int start_row;
   int end_row;
};

void *runner(void *param); /* the thread */

int main(int argc, char *argv[]) {
	int i, j, k;
	struct timespec t_start, t_end;
	double elapsedTime;
	// Re-define these arrays with the size of NUM_THREADS
	pthread_t tid[NUM_THREADS];
	pthread_attr_t attr[NUM_THREADS];
	
	for(i = 0; i < M; i++) {
	    for(j = 0; j < N; j++) {
			A[i][j] = rand()%100;
		}
	}
	for(i = 0; i < K; i++) {
 	   for(j = 0; j < N; j++) {
        	B[i][j] = rand()%100;
    }
}
	
	// start time
	clock_gettime( CLOCK_REALTIME, &t_start);  	
	
	// Distributing work to threads
	int chunk_size = M / NUM_THREADS; // Determine the size of each chunk
	for(i = 0; i < NUM_THREADS; i++) {
		struct v *data = (struct v *) malloc(sizeof(struct v));
		data->start_row = i * chunk_size;
		data->end_row = (i + 1) * chunk_size - 1;
		if(i == NUM_THREADS - 1) data->end_row = M - 1; // Ensure last thread covers all remaining
		pthread_attr_init(&attr[i]);
		pthread_create(&tid[i], &attr[i], runner, data);
	}
	
	// Adjusted the pthread_join to match the number of threads
	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(tid[i], NULL);
	}

	// stop time
	clock_gettime( CLOCK_REALTIME, &t_end);

	// compute and print the elapsed time in millisec
	elapsedTime = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
	elapsedTime += (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;
	printf("Parallel elapsedTime: %lf ms\n", elapsedTime);	
	//Print out the resulting matrix
	
	
	// start time
	clock_gettime( CLOCK_REALTIME, &t_start);  
	for(i = 0; i < M; i++) {
		for(j = 0; j < N; j++) {
			for(k=0; k<K; k++){
				goldenC[i][j]+=A[i][k] * B[k][j];
			}
		}
	}
	// stop time
	clock_gettime( CLOCK_REALTIME, &t_end);

	// compute and print the elapsed time in millisec
	elapsedTime = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
	elapsedTime += (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;
	printf("Sequential elapsedTime: %lf ms\n", elapsedTime);	
	
	int pass = 1;
	for(i = 0; i < M; i++) {
		for(j = 0; j < N; j++) {
			if(goldenC[i][j]!=C[i][j]){
				pass = 0;
			}
		}
	}	
	if(pass==1)
		printf("Test pass!\n");
	
	return 0;
}

// Adjusted runner function to process a chunk of the matrix
void *runner(void *param) {
    struct v *data = (struct v*) param;
    int i, j, k, sum;
    for(i = data->start_row; i <= data->end_row; i++) {
        for(j = 0; j < N; j++) {
            sum = 0;
            for(k = 0; k < K; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    pthread_exit(0);
}