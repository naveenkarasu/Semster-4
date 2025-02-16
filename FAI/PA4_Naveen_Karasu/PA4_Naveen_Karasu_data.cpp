#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/**
 * @brief Creates a CSV file with dataset information
 * 
 * This function writes a CSV file named "dataset.csv" containing
 * rows of data based on the given dataset. The generated file
 * includes the header and 20 data entries.
 * 
 * @param filename Name of the CSV file to be created.
 */
void create_csv(const string &filename) {
    ofstream file(filename);

    // Write the header for CSV columns
    file << "Author,Thread,Length,Where_read,User_action\n";

    // Define data as a 2D vector
    vector<vector<string>> data = {
        {"known", "new", "long", "home", "skips"},
        {"unknown", "new", "short", "work", "reads"},
        {"unknown", "followup", "long", "work", "skips"},
        {"known", "followup", "long", "home", "skips"},
        {"known", "new", "short", "home", "reads"},
        {"known", "followup", "long", "work", "skips"},
        {"unknown", "followup", "short", "work", "skips"},
        {"unknown", "new", "short", "work", "reads"},
        {"known", "followup", "long", "home", "skips"},
        {"known", "new", "long", "work", "skips"},
        {"unknown", "followup", "short", "home", "skips"},
        {"known", "new", "long", "work", "skips"},
        {"known", "followup", "short", "home", "reads"},
        {"known", "new", "short", "home", "reads"},
        {"known", "new", "short", "work", "reads"},
        {"known", "followup", "short", "home", "reads"},
        {"known", "new", "short", "work", "reads"},
        {"unknown", "new", "short", "work", "reads"},
       };

    // Write data rows to the CSV file
    for (const auto &row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
    cout << "CSV file created successfully: " << filename << endl;
}

/**
 * @brief Main function to execute CSV file generation
 */
int main() {
    create_csv("dataset.csv");  // Generates the CSV file
    return 0;
}
