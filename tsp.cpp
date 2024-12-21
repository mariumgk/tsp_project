#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cmath>
#include <cctype>
#include <chrono>

using namespace std;
using namespace chrono;

// Function to convert string to lowercase
string toLowerCase(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
              [](unsigned char c){ return tolower(c); });
    return lowerStr;
}

// Forward declaration
class TSPSolver;

// City Class
class City {
public:
    string name;
    double x, y;

    City(string cityName, double X, double Y) : name(cityName), x(X), y(Y) {}
};

// Linked List Node for Cities
class CityNode {
public:
    City* city;
    CityNode* next;

    CityNode(City* cityPtr) : city(cityPtr), next(nullptr) {}
};

// RouteInfo Struct
struct RouteInfo {
    double distance = -1;
    double cost = -1;
    double time = -1;

    RouteInfo() = default;
    RouteInfo(double dist, double cst, double tm) : distance(dist), cost(cst), time(tm) {}
};

// Graph Class
class Graph {
private:
    CityNode* head;  // Head of the linked list for cities
    vector<vector<RouteInfo>> adjacencyMatrix;

public:
    Graph() : head(nullptr) {}
    ~Graph() {
        while (head) {
            CityNode* temp = head;
            head = head->next;
            delete temp->city;
            delete temp;
        }
    }

    // Add a city to the linked list
    void addCity(const string& name, double X, double Y) {
        // Check if city already exists
        if (findCityIndex(name) != -1) {
            cout << "City '" << name << "' already exists. Skipping.\n";
            return;
        }

        City* newCity = new City(name, X, Y);
        CityNode* newNode = new CityNode(newCity);

        // Add city to the front of the linked list
        newNode->next = head;
        head = newNode;

        size_t newSize = size();
        // Resize the adjacency matrix
        for (auto& row : adjacencyMatrix) {
            row.resize(newSize, RouteInfo());
        }
        // Add new row
        adjacencyMatrix.emplace_back(newSize, RouteInfo());
        cout << "City '" << name << "' added successfully.\n";
    }

    // Add a route between cities
    void addRoute(const string& fromName, const string& toName, double distance, double cost, double time) {
        int fromIndex = findCityIndex(fromName);
        int toIndex = findCityIndex(toName);
        if (fromIndex == -1 || toIndex == -1) {
            cout << "One or both cities not found. Please add the cities first.\n";
            return;
        }
        if (fromIndex == toIndex) {
            cout << "Cannot add a route from a city to itself.\n";
            return;
        }

        adjacencyMatrix[fromIndex][toIndex] = RouteInfo(distance, cost, time);
        adjacencyMatrix[toIndex][fromIndex] = RouteInfo(distance, cost, time);
        cout << "Route between '" << fromName << "' and '" << toName << "' added successfully.\n";
    }

    // Find city index from linked list
    int findCityIndex(const string& cityName) const {
        CityNode* current = head;
        int index = 0;
        while (current) {
            if (current->city->name == cityName) return index;
            current = current->next;
            index++;
        }
        return -1;
    }

    // Display cities and routes
    void displayCities() const {
        if (isEmpty()) {
            cout << "No cities to display.\n";
            return;
        }

        // Display Distance, Cost, Time Matrices
        vector<string> labels = {"Distance", "Cost", "Time"};
        int maxCityNameLength = 0;
        CityNode* current = head;
        while (current) {
            maxCityNameLength = max(maxCityNameLength, static_cast<int>(current->city->name.length()));
            current = current->next;
        }
        int maxColumnWidth = max(maxCityNameLength, 10) + 2;

        for (const string& label : labels) {
            cout << "\n" << label << " Matrix:\n";
            // Header
            cout << setw(maxColumnWidth) << " ";
            current = head;
            while (current) {
                cout << setw(maxColumnWidth) << current->city->name;
                current = current->next;
            }
            cout << endl;

            // Rows
            for (size_t i = 0; i < size(); i++) {
                current = getCityNodeByIndex(i);
                cout << setw(maxColumnWidth) << current->city->name;
                for (size_t j = 0; j < size(); j++) {
                    double value = -1;
                    if (label == "Distance") value = adjacencyMatrix[i][j].distance;
                    else if (label == "Cost") value = adjacencyMatrix[i][j].cost;
                    else if (label == "Time") value = adjacencyMatrix[i][j].time;

                    if (value == -1)
                        cout << setw(maxColumnWidth) << "INF";
                    else
                        cout << setw(maxColumnWidth) << fixed << setprecision(2) << value;
                }
                cout << endl;
            }
        }
    }

    // Bubble Sort Cities by Name
    void sortCitiesByName() {
        for (size_t i = 0; i < size() - 1; ++i) {
            for (size_t j = 0; j < size() - i - 1; ++j) {
                CityNode* city1 = getCityNodeByIndex(j);
                CityNode* city2 = getCityNodeByIndex(j + 1);
                if (city1->city->name > city2->city->name) {
                    swap(city1->city, city2->city);
                }
            }
        }
        cout << "Cities sorted by name.\n";
    }

    // Accessor Methods
    bool isEmpty() const { return head == nullptr; }
    size_t size() const {
        size_t count = 0;
        CityNode* current = head;
        while (current) {
            current = current->next;
            count++;
        }
        return count;
    }

    // Get City Node by index
    CityNode* getCityNodeByIndex(int index) const {
        CityNode* current = head;
        for (int i = 0; current && i < index; i++) {
            current = current->next;
        }
        return current;
    }

    // Grant TSPSolver access to private members
    friend class TSPSolver;
};

// Function to load cities from a file
void loadCitiesFromFile(Graph& graph, const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error: Could not open " << filename << "\n";
        return;
    }
    string cityName;
    double cx, cy;
    while (inFile >> cityName >> cx >> cy) {
        graph.addCity(cityName, cx, cy);
    }
    inFile.close();
    cout << "Cities loaded from " << filename << " successfully.\n";
}

// Function to load routes from a file
void loadRoutesFromFile(Graph& graph, const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error: Could not open " << filename << "\n";
        return;
    }
    string fromCity, toCity;
    double dist, cost, time;
    while (inFile >> fromCity >> toCity >> dist >> cost >> time) {
        graph.addRoute(fromCity, toCity, dist, cost, time);
    }
    inFile.close();
    cout << "Routes loaded from " << filename << " successfully.\n";
}

// TSPSolver Class (No change in the algorithm)
class TSPSolver {
private:
    Graph& graph;

public:
    TSPSolver(Graph& g) : graph(g) {}

    // Brute Force TSP Solver
void solveBruteForce(const string& criterion, int startCity) {
    if (graph.isEmpty()) {
        cout << "No cities available for TSP.\n";
        return;
    }

    auto startTime = high_resolution_clock::now();  // Start time measurement

    vector<int> perm;
    for (int i = 0; i < static_cast<int>(graph.size()); i++) {
        if (i != startCity) {
            perm.push_back(i);
        }
    }

    sort(perm.begin(), perm.end());

    double minCost = numeric_limits<double>::max();
    vector<int> bestTour;

    do {
        vector<int> currentTour = {startCity};
        currentTour.insert(currentTour.end(), perm.begin(), perm.end());
        double currentCost = calculateTourCost(currentTour, criterion);  // Correctly use criterion
        if (currentCost < minCost) {
            minCost = currentCost;
            bestTour = currentTour;
        }
    } while (next_permutation(perm.begin(), perm.end()));

    if (bestTour.empty()) {
        cout << "No valid tour found.\n";
        return;
    }

    bestTour.push_back(startCity); // Complete the loop to start city

    auto endTime = high_resolution_clock::now();  // End time measurement
    auto duration = duration_cast<microseconds>(endTime - startTime);  // Calculate time duration

    cout << "\nBest tour cost (" << criterion << " optimized): " << fixed << setprecision(2) << minCost << " km" << endl;  // For Distance, print km
    cout << "Best tour path: ";
    for (size_t i = 0; i < bestTour.size(); i++) {
        cout << graph.getCityNodeByIndex(bestTour[i])->city->name;
        if (i != bestTour.size() - 1)
            cout << " -> ";
    }
    cout << endl;

    // Output time complexity
    cout << "Time taken for Brute Force TSP: " << duration.count() << " microseconds." << endl;
}

// Nearest Neighbor TSP Solver
void solveNearestNeighbor(const string& criterion, int startCity) {
    if (graph.isEmpty()) {
        cout << "No cities available for TSP.\n";
        return;
    }

    auto startTime = high_resolution_clock::now();  // Start time measurement

    vector<bool> visited(graph.size(), false);
    visited[startCity] = true;
    vector<int> tour = {startCity};
    int currentCity = startCity;
    double totalCost = 0;

    while (tour.size() < graph.size()) {
        int nextCity = findNearestCity(currentCity, visited, criterion);
        if (nextCity == -1) break; // No unvisited cities are reachable
        tour.push_back(nextCity);
        totalCost += getRouteCost(currentCity, nextCity, criterion);  // Correctly use criterion
        visited[nextCity] = true;
        currentCity = nextCity;
    }

    totalCost += getRouteCost(currentCity, startCity, criterion); // Return to the starting city
    tour.push_back(startCity);

    auto endTime = high_resolution_clock::now();  // End time measurement
    auto duration = duration_cast<microseconds>(endTime - startTime);  // Calculate time duration

    cout << "\nTour cost with Nearest Neighbor (" << criterion << " optimized): " << fixed << setprecision(2) << totalCost << " km" << endl;  // For Distance, print km
    cout << "Tour path: ";
    for (size_t i = 0; i < tour.size(); i++) {
        cout << graph.getCityNodeByIndex(tour[i])->city->name;
        if (i != tour.size() - 1)
            cout << " -> ";
    }
    cout << endl;

    // Output time complexity
    cout << "Time taken for Nearest Neighbor TSP: " << duration.count() << " microseconds." << endl;
}

private:
  double getRouteCost(int from, int to, const string& criterion) const {
    const RouteInfo& route = graph.adjacencyMatrix[from][to];
    
    if (criterion == "Distance") {
        return route.distance;  // Fetching distance in kilometers
    }
    if (criterion == "Cost") {
        return route.cost;      // Fetching cost in rupees
    }
    if (criterion == "Time") {
        return route.time;      // Fetching time in hours
    }

    // Default to -1 if none of the criteria match
    return -1;
}


    int findNearestCity(int currentCity, const vector<bool>& visited, const string& criterion) const {
        double minCost = numeric_limits<double>::max();
        int nearestCity = -1;
        for (size_t i = 0; i < graph.size(); i++) {
            if (!visited[i] && graph.adjacencyMatrix[currentCity][i].distance != -1) {
                double cost = getRouteCost(currentCity, i, criterion);
                if (cost < minCost) {
                    minCost = cost;
                    nearestCity = static_cast<int>(i);
                }
            }
        }
        return nearestCity;
    }

    double calculateTourCost(const vector<int>& tour, const string& criterion) const {
        double totalCost = 0;
        for (size_t i = 0; i < tour.size() - 1; i++) {
            double cost = getRouteCost(tour[i], tour[i + 1], criterion);
            if (cost == -1) return numeric_limits<double>::max(); // Invalid path
            totalCost += cost;
        }
        return totalCost;
    }
};

// Function to display the main menu
void displayMenu() {
    cout << "\n===== TSP Solver Menu =====\n";
    cout << "1. Load cities from file\n";
    cout << "2. Load routes from file\n";
    cout << "3. Add a city manually\n";
    cout << "4. Add a route manually\n";
    cout << "5. Display cities and routes\n";
    cout << "6. Solve TSP using Brute Force\n";
    cout << "7. Solve TSP using Nearest Neighbor\n";
    cout << "8. Sort cities by name\n";
    cout << "9. Exit\n";
    cout << "Enter your choice: ";
}
// Function to check case-insensitive string comparison
bool equalsIgnoreCase(const string& str1, const string& str2) {
    return str1.size() == str2.size() && 
           equal(str1.begin(), str1.end(), str2.begin(), 
                 [](unsigned char c1, unsigned char c2) { return tolower(c1) == tolower(c2); });
}

// Main Function
int main() {
    Graph graph;
    TSPSolver solver(graph);
    int choice;
    string filename;
    string criterion;

    while (true) {
        displayMenu();
        if (!(cin >> choice)) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Please enter a number between 1 and 9.\n";
            continue;
        }

        switch (choice) {
            case 1:
                cout << "Enter the filename to load cities from: ";
                cin >> filename;
                loadCitiesFromFile(graph, filename);
                break;
            case 2:
                cout << "Enter the filename to load routes from: ";
                cin >> filename;
                loadRoutesFromFile(graph, filename);
                break;
            case 3: {
                string cityName;
                double x, y;
                cout << "Enter city name: ";
                cin >> cityName;
                cout << "Enter X coordinate: ";
                while (!(cin >> x)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Enter a numeric X coordinate: ";
                }
                cout << "Enter Y coordinate: ";
                while (!(cin >> y)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Enter a numeric Y coordinate: ";
                }
                graph.addCity(cityName, x, y);
                break;
            }
            case 4: {
                string fromCity, toCity;
                double distance, cost, time;
                cout << "Enter the starting city name: ";
                cin >> fromCity;
                cout << "Enter the destination city name: ";
                cin >> toCity;
                cout << "Enter distance: ";
                while (!(cin >> distance) || distance < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Enter a non-negative numeric distance: ";
                }
                cout << "Enter cost: ";
                while (!(cin >> cost) || cost < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Enter a non-negative numeric cost: ";
                }
                cout << "Enter time: ";
                while (!(cin >> time) || time < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Enter a non-negative numeric time: ";
                }
                graph.addRoute(fromCity, toCity, distance, cost, time);
                break;
            }
            case 5:
                graph.displayCities();
                break;
            case 6:
            case 7: {
                if (graph.isEmpty()) {
                    cout << "No cities available. Please add cities first.\n";
                    break;
                }

                // Choose optimization criterion (case insensitive)
                cout << "Enter optimization criterion (Distance, Cost, Time): ";
                cin >> criterion;

                // Case-insensitive comparison for criterion
                if (!equalsIgnoreCase(criterion, "distance") && 
                    !equalsIgnoreCase(criterion, "cost") && 
                    !equalsIgnoreCase(criterion, "time")) {
                    cout << "Invalid criterion. Please choose Distance, Cost, or Time.\n";
                    break;
                }

                // Prompt for starting city
                string startCityName;
                cout << "Enter the starting city name: ";
                cin >> startCityName;
                int startCity = graph.findCityIndex(startCityName);
                if (startCity == -1) {
                    cout << "City not found. Please ensure the city exists.\n";
                    break;
                }

                if (choice == 6) {
                    // Brute Force
                    if (graph.size() > 10) { // Limit brute force for performance
                        cout << "Warning: Brute Force approach may take a long time with more than 10 cities.\n";
                        cout << "Do you want to continue? (y/n): ";
                        char confirm;
                        cin >> confirm;
                        if (tolower(confirm) != 'y') {
                            break;
                        }
                    }
                    solver.solveBruteForce(criterion, startCity);
                } else {
                    // Nearest Neighbor
                    solver.solveNearestNeighbor(criterion, startCity);
                }
                break;
            }
            case 8:
                graph.sortCitiesByName();
                break;
            case 9:
                cout << "Exiting the program. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 9.\n";
        }
    }

    return 0;
}
