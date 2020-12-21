#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

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

const short NUMBER_OF_SETS = 10;
const std::unordered_map<short, string> sets{{1,  "../votes1.txt"},
                                             {2,  "../votes2.txt"},
                                             {3,  "../votes3.txt"},
                                             {4,  "../votes4.txt"},
                                             {5,  "../votes5.txt"},
                                             {6,  "../votes6.txt"},
                                             {7,  "../votes7.txt"},
                                             {8,  "../votes8.txt"},
                                             {9,  "../votes9.txt"},
                                             {10, "../votes10.txt"}};

const short NUMBER_OF_ATTRIBUTES = 16;

const short int INVALID = -1;


// One for each Attribute and for each Class
class AnswerCount {
private:
    int yeaCount;
    int nayCount;

public:
    AnswerCount() : yeaCount(0), nayCount(0) {}

    void addOneVote(char answer) {
        if (answer == YEA) {
            yeaCount++;
        }
        else if (answer == NAY) {
            nayCount++;
        }
    }

    [[nodiscard]] double getProbability(char answer) const {
        if (answer == YEA) {
            return (double) yeaCount / (yeaCount + nayCount);
        } else if (answer == NAY) {
            return (double) nayCount / (yeaCount + nayCount);
        }
        return -1;
    }

    [[nodiscard]] int getVotesCount(char answer) const {
        if (answer == YEA) {
            return yeaCount;
        }
        else if (answer == NAY) {
            return nayCount;
        }
        return INVALID;
    }

    void clearAnswers() {
        yeaCount = 0;
        nayCount = 0;
    }
};

class DataSet {
private:
    // Attribute : yes/no count
    std::unordered_map<short, AnswerCount> votes;

public:
    explicit DataSet() { // TODO votes(16)
        for (int i = 1; i <= NUMBER_OF_ATTRIBUTES; i++) {
            votes.insert({i, AnswerCount()});
        }
    }

    void addVote(short attribute, char answer) {
        votes[attribute].addOneVote(answer);
    }

    int getVotesCount(short attribute, char answer){
        return votes[attribute].getVotesCount(answer);
    }

    double getProbability(short attribute, char answer) {
        if (answer == YEA) {
            return votes[attribute].getProbability(YEA);
        } else if (answer == NAY) {
            return votes[attribute].getProbability(NAY);
        }
        return -1;
    }

    void clearVotes() {
        for (short i = 1; i<= NUMBER_OF_ATTRIBUTES;i++) {
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
    void readTrainingSets(const vector<string> &trainingSets) {
        string line;
        for (auto &set : trainingSets) {
            std::ifstream myfile(set);
            short attr = 1;
            if (myfile.is_open()) {
                while (getline(myfile, line)) // "republican,n,y,n,y,y,y,n,n,n,y,n,y,y,y,n,y"
                {
                    if (line[0] == REPUBLICAN) {
                        for (int i = REPUBLICAN_FIRST_ATTRIBUTE; i <= REPUBLICAN_LAST_ATTRIBUTE; i = i + 2) {
                            republicansVotes.addVote(attr++, line[i]);
                        }
                        republicansCount++;
                    } else if (line[0] == DEMOCRAT) {
                        for (int i = DEMOCRAT_FIRST_ATTRIBUTE; i <= DEMOCRAT_LAST_ATTRIBUTE; i = i + 2) {
                            democratsVotes.addVote(attr++, line[i]);
                        }
                        democratsCount++;
                    }
                    attr = 1;
                }
                myfile.close();
            } else
                cout << "Unable to open file " << set << "\n";
        }
    }

    double getAccuracyOfValidationSet(const string &validationSet) {
        string line;
        std::ifstream myfile(validationSet);

        double democratsProbability = (double) democratsCount / (democratsCount + republicansCount);
        double republicansProbability = (double) republicansCount / (democratsCount + republicansCount);

        short attr;

        int correctAnswers = 0;
        int allAnswers = 0;

        if (myfile.is_open()) {
            while (getline(myfile, line)) // "republican,n,y,n,y,y,y,n,n,n,y,n,y,y,y,n,y"
            {
                attr = 1;

                if (line[0] == REPUBLICAN) {
                    for (int i = REPUBLICAN_FIRST_ATTRIBUTE; i <= REPUBLICAN_LAST_ATTRIBUTE; i = i + 2) {
                        republicansProbability *= republicansVotes.getProbability(attr, line[i]);
                        democratsProbability *= democratsVotes.getProbability(attr, line[i]);
                        attr++;
                    }
                } else if (line[0] == DEMOCRAT) {
                    for (int i = DEMOCRAT_FIRST_ATTRIBUTE; i <= DEMOCRAT_LAST_ATTRIBUTE; i = i + 2) {
                        republicansProbability *= republicansVotes.getProbability(attr, line[i]);
                        democratsProbability *= democratsVotes.getProbability(attr, line[i]);
                        attr++;
                    }
                }

                if ((republicansProbability > democratsProbability && line[0] == REPUBLICAN) ||
                    (democratsProbability > republicansProbability && line[0] == DEMOCRAT)) {
                    correctAnswers++;
                }
                allAnswers++;
            }
            myfile.close();
        } else
            cout << "Unable to open file " << validationSet << "\n";

        return (double) correctAnswers / allAnswers;
    }

    NaiveBayesClassifier() : democratsCount(0), republicansCount(0) { // TODO
        democratsVotes = DataSet();
        republicansVotes = DataSet();
    }

    void crossValidation() { // TODO add shuffle
        double allAccuracies = 0;
        double accuracy;

        for (int toValidate = 1; toValidate <= NUMBER_OF_SETS; toValidate++) {
            string validationSet;
            vector<string> trainingSets;

            for (int i = 1; i <= NUMBER_OF_SETS; i++) {
                if (i != toValidate) {
                    trainingSets.push_back(sets.at(i));
                } else {
                    validationSet = sets.at(i);
                }
            }
            readTrainingSets(trainingSets);
            accuracy = getAccuracyOfValidationSet(validationSet);
            allAccuracies += accuracy;
            cout << "Accuracy: " << accuracy << "\n";
            clearVotes();
        }
        cout << "Average accuracy: " << allAccuracies / NUMBER_OF_SETS << "\n";
    }

    void clearVotes() {
        democratsCount = 0;
        republicansCount = 0;
        democratsVotes.clearVotes();
        republicansVotes.clearVotes();
    }
};

int main() {
    NaiveBayesClassifier nbc = NaiveBayesClassifier();
    nbc.crossValidation();
    return 0;
}
