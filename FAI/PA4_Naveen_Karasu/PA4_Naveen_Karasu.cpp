#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <cstdlib>

using namespace std;

// Data structure to store each example's attributes and user action
struct Example {
    string author;
    string thread;
    string length;
    string where_read;
    string user_action;
};

/**
 * @brief Loads data from a CSV file
 * 
 * Reads a CSV file where each line represents an example with the following
 * columns: Author, Thread, Length, Where_read, and User_action.
 * These columns correspond to characteristics of each example.
 * 
 * @param filename Name of the CSV file to load data from.
 * @return vector<Example> The parsed dataset, where each Example contains
 *         values for the above-mentioned columns.
 * 
 * Example CSV structure:
 * Author, Thread, Length, Where_read, User_action
 * known, new, long, home, skips
 * unknown, new, short, work, reads
 */
vector<Example> load_data(const string &filename) {
    vector<Example> data;
    ifstream file(filename);
    string line, word;

    // Skip header line
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        Example example;
        getline(ss, example.author, ',');
        getline(ss, example.thread, ',');
        getline(ss, example.length, ',');
        getline(ss, example.where_read, ',');
        getline(ss, example.user_action, ',');
        data.push_back(example);
    }
    return data;
}

/**
 * @brief Splits the dataset into training and testing sets
 * 
 * This function divides the dataset into two parts: training and testing sets,
 * based on a specified ratio. We create multiple sets with different ratios
 * (80:20, 70:30, 50:50) to evaluate the model's performance on different data splits.
 * 
 * Why different splits?
 * - Using multiple train-test splits allows us to understand how the model performs
 *   with different amounts of training data.
 * - The more training data, the more accurate the model might be, but it’s good to
 *   evaluate different scenarios.
 * 
 * @param data Original dataset to split.
 * @param train Vector to store the training set.
 * @param test Vector to store the testing set.
 * @param train_ratio Ratio of training data to total dataset.
 */
void split_data(const vector<Example>& data, vector<Example>& train, vector<Example>& test, float train_ratio) {
    int train_size = static_cast<int>(data.size() * train_ratio);
    train.assign(data.begin(), data.begin() + train_size);
    test.assign(data.begin() + train_size, data.end());
}

// Node structure representing a decision tree node
struct TreeNode {
    string feature;
    map<string, TreeNode*> children;
    string label;
};

/**
 * @brief Calculates entropy of a dataset
 * 
 * Entropy is a measure of the disorder or impurity in the data.
 * For a dataset, entropy is calculated based on the distribution of
 * labels (user actions in this case).
 * 
 * Formula: Entropy = -Σ (p * log2(p))
 * where p is the probability of each label in the data.
 * 
 * Example Calculation:
 * - Suppose the dataset has 10 examples, 6 labeled "reads" and 4 labeled "skips".
 *   - Probability of "reads" = 6/10 = 0.6
 *   - Probability of "skips" = 4/10 = 0.4
 *   - Entropy = -(0.6 * log2(0.6) + 0.4 * log2(0.4)) ≈ 0.971
 * 
 * Use in Decision Tree:
 * - Entropy helps us determine the best feature to split the data. Lower entropy
 *   means a feature creates "purer" subsets.
 * 
 * @param data The dataset for which entropy is calculated.
 * @return double Entropy value.
 */
double calculate_entropy(const vector<Example>& data) {
    map<string, int> label_counts;
    for (const auto& ex : data) {
        label_counts[ex.user_action]++;
    }
    double entropy = 0.0;
    for (const auto& pair : label_counts) {
        double p = static_cast<double>(pair.second) / data.size();
        entropy -= p * log2(p);
    }
    return entropy;
}

/**
 * @brief Trains a decision tree based on the dataset
 * 
 * This function builds a decision tree by recursively splitting the data
 * using features that minimize entropy (i.e., create the "purest" subsets).
 * 
 * Steps:
 * 1. If all examples have the same label (e.g., all "reads"), create a leaf node
 *    with that label and stop.
 * 2. If there are mixed labels, calculate entropy for each feature to find the
 *    best split. The best feature is the one that gives the highest information gain.
 * 3. Divide data based on this feature and recursively build child nodes.
 * 
 * Example:
 * Let's assume we have a small dataset with 4 examples:
 * - {Author: "known", Thread: "new", Length: "short", User_action: "reads"}
 * - {Author: "unknown", Thread: "new", Length: "long", User_action: "skips"}
 * - {Author: "known", Thread: "followup", Length: "short", User_action: "reads"}
 * - {Author: "unknown", Thread: "followup", Length: "long", User_action: "skips"}
 * 
 * If splitting by "Author" reduces entropy the most, we split on "Author" and
 * create branches for "known" and "unknown". Each branch is further split
 * based on remaining features.
 * 
 * @param data The training dataset.
 * @param features List of features to consider for splits.
 * @return TreeNode* Pointer to the root node of the trained tree.
 */
TreeNode* train_decision_tree(const vector<Example>& data, const vector<string>& features) {
    if (data.empty()) return nullptr;

    // Check if all labels are the same
    map<string, int> label_counts;
    for (const auto& ex : data) {
        label_counts[ex.user_action]++;
    }
    if (label_counts.size() == 1) {
        TreeNode* leaf = new TreeNode;
        leaf->label = data[0].user_action;
        return leaf;
    }

    // Find the best feature for splitting
    double base_entropy = calculate_entropy(data);
    double best_gain = 0.0;
    string best_feature;
    map<string, vector<Example>> best_splits;

    for (const auto& feature : features) {
        map<string, vector<Example>> splits;
        for (const auto& ex : data) {
            string value = feature == "author" ? ex.author :
                           feature == "thread" ? ex.thread :
                           feature == "length" ? ex.length :
                           ex.where_read;
            splits[value].push_back(ex);
        }

        double new_entropy = 0.0;
        for (const auto& split : splits) {
            double weight = static_cast<double>(split.second.size()) / data.size();
            new_entropy += weight * calculate_entropy(split.second);
        }

        double gain = base_entropy - new_entropy;
        if (gain > best_gain) {
            best_gain = gain;
            best_feature = feature;
            best_splits = splits;
        }
    }

    // Stop if no gain in entropy
    if (best_gain == 0) {
        TreeNode* leaf = new TreeNode;
        leaf->label = data[0].user_action;
        return leaf;
    }

    TreeNode* node = new TreeNode;
    node->feature = best_feature;

    vector<string> remaining_features;
    for (const auto& feature : features) {
        if (feature != best_feature) remaining_features.push_back(feature);
    }

    for (const auto& split : best_splits) {
        node->children[split.first] = train_decision_tree(split.second, remaining_features);
    }

    return node;
}

/**
 * @brief Predicts the user action based on the trained tree
 * 
 * This function traverses the decision tree to predict the user action for a new example.
 * Starting from the root, it checks the example’s feature values and moves down the tree
 * until it reaches a leaf node, which holds the prediction.
 * 
 * @param root Pointer to the root of the decision tree.
 * @param example New example to classify.
 * @return string Predicted user action.
 */
string predict(TreeNode* root, const Example& example) {
    if (root->children.empty()) return root->label;
    string value = root->feature == "author" ? example.author :
                   root->feature == "thread" ? example.thread :
                   root->feature == "length" ? example.length :
                   example.where_read;
    if (root->children.count(value)) {
        return predict(root->children[value], example);
    } else {
        return "unknown"; // Handle unseen data
    }
}

/**
 * @brief Calculates accuracy of the model on the test dataset
 * 
 * This function compares the predicted labels to actual labels to measure
 * the model's performance on the test dataset.
 * 
 * Accuracy Formula: (Number of Correct Predictions / Total Predictions)
 * 
 * @param root Pointer to the root of the decision tree.
 * @param test_data Test dataset for evaluation.
 * @return double Accuracy of the model.
 */
double calculate_accuracy(TreeNode* root, const vector<Example>& test_data) {
    int correct = 0;
    for (const auto& ex : test_data) {
        if (predict(root, ex) == ex.user_action) {
            correct++;
        }
    }
    return static_cast<double>(correct) / test_data.size();
}

/**
 * @brief Main function to train, test, and evaluate the decision tree model
 */
int main() {
    string filename = "dataset.csv";  // The generated CSV file
    vector<Example> data = load_data(filename);
    
    // Define train-test split ratios
    vector<float> ratios = {0.8, 0.7, 0.5};

    for (float ratio : ratios) {
        vector<Example> train_data, test_data;
        split_data(data, train_data, test_data, ratio);

        // Train decision tree
        vector<string> features = {"author", "thread", "length", "where_read"};
        TreeNode* tree = train_decision_tree(train_data, features);

        // Calculate and display accuracy
        double accuracy = calculate_accuracy(tree, test_data);
        cout << "Accuracy for train:test ratio " << ratio << ":" << (1 - ratio) << " is " << accuracy << endl;

        // Predict user actions for examples e19 and e20
        Example e19 = {"unknown", "new", "long", "work", "?"};
        Example e20 = {"unknown", "followup", "short", "home", "?"};
        cout << "Prediction for e19: " << predict(tree, e19) << endl;
        cout << "Prediction for e20: " << predict(tree, e20) << endl;

           }
    
    return 0;
}
