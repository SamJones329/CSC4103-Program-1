#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unordered_set>

using namespace std;

#define NUM_THREADS 3

//spinlock
int turn [NUM_THREADS-1];
int flag = 0;

int validation [NUM_THREADS]; 
int done [NUM_THREADS];

typedef struct {
    int row;
    int col;
} square;

typedef struct {
    int id;
    square *squares; //array of square structs
    int numSquares;
} parameter;

void *validate(void *param) {
    int id = ((parameter*) param)->id;
    int valid = 1;
    unordered_set <string> found; 
    square *squares = ((parameter*) param)->squares;
    int numSquares = ((parameter*) param)->numSquares;
    for(int i = 0; i < numSquares; i++) {
        //get the number from the 2d array in row, col from squares[i]
        //check if that number is already in the unordered_set
            //if so, invalid set valid = 0 and break
    }

    //if no thread in critical section, flag for critical section, enter critical section
    //else get in line for critical section
    //critical section
    validation[id] = valid;
    done[id] = 1;
    //end critical section
    //flag off critical section

    pthread_exit(NULL);
}

int main() {
    
    //Read sudoku.txt from parameters
    //transpose the data into a 2-D array
    //make the array have global scope here
    
    pthread_t threads[NUM_THREADS];
    int rc;
    for(int i = 0; i < NUM_THREADS; i++ ) {
        done[i] = 0;
        cout << "main() : creating thread, " << i << endl;
        rc = pthread_create(&threads[i], NULL, validate, (void *)i);
        
        if (rc) {
            cout << "Error: unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    while(!done[0] && !done[1] && !done[2]);

    //if any threads say invalid, its invalid, so print invalid

    return 0;
}