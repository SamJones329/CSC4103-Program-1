#include <iostream>
#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <unordered_set>
#include <mutex>
#include <sstream>
using namespace std;

#define NUM_THREADS 27

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

// Enum for parameter patterns
enum PatternType{ row, column, subgrid };

// Struct for passing parameters to threads
typedef struct {
    square firstSquare; //top left square of the pattern to check
    PatternType pattern; //wbether to check row, column of subgrid
    int id; //id of thread
} parameter;


// Function to validate the squares it is passed
void *validate(void *param) {
    int id = ((parameter*) param)->id;
    PatternType pattern = ((parameter*) param)->pattern;
    square first = ((parameter*) param)->firstSquare;
    int firstCol = first.col;
    int firstRow = first.row;

    bool valid = true;
    unordered_set <int> found; 

    string checkType;
    if(pattern == row) {
        checkType = "Row " + to_string(firstRow+1);
        for(int i = firstCol; i < firstCol+9; i++) {
            int num = solution[firstRow][i];
            if(found.find(num) != found.end() || num < 1 || num > 9) {
                valid = false;
                break;
            }
            found.insert(num);
        }
    } else if(pattern == column) {
        checkType = "Column " + to_string(firstCol+1);
        for(int i = firstRow; i < firstRow+9; i++) {
            int num = solution[i][firstCol];
            if(found.find(num) != found.end() || num < 1 || num > 9) {
                valid = false;
                break;
            }
            found.insert(num);
        }
    } else if(pattern == subgrid) {
        checkType = "Subgrid R" + to_string(firstRow+1) + to_string(firstRow+2) + to_string(firstRow+3) 
                         + "-C" + to_string(firstCol+1) + to_string(firstCol+2) + to_string(firstCol+3);
        for(int i = firstRow; i < firstRow+3; i++) {
            for(int j = firstCol; j < firstCol+3; j++) {
                int num = solution[i][j];
                if(found.find(num) != found.end() || num < 1 || num > 9) {
                    valid = false;
                    break;
                }
                found.insert(num);
            }
        }
    }

    validation[id-1] = valid;
    
    string validStr = valid ? "Valid" : "Invalid";
    string outputStr = "Thread " + to_string(id) + ", " + checkType + ", " + validStr;

    mtx.lock();
    cout << outputStr << endl;
    mtx.unlock();

    pthread_exit(NULL);
}


// Main function
int main(int argc, char **argv) {
    
    // Open solution file
    char* filepath = argv[1];
    ifstream solutionFile( filepath );
    if(!solutionFile.is_open()) {
        cout << "Error: file could not be opened";
        exit(0);
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

    // Make array of parameters to be used for threads
    parameter paramArr [NUM_THREADS];
    for(int i = 0; i < 9; i++) {
        square curSq = {i, 0};
        paramArr[i] = {curSq, row, i+1};
    }
    for(int i = 9; i < 18; i++) {
        square curSq = {0, i-9};
        paramArr[i] = {curSq, column, i+1};
    }
    int index = 18;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            square curSq = {3*i, 3*j};
            paramArr[index] = {curSq, subgrid, index+1};
            index++;
        }
    }
    
    // Make threads
    pthread_t threads[NUM_THREADS];
    int rc;
    for(int i = 0; i < NUM_THREADS; i++ ) {
        validation[i] = false;
        rc = pthread_create(&threads[i], NULL, validate, (void *) &paramArr[i]);
        
        if (rc) {
            cout << "Error: unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    // Wait for all threads to complete before continuing
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // If any threads say invalid, it's invalid, so print invalid
    bool valid = true;
    for(int i = 0; i < NUM_THREADS; i++) {
        if(!validation[i]) valid = false;
    }
    if(!valid) cout << "Solution invalid" << endl;
    else cout << "Solution valid" << endl;

    return 0;
}