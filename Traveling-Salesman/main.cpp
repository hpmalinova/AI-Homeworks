#include <iostream>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
#include <utility>
#include <vector>
#include <algorithm>    // std::shuffle
#include <cmath>       /* sqrt */
#include <string>

using std::cout;
using std::vector;
using std::string;

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
    explicit City(int start = 0, int end = 999, const string &s = "Generate") {
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

    explicit Path(int parentSize) : path(parentSize, City(-1, -1, "Default-value")), cost(0.0) {
    }

    explicit Path(vector<City> nodes): path(std::move(nodes)), cost(0.0) {}

    vector<City> getPath() const {
        return path;
    }

    double getCost() const {
        return cost;
    }

    City getCityAt(int i) {
        return path[i];
    }

    void setCityAt(int i, City value) {
        path[i] = value;
    }

    bool hasCity(City c) {
        for (auto &city: path) {
            if (city == c)
                return true;
        }
        return false;
    }

    void clear() {
        path.clear();
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

public:
    vector<Path> selectBestParents(int N) {
        return vector<Path>(population.end() - N, population.end());
    }

    explicit Population(int populationCount) : population(populationCount), populationCount(populationCount) {
        generatePopulation();
        size = population.size();
        evaluate();
        sortByFitness();
    }

    explicit Population(vector<Path> paths, int populationCount) : populationCount(populationCount){
        population = std::move(paths);
        size = population.size();
        evaluate();
        sortByFitness();
    }

    vector<Path> getPopulation() const {
        return population;
    }

    int getPopulationCount() const {
        return populationCount;
    }

    int getPopulationSize() const{
        return size;
    }

    Path getBestIndividual() const {
        return population[size - 1];
    }

    void sortByFitness() {
        std::sort(population.begin(), population.end());
    }

    Population selectParents(int N) {
        vector<Path> bestParents = selectBestParents(N / 6);
//        for (auto & bp:bestParents) {
//            cout << "\n\nbest parents1" << bp << "\n";
//        }
        vector<Path> bestParents2 = selectBestParents(N / 2);
//        for (auto & bp:bestParents2) {
//            cout << "\n\nbest parents2" << bp << "\n";
//        }
        vector<Path> randomParents = selectRandomParents(N / 3);
//        for (auto & bp:randomParents) {
//            cout << "\n\nrandom" << bp << "\n";
//        }

        bestParents.reserve(bestParents.size() + bestParents2.size() + randomParents.size());
        bestParents.insert(bestParents.end(), bestParents2.begin(), bestParents2.end());
        bestParents.insert(bestParents.end(), randomParents.begin(), randomParents.end());
//        cout << "\nBEST PARENTS IN FUNC" << "\n";
//        for (auto & bp:bestParents) {
//            cout  << bp << "\n";
//        }
        return Population(bestParents, populationCount);
    }

    static int getNext(int current) {
        int endOfVector = Cities::getCitiesCount() - 1;

        if (current < endOfVector)
            return ++current;
        if (current == endOfVector)
            return 0;
    }

    // Two-point
    Population crossover(Population parentsPopulation) {
        vector<Path> parents = parentsPopulation.getPopulation();
        int parentsSize = parents.size();
//        cout << "\n Parents Size: " << parentsSize << "\n";
        std::random_shuffle(parents.begin(), parents.end());
//        for (auto &p : parents) {
//            cout << p << "\n";
//        }
        vector<Path> children;

        int citiesCount = Cities::getCitiesCount();

        Path child1 = Path(citiesCount);
        Path child2 = Path(citiesCount);

        int start = (int) (citiesCount * 0.3);
        int end = (int) (citiesCount * 0.7);
//        cout << "start: " << start << ", end: " << end << "\n";

        for (int i = 0; i < parentsSize - 1; i++) {
            for (int a = start; a <= end; a++) {
                child1.setCityAt(a, parents[i].getCityAt(a));
                child2.setCityAt(a, parents[i + 1].getCityAt(a));
            }

            City parentCity;

            // Child 1:
            int ch1 = getNext(end);
            int p2 = getNext(end);
            while (child1.hasCity(City(-1, -1, "Default-value"))) {
                parentCity = parents[i + 1].getCityAt(p2);
                if (!child1.hasCity(parentCity)) {
                    child1.setCityAt(ch1, parentCity);
                    ch1 = getNext(ch1);
                    p2 = getNext(p2);
                } else {
                    p2 = getNext(p2);
                }
            }

            // Child 2:
            int ch2 = getNext(end);
            int p1 = getNext(end);
            while (child2.hasCity(City(-1, -1, "Default-value"))) {
                parentCity = parents[i].getCityAt(p1);
                if (!child2.hasCity(parentCity)) {
                    child2.setCityAt(ch2, parentCity);
                    ch2 = getNext(ch2);
                    p1 = getNext(p1);
                } else {
                    p1 = getNext(p1);
                }
            }

            children.push_back(child1);
            children.push_back(child2);

            child1 = Path(citiesCount);
            child2 = Path(citiesCount);
        }
        return Population(children, populationCount);
    }

    void mutate (double probabilityForMutation) {
        int swaps = (int) (probabilityForMutation * size);
        for (int s=0; s< swaps; s++) {
            // Random child -> random i->j mutation
            int pathToSwap = rand() % size;

            int i = rand() % Cities::getCitiesCount();
            int j = rand() % Cities::getCitiesCount();
            while (j==i) {
                j = rand() % Cities::getCitiesCount();
            }

            City temp = population[pathToSwap].getCityAt(i);
            population[pathToSwap].setCityAt(i, population[pathToSwap].getCityAt(j));
            population[pathToSwap].setCityAt(j, temp);
        }
//        iter_swap(v.begin() + position, v.begin() + next_position);

    }

    void evaluate() {
        for (auto& path: population) {
            path.evaluatePathCost();
        }
    }

    void eraseTheWorstIndividuals() {
//        population.erase(population.begin()+populationCount, population.end());
        int eraseNumber = population.size() - populationCount;
        population.erase(population.begin(), population.begin()+eraseNumber);
//        for (int i=startIndex; i<populationCount; i++) {
//
//        }
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

// А salesman is given a set of cities,
// he has to find the shortest route to visit each city exactly once
class TravelingSalesman {
private:
    int populationCount;
    double probabilityForMutation;
    double parentPercent;
public:
    explicit TravelingSalesman(int citiesCount = 50, double parentPercent = 0.31, double probabilityForMutation = 0.10) {
        Cities::generateCities(citiesCount);
        populationCount = citiesCount * 5;
        this->parentPercent = parentPercent;
        this->probabilityForMutation = probabilityForMutation;
    }

    Population buildNextGenerationFrom(Population children, Population population) {
//        cout << "In new gen\n";
        vector<Path> newGeneration = children.getPopulation();
        vector<Path> pop = population.getPopulation();
//        newGeneration.reserve(children.getPopulation().size() + population.getPopulation().size());
//        newGeneration.insert(newGeneration.end(), children.getPopulation().begin(), children.getPopulation().end());

//        newGeneration.insert(newGeneration.end(), population.getPopulation().begin(), population.getPopulation().end());
        newGeneration.insert(newGeneration.end(), pop.begin(), pop.end());

        Population newPopulation = Population(newGeneration, populationCount);
//        newPopulation.evaluate();
        newPopulation.sortByFitness();

        newPopulation.eraseTheWorstIndividuals();
//        cout << "\n AfterErase: " << newPopulation.getPopulation().size() << "\n";
        return newPopulation;
    }

    void findShortestPath() {
        /*
        int generations = 0;
        Population population = Population(populationCount);
        population.sortByFitness();
//        cout << population << "\n\n";
//        cout << "Population:" << population.getBestIndividual()  << " count:" << population.getPopulationCount() << " size:" << population.getPopulationSize();

        int parentsCount = (int) (parentPercent * populationCount);
        Population parents = population.selectParents(parentsCount);

//        cout << "\nPOPULATION" << population << "\n\n";
//        cout << parents << "\n";
//        cout << "\nParents:" << parents.getBestIndividual()  << " count:" << parents.getPopulationCount() << " size:" << parents.getPopulationSize();

        Population children = population.crossover(parents);
        children.mutate(probabilityForMutation);
//        children.evaluate();
//        cout << "\nchildren: " << children << "\n" << children.getPopulationSize()<<"\n";
//        cout << "\n\nBest child:" << children.getBestIndividual().getCost();

        Population newPopulation = buildNextGenerationFrom(children, population);
//        cout << "New BEST INDIVIDUAL:" << newPopulation.getBestIndividual().getCost();
//
//        cout << "\n\nNEW: " << newPopulation << "\n" << "Size: " << newPopulation.getPopulation().size();
//        cout << "\nBest path: " << newPopulation.getBestIndividual();
    */
        int generations = 0;
        // Initialize Population: (+ evaluation and sorting)
        cout << "Generation: " << generations << "\n";
        Population population = Population(populationCount);
        cout << "Best cost: " << population.getBestIndividual().getCost() << "\n";

        while (generations++ <= 1000) { // TODO

            // Pp: population.selectParents
            int parentsCount = (int) (parentPercent * populationCount);
            Population parents = population.selectParents(parentsCount);

            // Pc: crossover(Pp)
            Population children = population.crossover(parents);

            // mutate(Pc)
            children.mutate(probabilityForMutation);
            children.evaluate();

            // newPopulation = nextGeneration(Pc, population)
//            Population newPopulation = buildNextGenerationFrom(children, population);
            population = buildNextGenerationFrom(children, population);
            if (generations % 100 == 0) {
                cout << "Generation: " << generations << "\n";
                cout << "Best cost: " << population.getBestIndividual().getCost() << "\n";
            }


        }
    }

};

// В1: Ако при последните (10) генерации нямаме подобрение на най-добрия индивид, да спрем
// В2: Да му кажем точно колко генерации да направи
// Мутация на 5% // или при схождане - по-голям процент на мутация
// Популацията винаги е с една съща дължина - ако добавяме N нови деца, премахваме N-те най-слаби от предната итерация

int main() {
    srand(time(nullptr));
    TravelingSalesman ts = TravelingSalesman(100);
    ts.findShortestPath();
    return 0;

}
