#include <iostream>
#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <unordered_set>
#include <mutex>
#include <sstream>
using namespace std;

#define NUM_THREADS 3

// mutex lock
mutex mtx;

// Indicates whether solution presented is found to be valid or not
bool validation [NUM_THREADS];

// Variable containing sudoku solution to test in format [row][col]
int solution [9][9];

// Contains indices for a given sudoku square
typedef struct {
    int row;
    int col;
} square;

// Struct for passing parameters to threads
typedef struct {
    int id;
    square *squares; //array of square structs
    int numSquares;
} parameter;

// Function to validate the squares it is passed
void *validate(void *param) {
    int id = ((parameter*) param)->id;
    bool valid = true;
    unordered_set <string> found; 
    square *squares = ((parameter*) param)->squares;
    int numSquares = ((parameter*) param)->numSquares;
    for(int i = 0; i < numSquares; i++) {
        //get the number from the 2d array in row, col from squares[i]
        //check if that number is already in the unordered_set
            //if so, invalid set valid = 0 and break
    }

    validation[id] = valid;

    mtx.lock();
    //print stuff
    mtx.unlock();

    pthread_exit(NULL);
}


int main(int argc, char **argv) {
    
    // Open solution file
    char* filepath = argv[1];
    ifstream solutionFile( filepath );
    if(!solutionFile.is_open()) {
        cout << "Error: file could not be opened";
        exit(0);
    } else {
        cout << "Successfully opened file: " << filepath << endl;
    }

    // Read solution file into solution array
    string line;
    int i = 0;
    while(getline(solutionFile, line) && i < 9) {
        stringstream ss( line );
        int num;
        int j = 0;
        while(ss >> num && j < 9) {
            solution[i][j] = num;
            j++;
        }
        i++;
    }

    solutionFile.close();

    //print solution array obtained
    cout << "Obtained solution array: " << endl;
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            cout << solution[i][j] << " ";
        }
        cout << endl;
    }
    
    //make threads
    pthread_t threads[NUM_THREADS];
    int rc;
    for(int i = 0; i < NUM_THREADS; i++ ) {
        validation[i] = false;
        cout << "main() : creating thread, " << i << endl;
        rc = pthread_create(&threads[i], NULL, validate, (void *)i);
        
        if (rc) {
            cout << "Error: unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    //wait for all threads to complete before continuing
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //if any threads say invalid, its invalid, so print invalid
    bool valid = true;
    for(int i = 0; i < NUM_THREADS; i++) {
        if(!validation[i]) valid = false;
    }

    return 0;
}