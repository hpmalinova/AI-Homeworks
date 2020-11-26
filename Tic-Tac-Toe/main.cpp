#include <iostream>
#include <vector>

using std::vector;
using std::string;
using std::cout;

//const string PlayerOneWins = 1;
const char PLAYER1 = 'X';
const char PLAYER2 = 'O';
const char EMPTY_SPACE = '.';
const char CONTINUE_THE_GAME = 'C';
const char TIE = 'T';

class Game {
private:
    vector<vector<char>> board;
    int N;
    char playerTurn;
    char AI;        // 'X' or 'O'
    char Player;    // 'O' or 'X'

    void initializeBoard() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                board[i][j] = EMPTY_SPACE;
            }
        }
    }

    bool isValid(int x, int y) {
        if (x < 0 or x >= N or y < 0 or y >= N) {
            return false;
        } else if (board[x][y] != EMPTY_SPACE) {
            return false;
        } else {
            return true;
        }
    }

    // Checks if the game has ended and returns the winner or "continue"
    char hasEnded() {
        char first;
        char current;

        // Vertical
        for (int column = 0; column < N; column++) {
            int row = 0;
            first = board[row++][column];

            if (first != EMPTY_SPACE) {
                current = first;
                while (column < N and first == current) {
                    current = board[row++][column];
                }
                if (column == N and first == current) {
                    return first;
                }
            }
        }

        // Horizontal
        for (int row = 0; row < N; row++) {
            int column = 0;
            first = board[row][column++];

            if (first != EMPTY_SPACE) {
                current = first;
                while (column < N and first == current) {
                    current = board[row][column++];
                }
                if (column == N and first == current) {
                    return first;
                }
            }
        }

        // Main diagonal
        int k = 0;
        first = board[k][k];
        k++;
        current = first;
        while (k < N and first == current) {
            current = board[k][k];
            k++;
        }
        if (k == N and first == current) {
            return first;
        }

        // Second diagonal
        int r = N - 1;
        int c = 0;
        first = board[r--][c++];
        current = first;

        while (c < N and first == current) {
            current = board[r--][c++];
        }
        if (c == N and first == current) {
            return first;
        }

        // If the board is not full, continue the game
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == EMPTY_SPACE) {
                    return CONTINUE_THE_GAME;
                }
            }
        }

        // The board is full and there is no winner, return Tie
        return TIE;
    }

    char max() {
        return 'X';
    }

    char min() {
        return 'O';
    }

public:
    explicit Game(int N = 3, bool firstAI = true) : N(N) {
        initializeBoard();
        if (firstAI) {
            AI = PLAYER1;
            Player = PLAYER2;
            playerTurn = AI;
        } else {
            Player = PLAYER1;
            AI = PLAYER2;
            playerTurn = Player;
        }
    }

    void minimaxWithAlphaBetaPruning() {
        while (true) {
            printBoard();
            char winner = hasEnded();

            if (winner != CONTINUE_THE_GAME) {
                if (winner == AI) {
                    cout << "The winner is " << AI << "!\n";
                } else if (winner == Player) {
                    cout << "The winner is " << Player << "!\n";
                } else if (winner == TIE) {
                    cout << "It`s a tie\n";
                }
                break;
            }

            // TODO algorithm

            break;
        }

    }

    void printBoard() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << board[i][j] << " ";
            }
            cout << "\n";
        }
    }
};


int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
