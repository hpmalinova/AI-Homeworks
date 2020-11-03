#include <iostream>
#include <string>
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

/*
 * What is the last number? 8
 *       Calculate row=column=3
 * Make goal board in variable? // or operator =
 *
 */

class Board{
private:
    vector<int> board; // int* board
    int n; // rows = columns;
    int manhattan; // h()
    int zeroPosition;

    void fillBoardWithNumbers(vector<int> numbers) {
        //if (numbers.size() != n*n - 1) {
        //    cout << "Incorrect number of ";
        //}
        for (int i = 0; i< n*n; i++) {
            board[i] = numbers[i];
        }
    }

    int getPositionOfZero() {
        for (int i=0; i < n*n; i++) {
            if (board[i] == 0)
                return i;
        }
    }

    void calculateManhattan() { // TODO when zero_position != - 1
        manhattan = 0;

        for (int i=0; i<n*n; i++) {
            if (board[i] != i + 1) {
                // |currentRow - goalRow| + |currentColumn - goalColumn|
                if (board[i] != 0)
                    manhattan += abs(i / 3 - ((board[i] - 1) / 3)) + abs(i % 3 - ((board[i] - 1) % 3));
                else // TODO
                    manhattan += abs(i / 3 - ((n*n - 1) / 3)) + abs(i % 3 - ((n*n - 1) % 3));
            }
        }
    }

public:
    Board(int lastNumber, vector<int> numbers, int zeroPosition = -1, Board* parent = NULL, int distanceFromStart=0) { // TODO make it work with zeroPosition !=0
        n = sqrt(lastNumber + 1);
        board = numbers;
        //board = new int [n * n];
        //fillBoardWithNumbers(numbers);
        calculateManhattan();
        this->zeroPosition = zeroPosition;
    }

    string showBoard() {
        string str = "";
        for (int i=0; i< n*n; i++) {
            str.append(to_string(board[i]) + " ");
            if (i % n == n-1) {
                str.append("\n");
            }
        }
        return str;
    }

    bool isSolvable() {return true;}; // TODO

//    int tileAt(int row, int column) {
//        if (row >= n || column >= n) {
//            return -1;
//        }
//        return board[row * n + column];
//    }
//
//    int tileAt(int position) {
//        if (position >= n*n) {
//            return -1;
//        }
//        return board[position];
//    }
    vector<int> getBoard() {
        return board;
    }

    int getManhattan() {
        return manhattan;
    }

    bool operator < (Board& otherBoard) {
        return manhattan < otherBoard.getManhattan();
    }

//    bool areBoardsEqual(Board& otherBoard) {
//        return board == otherBoard.getBoard() and
//               manhattan == otherBoard.getManhattan();
//    }

    bool isGoalState() {
        return manhattan == 0;
    }

    vector<Board> getNeighbours() {
       vector<Board> neighbours;
       int positionOfZero = getPositionOfZero();

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

public:
    Puzzle() {
//        int result =
          ida_algorithm();
//        if (result == -1) {
//            cout << "Success" << endl;
//        }
    }

    Board createInitialBoard() {
        int N = 8;
       // vector<int> numbers {6, 5, 3, 2, 4, 8, 7, 0, 1}; //21
        vector<int> numbers {1,3,5,4,2,6,7,8,0}; //6
        //vector<int> numbers {1, 2,3,4,5,6,7,8,0};
        //vector<int> numbers {1, 2, 3, 4, 5, 6, 7,8,0};
        Board initialBoard = Board(N, numbers);
        return initialBoard;
//        if (initialBoard.isGoalState() == true) {
//            cout << "true";
//        }

    }

    void ida_algorithm() {
        Board initialBoard = createInitialBoard();
        int threshold = initialBoard.getManhattan();

        while(1) {
            cout << "Threshold: " << threshold << endl;
            int r = 0; // TODO rename
            int result = search(initialBoard, 0, threshold, r);
            if (result == -1) {
                cout << "Success: " << r << endl;
                break;
            }
            threshold = result;
        }
    }

    int search(Board board, int g, int threshold, int& r) {
        int f = g + board.getManhattan();
       // cout << "g: " << g << endl;
        cout << "h: " << board.getManhattan() << " f: " << f << " g: " << g << endl;
        cout << board.showBoard() << endl;

        if (board.isGoalState()) {
            r = g;
            return -1;
        }

        if (f > threshold) {
            return f;
        }

        int min = INT_MAX;
        vector<Board> neighbours = board.getNeighbours();

        for (int i = 0; i < neighbours.size(); i++) {
            int result = search(neighbours[i], g+1, threshold, r);
            if (result == -1) {
                // cout << neighbours[i].showBoard() << endl;
                return -1;
            }
            if (result < min) {
                min = result;
            }
        }
        g-=1;
        return min;
    }


    void init() {
        int N;
        cout << "Enter count of tiles (ex: 8, 15): " << endl;
        cin >> N;
        // TODO
//        int positionOfZero;
//        cout << "Enter position of zero (ex: 0-" << N << ")" << endl;
//        cin >> positionOfZero;
        vector<int> board;
        cout << "Enter the board (ex: all numbers between 0-" << N << ")" << endl;
        for (int i=0; i<=N; i++) {
            int number;
            cin >> number;
            board.push_back(number);
        }
        Board initialBoard = Board(N, board);
        //goalState = findGoalState(N);
    }

};

int main() {
//    vector<int> numbers {1, 3, 8, 7, 5, 2, 4, 0, 6};
//    Board my_board = Board(8, numbers);
//    cout << my_board.showBoard();
//    vector<Board> ng = my_board.getNeighbours();
////    for (int i=0; i< ng.size(); i++ ){
////        cout << ng[i].showBoard();
////        cout << '\n';
////    }
    Puzzle puz = Puzzle();

    //cout << isGoal;

}
