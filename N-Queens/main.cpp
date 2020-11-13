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

// За четни нямаме в ъгъла (0,0), (0,N-1), (N-1,0), (N-1, N-1)
// По хода на коня?

class Board {
private:
    int N;
    vector<int> queens; // queens[y] = x; Keep only the positions of the queens
    vector<int> rows;
    vector<int> diagonalL; // starts in the upper left corner
    vector<int> diagonalR; // starts in the upper right corner

public:
    explicit Board(int N) : N(N), queens(N, -1), rows(N, 0), diagonalL(2*N-1, 0), diagonalR(2*N-1, 0) {
    }

    static vector<int> myShuffle(int start, int end) {
        vector<int> vec(end);
        std::iota (std::begin(vec), std::end(vec), start); // Fill with 0, 1, ..., end-1.
        // obtain a time-based seed:
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (vec.begin(), vec.end(), std::default_random_engine(seed));
        return vec;
    }

    void initializeBoard() { // TODO if (N%2==0)
        vector<int> randomColumn = myShuffle(0, N);
        vector<int> randomRow = randomColumn;
//        std::rotate(randomRow.begin(), randomRow.begin()+3,randomRow.end());

        for (auto& c: randomColumn) {
            cout << c << " ";
        }
        cout << "\n";
        for (auto& r: randomRow) {
            cout << r << " ";
        }
        cout << "\n";

        int minX = -1;
        int minY = -1;
        int minConflicts = -1;

        for (int c = 0; c < N; c++) {
            int y = randomColumn[c];
            int x = randomRow[0];
            minX = x;
            minY = y;
            minConflicts = rows[x] + diagonalL[y - x + N - 1] + diagonalR[y + x];
            for (int r = 1; r < N; r++) {
                x = randomRow[r];
                int curConflicts = rows[x] + diagonalL[y - x + N - 1] + diagonalR[y + x];
                if (curConflicts < minConflicts) {
                    minX = x;
                    minY = y;
                    minConflicts = curConflicts;
                }
            }
            // Place Queen
            queens[minY] = minX;
            rows[minX]++;
            // column - row + (N - 1)
            diagonalL[minY - minX + N - 1]++;
            // row + column
            diagonalR[minY + minX]++;
        }
    }

    /*
        int firstQueenX = rand() % N; // 0, 1, 2, ... N
        int firstQueenY = columns[0];
        cout << firstQueenY<<columns[1]<<columns[2]<<columns[3] << firstQueenX << " ";
        queens[firstQueenY] = firstQueenX;
        rows[firstQueenX] ++;
        // column - row + (N - 1)
        diagonalL[firstQueenY - firstQueenX + N - 1] ++;
        // row + column
        diagonalR[firstQueenY + firstQueenX] ++;
        vector<int> rowsSequence = columns;
        for (int c = 1; c < N; c++) { // 0 1 2
            int x = rowsSequence[0];
            int y = columns[c];
            int minConflict = rows[x] + diagonalL[y-x+N-1] + diagonalR[y+x];
            int minX = x;
            int minY = y;
            for (int r = 1; r < N; r++) {
                x = rowsSequence[r];
                int conflicts = rows[x] + diagonalL[y-x+N-1] + diagonalR[y+x];
                if (conflicts < minConflict) {
                    minConflict = conflicts;
                }
            }
*/

    void printBoard() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
               if (queens[j] == i){
                   cout << "* ";
               } else {
                   cout << "_ ";
               }
            }
            cout << "\n";
        }
    }

    bool isFinal() {
        int conflicts = 0;
        for (int y=0; y<N;y++) {
            int x = queens[y];
            conflicts += rows[x] + diagonalL[y - x + N - 1] + diagonalR[y + x];
            if (conflicts != 0)
                return false;
        }
        return true;
    }
};


int main() {
    srand(time(0));
    Board board = Board(4);
    board.initializeBoard();
    board.printBoard();
    cout << board.isFinal();
    return 0;
}
