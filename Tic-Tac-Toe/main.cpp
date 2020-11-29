#include <iostream>
#include <vector>
#include <ctime>        /* time */
#include <algorithm>    /* std::shuffle */
#include <cstdlib>      /* srand, rand */

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

    [[nodiscard]] bool shouldContinue() const {
        return value == -1 and position == -1;
    }
};

class Board {
private:
    vector<vector<char>> board;
    short positionChanged;

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

    bool isValid(short position) {
        return !(position > N * N - 1 or position < 0 or getAt(position) != EMPTY_SPACE);
    }

    vector<vector<char>> getBoard() { return board; }

    void setAt(short position, char value) {
        if (isValid(position)) {
            board[position / N][position % N] = value;
            positionChanged = position;
        }
    }

    char getAt(int position) {
        return board[position / N][position % N];
    }

    [[nodiscard]] short getPosition() const { return positionChanged; }

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

                successors.emplace_back(*this, position, value);
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
    char playerTurn{};
    char AI{};        // 'X' or 'O'
    char Player{};    // 'O' or 'X'
    Board currentBoard;

    void setFirstPlayer() {
        cout << "Do you want to play first? (y/n)  \n";
        char answer = '.';
        while (answer != 'y' and answer != 'n') {
            std::cin >> answer;
        }

        Player = (answer == 'y') ? PLAYER1 : PLAYER2;
        AI = (answer == 'n') ? PLAYER1 : PLAYER2;
        playerTurn = (answer == 'y') ? Player : AI;
    }

    void printWinner(char winner) const {
        if (winner == AI) {
            cout << "You`ve lost! \n";
        } else if (winner == Player) {
            cout << "You`ve won! \n";
        } else if (winner == TIE) {
            cout << "It`s a tie! \n";
        }
    }

    void humanPlay() {
        short x, y, position;
        while (true) {
            cout << "Enter x, y: ";
            std::cin >> x >> y;
            cout << "\n";
            position = x * N + y;
            if (currentBoard.isValid(position)) {
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
            return Result(depth - (N * N + 1), position);
        }
        return Result(-1, -1); // There is no winner
    }

    Result max(Board myBoard, int alpha, int beta, int depth) {
        Result result = evaluate(myBoard.isTerminal(), depth, myBoard.getPosition());
        if (!result.shouldContinue()) {
            return result;
        }

        int maxValue = INT_MIN; // - Infinity
        short position;

        for (auto &child: myBoard.getSuccessors()) {
            Result r = min(child, alpha, beta, depth + 1);

            if (r.value > maxValue) {
                maxValue = r.value;
                position = child.getPosition();
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
            Result r = max(child, alpha, beta, depth + 1);

            if (r.value < minValue) {
                minValue = r.value;
                position = child.getPosition();
            }

            beta = std::min(beta, minValue);
            if (alpha > beta) {
                break;
            }
        }
        return Result(minValue, position);
    }

public:
    explicit Game() {
        setFirstPlayer();
        currentBoard = Board();
    }

    void playMinimaxWithAlphaBetaPruning() {
        char winner = currentBoard.isTerminal();
        int depth = 0;
        cout << "\n";
        currentBoard.printBoard();
        cout << "\n";

        while (winner == CONTINUE_THE_GAME) {
            if (playerTurn == Player) {
                humanPlay();
                playerTurn = AI;
            } else { // AI
                if (depth == 0) {
                    currentBoard.setFirstInRandomCorner(AI);
                } else if (AI == PLAYER1) {
                    Result r = max(currentBoard, INT_MIN, INT_MAX, depth);
                    currentBoard.setAt(r.position, AI);
                } else if (AI == PLAYER2) {
                    Result r = min(currentBoard, INT_MIN, INT_MAX, depth);
                    currentBoard.setAt(r.position, AI);
                }
                playerTurn = Player;
            }
            winner = currentBoard.isTerminal();
            depth++;
            currentBoard.printBoard();
            cout << "\n";
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
