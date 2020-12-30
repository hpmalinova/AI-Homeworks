#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>    /* std::shuffle */
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

using std::string;
using std::vector;
using std::cout;

//Class: no-recurrence-events, recurrence-events

const string RECURRENCE = "recurrence-events";
const string NO_RECURRENCE = "no-recurrence-events";
const string MISSING_VALUE = "?";
const short NUMBER_OF_SETS = 11;
const int K = 10;

const std::unordered_map<string, vector<string>> ATTRIBUTE_TO_VALUES{{"age",        {"10-19",   "20-29",    "30-39",    "40-49",     "50-59",   "60-69", "70-79", "80-89", "90-99"}},
                                                               {"menopause",  {"lt40",    "ge40",     "premeno"}},
                                                               {"tumorSize",  {"0-4",     "5-9",      "10-14",    "15-19",     "20-24",   "25-29", "30-34", "35-39", "40-44",
                                                                                                                                                                              "45-49", "50-54", "55-59"}},
                                                               {"invNodes",   {"0-2",     "3-5",      "6-8",      "9-11",      "12-14",   "15-17", "18-20", "21-23", "24-26",
                                                                                                                                                                              "27-29", "30-32", "33-35", "36-39"}},
                                                               {"nodeCaps",   {"yes",     "no", MISSING_VALUE}},
                                                               {"degMalig",   {"1",       "2",        "3"}},
                                                               {"breast",     {"left",    "right"}},
                                                               {"breastQuad", {"left_up", "left_low", "right_up", "right_low", "central", MISSING_VALUE}},
                                                               {"irradiat",   {"yes",     "no"}}};

std::unordered_map<string, short> ATTRIBUTE_TO_POSITION{{"age",        1},
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
        if (recurrenceCount == 0 || noRecurrenceCount == 0) {
            return 0;
        }
        double recurrenceProb = (double) recurrenceCount / (recurrenceCount + noRecurrenceCount);
        double noRecurrenceProb = (double) noRecurrenceCount / (recurrenceCount + noRecurrenceCount);
        return ((-(recurrenceProb * log2(recurrenceProb))) - (noRecurrenceProb * log2(noRecurrenceProb)));
    }

    [[nodiscard]] int getCount() const { return recurrenceCount + noRecurrenceCount; }
};

// == Age, Menopause, ..
class Attribute {
private:
    string attributeName;
    // ValueName -> Count of Recurrence/No Recurrence
    // Ex: "10-19" -> {5 Recurrence, 3 NoRecurrence}
    std::unordered_map<string, AnswerCount> data;
public:
    explicit Attribute(const string& attributeName) : attributeName(attributeName) {
        vector<string> values = ATTRIBUTE_TO_VALUES.at(attributeName);
        for (auto &value : values) {
            data.insert({value, AnswerCount()});
        }
    }

    double getEntropy() {
        double entropy = 0;
        // Ex: value.first == "10-19", "20-29", ..
        for (auto &value: data) {
            double probability = getProbability(value.first);
            double entropy2 = value.second.getEntropy();
            entropy += probability * entropy2;
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
        string toAdd = info[ATTRIBUTE_TO_POSITION[attributeName]]; // "10-19"
        data[toAdd].add(info[0]); // Ex: info[0] == Recurrence/ NoRecurrence
    }

    const string &getAttributeName() const {
        return attributeName;
    }
};

class DecisionTreeNode {
private:
    vector<Attribute> attributes;
    DecisionTreeNode *parent;
    std::unordered_map<string, DecisionTreeNode *> children;
    vector<string> withoutAttributes; // TODO ??? exist
    int recurrenceCount;
    int allAnswers;
    string bestAttribute;

    bool contains(const string &attribute) { // TODO RENAME
        for (auto &without : withoutAttributes) {
            if (without == attribute) {
                return true;
            }
        }
        return false;
    }

public:
    explicit DecisionTreeNode(vector<string> withoutAttributes = {},
                              DecisionTreeNode *parent = nullptr) : parent(parent), recurrenceCount(0), allAnswers(0),
                                                                    withoutAttributes(std::move(withoutAttributes)) {
    }

    DecisionTreeNode *getChild(const string &value) {
        return children.at(value);
    }

    void processData(const vector<vector<string>> &info) {
        for (auto &attribute : ATTRIBUTE_TO_VALUES) {
            if (!contains(attribute.first)) {
                attributes.emplace_back(attribute.first);
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

    vector<vector<string>> filterInfo(const string &value, vector<vector<string>> data) {
        vector<vector<string>> filteredInfo;

        for (auto &line : data) {
            if (line[ATTRIBUTE_TO_POSITION.at(bestAttribute)]  == value) {
                filteredInfo.push_back(line);
            }
        }
        return filteredInfo;
    }

    string calculateBestAttribute() {
        double entropyOfTarget = getEntropyOfTarget();
        double maxGain = INT_MIN;
        string attributeWithMaxGain;
        double gain;
        for (auto &attribute : attributes) {
            gain = entropyOfTarget - attribute.getEntropy();
            if (gain > maxGain) {
                maxGain = gain;
                attributeWithMaxGain = attribute.getAttributeName();
            }
        }
        return attributeWithMaxGain;
    }

    [[nodiscard]] double getEntropyOfTarget() const {
        double recurrenceProbability = (double) recurrenceCount / allAnswers;
        double noRecurrenceProbability = 1.0 - recurrenceProbability;
        if (recurrenceProbability == 0 or noRecurrenceProbability == 0) {
            return 0;
        } else {
            return ((-(recurrenceProbability * log2(recurrenceProbability))) - (noRecurrenceProbability * log2(noRecurrenceProbability)));
        }
    }

    string getClassWithMoreAnswers() const {
        if (2 * recurrenceCount > allAnswers) {
            return RECURRENCE;
        } else if (2 * recurrenceCount == allAnswers) {
            return parent->getClassWithMoreAnswers();
        } else {
            return NO_RECURRENCE;
        }
    }

    [[nodiscard]] bool isLeafNode() const {
        return (recurrenceCount == 0 || recurrenceCount == allAnswers || allAnswers < K);
    }

    string getTargetClass() const {
        if (recurrenceCount == 0) {
            return NO_RECURRENCE;
        } else if (recurrenceCount == allAnswers) {
            return RECURRENCE;
        } else {
            return getClassWithMoreAnswers();
        }
    }

    void setBestAttribute(const string &attribute) {
        bestAttribute = attribute;
    }

    vector<string> getValues() {
        return ATTRIBUTE_TO_VALUES.at(bestAttribute);
    }

    const string &getBestAttribute() const {
        return bestAttribute;
    }
};

static vector<string> getFileLines(const string &fileName) {
    string line;
    vector<string> fileLines;
    std::ifstream myFile(fileName);
    if (myFile.is_open()) {
        while (getline(myFile, line)) {
            fileLines.push_back(line);
        }
        myFile.close();
    } else
        cout << "Unable to open file " << fileName << "\n";
    std::random_shuffle(fileLines.begin(), fileLines.end());
    return fileLines;
}

static vector<vector<string>> getDataInTokens(vector<string> someData) {
    string delimiter = ",";
    int position;
    vector<vector<string>> dataInTokens;

    for (auto &line : someData) {
        vector<string> token;
        while ((position = line.find(delimiter)) != std::string::npos) {
            token.push_back(line.substr(0, position));
            line.erase(0, position + 1);
        }
        token.push_back(line.substr(0, position));
        dataInTokens.push_back(token);
    }

    return dataInTokens;
}


class ID3 {
private:
    string fileName;
public:
    explicit ID3(string fileName) : fileName(std::move(fileName)) {}

    void buildTree(DecisionTreeNode *currentNode, const vector<vector<string>>& data) {
        currentNode->processData(data);

        if (!currentNode->isLeafNode()) {
        string bestAttribute = currentNode->calculateBestAttribute();
        currentNode->setBestAttribute(bestAttribute);
        vector<string> allValues = currentNode->getValues();
            for (auto &value : allValues) {
                DecisionTreeNode *child = currentNode->createChild(value);
                vector<vector<string>> filteredData = currentNode->filterInfo(value, data);
                buildTree(child, filteredData);
            }
        }
    }

    string testSet(vector<string> line, DecisionTreeNode *currentNode) {
        if (currentNode->isLeafNode()) {
            return currentNode->getTargetClass();
        } else {
            string attribute = currentNode->getBestAttribute(); // 'age'
            string value = line[ATTRIBUTE_TO_POSITION[attribute]]; // '60-69'
            return testSet(line, currentNode->getChild(value));
        }
    }

    double getAccuracyOfValidationSet(vector<vector<string>> validationSet, DecisionTreeNode *root) {
        int correctAnswers = 0;
        int allAnswers = 0;

        for (auto &line : validationSet) { // line: ["no-recurrence-events", "60-69", "ge40", ..]
            string result = testSet(line, root);
            if (line[0] == result) {
                correctAnswers++;
            }
            allAnswers++;
        }
        return (double) correctAnswers / (double) allAnswers; // Accuracy
    }

    void crossValidation() {
        double accuracy;
        double allAccuracies = 0;
        vector<string> fileLines = getFileLines(fileName);

        int linesSize = fileLines.size();
        int step = linesSize / NUMBER_OF_SETS;

        for (int toValidate = 0; toValidate < linesSize; toValidate += step) {
            vector<string> trainingSet(fileLines.begin(), fileLines.begin() + toValidate);
            vector<string> validationSet(fileLines.begin() + toValidate, fileLines.begin() + toValidate + step);
            vector<string> endOfTrainingSets(fileLines.begin() + step + toValidate, fileLines.end());
            trainingSet.insert(trainingSet.end(), endOfTrainingSets.begin(), endOfTrainingSets.end());

            vector<vector<string>> trainingDataInTokens = getDataInTokens(trainingSet);
            auto *root = new DecisionTreeNode();
            buildTree(root, trainingDataInTokens);
            vector<vector<string>> validationDataInTokens = getDataInTokens(validationSet);
            accuracy = getAccuracyOfValidationSet(validationDataInTokens, root);
            allAccuracies += accuracy;
            cout << "Accuracy: " << accuracy << "\n";
        }
        cout << "\nAverage accuracy: " << (double) allAccuracies / NUMBER_OF_SETS << "\n";
    }
};


int main() {
    srand(time(nullptr));

    string fileName = "../breast-cancer.data";
    ID3 tree = ID3(fileName);
    tree.crossValidation();

    return 0;
};
