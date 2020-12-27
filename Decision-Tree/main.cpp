#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
//#include "attributes.cpp"

using std::string;
using std::vector;
using std::cout;

//Class: no-recurrence-events, recurrence-events

static const string RECURRENCE = "recurrence-events";
static const string NO_RECURRENCE = "no-recurrence-events";
static const string TIE = "tie";
static const string MISSING_VALUE = "?";
static const char INVALID = -1;


static const string fileName = "../breast-cancer.data";
std::unordered_map<string, vector<string>> ATTRIBUTES{{"age",        {"10-19",   "20-29",    "30-39",    "40-49",     "50-59",   "60-69", "70-79", "80-89", "90-99", MISSING_VALUE}},
                                                      {"menopause",  {"lt40",    "ge40",     "premeno",  MISSING_VALUE}},
                                                      {"tumorSize",  {"0-4",     "5-9",      "10-14",    "15-19",     "20-24",   "25-29", "30-34", "35-39", "40-44",
                                                                                                                                                                     "45-49", "50-54", "55-59", MISSING_VALUE}},
                                                      {"invNodes",   {"0-2",     "3-5",      "6-8",      "9-11",      "12-14",   "15-17", "18-20", "21-23", "24-26",
                                                                                                                                                                     "27-29", "30-32", "33-35", "36-39", MISSING_VALUE}},
                                                      {"nodeCaps",   {"yes",     "no",       MISSING_VALUE}},
                                                      {"degMalig",   {"1",       "2",        "3",        MISSING_VALUE}},
                                                      {"breast",     {"left",    "right",    MISSING_VALUE}},
                                                      {"breastQuad", {"left-up", "left-low", "right-up", "right-low", "central", MISSING_VALUE}},
                                                      {"irradiat",   {"yes",     "no",       MISSING_VALUE}}};

std::unordered_map<string, short> ATTRIBUTES_POSITION{{"age",        1},
                                                      {"menopause",  2},
                                                      {"tumorSize",  3},
                                                      {"invNodes",   4},
                                                      {"nodeCaps",   5},
                                                      {"degMalig",   6},
                                                      {"breast",     7},
                                                      {"breastQuad", 8},
                                                      {"irradiat",   9}};

class AnswerCount {
private:
    int recurrenceCount;
    int noRecurrenceCount;
public:
    AnswerCount() : recurrenceCount(0), noRecurrenceCount(0) {}

    void add(const string &answer) {
        if (answer == RECURRENCE) {
            recurrenceCount++;
        } else if (answer == NO_RECURRENCE) {
            noRecurrenceCount++;
        }
    }

    [[nodiscard]] double getEntropy() const {
        double recurrenceProb = (double) recurrenceCount / (recurrenceCount + noRecurrenceCount);
        double noRecurrenceProb = (double) noRecurrenceCount / (recurrenceCount + noRecurrenceCount);
        return -recurrenceProb * log2(recurrenceProb) - noRecurrenceProb * log2(noRecurrenceProb);
    }

    [[nodiscard]] int getCount() const { return recurrenceCount + noRecurrenceCount; }

    void clearAnswers() {
        recurrenceCount = 0;
        noRecurrenceCount = 0;
    }
};

// == Age, Menopause, ..
class Attribute {
private:
    string attributeName;
    // ValueName -> Count of Recurrence/No Recurrence
    // Ex: "10-19" -> {5 Recurrence, 3 NoRecurrence}
    std::unordered_map<string, AnswerCount> data;
public:
    explicit Attribute(const string &attributeName) : attributeName(attributeName) {
        for (auto &value : ATTRIBUTES[attributeName]) {
            data.insert({value, AnswerCount()});
        }
    }

    double getEntropy() {
        double entropy = 0;
        // Ex: value.first == "10-19", "20-29", ..
        for (auto &value: data) {
            entropy += getProbability(value.first) * value.second.getEntropy();
        }
        return entropy;
    }

    int getAllCount() {
        int count = 0;
        for (auto &value : data) {
            count += value.second.getCount();
        }
        return count;
    }

    double getProbability(const string &valueName) {
        return (double) data[valueName].getCount() / getAllCount();
    }

    void fill(vector<string> info) {
        string toAdd = info[ATTRIBUTES_POSITION[attributeName]]; // "10-19"
        data[toAdd].add(info[0]); // Ex: info[0] == Recurrence/ NoRecurrence
    }

    void clearData() { // TODO
        data.clear();
    }

    const string &getAttributeName() const {
        return attributeName;
    }
};

class DecisionTreeNode {
private:
    vector<Attribute> attributes;
    DecisionTreeNode *parent;
//    vector<DecisionTreeNode *> children; // TODO node?
    std::unordered_map<string, DecisionTreeNode *> children;
    vector<string> withoutAttributes; // TODO ??? exist
    int recurrenceCount;
    int allAnswers;
    string bestAttribute;
    vector<vector<string>> info;

    bool contains(const string &attribute) { // TODO RENAME
        for (auto &without : withoutAttributes) {
            if (without == attribute) {
                return true;
            }
        }
        return false;
    }

public:
    // TODO check to see if constructor works :D
    explicit DecisionTreeNode(vector<string> withoutAttributes = {},
                              DecisionTreeNode *parent = nullptr) : parent(parent), recurrenceCount(0), allAnswers(0),
                                                                    info(info),
                                                                    withoutAttributes(std::move(withoutAttributes)) {
    }

    void processData(const vector<vector<string>> &info) {
        for (auto &attribute : ATTRIBUTES) {
            if (!contains(attribute.first)) {
                attributes.push_back(Attribute(attribute.first));
//                attributes.emplace_back(attribute.first); // TODO WHAT
            }
        }

        for (auto &attributeLine : info) {
            for (auto &attribute: attributes) {
                attribute.fill(attributeLine);
            }
            if (attributeLine[0] == RECURRENCE) {
                recurrenceCount++;
            }
            allAnswers++;
        }
    }

    DecisionTreeNode *createChild(const string &valueName) {
        vector<string> without = withoutAttributes;
        without.push_back(bestAttribute);

        auto *child = new DecisionTreeNode(without, this);
        children.insert({valueName, child});
        return child;
    }

    vector<vector<string>> filterInfo(const string &value) {
        vector<vector<string>> filteredInfo;

        for (auto &line : info) {
            if (line[ATTRIBUTES_POSITION[bestAttribute]] == value) {
                filteredInfo.push_back(line);
            }
        }
        return filteredInfo;
    }

    string getBestAttribute() {
        double entropyOfTarget = getEntropyOfTarget();
        double maxGain = INT_MIN;
        string attributeWithMaxGain;
        double gain;
        for (auto &attribute : attributes) {
            gain = entropyOfTarget - attribute.getEntropy();
            if (gain - maxGain > 0.000000001) {
                maxGain = gain;
                attributeWithMaxGain = attribute.getAttributeName();
            }
        }
        return attributeWithMaxGain;
    }

    [[nodiscard]] double getEntropyOfTarget() const {
        double recurrenceProbability = (double) recurrenceCount / allAnswers;
        double noRecurrenceProbability = 1.0 - recurrenceProbability;
        return -recurrenceProbability * log2(recurrenceProbability) -
               noRecurrenceProbability * log2(noRecurrenceProbability);
    }

    string getClassWithMoreAnswers() {
        if (2 * recurrenceCount > allAnswers) {
            return RECURRENCE;
        } else if (2 * recurrenceCount == allAnswers) {
            return parent->getClassWithMoreAnswers();
        } else {
            return NO_RECURRENCE;
        }
    }

    [[nodiscard]] bool isLeafNode() const {
        if (recurrenceCount == 0 || recurrenceCount == allAnswers || 2 * recurrenceCount == allAnswers) {
            return true;
        }
        return false;
    }

    void setBestAttribute(const string &attribute) {
        bestAttribute = attribute;
    }

    vector<string> getValues() {
        return ATTRIBUTES[bestAttribute];
    }
};

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
//    std::random_shuffle(fileLines.begin(), fileLines.end());
    return fileLines;
}

class ID3 {
private:
    DecisionTreeNode *root;
    vector<vector<string>> lines; // TODO RENAME
public:
    ID3(string fileName) {
        root = new DecisionTreeNode();
    }

    void readTrainingSets(vector<string> trainingSet) {
        string delimiter = ",";
        int position;

        // line: no-recurrence-events,30-39,premeno,30-34,0-2,no,3,left,left_low,no
        for (auto &line : trainingSet) {
            vector<string> token;
            while ((position = line.find(delimiter)) != std::string::npos) {
                token.push_back(line.substr(0, position)); // TODO
                line.erase(0, position + 1);
            }
            lines.push_back(token);
        }
    };

    void buildTree(DecisionTreeNode *currentNode, vector<vector<string>> data) {
        currentNode->processData(data);
        string bestAttribute = currentNode->getBestAttribute();
        currentNode->setBestAttribute(bestAttribute);
        vector<string> allValues = currentNode->getValues();
        for (auto& value : allValues){
            DecisionTreeNode* child = currentNode->createChild(value);
            vector<vector<string>> filteredData = currentNode->filterInfo(value);
            buildTree(child, filteredData);
        }
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
};


int main() {
//    Age pesho = Age();
//    pesho.add("10-19");
//    cout << pesho.getAll();


//    vector<string> trainingSet {"no-recurrence-events,60-69,ge40,20-24,0-2,no,1,left,left_low,no",
//            "no-recurrence-events,40-49,premeno,50-54,0-2,no,2,left,left_low,no"};
//    string delimiter = ",";
//
//    int position;
//    string token;
//
//    for (auto &line : trainingSet) {
//        while ((position = line.find(delimiter)) != std::string::npos) {
//            token = line.substr(0, position);
//            cout << token << "\n";
//            line.erase(0, position + 1);
//        }
//    }
    return 0;
};
