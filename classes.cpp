//city data class( storage of city names and coordinates)
//graph class ( manage connection between cities and compute distances)
//TSP solver class ( implement various algorithms to solve the tsp)
//visualiser class (handles the graphical display of routes)
//utility class ( for file handling and time complexity analysis)

#include <iostream>
#include <string>
#include <cmath>
#include <cstring>
#include <fstream>
using namespace std;

class City; // Forward declaration of City to be used in Edge

struct Edge {
    City* destination;
    double distance;
    double travelTime;
    Edge* next; // Pointer to the next edge

    Edge(City* dest, double dist, double time) : destination(dest), distance(dist), travelTime(time), next(NULL) {}
};
//this class manages a list of cities, each city is linked to the next city, allowing for dynamic addition and deletion of cities
class City {
public:
    string name;
    double x, y; // coordinates
    City* next; // pointer to the next city in the list
    Edge* edges; // linked list of edges to other cities

    City(string name, double x, double y) : name(name), x(x), y(y), next(NULL), edges(NULL) {}

    // function to add an edge to this city
    void addEdge(City* destination, double distance, double travelTime) {
        Edge* newEdge = new Edge(destination, distance, travelTime);
        newEdge->next = edges;
        edges = newEdge;
    }
};

class Graph {
private:
    City* head; // head pointer for the list of cities

public:
    Graph() : head(NULL) {}

    ~Graph() {
        City* current = head;
        while (current != NULL) {
            Edge* edge = current->edges;
            while (edge != NULL) {
                Edge* tempEdge = edge;
                edge = edge->next;
                delete tempEdge;
            }
            City* tempCity = current;
            current = current->next;
            delete tempCity;
        }
    }

    void addCity(string name, double x, double y) {
        City* newCity = new City(name, x, y);
        newCity->next = head;
        head = newCity;
    }

    void addRoute(string fromCity, string toCity, double distance, double travelTime) {
        City* from = findCity(fromCity);
        City* to = findCity(toCity);
        if (from && to) {
            from->addEdge(to, distance, travelTime);
        }
    }

    City* findCity(string name) {
        City* current = head;
        while (current != NULL) {
            if (current->name == name) {
                return current;
            }
            current = current->next;
        }
        return NULL;
    }

    void displayCities() {
        City* current = head;
        while (current != NULL) {
            cout << "City: " << current->name << " (" << current->x << ", " << current->y << ")" << endl;
            Edge* edge = current->edges;
            while (edge != NULL) {
                cout << "  -> " << edge->destination->name << " (Distance: " << edge->distance << ", Travel Time: " << edge->travelTime << ")" << endl;
                edge = edge->next;
            }
            current = current->next;
        }
    }
    double** createAdjacencyMatrix(int &cityCount, City** &cityArray){
        cityCount = 0;{
            City* temp = head;
            while(temp!=NULL){
                cityCount++;
                temp = temp->next;
            }
        }
        //an array to store pointers to each city
        //lets u refer to cities by an index

        cityArray = new City*[cityCount];
        {
            City* temp = head;
            int index = 0;
            while(temp!=NULL){
                cityArray[index] = temp;
                temp = temp->next;
                index++;
            }
        }
        //adjacency matrix for distances
        //adjacency matrix that stores distances from city i to city j
        double** adjacencyMatrix = new double*[cityCount];
        for(int i=0; i < cityCount; i++){
            adjacencyMatrix[i] = new double[cityCount];
        }
       // Initialize all distances to -1, meaning "no direct route"
        for (int i = 0; i < cityCount; i++) {
            for (int j = 0; j < cityCount; j++) {
                adjacencyMatrix[i][j] = -1.0;
            }
        }

        // Fill the adjacency matrix using the edges from each city
        for (int i = 0; i < cityCount; i++) {
            City* sourceCity = cityArray[i];
            Edge* e = sourceCity->edges;
            while (e != NULL) {
                // Find the index of the destination city
                int destIndex = -1;
                for (int k = 0; k < cityCount; k++) {
                    if (cityArray[k] == e->destination) {
                        destIndex = k;
                        break;
                    }
                }

                // If we found the destination index, record the distance
                if (destIndex != -1) {
                    adjacencyMatrix[i][destIndex] = e->distance;
                }

                e = e->next;
            }
        }

        return adjacencyMatrix;
    }
};

int main() {
    Graph graph;

    // Add some cities
    graph.addCity("New York", 40.7128, -74.0060);
    graph.addCity("Los Angeles", 34.0522, -118.2437);
    graph.addCity("Chicago", 41.8781, -87.6298);

    // Add routes between these cities
    graph.addRoute("New York", "Los Angeles", 3940.0, 5.5);
    graph.addRoute("New York", "Chicago", 790.0, 2.0);
    graph.addRoute("Chicago", "Los Angeles", 2015.0, 4.0);

    // Display the graph as a list of cities and their connections
    cout << "Cities and Their Routes:\n";
    graph.displayCities();

    // Create the adjacency matrix
    int cityCount = 0;
    City** cityArray = NULL;
    double** adjacencyMatrix = graph.createAdjacencyMatrix(cityCount, cityArray);

    // Print the adjacency matrix
    cout << "\nAdjacency Matrix (Distances):\n";

    // Print city names on top
    cout << "          ";
    for (int i = 0; i < cityCount; i++) {
        cout << cityArray[i]->name << "    ";
    }
    cout << "\n";

    // Print city names on the left and the distances in the grid
    for (int i = 0; i < cityCount; i++) {
        cout << cityArray[i]->name << "   ";
        for (int j = 0; j < cityCount; j++) {
            cout << adjacencyMatrix[i][j] << "     ";
        }
        cout << "\n";
    }

    // Cleanup memory for adjacencyMatrix and cityArray
    for (int i = 0; i < cityCount; i++) {
        delete[] adjacencyMatrix[i];
    }
    delete[] adjacencyMatrix;
    delete[] cityArray;

    // The graph and all cities/edges are cleaned up in the Graph destructor when main ends

    return 0;
}
