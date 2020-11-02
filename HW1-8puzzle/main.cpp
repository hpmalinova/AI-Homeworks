#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using std::cout;
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
// * // goal state:
//        for (int i = 0; i < n * n; i++) {
//                board[i] = i + 1;
//        }
//        board[n*n -1] = 0
 */

class Board{
private:
    vector<int> board; // int* board
    int n; // rows = columns;
    Board* parent;
    int distanceFromStart; // g()
    int manhattan;

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
    Board(int lastNumber, vector<int> numbers, int zeroPosition = -1) { // TODO make it work with zeroPosition !=0
        n = sqrt(lastNumber + 1);
        board = numbers;
        //board = new int [n * n];
        //fillBoardWithNumbers(numbers);
        calculateManhattan();
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

    int getManhattan() {
        return manhattan;
    }

//    bool compareManhattan(Board otherBoard) {
//        return manhattan < otherBoard.getManhattan();
//    }

    bool operator < (Board& otherBoard) {
        return manhattan < otherBoard.getManhattan();
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

        sort(neighbours.begin(), neighbours.end());
        return neighbours;
    }
};

int main() {
    vector<int> numbers {1, 3, 8, 7, 5, 2, 4, 0, 6};
    Board my_board = Board(8, numbers);
    cout << my_board.showBoard();
    vector<Board> ng = my_board.getNeighbours();
//    for (int i=0; i< ng.size(); i++ ){
//        cout << ng[i].showBoard();
//        cout << '\n';
//    }
}
