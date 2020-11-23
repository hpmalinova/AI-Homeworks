//
// Created by Xpucu on 23.11.2020 г..
//
// Gene:
#include <cstdlib>     /* srand, rand */
#include <string>
#include <iostream>

class City {
private:
    int x{};
    int y{};

    void generateCoordinates(int start, int end) {
        x = rand() % (end - start + 1) + start;
        y = rand() % (end - start + 1) + start;
    }

public:
    explicit City(int start = 0, int end = 9999, const std::string &s = "Generate") {
        if (s == "Generate") {
            generateCoordinates(start, end);
        } else {
            x = -1;
            y = -1;
        }
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    bool operator==(const City &other) const {
        return x == other.getX() and y == other.getY();
    }

    friend std::ostream &operator<<(std::ostream &os, const City &c) {
        os << "(" << c.getX() << ", " << c.getY() << ")";
        return os;
    }
};

