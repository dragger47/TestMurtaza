

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 4
#define MATRIX_SIZE 1024
#define BLOCK_SIZE (MATRIX_SIZE / NUM_THREADS)

int matrix[MATRIX_SIZE][MATRIX_SIZE];
int results[MATRIX_SIZE]; 
typedef struct {
    int thread_id;
    int buffer[BLOCK_SIZE][MATRIX_SIZE];
} ThreadInfo;


void fill_matrix() {
    // Random number init with seed time
    srand((unsigned int)time(NULL));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i][j] = rand() % 101;
        }
    }
}

bool CorrectOutPutCheck() {
    //serial sum
    long long serial_sum = 0;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            serial_sum += matrix[i][j];
        }
    }

    //parallel sum
    long long parallel_sum = 0;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        parallel_sum += results[i];
    }

    if (serial_sum == parallel_sum) {
        printf("Serial Sum = %lld\n", serial_sum);
        printf("Parallel Sum = %lld\n", parallel_sum);
        printf("Operation successful, simmilar results obtained\n");
        return true;
    } else {
        printf("Serial Sum = %lld\n", serial_sum);
        printf("Parallel Sum = %lld\n", parallel_sum);
        printf("Operation failed, could not obtain simmilar results\n");
        return false;
    }
}


void *row_wise_sum(void *arg) {
    ThreadInfo *info = (ThreadInfo *)arg;

    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            results[info->thread_id * BLOCK_SIZE + i] += info->buffer[i][j];
        }
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadInfo thread_info[NUM_THREADS];

    fill_matrix();

    for (int i = 0; i < NUM_THREADS; i++) {
        //setting thread attr
        thread_info[i].thread_id = i;

        // Creating buffer
        int start_row = i * BLOCK_SIZE;
        int end_row = (i + 1) * BLOCK_SIZE;

        size_t block_size = BLOCK_SIZE * MATRIX_SIZE * sizeof(int);
        memcpy(thread_info[i].buffer, &matrix[start_row][0], block_size);
       
        
        // Creating thread
        pthread_create(&threads[i], NULL, row_wise_sum, &thread_info[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
            printf("Sums calculated by thread \n%d: [ ", i);
            for (int j = 0; j < BLOCK_SIZE; j++) {
                printf("%d", results[i * BLOCK_SIZE + j]);
                if (j < BLOCK_SIZE - 1) {
                    printf(", ");
                }
            }
            printf(" ]\n\n");
        }
    CorrectOutPutCheck();
    return 0;
}
