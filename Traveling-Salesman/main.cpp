#include <iostream>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
#include <vector>
#include <algorithm>    // std::shuffle
#include <cmath>       /* sqrt */

using std::cout;
using std::vector;

// Gene:
class City {
private:
    int x{};
    int y{};

    void generateCoordinates(int start, int end) {
        x = rand() % (end - start + 1) + start;
        y = rand() % (end - start + 1) + start;
    }

public:
    explicit City(int start = 0, int end = 999) {
        generateCoordinates(start, end);
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    friend std::ostream &operator<<(std::ostream &os, const City &c) {
        os << "(" << c.getX() << ", " << c.getY() << ")";
        return os;
    }
};

struct Cities {
private:
    static vector<City> cities;
    static int size;

public:
    static void generateCities(int citiesCount) {
        cities.clear();
        size = citiesCount;
        for (int i = 0; i < size; i++) {
            cities.emplace_back(City());
        }
    }

    static vector<City> getCities() {
        return cities;
    }

    static int getCitiesCount() {
        return size;
    }
};

vector<City> Cities::cities;
int Cities::size = 0;

// Individual:
class Path {
private:
    vector<City> path;
    double cost; // Fitness function

    void generatePath() {
        path = std::move(Cities::getCities());
        std::random_shuffle(path.begin(), path.end());
    }

    // Euclidean Distance
    void evaluatePathCost() {
        for (int i = 0; i < path.size() - 1; i++) {
            cost += sqrt(pow((path[i].getX() - path[i + 1].getX()), 2) + pow((path[i].getY() - path[i + 1].getY()), 2));
        }
    }

public:
    explicit Path() : path(Cities::getCitiesCount()), cost(0.0) {
        generatePath();
        evaluatePathCost();
    }

    vector<City> getPath() const {
        return path;
    }

    double getCost() const {
        return cost;
    }

    friend std::ostream &operator<<(std::ostream &os, const Path &p) {
        vector<City> path = p.getPath();
        os << "[";
        for (int i = 0; i < path.size(); i++) {
            os << path[i];
            if (i != path.size() - 1) {
                os << ", ";
            }
        }
        os << "]";
        os << ": " << p.getCost();
        return os;
    }
};

// Collection of Individuals:
class Population {
private:
    vector<Path> population;

    void generatePopulation(int populationCount) {
        for (int i = 0; i < populationCount; i++) {
            population[i] = Path();
        }
    }

public:
    explicit Population(int populationCount) : population(populationCount) {
        generatePopulation(populationCount);
    }

    vector<Path> getPopulation() const {
        return population;
    }

    friend std::ostream &operator<<(std::ostream &os, const Population &p) {
        os << "[";
        for (int i = 0; i < p.getPopulation().size(); i++) {
            os << p.getPopulation()[i];
            if (i != p.getPopulation().size() - 1) {
                os << ", \n";
            }
        }
        os << "]";
        return os;
    }
};


int main() {
    srand(time(nullptr));
    Cities::generateCities(5);
    Population p = Population(5);
    cout << p;
    return 0;
}
