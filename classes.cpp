#include <iostream>
using namespace std;
//city data class( storage of city names and coordinates)
//graph class ( manage connection between cities and compute distances)
//TSP solver class ( implement various algorithms to solve the tsp)
//visualiser class (handles the graphical display of routes)
//utility class ( for file handling and time complexity analysis)

struct City{
    public:
    string name;
    double x,y; //coordinates
    City* next; //single list pointer to next 

    City(string name, double x, double y):name(name), x(x), y(y), next(NULL){}
};
//this class manages a list of cities, each city is linked to the next city, allowing for dynamic addition and deletion of cities
class CityList{
    private:
    City* head; // head pointer for the list of cities 
    public:
    CityList():head(NULL){}
    
    //function to add cities
    void addCity(string name, double x, double y){
        City* newCity = new City(name, x, y);
        newCity->next = head;
        head = newCity;
    }
    //function to display all cities in the list 
    void displayCities(){
        City* current = head;
        while(current!=NULL){
            cout<<"City: "<<current->name<<" ( "<<current->x<<","<<current->y<<" ) "<<endl;
            current = current->next;
        }
    }
};
int main(){
    CityList cityList;

    cityList.addCity("Lahore", 40.4422, -99.5);
    cityList.addCity("Karachi", 33.77, -134.55);
    cityList.addCity("Islamabad", 41.8781, -87.6298);

    //displaying
    cityList.displayCities();
    return 0;
}