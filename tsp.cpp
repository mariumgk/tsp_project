#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <numeric>  // For iota

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

// City Node for Linked List
struct CityNode {
    City* city;
    CityNode* next;
    CityNode(City* c) : city(c), next(NULL) {}
};

// Linked List Class for Cities
class CityLinkedList {
private:
    CityNode* head;

public:
    CityLinkedList() : head(NULL) {}

    void addCity(City* city) {
        CityNode* newNode = new CityNode(city);
        newNode->next = head;
        head = newNode;
    }

    City* findCity(const string& cityName) {
        CityNode* current = head;
        while (current) {
            if (current->city->name == cityName) {
                return current->city;
            }
            current = current->next;
        }
        return NULL;
    }

    void displayCities() const {
        CityNode* current = head;
        while (current) {
            cout << current->city->name << " (" << current->city->x << ", " << current->city->y << ")\n";
            current = current->next;
        }
    }

    bool isEmpty() const { return head == NULL; }

    size_t size() const {
        size_t count = 0;
        CityNode* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }

    void bubbleSortByName() {
        if (head == NULL || head->next == NULL) return;
        bool swapped;
        do {
            swapped = false;
            CityNode* current = head;
            CityNode* prev = NULL;
            while (current && current->next) {
                if (current->city->name > current->next->city->name) {
                    CityNode* nextNode = current->next;
                    current->next = nextNode->next;
                    nextNode->next = current;
                    if (prev) {
                        prev->next = nextNode;
                    } else {
                        head = nextNode;
                    }
                    swapped = true;
                }
                prev = current;
                current = current->next;
            }
        } while (swapped);
    }

    vector<City*> getCities() const {
        vector<City*> cityArray;
        CityNode* current = head;
        while (current) {
            cityArray.push_back(current->city);
            current = current->next;
        }
        return cityArray;
    }

    friend class Graph;
};

// Graph Class
class Graph {
private:
    CityLinkedList cityList;
    vector<vector<RouteInfo>> adjacencyMatrix;

public:
    void addCity(const string& name, double X, double Y) {
        City* newCity = new City(name, X, Y);
        cityList.addCity(newCity);
        size_t newSize = adjacencyMatrix.size() + 1;
        adjacencyMatrix.resize(newSize, vector<RouteInfo>(newSize, RouteInfo()));
        cout << "City '" << name << "' added successfully.\n";
    }

    void addRoute(const string& fromName, const string& toName, double distance, double cost, double time) {
        City* fromCity = cityList.findCity(fromName);
        City* toCity = cityList.findCity(toName);
        if (!fromCity || !toCity) {
            cout << "One or both cities not found. Please add the cities first.\n";
            return;
        }

        int fromIndex = findCityIndex(fromCity);
        int toIndex = findCityIndex(toCity);
        if (fromIndex == -1 || toIndex == -1) {
            cout << "Error adding route between cities.\n";
            return;
        }

        adjacencyMatrix[fromIndex][toIndex] = RouteInfo(distance, cost, time);
        adjacencyMatrix[toIndex][fromIndex] = RouteInfo(distance, cost, time);
        cout << "Route between '" << fromName << "' and '" << toName << "' added successfully.\n";
    }

    int findCityIndex(City* city) const {
        int index = 0;
        CityNode* current = cityList.head;
        while (current) {
            if (current->city == city) return index;
            current = current->next;
            index++;
        }
        return -1; // City not found
    }

    void displayCities() const {
        cityList.displayCities();
    }

    bool isEmpty() const { return cityList.isEmpty(); }
    size_t size() const { return cityList.size(); }

    void sortCities() {
        cityList.bubbleSortByName(); // Sort cities using the linked list's bubble sort
    }

    void displayAdjacencyMatrix() const {
        vector<string> labels = {"Distance", "Cost", "Time"};
        int maxCityNameLength = 0;
        for (const auto& city : cityList.getCities()) {
            maxCityNameLength = max(maxCityNameLength, static_cast<int>(city->name.length()));
        }
        int maxColumnWidth = max(maxCityNameLength, 10) + 2;

        for (const string& label : labels) {
            cout << "\n" << label << " Matrix:\n";
            // Header
            cout << setw(maxColumnWidth) << " ";
            for (const auto& city : cityList.getCities()) {
                cout << setw(maxColumnWidth) << city->name;
            }
            cout << endl;

            // Rows
            for (size_t i = 0; i < cityList.size(); i++) {
                cout << setw(maxColumnWidth) << cityList.getCities()[i]->name;
                for (size_t j = 0; j < cityList.size(); j++) {
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

    friend class TSPSolver;
};

// Function to load cities from a file
void loadCitiesFromFile(Graph& graph, const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error: Could not open " << filename << "\n";
        exit(EXIT_FAILURE);
    }
    string cityName;
    double cx, cy;
    while (inFile >> cityName >> cx >> cy) {
        graph.addCity(cityName, cx, cy);
    }
    inFile.close();
}

// Function to load routes from a file
void loadRoutesFromFile(Graph& graph, const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error: Could not open " << filename << "\n";
        exit(EXIT_FAILURE);
    }

    string fromCity, toCity;
    double dist, cost, time;
    int lineNumber = 1;  // To track line number
    while (inFile >> fromCity >> toCity >> dist >> cost >> time) {
        cout << "Reading line " << lineNumber << ": " << fromCity << " " << toCity << " " << dist << " " << cost << " " << time << endl;
        graph.addRoute(fromCity, toCity, dist, cost, time);
        lineNumber++;
    }

    inFile.close();
    cout << "Routes loaded from " << filename << " successfully.\n";
}


// TSPSolver Class
class TSPSolver {
public:
    TSPSolver(Graph& g) : graph(g) {}  // Constructor that accepts Graph reference

    void solveTSPBruteForce(const Graph& graph, const string& criterion) {
        vector<int> tour(graph.cityList.size());
        iota(tour.begin(), tour.end(), 0); // Fill with 0, 1, 2, ..., n-1
        double minCost = numeric_limits<double>::max();
        vector<int> bestTour;

        do {
            double currentCost = calculateTourCost(graph, tour, criterion);
            if (currentCost < minCost) {
                minCost = currentCost;
                bestTour = tour;
            }
        } while (next_permutation(tour.begin(), tour.end()));

        cout << "Best tour cost (" << criterion << " optimized): " << minCost << endl;
        cout << "Best tour path: ";
        for (int cityIndex : bestTour) {
            cout << graph.cityList.getCities()[cityIndex]->name << " -> ";
        }
        cout << graph.cityList.getCities()[bestTour.front()]->name << endl; // Completing the loop to start city
    }

    void solveTSPNearestNeighbor(const Graph& graph, const string& criterion, int startCity) {
        vector<bool> visited(graph.cityList.size(), false);
        visited[startCity] = true;
        vector<int> tour = {startCity};
        int currentCity = startCity;
        double totalCost = 0;

        while (tour.size() < graph.cityList.size()) {
            int nextCity = findNearestCity(graph, currentCity, visited, criterion);
            if (nextCity == -1) break; // No unvisited cities are reachable
            tour.push_back(nextCity);
            totalCost += getRouteCost(graph, currentCity, nextCity, criterion);
            visited[nextCity] = true;
            currentCity = nextCity;
        }

        // Return to the starting city to complete the tour
        totalCost += getRouteCost(graph, currentCity, startCity, criterion);
        tour.push_back(startCity); // Completing the loop

        cout << "Tour cost with Nearest Neighbor (" << criterion << " optimized): " << totalCost << endl;
        cout << "Tour path: ";
        for (int cityIndex : tour) {
            cout << graph.cityList.getCities()[cityIndex]->name << " -> ";
        }
        cout << graph.cityList.getCities()[startCity]->name << endl; // Completing the loop to start city
    }

private:
    Graph& graph;

    double getRouteCost(const Graph& graph, int from, int to, const string& criterion) const {
        const RouteInfo& route = graph.adjacencyMatrix[from][to];
        if (criterion == "Distance") return route.distance;
        if (criterion == "Cost") return route.cost;
        return route.time; // Default to time if unspecified
    }

    int findNearestCity(const Graph& graph, int currentCity, const vector<bool>& visited, const string& criterion) const {
        double minCost = numeric_limits<double>::max();
        int nearestCity = -1;
        for (size_t i = 0; i < graph.cityList.size(); i++) {
            if (!visited[i] && graph.adjacencyMatrix[currentCity][i].distance != -1) {
                double cost = getRouteCost(graph, currentCity, i, criterion);
                if (cost < minCost) {
                    minCost = cost;
                    nearestCity = i;
                }
            }
        }
        return nearestCity;
    }

    double calculateTourCost(const Graph& graph, const vector<int>& tour, const string& criterion) const {
        double totalCost = 0;
        for (size_t i = 0; i < tour.size() - 1; i++) {
            totalCost += getRouteCost(graph, tour[i], tour[i + 1], criterion);
        }
        return totalCost + getRouteCost(graph, tour.back(), tour.front(), criterion);
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

// Main Function
int main() {
    Graph graph;
    TSPSolver solver(graph);
    int choice;
    string filename;
    string criterion;

    string toCity;  // Declare variables before the switch case

    while (true) {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter filename for cities: ";
                cin >> filename;
                loadCitiesFromFile(graph, filename);
                break;
            case 2:
                cout << "Enter filename for routes: ";
                cin >> filename;
                loadRoutesFromFile(graph, filename);
                break;
            case 3:
                cout << "Enter city name: ";
                cin >> filename;
                graph.addCity(filename, 0, 0); // For simplicity
                break;
            case 4:
                cout << "Enter starting city name: ";
                cin >> filename;
                cout << "Enter destination city name: ";
                cin >> toCity;
                double dist, cost, time;
                cout << "Enter distance: ";
                cin >> dist;
                cout << "Enter cost: ";
                cin >> cost;
                cout << "Enter time: ";
                cin >> time;
                graph.addRoute(filename, toCity, dist, cost, time);
                break;
            case 5:
                graph.displayAdjacencyMatrix();
                break;
            case 6:
                cout << "Enter optimization criterion (Distance, Cost, Time): ";
                cin >> criterion;
                solver.solveTSPBruteForce(graph, criterion);
                break;
            case 7:
                cout << "Enter optimization criterion (Distance, Cost, Time): ";
                cin >> criterion;
                int startCity;
                cout << "Enter starting city index (0-based): ";
                cin >> startCity;
                solver.solveTSPNearestNeighbor(graph, criterion, startCity);
                break;
            case 8:
                graph.sortCities();
                cout << "Cities sorted by name.\n";
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
