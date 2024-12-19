#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>  // For setting precision and formatting output

using namespace std;

class City {
public:
    string name;
    double x, y;

    City(string cityName, double X, double Y) : name(cityName), x(X), y(Y) {}
};

struct RouteInfo {
    double distance = -1;
    double cost = -1;
    double time = -1;

    RouteInfo() = default;
    RouteInfo(double dist, double cst, double tm) : distance(dist), cost(cst), time(tm) {}
};

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

    void addCity(string name, double X, double Y) {
        cities.push_back(new City(name, X, Y));
        size_t newSize = cities.size();
        adjacencyMatrix.resize(newSize);
        for (auto &row : adjacencyMatrix) {
            row.resize(newSize);
        }
    }

    void addRoute(string fromName, string toName, double distance, double cost, double time) {
        int fromIndex = findCityIndex(fromName);
        int toIndex = findCityIndex(toName);
        if (fromIndex != -1 && toIndex != -1) {
            adjacencyMatrix[fromIndex][toIndex] = RouteInfo(distance, cost, time);
            adjacencyMatrix[toIndex][fromIndex] = RouteInfo(distance, cost, time);  // Symmetric for TSP
        }
    }

    int findCityIndex(string cityName) {
        for (size_t i = 0; i < cities.size(); ++i) {
            if (cities[i]->name == cityName) {
                return i;
            }
        }
        return -1;
    }

    void displayCities() {
        cout << "Adjacency Matrix (Distances, Costs, Times):\n";
        cout << setw(15) << " ";
        for (auto city : cities) {
            cout << setw(15) << city->name;
        }
        cout << "\n";

        for (size_t i = 0; i < cities.size(); i++) {
            cout << setw(15) << cities[i]->name;
            for (size_t j = 0; j < cities.size(); j++) {
                auto& route = adjacencyMatrix[i][j];
                if (route.distance != -1) {
                    cout << setw(15) << "D:" + to_string((int)route.distance) + " C:" + to_string((int)route.cost) + " T:" + to_string(route.time);
                } else {
                    cout << setw(15) << "NULL";
                }
            }
            cout << "\n";
        }
    }
};

void loadCitiesFromFile(Graph &graph, const string &filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error: Could not open " << filename << "\n";
        return;
    }

    string cityName;
    double cx, cy;
    while (inFile >> cityName >> cx >> cy) {
        graph.addCity(cityName, cx, cy);
    }

    inFile.close();
    cout << "Cities loaded from " << filename << "\n";
}

void loadRoutesFromFile(Graph &graph, const string &filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error: Could not open " << filename << "\n";
        return;
    }

    string fromCity, toCity;
    double dist, cost, time;
    while (inFile >> fromCity >> toCity >> dist >> cost >> time) {
        graph.addRoute(fromCity, toCity, dist, cost, time);
    }

    inFile.close();
    cout << "Routes loaded from " << filename << "\n";
}

int main() {
    Graph graph;

    // Load cities and routes from files
    loadCitiesFromFile(graph, "cities.txt");
    loadRoutesFromFile(graph, "routes.txt");

    // Display the graph (now using an adjacency matrix)
    graph.displayCities();

    return 0;
}
