#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>

using std::cin;
using std::cout;
using std::vector;
using std::string;
using std::to_string;
using std::swap;
using namespace std::chrono;

const int SUCCESS = -1;


class Board{
private:
    int n; // n = rows = columns;
    vector<int> board;
    int zeroPosition;
    int zeroPositionInGoalState;
    int heuristic; // h() - Heuristic function
    string parentMove; // "init" "left" "right" "up" "down"

    int getInversionsCount() {
        int inversions = 0;
        for (int i = 0; i < n * n - 1; i++)
            for (int j = i + 1; j < n*n; j++)
                if (board[i] > board[j] and board[j] != 0) {
                    inversions++;
                }

        return inversions;
    }

    int getCurrentPositionOfZero() {
        for (int i=0; i < n*n; i++) {
            if (board[i] == 0)
                return i;
        }
        return -1; // Zero not found;
    }

    // Where heuristic function is:
    // Summation of the Manhattan distance between misplaced nodes
    int calculateHeuristic() {
        int manhattan = 0;

        // Calculate goal state:
        // goal[i] = v, v: index of i in the goal state

        int goal[n*n];
        goal[0] = zeroPositionInGoalState;
        for (int i = 1; i <= zeroPositionInGoalState; i++)
            goal[i] = i - 1;
        for (int i = zeroPositionInGoalState + 1; i < n*n; i++)
            goal[i] = i;

        // Calculate Manhattan distance:
        for (int i=0; i < n*n; i++) {
            // |currentRow - goalRow| + |currentColumn - goalColumn||
            manhattan += abs(i / n - goal[board[i]] / n) + abs(i % n - goal[board[i]] % n);
        }
        return manhattan;
    }

public:
    Board(int lastNumber, vector<int> numbersInBoard, int zeroPositionInGoalState = -1, string parentMove = "init") {
        n = (int) sqrt(lastNumber + 1);

        board = std::move(numbersInBoard);

        zeroPosition = getCurrentPositionOfZero();

        if (zeroPositionInGoalState == -1) {
            zeroPositionInGoalState = lastNumber;
        }
        this->zeroPositionInGoalState = zeroPositionInGoalState;

        heuristic = calculateHeuristic();

        this->parentMove = std::move(parentMove);
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

    bool isSolvable() {
        int inversions = getInversionsCount();
        // If n is odd => #inversions is even
        if (n % 2 != 0)
            return inversions % 2 == 0;
        // If n is even => #inversions + rowOfZero is odd
        else
            return inversions + (zeroPosition / n) % 2 != 0;
    };

    [[nodiscard]] int getHeuristic() const {
        return heuristic;
    }

    [[nodiscard]] string getParentMove() const {
        return parentMove;
    }

    [[nodiscard]] bool isGoalState() const {
        return heuristic == 0;
    }

    bool operator < (Board& otherBoard) const {
        return heuristic < otherBoard.getHeuristic();
    }

    vector<Board> getNeighbours() {
       vector<Board> neighbours;

        // left
        if (zeroPosition % n != 0 and parentMove != "right"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[zeroPosition], neighbourBoard[zeroPosition - 1]);
            string move = "left";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        // right
        if (zeroPosition % n != (n - 1) and parentMove != "left"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[zeroPosition], neighbourBoard[zeroPosition + 1]);
            string move = "right";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        // up
        if (zeroPosition / n != 0 and parentMove != "down"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[zeroPosition], neighbourBoard[zeroPosition - n]);
            string move = "up";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        // down
        if (zeroPosition / n != (n - 1) and parentMove != "up"){
            vector<int> neighbourBoard = board;
            swap(neighbourBoard[zeroPosition], neighbourBoard[zeroPosition + n]);
            string move = "down";
            Board neighbour(n*n - 1, neighbourBoard, zeroPositionInGoalState, move);
            neighbours.push_back(neighbour);
        }

        return neighbours;
    }
};


class Puzzle{
private:
    static Board createInitialBoard() {
        int N;
        vector<int> numbers;

        N = 8;
        numbers = {6, 5, 3,
                   2, 4, 8,
                   7, 0, 1};
        // 21:   right, up, up, left, left, down, right, up, right, down, left
        //       down, left, up,  right, up, left, down, down, right, right
         Board initialBoard = Board(N, numbers);

        // 21: right, up, up, left, down, left, up, right, down, right, up
        //     left,  down, down, left,  up, up, right, down, left, up
        //Board initialBoard = Board(N, numbers, 0);

//        N = 15;
//        numbers = {1,2,3,4,
//                   5,6,7,10,
//                   11,8,9,12,
//                   13,0,14,15};
//        // 26: right, up, left, left, down, right, right, right, up, left, left, up, right,
        //     right, down, left, up, left, down, down, left, up, right, right, right, down

//        numbers = {3,6,11,4,
//                   2,14,12,7,
//                   5,9,15,10,
//                   13,1,8,0};
//        // 36: ..

//        numbers = {8,7,6,3,
//                   2,15,12,13,
//                   1,14,5,11,
//                   9,4,10,0};
//        // 48: ..

//        Board initialBoard = Board(N, numbers);
        return initialBoard;
    }

    static Board createInitialBoardFromUser() {
        int N;
        cout << "Enter count of tiles (ex: 8, 15): \n ";
        cin >> N;

        int zeroPositionInGoalState;
        cout << "Enter position of zero in goal state (ex: 0-" << N << ") \n";
        cin >> zeroPositionInGoalState;

        vector<int> board;
        cout << "Enter the board (ex: all numbers between 0-" << N << ") \n";
        for (int i=0; i<=N; i++) {
            int number;
            cin >> number;
            board.push_back(number);
        }
        return Board(N, board, zeroPositionInGoalState);
    }

public:
    Puzzle() {
        //Board initialBoard = createInitialBoardFromUser();
        Board initialBoard = createInitialBoard();
        if (initialBoard.isSolvable()) {
            ida_algorithm(initialBoard);
        } else {
            cout << "This board is unsolvable! \n" ;
        }
    }

    void ida_algorithm(const Board& initialBoard) {
        int threshold = initialBoard.getHeuristic();

        while(true) {
            //cout << "Threshold: " << threshold << std::endl;
            int lengthOfOptimalPath = 0;
            int g = 0; // Number of nodes traversed from a start node to get to the current node
            vector<string> path;
            int result = search(initialBoard, g, threshold, lengthOfOptimalPath, path);
            if (result == SUCCESS) {
                cout << "<-------------------------> \n";
                cout << "Length of Optimal Path: " << lengthOfOptimalPath << "\n";
                cout << "<-------------------------> \n";
                cout << "           Path:           \n";
                cout << "<-------------------------> \n";
                path.erase(path.begin()); // Remove "init" state
                for (auto & move : path) {
                    cout << "           " << move << "\n";
                }
                cout << "<-------------------------> \n";
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

        path.push_back(board.getParentMove());

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
    auto start = high_resolution_clock::now();
    Puzzle puz = Puzzle();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: " << duration.count() / 1000 << "milliseconds" << "\n";
}
