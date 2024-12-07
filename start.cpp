#include <iostream>
using namespace std;

// Node class for linked list
class Node {
public:
    int city;        // City number;
    Node* next;      // Pointer to the next city in the path

    Node(int c) : city(c), next(NULL) {}
};

// Linked List class for the current path
class Path {
private:
    Node* head;
    Node* tail;

public:
    Path() : head(NULL), tail(NULL) {}

    // Add a city to the path
    void addCity(int city) {
        Node* newNode = new Node(city);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    // Display the current path
    void displayPath() {
        Node* temp = head;
        cout << "Current Path: ";
        while (temp) {
            cout << temp->city << " -> ";
            temp = temp->next;
        }
        cout << "END" << endl;
    }

};

int main() {
    Path currentPath;

    // Example: Adding cities to the path
    currentPath.addCity(1);
    currentPath.addCity(3);
    currentPath.addCity(5);

    // Display the path
    currentPath.displayPath();

    return 0;
}
