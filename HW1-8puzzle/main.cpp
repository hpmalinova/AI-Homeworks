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
    int n; // n = rows = columns;
    vector<int> board;
    int zeroPositionInGoalState;
    int heuristic; // h() - Heuristic function
    string parentMove; // "init" "left" "right" "up" "down" "goal"

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
    Board(int lastNumber, vector<int> numbersInBoard, int zeroPositionInGoalState = -1, string parentMove = "init") { // TODO make it work with zeroPositionInGoalState !=0
        n = (int) sqrt(lastNumber + 1);
        board = std::move(numbersInBoard);
        if (zeroPositionInGoalState == -1)
            zeroPositionInGoalState = lastNumber;
        this->zeroPositionInGoalState = zeroPositionInGoalState;
        heuristic = calculateHeuristic();
        this->parentMove = std::move(parentMove);
        //if (isGoalState())
        //    this->parentMove = "goal";
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

    vector<int> getBoard() const {
        return board;
    }

    int getHeuristic() const {
        return heuristic;
    }

    string getParentMove() const {
        return parentMove;
    }

    bool operator < (Board& otherBoard) const {
        return heuristic < otherBoard.getHeuristic();
    }

    bool isGoalState() const {
        return heuristic == 0;
    }

    vector<Board> getNeighbours() {
       vector<Board> neighbours;
       int positionOfZero = getCurrentPositionOfZero();

       // left
        if (positionOfZero % n != 0 and parentMove!="right"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero - 1]);
            string move = "left";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        // right
        if (positionOfZero % n != 2 and parentMove!="left"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero + 1]);
            string move = "right";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        // up
        if (positionOfZero / n != 0 and parentMove!="down"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero - n]);
            string move = "up";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        // down
        if (positionOfZero / n != 2 and parentMove!="up"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[positionOfZero], neighbourBoard[positionOfZero + n]);
            string move = "down";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        return neighbours;
    }
};


class Puzzle{
private:
    Board createInitialBoard() {
        int N = 8;
        // 6: up, up, left, down, right, down
        //vector<int> numbers {1, 3, 5, 4, 2, 6, 7, 8, 0};

        // 21:   right, up, up, left, left, down, right, up, right, down,
        // left, down, left, up,  right, up, left, down, down, right, right
        vector<int> numbers {6, 5, 3, 2, 4, 8, 7, 0, 1};

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
            // cout << "Threshold: " << threshold << endl;
            int lengthOfOptimalPath = 0;
            int g = 0; // Number of nodes traversed from a start node to get to the current node
            vector<string> path;
            int result = search(initialBoard, g, threshold, lengthOfOptimalPath, path);
            if (result == SUCCESS) {
                cout << "<------------------------->" << endl;
                cout << "Length of Optimal Path: " << lengthOfOptimalPath << endl;
                cout << "<------------------------->" << endl;
                cout << "           Path:           " << endl;
                cout << "<------------------------->" << endl;
                for (auto & move : path) {
                    cout << "           " << move << endl;
                }
                break;
            }
            threshold = result;
        }
    }

    // Returns:
    // SUCCESS,                         when an optimal path to the goal is found
    // min{f1,f2,..} = next-threshold,  where fi: fi > current-threshold;
    int search(Board board, int g, int threshold, int& lengthOfOptimalPath, vector<string>& path) {
        int f = g + board.getHeuristic();
        if (board.getParentMove() != "init")
            path.push_back(board.getParentMove());

        //cout << "h: " << board.getHeuristic() << " g: " << g << " f: " << f << endl;
        //cout << board.getParentMove() << endl;
        //cout << board.showBoard() << endl;
        //cout << endl;

        if (board.isGoalState()) {
            lengthOfOptimalPath = g;
            return SUCCESS;
        }

        if (f > threshold) {
            return f;
        }

        int min = INT_MAX;
        vector<Board> neighbours = board.getNeighbours();

        for (auto & neighbour : neighbours) {
            int result = search(neighbour, g+1, threshold, lengthOfOptimalPath, path);
            if (result == SUCCESS) {
                return SUCCESS;
            }
            if (result < min) {
                min = result;
            }
            path.pop_back(); // Unsuccessful path, go back
        }

        return min;
    }
};

int main() {
    Puzzle puz = Puzzle();
}
