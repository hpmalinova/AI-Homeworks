#include <iostream>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
#include <vector>
#include <algorithm>    /* std::shuffle */
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
    explicit City(int start = 0, int end = 999, const std::string &s = "Generate") {
        if (s == "Generate") {
            generateCoordinates(start, end);
        } else {
            x = -1;
            y = -1;
        }
    }

    int getX() const { return x; }

    int getY() const { return y; }

    bool operator==(const City &other) const {
        return x == other.getX() and y == other.getY();
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
    double cost;

    void generatePath() {
        path = std::move(Cities::getCities());
        std::random_shuffle(path.begin(), path.end());
    }

    // The shortest path has the greatest value
    double getFitnessFunction() const {
        return 1 / cost;
    }

public:
    // Euclidean Distance
    void evaluatePathCost() {
        cost = 0.0;
        for (int i = 0; i < path.size() - 1; i++) {
            cost += sqrt(pow((path[i].getX() - path[i + 1].getX()), 2) + pow((path[i].getY() - path[i + 1].getY()), 2));
        }
    }

    explicit Path() : path(Cities::getCitiesCount()), cost(0.0) {
        generatePath();
        evaluatePathCost();
    }

    explicit Path(int parentSize) : path(parentSize, City(-1, -1, "Default-value")), cost(0.0) {}

    explicit Path(vector<City> nodes) : path(std::move(nodes)), cost(0.0) {}

    vector<City> getPath() const { return path; }

    double getCost() const { return cost; }

    City getCityAt(int i) { return path[i]; }

    void setCityAt(int i, City value) { path[i] = value; }

    bool hasCity(City c) {
        for (auto &city: path) {
            if (city == c)
                return true;
        }
        return false;
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

    bool operator<(const Path &otherPath) const {
        return getFitnessFunction() < otherPath.getFitnessFunction();
    }
};

// Collection of Individuals:
class Population {
private:
    vector<Path> population;
    int populationCount;
    int size;

    void generatePopulation() {
        for (int i = 0; i < populationCount; i++) {
            population[i] = Path();
        }
    }

    vector<Path> selectRandomParents(int N) {
        vector<Path> paths = vector<Path>(population.begin(), population.end() - N);
        std::random_shuffle(paths.begin(), paths.end());
        return vector<Path>(paths.begin(), paths.begin() + N);
    }

    static int getNext(int current) {
        int endOfVector = Cities::getCitiesCount() - 1;

        if (current < endOfVector)
            return ++current;
        else
            return 0;
    }

    static void completeNonFixedPartOfChild(Path &child, Path parent, int end) {
        City parentCity;

        int ch = getNext(end);
        int p = getNext(end);

        while (child.hasCity(City(-1, -1, "Default-value"))) {
            parentCity = parent.getCityAt(p);
            if (!child.hasCity(parentCity)) {
                child.setCityAt(ch, parentCity);
                ch = getNext(ch);
                p = getNext(p);
            } else {
                p = getNext(p);
            }
        }
    }


public:
    explicit Population(int populationCount) : population(populationCount), populationCount(populationCount) {
        generatePopulation();
        size = population.size();
        evaluate();
        sortByFitness();
    }

    explicit Population(vector<Path> paths, int populationCount) : populationCount(populationCount) {
        population = std::move(paths);
        size = population.size();
        evaluate();
        sortByFitness();
    }

    vector<Path> selectBestParents(int N) {
        return vector<Path>(population.end() - N, population.end());
    }

    vector<Path> getPopulation() const { return population; }

    int getPopulationSize() const { return size; }

    Path getBestIndividual() const { return population[size - 1]; }

    void sortByFitness() {
        std::sort(population.begin(), population.end());
    }

    Population selectParents(int N) {
        vector<Path> bestParents = selectBestParents(N / 6);
        vector<Path> bestParents2 = selectBestParents(N / 2);
        vector<Path> randomParents = selectRandomParents(N / 3);

        bestParents.reserve(bestParents.size() + bestParents2.size() + randomParents.size());
        bestParents.insert(bestParents.end(), bestParents2.begin(), bestParents2.end());
        bestParents.insert(bestParents.end(), randomParents.begin(), randomParents.end());
        return Population(bestParents, populationCount);
    }

    // Two-point
    Population crossover(const Population &parentsPopulation) const {
        vector<Path> parents = parentsPopulation.getPopulation();
        int parentsSize = parents.size();
        std::random_shuffle(parents.begin(), parents.end());
        vector<Path> children;

        int citiesCount = Cities::getCitiesCount();

        Path child1 = Path(citiesCount);
        Path child2 = Path(citiesCount);

        int start = (int) (citiesCount * 0.3);
        int end = (int) (citiesCount * 0.7);

        for (int i = 0; i < parentsSize - 1; i++) {
            for (int a = start; a <= end; a++) {
                child1.setCityAt(a, parents[i].getCityAt(a));
                child2.setCityAt(a, parents[i + 1].getCityAt(a));
            }
            completeNonFixedPartOfChild(child1, parents[i + 1], end);
            completeNonFixedPartOfChild(child2, parents[i], end);

            children.push_back(child1);
            children.push_back(child2);

            child1 = Path(citiesCount);
            child2 = Path(citiesCount);
        }
        return Population(children, populationCount);
    }

    void mutate(double probabilityForMutation) {
        int swaps = (int) (probabilityForMutation * size);
        for (int s = 0; s < swaps; s++) {
            // Random child -> random i->j mutation
            int pathToSwap = rand() % size;

            int i = rand() % Cities::getCitiesCount();
            int j = rand() % Cities::getCitiesCount();
            while (j == i) {
                j = rand() % Cities::getCitiesCount();
            }

            City temp = population[pathToSwap].getCityAt(i);
            population[pathToSwap].setCityAt(i, population[pathToSwap].getCityAt(j));
            population[pathToSwap].setCityAt(j, temp);
        }
    }

    void evaluate() {
        for (auto &path: population) {
            path.evaluatePathCost();
        }
    }

    void eraseTheWorstIndividuals() {
        int eraseNumber = size - populationCount;
        population.erase(population.begin(), population.begin() + eraseNumber);
        size -= eraseNumber;
    }

    friend std::ostream &operator<<(std::ostream &os, const Population &p) {
        os << "[";
        for (int i = 0; i < p.getPopulation().size(); i++) {
            os << p.getPopulation()[i];
            if (i != p.getPopulationSize() - 1) {
                os << ", \n";
            }
        }
        os << "]";
        return os;
    }
};

// А salesman is given a set of cities,
// he has to find the shortest route to visit each city exactly once
class TravelingSalesman {
private:
    int populationCount;
    double probabilityForMutation;
    double parentPercent;
    int breakNumber;

public:
    explicit TravelingSalesman(int citiesCount = 50, double parentPercent = 0.31,
                               double probabilityForMutation = 0.1, int breakNumber = 50) : parentPercent(
            parentPercent), probabilityForMutation(probabilityForMutation), breakNumber(breakNumber) {
        Cities::generateCities(citiesCount);
        populationCount = citiesCount * 5;
    }

    Population buildNextGenerationFrom(const Population &children, const Population &population) const {
        vector<Path> newGeneration = children.getPopulation();
        vector<Path> pop = population.getPopulation();
        newGeneration.insert(newGeneration.end(), pop.begin(), pop.end());

        Population newPopulation = Population(newGeneration, populationCount);
        newPopulation.sortByFitness();
        newPopulation.eraseTheWorstIndividuals();
        return newPopulation;
    }

    void findShortestPath() {
        int generations = 0;
        // Initialize Population: (+ evaluation and sorting)
        Population population = Population(populationCount);
        cout << "\n\nGeneration: " << generations << ", ";
        cout << "Best cost: " << population.getBestIndividual().getCost() << "\n\n";

        int seq = 0;
        double minCost = population.getBestIndividual().getCost();

        while (true) {
            generations++;
            int parentsCount = (int) (parentPercent * populationCount);
            Population parents = population.selectParents(parentsCount);

            Population children = population.crossover(parents);

            children.mutate(probabilityForMutation);
            children.evaluate();

            population = buildNextGenerationFrom(children, population);

            ///////////////////////////////////////////////////////////

            double cost = population.getBestIndividual().getCost();

            if (cost == minCost)
                seq++;
            else {
                seq = 0;
                minCost = cost;
            }

            if (seq != 0 and seq % breakNumber == 0 and probabilityForMutation < 0.3) {
//                cout << "\n** More mutation in generation: " << generations << " **\n\n";
                seq = 0;
                probabilityForMutation += 0.05;
            }

            if (seq > breakNumber)
                break;

            if (generations % 200 == 0) {
                cout << "Generation: " << generations << ", ";
                cout << "Best cost: " << cost << "\n";
            }
        }
        cout << "\nFinal generation: " << generations << ", ";
        cout << "Best cost: " << population.getBestIndividual().getCost() << "\n";
    }
};

int main() {
    srand(time(nullptr));
    TravelingSalesman ts = TravelingSalesman(100);
    ts.findShortestPath();
    return 0;
}