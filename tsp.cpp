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
#include <chrono> // For measuring time complexity

using namespace std;
using namespace chrono;

// Forward declaration
class TSPSolver;

// City Class
class City {
public:
    string name;
    double x, y;

    City(string cityName, double X, double Y) : name(cityName), x(X), y(Y) {}
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
    vector<City*> cities;
    vector<vector<RouteInfo>> adjacencyMatrix;

public:
    Graph() {}
    ~Graph() {
        for (auto city : cities) {
            delete city;
        }
    }

    void addCity(const string& name, double X, double Y) {
        // Check if city already exists
        if (findCityIndex(name) != -1) {
            cout << "City '" << name << "' already exists. Skipping.\n";
            return;
        }

        cities.push_back(new City(name, X, Y));
        size_t newSize = cities.size();
        // Resize existing rows
        for (auto& row : adjacencyMatrix) {
            row.resize(newSize, RouteInfo());
        }
        // Add new row
        adjacencyMatrix.emplace_back(newSize, RouteInfo());
        cout << "City '" << name << "' added successfully.\n";
    }

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

    int findCityIndex(const string& cityName) const {
        for (size_t i = 0; i < cities.size(); i++) {
            if (cities[i]->name == cityName) return static_cast<int>(i);
        }
        return -1;
    }

    void displayCities() const {
        if (isEmpty()) {
            cout << "No cities to display.\n";
            return;
        }

        // Display Distance, Cost, Time Matrices
        vector<string> labels = {"Distance", "Cost", "Time"};
        int maxCityNameLength = 0;
        for (const auto& city : cities) {
            maxCityNameLength = max(maxCityNameLength, static_cast<int>(city->name.length()));
        }
        int maxColumnWidth = max(maxCityNameLength, 10) + 2;

        for (const string& label : labels) {
            cout << "\n" << label << " Matrix:\n";
            // Header
            cout << setw(maxColumnWidth) << " ";
            for (const auto& city : cities) {
                cout << setw(maxColumnWidth) << city->name;
            }
            cout << endl;

            // Rows
            for (size_t i = 0; i < cities.size(); i++) {
                cout << setw(maxColumnWidth) << cities[i]->name;
                for (size_t j = 0; j < cities.size(); j++) {
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
        for (size_t i = 0; i < cities.size() - 1; ++i) {
            for (size_t j = 0; j < cities.size() - i - 1; ++j) {
                if (cities[j]->name > cities[j + 1]->name) {
                    swap(cities[j], cities[j + 1]);
                }
            }
        }
        cout << "Cities sorted by name.\n";
    }

    // Accessor Methods
    bool isEmpty() const { return cities.empty(); }
    size_t size() const { return cities.size(); }

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

// TSPSolver Class
class TSPSolver {
private:
    Graph& graph;

public:
    TSPSolver(Graph& g) : graph(g) {}

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
            double currentCost = calculateTourCost(currentTour, criterion);
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

        cout << "\nBest tour cost (" << criterion << " optimized): " << fixed << setprecision(2) << minCost << endl;
        cout << "Best tour path: ";
        for (size_t i = 0; i < bestTour.size(); i++) {
            cout << graph.cities[bestTour[i]]->name;
            if (i != bestTour.size() - 1)
                cout << " -> ";
        }
        cout << endl;

        // Output time complexity
        cout << "Time taken for Brute Force TSP: " << duration.count() << " microseconds.\n";
    }

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
            totalCost += getRouteCost(currentCity, nextCity, criterion);
            visited[nextCity] = true;
            currentCity = nextCity;
        }

        totalCost += getRouteCost(currentCity, startCity, criterion); // Return to the starting city
        tour.push_back(startCity);

        auto endTime = high_resolution_clock::now();  // End time measurement
        auto duration = duration_cast<microseconds>(endTime - startTime);  // Calculate time duration

        cout << "\nTour cost with Nearest Neighbor (" << criterion << " optimized): " << fixed << setprecision(2) << totalCost << endl;
        cout << "Tour path: ";
        for (size_t i = 0; i < tour.size(); i++) {
            cout << graph.cities[tour[i]]->name;
            if (i != tour.size() - 1)
                cout << " -> ";
        }
        cout << endl;

        // Output time complexity
        cout << "Time taken for Nearest Neighbor TSP: " << duration.count() << " microseconds.\n";
    }

private:
    double getRouteCost(int from, int to, const string& criterion) const {
        const RouteInfo& route = graph.adjacencyMatrix[from][to];
        if (criterion == "Distance") return route.distance;
        if (criterion == "Cost") return route.cost;
        return route.time; // Default to time if unspecified
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

// Function to convert string to lowercase
string toLowerCase(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
              [](unsigned char c){ return tolower(c); });
    return lowerStr;
}

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
    cout << "8. Sort cities by name\n";  // New option for sorting cities
    cout << "9. Exit\n";
    cout << "Enter your choice: ";
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

                // Choose optimization criterion
                cout << "Enter optimization criterion (Distance, Cost, Time): ";
                cin >> criterion;
                // Convert to lowercase for case-insensitive comparison
                string lowerCriterion = toLowerCase(criterion);
                if (lowerCriterion != "distance" && lowerCriterion != "cost" && lowerCriterion != "time") {
                    cout << "Invalid criterion. Please choose Distance, Cost, or Time.\n";
                    break;
                }
                // Capitalize first letter for consistency
                criterion = string(1, toupper(criterion[0])) + criterion.substr(1);

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
