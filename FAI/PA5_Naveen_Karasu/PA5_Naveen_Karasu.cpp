#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

/**
 * @struct Example
 * @brief Stores each example's encoded features and label.
 *
 * This structure represents each individual data point in the dataset.
 * - `features`: A vector that holds the encoded values for each categorical feature.
 * - `label`: A binary outcome representing the user action (0 = "skips", 1 = "reads").
 * 
 * For example, a user who is "unknown", views a "new" thread of "long" length at "work"
 * could be represented as:
 * - features: {0.0, 1.0, 1.0, 0.0} (encoded values for categorical features)
 * - label: 1 (assume "reads")
 */
struct Example {
    vector<double> features; // Encoded values for input features
    int label;               // 0 for "skips" and 1 for "reads"
};

/**
 * @class NeuralNetwork
 * @brief Implements a neural network with a single hidden layer for binary classification.
 *
 * This class represents a simple feed-forward neural network with:
 * - An input layer (4 features)
 * - A single hidden layer (2 units)
 * - An output layer (1 output for binary classification)
 * 
 * The network uses sigmoid activation functions and trains with gradient descent.
 */
class NeuralNetwork {
private:
    // Member Variables
    double weightsInputHidden[4][2]; // Weights from input to hidden layer
    double weightsHiddenOutput[2];   // Weights from hidden to output layer
    double hiddenBias[2], outputBias; // Biases for hidden and output layers
    double learningRate;             // Step size for weight updates

    /**
     * @brief Sigmoid activation function
     * 
     * @param x Input value
     * @return Sigmoid of x, a value between 0 and 1
     * 
     * Example:
     * If `x` = 0, then `sigmoid(0)` = 0.5.
     */
    double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }

public:
    /**
     * @brief Constructor to initialize weights and biases.
     * 
     * Initializes weights and biases with small random values for symmetry breaking.
     * Uses a fixed seed for reproducibility and sets the learning rate.
     * 
     * @param lr Learning rate for training (e.g., 0.1)
     */
    NeuralNetwork(double lr) : learningRate(lr) {
        srand(42); // Fixed seed for reproducibility
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 2; ++j)
                weightsInputHidden[i][j] = (rand() / double(RAND_MAX)) * 0.1 - 0.05;

        for (int j = 0; j < 2; ++j)
            weightsHiddenOutput[j] = (rand() / double(RAND_MAX)) * 0.1 - 0.05;

        hiddenBias[0] = (rand() / double(RAND_MAX)) * 0.1 - 0.05;
        hiddenBias[1] = (rand() / double(RAND_MAX)) * 0.1 - 0.05;
        outputBias = (rand() / double(RAND_MAX)) * 0.1 - 0.05;
    }

    /**
     * @brief Forward propagation through the network
     * 
     * Computes the activations for the hidden layer and output layer.
     * 
     * @param example The input example with features
     * @param hiddenLayer Array to store hidden layer activations
     * @return Output activation (prediction) for the given example
     * 
     * Example:
     * Suppose `example.features` is {0.0, 1.0, 1.0, 0.0}, and initial weights and biases
     * are small random values. This function computes hidden layer activations and a final
     * output activation (e.g., 0.72), which can be interpreted as the probability of "reads".
     */
    double forward(const Example& example, double hiddenLayer[2]) {
        // Compute activations for hidden layer neurons
        for (int i = 0; i < 2; ++i) {
            hiddenLayer[i] = hiddenBias[i];
            for (int j = 0; j < 4; ++j) {
                hiddenLayer[i] += example.features[j] * weightsInputHidden[j][i];
            }
            hiddenLayer[i] = sigmoid(hiddenLayer[i]); // Activation for hidden neuron
        }

        // Compute activation for output layer neuron
        double output = outputBias;
        for (int i = 0; i < 2; ++i) {
            output += hiddenLayer[i] * weightsHiddenOutput[i];
        }
        return sigmoid(output); // Final output prediction
    }

    /**
     * @brief Backward propagation for weight updates
     * 
     * Computes gradients for output and hidden layers, then updates weights and biases
     * using the gradient descent algorithm.
     * 
     * @param example The input example (features and label)
     * @param output The computed output from forward propagation
     * @param hiddenLayer Hidden layer activations
     * 
     * Example:
     * Given `output` = 0.72 and `example.label` = 1, this function calculates the error and
     * adjusts weights and biases to reduce the error in subsequent iterations.
     */
    void backward(const Example& example, double output, double hiddenLayer[2]) {
        // Calculate error and delta for output layer
        double outputError = example.label - output;
        double outputDelta = outputError * output * (1 - output); // Gradient for output

        // Calculate errors and deltas for hidden layer
        double hiddenError[2];
        double hiddenDelta[2];
        for (int i = 0; i < 2; ++i) {
            hiddenError[i] = outputDelta * weightsHiddenOutput[i];
            hiddenDelta[i] = hiddenError[i] * hiddenLayer[i] * (1 - hiddenLayer[i]);
        }

        // Update weights and biases for output layer
        for (int i = 0; i < 2; ++i) {
            weightsHiddenOutput[i] += learningRate * outputDelta * hiddenLayer[i];
        }
        outputBias += learningRate * outputDelta;

        // Update weights and biases for hidden layer
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 4; ++j) {
                weightsInputHidden[j][i] += learningRate * hiddenDelta[i] * example.features[j];
            }
            hiddenBias[i] += learningRate * hiddenDelta[i];
        }
    }

    /**
     * @brief Train the neural network on a given dataset
     * 
     * Runs forward and backward propagation for each example in the dataset over multiple epochs.
     * Reports misclassified examples after each epoch for monitoring learning progress.
     * 
     * @param train_data Training dataset of examples
     * @param epochs Number of training epochs (e.g., 80)
     */
    void train(vector<Example>& train_data, int epochs) {
        for (int epoch = 0; epoch < epochs; ++epoch) {
            int misclassified = 0;
            for (auto& example : train_data) {
                double hiddenLayer[2];
                double output = forward(example, hiddenLayer);
                backward(example, output, hiddenLayer);
                
                // Count misclassifications
                if ((output >= 0.5) != example.label) {
                    misclassified++;
                }
            }
            cout << "Epoch " << epoch + 1 << ": Misclassified = " << misclassified << "\n";
        }
    }

    /**
     * @brief Test the neural network on a test dataset
     * 
     * Computes the number of misclassified examples for evaluating the network's performance.
     * 
     * @param test_data Testing dataset of examples
     * @return Number of misclassified examples in the test dataset
     */
    int test(const vector<Example>& test_data) {
        int misclassified = 0;
        for (const auto& example : test_data) {
            double hiddenLayer[2];
            double output = forward(example, hiddenLayer);
            if ((output >= 0.5) != example.label) {
                misclassified++;
            }
        }
        return misclassified;
    }

    /**
     * @brief Calculate accuracy on the test dataset
     * 
     * Computes accuracy as the percentage of correctly classified examples.
     * 
     * @param test_data Testing dataset of examples
     * @return Accuracy as a percentage of correct predictions
     */
    double calculate_accuracy(const vector<Example>& test_data) {
        int correct = 0;
        for (const auto& example : test_data) {
            double hiddenLayer[2];
            double output = forward(example, hiddenLayer);
            if ((output >= 0.5) == example.label) {
                correct++;
            }
        }
        return static_cast<double>(correct) / test_data.size();
    }
};

/**
 * @brief Load data from a CSV file and encode categorical features.
 * 
 * Reads CSV data, converts categorical features to numerical values, and creates
 * examples with features and labels.
 * 
 * @param filename Path to the CSV file (e.g., "dataset.csv")
 * @return Encoded dataset
 */
vector<Example> load_data(const string &filename) {
    vector<Example> data;
    ifstream file(filename);
    string line;

    // Skip header line
    getline(file, line);

    // Load data from file and encode each example
    while (getline(file, line)) {
        stringstream ss(line);
        Example example;
        string word;

        // Encode each feature as a numeric value (1.0 for specific values, 0.0 otherwise)
        getline(ss, word, ','); // Author
        example.features.push_back(word == "known" ? 1.0 : 0.0);

        getline(ss, word, ','); // Thread
        example.features.push_back(word == "new" ? 1.0 : 0.0);

        getline(ss, word, ','); // Length
        example.features.push_back(word == "long" ? 1.0 : 0.0);

        getline(ss, word, ','); // Where_read
        example.features.push_back(word == "home" ? 1.0 : 0.0);

        getline(ss, word, ','); // User_action
        example.label = (word == "reads") ? 1 : 0;

        data.push_back(example);
    }
    return data;
}

/**
 * @brief Split dataset into training and testing sets based on a specified ratio.
 * 
 * Divides the dataset into two parts based on a given training ratio (e.g., 0.8 for 80% train).
 * 
 * @param data Full dataset
 * @param train Vector to store training set examples
 * @param test Vector to store testing set examples
 * @param train_ratio Proportion of examples for training (0.8 means 80% train, 20% test)
 */
void split_data(const vector<Example>& data, vector<Example>& train, vector<Example>& test, float train_ratio) {
    int train_size = static_cast<int>(data.size() * train_ratio);
    train.assign(data.begin(), data.begin() + train_size);
    test.assign(data.begin() + train_size, data.end());
}

int main() {
    string filename = "dataset.csv"; // Dataset file path
    vector<Example> data = load_data(filename);

    vector<float> ratios = {0.8, 0.7, 0.5}; // Different train-test split ratios

    for (float ratio : ratios) {
        vector<Example> train_data, test_data;
        split_data(data, train_data, test_data, ratio);

        NeuralNetwork nn(0.7); // Initialize network with a learning rate of 0.7
        nn.train(train_data, 80); // Train the network for 80 epochs

        int testMisclassified = nn.test(test_data);
        double accuracy = nn.calculate_accuracy(test_data);

        cout << "Test Misclassified = " << testMisclassified << "\n";
        cout << "Accuracy for train:test ratio " << ratio << ":" << (1 - ratio) << " is " << accuracy << endl;

        // Predictions for specific examples (e19 and e20)
        Example e19 = {{0.0, 1.0, 1.0, 0.0}, -1}; // Encoded for "unknown", "new", "long", "work"
        Example e20 = {{0.0, 0.0, 0.0, 1.0}, -1}; // Encoded for "unknown", "followup", "short", "home"

        double hiddenLayer[2];
        cout << "Prediction for e19: " << (nn.forward(e19, hiddenLayer) >= 0.5 ? "reads" : "skips") << endl;
        cout << "Prediction for e20: " << (nn.forward(e20, hiddenLayer) >= 0.5 ? "reads" : "skips") << endl;
    }

    return 0;
}
