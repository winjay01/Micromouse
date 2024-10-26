#include "floodfill.hpp"
using namespace std;

int path[256]; // save path traversed

// Add detected wall to represntation of maze layout
void update_layout(int cell1, int cell2) {
    wall[cell1][cell2] = 1;
}

int in_path(int cell) {
    for (int i = 0; i < 256; i++) {
        if (path[i] == cell) return 1;
    }
    return 0;
}

void print_path() {
    int cell;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            cell = i*16 + j;
            if (in_path(cell)) {
                printf("*  ");
            }
            else {
                printf("%d ", path_cost[i][j]);
                if (path_cost[i][j] <= 9) printf(" ");
            }
        }
        printf("\n");
    }
}

int center_reached(int current_cell) {
    return ((current_cell == (7*16 + 7)) || (current_cell == (7*16 + 8)) || (current_cell == (8*16 + 7)) || (current_cell == (8*16 + 8)));
}

int min(int row, int col) {
    //printf("Checking min...\n");
    int min = 256;
    int min_cell = row*16 + col;

    // Check cell below
    if (row < 15) {
        if (path_cost[row + 1][col] < min) {
            min = path_cost[row + 1][col];
            min_cell = (row+1)*16 + col;
        }
    }
    // Check cell to left
    if (col > 0) {
        if (path_cost[row][col - 1] < min) {
            min = path_cost[row][col - 1];
            min_cell = row*16 + (col - 1);
        }
    }
    // Check cell above
    if (row > 0) {
        if (path_cost[row - 1][col] < min) {
            min = path_cost[row - 1][col];
            min_cell = (row-1)*16 + col;
        }
    }
    // Check cell to right
    if (col < 15) {
        if (path_cost[row][col + 1] < min) {
            min = path_cost[row][col + 1];
            min_cell = row*16 + (col + 1);
        }
    }

    return min_cell;
}

int traverse(int current_cell, int* i) {
    int row = current_cell / 16;
    int col = current_cell % 16;

    while (!wall[current_cell][min(row, col)]) {
        path[*i] = current_cell;
        //printf("Depth: %d ", *i);
        *i = *i + 1;

        current_cell = min(row, col);
        //printf("New current cell: %d\n", current_cell);
        row = current_cell / 16;
        col = current_cell % 16;

        if (center_reached(current_cell)) return current_cell;
    }
    return current_cell;
}

int main() {
    init_layout();
    int current_cell = 0; // start at top left
    int i = 0;

    while(!center_reached(current_cell)) {
        floodfill();
        current_cell = traverse(current_cell, &i);
        print_path();
    }
}