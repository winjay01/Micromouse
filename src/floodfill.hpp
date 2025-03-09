#include "myQueue.hpp"
#include <iostream>
using namespace std;

// Maze representation: 16x16 unit grid
int wall[16*16][16*16]; // wall[i][j] = 1 means there is a wall between cell i and cell j
int path_cost[16][16]; // path_cost[i][j] = k means cell (i,j) has a path cost of k to the 4 center cells
const int CLEAR = 256;

arrayQueue<int> q;

// initially assume there are no walls in the maze
void init_layout() {
    for (int i = 0; i < 16*16; i++) {
        for (int j = 0; j < 16*16; j++) {
            wall[i][j] = 0;
        }
    }
}

// set all path costs to CLEAR (not yet set)
void clear_costs() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            path_cost[i][j] = CLEAR; // just a number to indicate a cleared path cost; 0 is used for center cells
        }
    }
}

void print_costs() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            printf("%d ", path_cost[i][j]);
            if (path_cost[i][j] <= 9) printf(" ");
        }
        printf("\n");
    }
}

void check_neighbors(int current_cell) {
    int row = current_cell / 16;
    int col = current_cell % 16;
    
    // Check each direction
    // Down
    if ( (path_cost[row + 1][col] == CLEAR) && !(wall[current_cell][current_cell + 16]) ) {
        path_cost[row + 1][col] = path_cost[row][col] + 1;
        q.enQueue((row+1)*16 + col);
    }
    // Right
    if ( (path_cost[row][col + 1] == CLEAR) && !(wall[current_cell][current_cell + 1]) ) {
        path_cost[row][col + 1] = path_cost[row][col] + 1;
        q.enQueue(row*16 + (col + 1));
    }
    // Up
    if ( (path_cost[row - 1][col] == CLEAR) && !(wall[current_cell][current_cell - 16]) ) {
        path_cost[row - 1][col] = path_cost[row][col] + 1;
        q.enQueue((row-1)*16 + col);
    }
    // Left
    if ( (path_cost[row ][col - 1] == CLEAR) && !(wall[current_cell][current_cell - 1]) ) {
        path_cost[row][col - 1] = path_cost[row][col] + 1;
        q.enQueue(row*16 + (col - 1));
    }
    return;
}

void floodfill() {
    // S0
    clear_costs();
    // Set path cost of center cells to 0
    path_cost[16/2 - 1][16/2 - 1] = 0;
    path_cost[16/2 - 1][16/2] = 0;
    path_cost[16/2][16/2 - 1] = 0;
    path_cost[16/2][16/2] = 0;
    
    // Cell (row,col) given a number n is (n/16, n%16) ; n = row*16 + col
    q.enQueue(7*16 + 7);
    q.enQueue(7*16 + 8);
    q.enQueue(8*16 + 7);
    q.enQueue(8*16 + 8);

    // S1
    int current_cell;
    do {
        current_cell = q.front();
        q.deQueue();
        check_neighbors(current_cell);
    } while(!q.empty());

    //print_costs();
}
