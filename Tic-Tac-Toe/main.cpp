#include <iostream>
#include <utility>
#include <vector>
#include <ctime>       /* time */
#include <algorithm>    /* std::shuffle */
#include <cstdlib>     /* srand, rand */

using std::vector;
using std::string;
using std::cout;

const short N = 3;

const vector<short> CHILDREN_ORDER{0, 2, 6, 8, 1, 3, 5, 7, 4};

const char PLAYER1 = 'X';
const char PLAYER2 = 'O';
const char EMPTY_SPACE = '.';
const char CONTINUE_THE_GAME = 'C';
const char TIE = 'T';

struct Result {
    int value;
    short position;

    Result(int value, short position) : value(value), position(position) {}

    bool shouldContinue() const {
        return value == -1 and position == -1;
    }
};

class Board {
private:
    vector<vector<char>> board;
    short positionChanged;

    bool isValid(short position) {
        return !(position > N * N - 1 or position < 0 or getAt(position) != EMPTY_SPACE);
    }

    bool isFull() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == EMPTY_SPACE)
                    return false;
            }
        }
        return true;
    }

public:
    Board() : board(N, vector<char>(N, '.')), positionChanged(-1) {}

    explicit Board(Board parent, short position, char value) : positionChanged(position) {
        board = parent.getBoard();
        setAt(position, value);
    }

    bool isValid(short x, short y) {
        if (x < 0 or x >= N or y < 0 or y >= N or board[x][y] != EMPTY_SPACE)
            return false;
        return true;
    }

    vector<vector<char>> getBoard() { return board; }

    bool setAt(short position, char value) {
        if (isValid(position)) {
            board[position / N][position % N] = value;
            positionChanged = position;
            return true;
        }
        return false;
    }

    char getAt(int position) {
        return board[position / N][position % N];
    }

    short getPosition() const { return positionChanged; }

    void printBoard() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << board[i][j] << " ";
            }
            cout << "\n";
        }
    }

    // Is it a terminal state?
    // Returns the winner or CONTINUE_THE_GAME
    char isTerminal() {
        char first;

        // Vertical
        for (short column = 0; column < N; column++) {
            first = board[0][column];
            if (first != EMPTY_SPACE and first == board[1][column] and first == board[2][column]) {
                return first;
            }
        }

        // Horizontal
        for (short row = 0; row < N; row++) {
            first = board[row][0];
            if (first != EMPTY_SPACE and first == board[row][1] and first == board[row][2]) {
                return first;
            }
        }

//            if (board[row] == vector<char>{PLAYER1, PLAYER1, PLAYER1})
//                return PLAYER1;
//            else if (board[row] == vector<char>{PLAYER2, PLAYER2, PLAYER2})
//                return PLAYER2;

        // Main Diagonal
        first = board[0][0];
        if (first != '.' and first == board[1][1] and first == board[2][2]) {
            return first;
        }

        // Second diagonal
        first = board[0][2];
        if (first != '.' and first == board[1][1] and first == board[2][0]) {
            return first;
        }

        return (isFull()) ? TIE : CONTINUE_THE_GAME;
    }

    vector<Board> getSuccessors() {
        vector<Board> successors;
        for (auto &position : CHILDREN_ORDER) {
            if (getAt(position) == EMPTY_SPACE) {
                char value;

                if (positionChanged == -1)
                    value = PLAYER1;
                else
                    value = (getAt(positionChanged) == PLAYER1) ? PLAYER2 : PLAYER1;

                successors.push_back(Board(*this, position, value));
            }
        }
        return successors;
    }

    void setFirstInRandomCorner(char value) {
        vector<int> corners = vector<int>(CHILDREN_ORDER.begin(), CHILDREN_ORDER.begin() + 4);
        std::random_shuffle(corners.begin(), corners.end());

        short i = 0;
        while (!isValid(i)) { i++; }
        setAt(corners[i], value);
        positionChanged = corners[i];
    }
};

class Game {
private:
    char playerTurn;
    char AI;        // 'X' or 'O'
    char Player;    // 'O' or 'X'
    Board currentBoard;

    void printWinner(char winner) const {
        if (winner == AI) {
            cout << "The winner is " << AI << "!\n";
        } else if (winner == Player) {
            cout << "The winner is " << Player << "!\n";
        } else if (winner == TIE) {
            cout << "It`s a tie\n";
        }
    }

    void humanPlay() {
        short x, y, position;
        while (true) {
            cout << "Enter x,y: ";
            std::cin >> x >> y;
            cout << "\n";
            position = x * N + y;
            if (currentBoard.isValid(x, y)) {
                currentBoard.setAt(position, playerTurn);
                break;
            } else {
                cout << "Invalid position, try again\n";
            }
        }
    }

    static Result evaluate(char winner, int depth, short position) {
        if (winner == PLAYER1) {
            return Result((N * N + 1) - depth, position);
        }
        if (winner == TIE) {
            return Result(0, position);
        }
        if (winner == PLAYER2) {
            return Result(depth - (N * N + 1), position); // todo change
        }
        return Result(-1, -1); // CONTINUE
    }

    Result max(Board myBoard, int alpha, int beta, int depth) {
        Result result = evaluate(myBoard.isTerminal(), depth, myBoard.getPosition());
        if (!result.shouldContinue()) {
            return result;
        }

        int maxValue = INT_MIN; // - Infinity
        short position;

        vector<Board> successors = myBoard.getSuccessors();
        for (auto &child: successors) {
            Result r = min(child, alpha, beta, depth + 1); // todo fix

            if (r.value > maxValue) {
                maxValue = r.value;
                position = child.getPosition(); // todo ванка r.position
            }

            alpha = std::max(alpha, maxValue);
            if (alpha > beta) {
                break;
            }
        }
        return Result(maxValue, position);
    }

    Result min(Board myBoard, int alpha, int beta, int depth) {
        Result result = evaluate(myBoard.isTerminal(), depth, myBoard.getPosition());
        if (!result.shouldContinue()) {
            return result;
        }

        int minValue = INT_MAX; // + Infinity
        short position;

        for (auto &child: myBoard.getSuccessors()) {
            Result r = max(child, alpha, beta, depth + 1); // todo fix

            if (r.value < minValue) {
                minValue = r.value;
                position = child.getPosition();// todo ванка r.position
            }
            beta = std::min(beta, minValue);
            if (alpha > beta) {
                break;
            }
        }
        return Result(minValue, position);
    }

public:
    explicit Game(bool firstAI = false) {
        if (firstAI) {
            AI = PLAYER1;
            Player = PLAYER2;
            playerTurn = AI;
        } else {
            Player = PLAYER1;
            AI = PLAYER2;
            playerTurn = Player;
        }
        currentBoard = Board();
    }

    void playMinimaxWithAlphaBetaPruning() {
        char winner = currentBoard.isTerminal();
        int depth = 0;
        currentBoard.printBoard();

        while (winner == CONTINUE_THE_GAME) {
            cout << "\n";

            if (playerTurn == Player) {
                humanPlay();
                playerTurn = AI;
            } else { // AI
                // Start the Corner Strategy if AI plays first
                if (depth == 0) {
                    currentBoard.setFirstInRandomCorner(AI);
                } else if (AI == PLAYER1) {
                    Result r = max(currentBoard, INT_MIN, INT_MAX, depth);
                    bool res = currentBoard.setAt(r.position, AI);
                    if (!res) {
                        cout << "Not a successful set\n";
                    }
                } else if (AI == PLAYER2) {
                    Result r = min(currentBoard, INT_MIN, INT_MAX, depth);
                    bool res = currentBoard.setAt(r.position, AI);
                    if (!res) {
                        cout << "Not a successful set\n";
                    }
                }
                playerTurn = Player;
            }
            winner = currentBoard.isTerminal();
            depth++;
            currentBoard.printBoard();
        }
        printWinner(winner);
    }
};

int main() {
    srand(time(nullptr));
    Game g = Game();
    g.playMinimaxWithAlphaBetaPruning();
    return 0;
}
