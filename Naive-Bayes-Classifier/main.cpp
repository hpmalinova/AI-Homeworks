#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>    /* std::shuffle */
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
#include <cmath>
#include <string>

using std::string;
using std::vector;
using std::cout;

// Class name:
const char DEMOCRAT = 'd';
const short DEMOCRAT_FIRST_ATTRIBUTE = 9; // Position of first answer in file
const short DEMOCRAT_LAST_ATTRIBUTE = 39; // Position of last answer in file

const char REPUBLICAN = 'r';
const short REPUBLICAN_FIRST_ATTRIBUTE = 11;
const short REPUBLICAN_LAST_ATTRIBUTE = 41;

const char YEA = 'y';
const char NAY = 'n';
const char MISSING_VALUE = '?';

const short NUMBER_OF_SETS = 10;
const string fileName = "../allVotes.data";

const short NUMBER_OF_ATTRIBUTES = 16;

const short int INVALID = -1;


static vector<string> getFileLines() {
    string line;
    vector<string> fileLines;
    std::ifstream myfile(fileName);
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            fileLines.push_back(line);
        }
        myfile.close();
    } else
        cout << "Unable to open file " << fileName << "\n";
    std::random_shuffle(fileLines.begin(), fileLines.end());
    return fileLines;
}

// One for each Attribute and for each Class
class AnswerCount {
private:
    int yeaCount;
    int nayCount;
    int missingCount;

public:
    AnswerCount() : yeaCount(1), nayCount(1), missingCount(1) {}

    void addOneVote(char answer) {
        if (answer == YEA) {
            yeaCount++;
        } else if (answer == NAY) {
            nayCount++;
        } else if (answer == MISSING_VALUE) {
            missingCount++;
        }
    }

    [[nodiscard]] double getProbability(char answer) const {
        if (answer == YEA) {
            return (double) yeaCount / (yeaCount + nayCount + missingCount - 3);
        } else if (answer == NAY) {
            return (double) nayCount / (yeaCount + nayCount + missingCount - 3);
        } else if (answer == MISSING_VALUE) {
            return (double) missingCount / (yeaCount + nayCount + missingCount - 3);
        }
        return INVALID;
    }

    void clearAnswers() {
        yeaCount = 1;
        nayCount = 1;
        missingCount = 1;
    }
};

class DataSet {
private:
    // Attribute : yes/no count
    std::unordered_map<short, AnswerCount> votes;

public:
    DataSet() {
        for (int i = 1; i <= NUMBER_OF_ATTRIBUTES; i++) {
            votes.insert({i, AnswerCount()});
        }
    }

    void addVote(short attribute, char answer) {
        votes[attribute].addOneVote(answer);
    }

    double getProbability(short attribute, char answer) {
        if (answer == YEA) {
            return votes[attribute].getProbability(YEA);
        } else if (answer == NAY) {
            return votes[attribute].getProbability(NAY);
        } else if (answer == MISSING_VALUE) {
            return votes[attribute].getProbability(MISSING_VALUE);
        }
        return INVALID;
    }

    void clearVotes() {
        for (short i = 1; i <= NUMBER_OF_ATTRIBUTES; i++) {
            votes.at(i).clearAnswers();
        }
    }
};

class NaiveBayesClassifier {
private:
    DataSet democratsVotes;
    DataSet republicansVotes;
    int democratsCount;
    int republicansCount;

public:
    void readTrainingSets(const vector<string> &trainingSet) {
        short attr;

        // "republican,n,y,n,y,y,y,n,n,n,y,n,y,y,y,n,y"
        for (auto &line : trainingSet) {
            attr = 1;

            if (line[0] == REPUBLICAN) {
                for (int i = REPUBLICAN_FIRST_ATTRIBUTE; i <= REPUBLICAN_LAST_ATTRIBUTE; i += 2) {
                    republicansVotes.addVote(attr, line[i]);
                    attr++;
                }
                republicansCount++;
            } else if (line[0] == DEMOCRAT) {
                for (int i = DEMOCRAT_FIRST_ATTRIBUTE; i <= DEMOCRAT_LAST_ATTRIBUTE; i += 2) {
                    democratsVotes.addVote(attr, line[i]);
                    attr++;
                }
                democratsCount++;
            }
        }
    }

    double getAccuracyOfValidationSet(const vector<string> &validationSet) {
        double democratsProbability = std::log((double) democratsCount / (double) (democratsCount + republicansCount));
        double republicansProbability = std::log(
                (double) republicansCount / (double) (democratsCount + republicansCount));

        int correctAnswers = 0;
        int allAnswers = 0;

        short attr;

        for (auto &line : validationSet) {
            attr = 1;

            int firstIndex = 0;
            int lastIndex = 0;

            if (line[0] == REPUBLICAN) {
                firstIndex = REPUBLICAN_FIRST_ATTRIBUTE;
                lastIndex = REPUBLICAN_LAST_ATTRIBUTE;
            } else if (line[0] == DEMOCRAT) {
                firstIndex = DEMOCRAT_FIRST_ATTRIBUTE;
                lastIndex = DEMOCRAT_LAST_ATTRIBUTE;
            }

            double beADemocratProbability = democratsProbability;
            double beARepublicanProbability = republicansProbability;

            for (int i = firstIndex; i <= lastIndex; i += 2) {
                beARepublicanProbability += std::log(republicansVotes.getProbability(attr, line[i]));
                beADemocratProbability += std::log(democratsVotes.getProbability(attr, line[i]));
                attr++;
            }

            if (((beARepublicanProbability - beADemocratProbability > 0.00000000000001) && line[0] == REPUBLICAN) ||
                ((beADemocratProbability - beARepublicanProbability > 0.00000000000001) && line[0] == DEMOCRAT)) {
                correctAnswers++;
            }
            allAnswers++;
        }

        return (double) correctAnswers / (double) allAnswers; // Accuracy
    }

    NaiveBayesClassifier() : democratsCount(0), republicansCount(0) {
        democratsVotes = DataSet();
        republicansVotes = DataSet();
    }

    void crossValidation() {
        double accuracy;
        double allAccuracies = 0;
        vector<string> fileLines = getFileLines();

        int linesSize = fileLines.size();
        int step = linesSize / NUMBER_OF_SETS;

        for (int toValidate = 0; toValidate < linesSize; toValidate += step) {
            vector<string> trainingSet(fileLines.begin(), fileLines.begin() + toValidate);
            vector<string> validationSet(fileLines.begin() + toValidate, fileLines.begin() + toValidate + step);
            vector<string> endOfTrainingSets(fileLines.begin() + step + toValidate, fileLines.end());
            trainingSet.insert(trainingSet.end(), endOfTrainingSets.begin(), endOfTrainingSets.end());

            readTrainingSets(trainingSet);
            accuracy = getAccuracyOfValidationSet(validationSet);
            allAccuracies += accuracy;
            cout << "Accuracy: " << accuracy << "\n";

            clearVotes();
        }
        cout << "\nAverage accuracy: " << allAccuracies / NUMBER_OF_SETS << "\n";
    }

    void clearVotes() {
        democratsCount = 0;
        republicansCount = 0;
        democratsVotes.clearVotes();
        republicansVotes.clearVotes();
    }
};

int main() {
    srand(time(nullptr));
    NaiveBayesClassifier nbc = NaiveBayesClassifier();
    cout << "\n";
    nbc.crossValidation();

    return 0;
}
