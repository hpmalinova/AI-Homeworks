#include <iostream>
#include <vector>
#include <string>
#include <algorithm>    // std::shuffle
#include <array>        // std::array
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using std::vector;
using std::string;
using std::cout;
using namespace std::chrono;

const int k = 5;

class Board {
private:
    int N; // Size of the board: NxN

    // Keeps only the positions of the queens
    vector<int> queens; // queen := (x,y), where queens[y] = x;

    // Number of conflicts in each row, diagonalL and diagonalR:
    vector<int> rows;
    vector<int> diagonalL; // Starts in the upper left corner
    vector<int> diagonalR; // Starts in the upper right corner

    int lastY; // Column of the last moved queen

    static vector<int> myShuffle(int start, int end) {
        vector<int> vec(end);
        std::iota(std::begin(vec), std::end(vec), start); // Fill with 0, 1, ..., end-1.
        // Obtains a time-based seed:
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
        return vec;
    }

    void upgradeConflicts(int x, int y, int summand) {
        rows[x] += summand;
        diagonalL[y - x + N - 1] += summand;
        diagonalR[x + y] += summand;
    }

    // Places one queen in each column
    void initializeBoard() {
        vector<int> randomColumn = myShuffle(0, N);
        vector<int> randomRow = randomColumn;

        int minX = -1;
        int minY = -1;
        int minConflicts = -1;

        // Goes through all columns in random order
        for (int c = 0; c < N; c++) {
            int y = randomColumn[c];
            int x = randomRow[0];
            minX = x;
            minY = y;
            minConflicts = rows[x] + diagonalL[y - x + N - 1] + diagonalR[y + x];

            // Finds the row with the least conflicts
            for (int r = 1; r < N; r++) {
                x = randomRow[r];
                int curConflicts = rows[x] + diagonalL[y - x + N - 1] + diagonalR[y + x];
                if (curConflicts < minConflicts) {
                    minX = x;
                    minY = y;
                    minConflicts = curConflicts;
                }
            }

            // Places the queen on the board and upgrades conflicts
            queens[minY] = minX;
            upgradeConflicts(minX, minY, 1);
        }
    }

    bool isFinal() {
        int conflicts = 0;
        for (int y = 0; y < N; y++) {
            int x = queens[y];
            conflicts += rows[x] + diagonalL[y - x + N - 1] + diagonalR[y + x];
        }
        return conflicts == 3 * N;
    }

public:
    explicit Board(int N) : N(N), queens(N), rows(N), diagonalL(2 * N - 1), diagonalR(2 * N - 1), lastY(-1) {
        initializeBoard();
    }

    // Min-Conflicts Algorithm
    /*
     Given an initial assignment of values to all the variables of a CSP,
     the algorithm randomly selects a variable from the set of variables with conflicts violating one or more constraints of the CSP.
     Then it assigns to this variable the value that minimizes the number of conflicts.
     If there is more than one value with a minimum number of conflicts, it chooses one randomly.
     This process of random variable selection and min-conflict value assignment is iterated until a solution is found
     or a pre-selected maximum number of iterations is reached.
    */
    void solvePuzzle() {
        int iter = 0;
        while (iter++ <= k * N) {
            if (isFinal()) {
                if (N < 20) {
                    cout << "\n" << "Solution: " << "\n";
                    printBoard();
                }
                break;
            }

            // Takes the queen that violates the most constraints
            int maxY = -1;
            int maxX = -1;
            int maxConflicts = -1;

            // Goes through the columns in a random order
            vector<int> randomColumnQueen = myShuffle(0, N);
            for (int c = 0; c < N; c++) {
                int y = randomColumnQueen[c];
                int x = queens[y];
                int conflicts = rows[x] + diagonalL[y - x + N - 1] + diagonalR[y + x];
                if (conflicts > maxConflicts and lastY != y) {
                    maxY = y;
                    maxX = x;
                    maxConflicts = conflicts;
                }
            }

            // Finds the row that minimizes the number of conflicts
            int minConflicts = N;
            int minX = -1;

            // Goes through the rows in a random order
            vector<int> randomRow = myShuffle(0, N);
            for (int r = 0; r < N; r++) {
                int x = randomRow[r];
                int conflicts = rows[x] + diagonalL[maxY - x + N - 1] + diagonalR[maxY + x];
                if (conflicts < minConflicts) {
                    minX = x;
                    minConflicts = conflicts;
                }
            }

            // Removes conflicts with the current queen:
            upgradeConflicts(maxX, maxY, -1);

            // Places the queen that violates the most constraints
            // on the row that minimizes the number of conflicts
            queens[maxY] = minX;

            // Removes conflicts with the new queen:
            upgradeConflicts(minX, maxY, 1);

            // Saves the position of the queen that was moved
            // So that it won't be moved on the next move
            lastY = maxY;
        }
        if (!isFinal()) {
            // Random Restart
            solvePuzzle();
        }
    }

    void printBoard() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (queens[j] == i) {
                    cout << "* ";
                } else {
                    cout << "_ ";
                }
            }
            cout << "\n";
        }
        cout << "\n";
    }

};


int main() {
    srand(time(0));

    auto start = high_resolution_clock::now();

    Board board = Board(10000);
    board.solvePuzzle();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: " << duration.count() / 1000 << " milliseconds" << "\n";

    return 0;
}
