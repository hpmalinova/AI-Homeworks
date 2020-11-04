#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::to_string;
using std::swap;
//using std::sort;

const int SUCCESS = -1;


class Board{
private:
    vector<int> board;
    int n; // n = rows = columns;
    int heuristic; // h() - Heuristic function
    int zeroPositionInGoalState;

    int getCurrentPositionOfZero() {
        for (int i=0; i < n*n; i++) {
            if (board[i] == 0)
                return i;
        }
        return -1; // Zero not found;
    }

    // Where heuristic function is:
    // Summation of the Manhattan distance between misplaced nodes
    int calculateHeuristic() { // TODO when zero_position != - 1
        int manhattan = 0;

        for (int i=0; i<n*n; i++) {
            if (board[i] != i + 1) {
                // |currentRow - goalRow| + |currentColumn - goalColumn|
                if (board[i] != 0)
                    manhattan += abs(i / 3 - ((board[i] - 1) / 3)) + abs(i % 3 - ((board[i] - 1) % 3));
                else // TODO
                    manhattan += abs(i / 3 - ((n*n - 1) / 3)) + abs(i % 3 - ((n*n - 1) % 3));
            }
        }
        return manhattan;
    }

public:
    Board(int lastNumber, vector<int> numbersInBoard, int zeroPositionInGoalState = -1, Board* parent = nullptr) { // TODO make it work with zeroPositionInGoalState !=0
        n = (int) sqrt(lastNumber + 1);
        board = std::move(numbersInBoard);
        if (zeroPositionInGoalState == -1)
            zeroPositionInGoalState = lastNumber;
        this->zeroPositionInGoalState = zeroPositionInGoalState;
        heuristic = calculateHeuristic();
    }

    string showBoard() {
        string str;
        for (int i = 0; i < n * n; i++) {
            str.append(to_string(board[i]) + " ");
            if (i % n == n - 1) {
                str.append("\n");
            }
        }
        return str;
    }

    bool isSolvable() {return true;}; // TODO

    vector<int> getBoard() {
        return board;
    }

    int getHeuristic() const {
        return heuristic;
    }

    bool operator < (Board& otherBoard) const {
        return heuristic < otherBoard.getHeuristic();
    }

//    bool areBoardsEqual(Board& otherBoard) {
//        return board == otherBoard.getBoard() and
//               heuristic == otherBoard.getHeuristic();
//    }

    bool isGoalState() const {
        return heuristic == 0;
    }

    vector<Board> getNeighbours() {
       vector<Board> neighbours;
       int positionOfZero = getCurrentPositionOfZero();

       // left
        if (positionOfZero % n != 0){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero - 1]);
            Board neighbour(n*n - 1, neighbourBoard);
            neighbours.push_back(neighbour);
        }

        // right
        if (positionOfZero % n != 2){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero + 1]);
            Board neighbour(n*n - 1, neighbourBoard);
            neighbours.push_back(neighbour);
        }

        // up
        if (positionOfZero / n != 0){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero - n]);
            Board neighbour(n*n - 1, neighbourBoard);
            neighbours.push_back(neighbour);
        }

        // down
        if (positionOfZero / n != 2){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero + n]);
            Board neighbour(n*n - 1, neighbourBoard);
            neighbours.push_back(neighbour);
        }

        //sort(neighbours.begin(), neighbours.end());
        return neighbours;
    }
};


class Puzzle{
private:
    Board createInitialBoard() {
        int N = 8;
        vector<int> numbers {1, 3, 5, 4, 2, 6, 7, 8, 0}; // 6
        //vector<int> numbers {6, 5, 3, 2, 4, 8, 7, 0, 1}; // 21
        //vector<int> numbers {1, 2, 3, 4, 5, 6, 7, 8, 0}; // 0
        Board initialBoard = Board(N, numbers);
        return initialBoard;
    }

    Board createInitialBoardFromUser() {
        int N;
        cout << "Enter count of tiles (ex: 8, 15): " << endl;
        cin >> N;

        int zeroPositionInGoalState;
        cout << "Enter position of zero in goal state (ex: 0-" << N << ")" << endl;
        cin >> zeroPositionInGoalState;

        vector<int> board;
        cout << "Enter the board (ex: all numbers between 0-" << N << ")" << endl;
        for (int i=0; i<=N; i++) {
            int number;
            cin >> number;
            board.push_back(number);
        }
        return Board(N, board, zeroPositionInGoalState);
    }

public:
    Puzzle() {
        // Board initialBoard = createInitialBoardFromUser();
        Board initialBoard = createInitialBoard();
        ida_algorithm(initialBoard);
    }

    void ida_algorithm(const Board& initialBoard) {
        int threshold = initialBoard.getHeuristic();

        while(true) {
            int lengthOfOptimalPath = 0; // TODO rename
            int g = 0; // Number of nodes traversed from a start node to get to the current node
            int result = search(initialBoard, g, threshold, lengthOfOptimalPath);
            if (result == SUCCESS) {
                cout << "Length of Optimal Path: " << lengthOfOptimalPath << endl;
                break;
            }
            threshold = result;
        }
    }

    // Returns:
    // SUCCESS,                        when an optimal path to the goal is found
    // min{f1,f2,..} = next-threshold, where fi: fi > current-threshold;
    int search(Board board, int g, int threshold, int& lengthOfOptimalPath) {
        int f = g + board.getHeuristic();
        // cout << board.showBoard() << endl;

        if (board.isGoalState()) {
            lengthOfOptimalPath = g;
            return -1;
        }

        if (f > threshold) {
            return f;
        }

        int min = INT_MAX;
        vector<Board> neighbours = board.getNeighbours();

        for (auto & neighbour : neighbours) {
            int result = search(neighbour, g+1, threshold, lengthOfOptimalPath);
            if (result == SUCCESS) {
                return SUCCESS;
            }
            if (result < min) {
                min = result;
            }
        }

        return min;
    }
};

int main() {
    Puzzle puz = Puzzle();
}
