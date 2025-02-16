#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <climits>
#include <algorithm>

using namespace std;

// Graph representation: Each node maps to a vector of (neighbor, cost) pairs
map<char, vector<pair<char, int>>> graph;

void show_graph(const std::map<char, std::vector<std::pair<char, int>>>& graph) {
    for (const auto& node : graph) {
        std::cout << node.first << " -> ";
        for (const auto& edge : node.second) {
            std::cout << "(" << edge.first << ", " << edge.second << ") ";
        }
        std::cout << std::endl;
    }
}
// Function to add edges to the graph
void add_edge(char from, char to, int cost) {
    // To ensure both 'from' and 'to' nodes exist in the graph
    if (graph.find(from) == graph.end()) {
        graph[from] = {}; // Initialize the node if it doesn't exist
    }
    if (graph.find(to) == graph.end()) {
        graph[to] = {}; // Initialize the 'to' node even if it has no outgoing edges yet
    }
    
    // Add the edge from 'from' to 'to'
    graph[from].push_back({to, cost});
}

// Comparator for priority queue
struct Compare {
    bool operator()(const pair<int, char>& a, const pair<int, char>& b) {
        return a.first > b.first; // Min-heap based on the cost
    }
};

//To display the contents of the priority queue
void print_priority_queue(priority_queue<pair<int, char>, vector<pair<int, char>>, Compare> pq) {
    cout << endl<<"Priority Queue Contents: "<<endl;
    while (!pq.empty()) {
        cout << "(" << pq.top().second << ", Cost: " << pq.top().first << ") ";
        pq.pop();  // Pop the top element to move through the queue
    }
    cout << endl<<endl;
}

// Lowest-cost-first search to find the shortest path from start to goal
void lowest_cost_first_search(char start, char goal) {
    priority_queue<pair<int, char>, vector<pair<int, char>>, Compare> pq;
    map<char, int> costs;
    map<char, char> parent;
    vector<bool> visited(256, false); // Keep track of visited nodes.

    // Initialize costs to infinity for all nodes
    for (const auto& node : graph) {
        costs[node.first] = INT_MAX;
    }

    pq.push({0, start});
    costs[start] = 0;
    parent[start] = start;

    while (!pq.empty()) {
        print_priority_queue(pq);  // Print the priority queue contents at each step

        int current_cost = pq.top().first;
        char current = pq.top().second;
        pq.pop();

        // If the node has already been visited, skip it
        if (visited[current]) {
            continue;
        }
        visited[current] = true;

        cout << "Processing node: " << current << " with current cost: " << current_cost << endl;

        // Check all neighbors of the current node
        for (const auto& neighbor_info : graph[current]) {
            char neighbor = neighbor_info.first;
            int edge_cost = neighbor_info.second;

            // Log the neighbor being checked
            cout << "Checking neighbor " << neighbor << " from node " << current << " with edge cost " << edge_cost << endl;

            int new_cost = current_cost + edge_cost;
            cout << "New cost to reach " << neighbor << " is " << new_cost << " (current known cost: " << costs[neighbor] << ")" << endl;

            // If the new cost is lower, update the cost and push it to the priority queue
            if (new_cost < costs[neighbor]) {
                cout << "Updating cost for neighbor " << neighbor << " from " << costs[neighbor] << " to " << new_cost << endl;
                costs[neighbor] = new_cost;
                parent[neighbor] = current;
                pq.push({new_cost, neighbor});
            } else {
                cout << "No update needed for neighbor " << neighbor << " (current cost: " << costs[neighbor] << ", new cost: " << new_cost << ")" << endl;
            }
        }
    }

    if (costs[goal] == INT_MAX) {
        cout << "There is no path from " << start << " to " << goal << endl;
    } else {
        cout << "Minimum cost from " << start << " to " << goal << " is " << costs[goal] << endl;

        // Reconstruct the path from goal to start
        vector<char> path;
        for (char at = goal; at != start; at = parent[at]) {
            path.push_back(at);
        }
        path.push_back(start);
        reverse(path.begin(), path.end());

        // Print the path
        cout << "Path: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i < path.size() - 1) cout << " -> ";
        }
        cout << endl;
    }
}

int main() {
    // Constructing the graph
    add_edge('A', 'B', 2);
    add_edge('A', 'C', 3);
    add_edge('A', 'D', 4);
    add_edge('B', 'E', 2);
    add_edge('B', 'F', 3);
    add_edge('C', 'J', 7);
    add_edge('D', 'H', 4);
    add_edge('F', 'D', 2);
    add_edge('H', 'G', 3);  
    add_edge('J', 'G', 4);

    // Display the graph
    show_graph(graph);
    // Performing Lowest-cost-first to search the routefrom A to G
    cout << endl<<"Performing lowest-cost-first search from A to G:" << endl<<endl;
    lowest_cost_first_search('A', 'G');

    return 0;
}
