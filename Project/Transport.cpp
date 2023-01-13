#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <cstring>
#include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>

using std::cout, std::vector, std::string, std::endl;
using namespace sf;

class Transport
{
protected:
    vector<int> route;
    string type, orientation;
    bool disabilities, goBack;
    unsigned int zoneCount;
    int ID, speed, currentHold;
    double capacity;
    bool needAsistance;

public:
    virtual short int getOccupation() = 0;
    virtual int getCapacity() = 0;
    virtual int *getcurrentHold() = 0;
    virtual string getType() = 0;
    virtual int *getSpeed() = 0;
    virtual string getOrientation() = 0;
    virtual void setOrientation() = 0;
    virtual vector<int> *getRoute() = 0;
    virtual bool getDisabilities() = 0;
    virtual unsigned int *getZoneCount() = 0;
    virtual int getID() = 0;
    virtual bool getBack() = 0;
    virtual void setBack(bool) = 0;
    virtual void setAsistance(bool) = 0;
    virtual bool getAsistance() = 0;
};

class Tram : public Transport
{
public:
    Tram(int ID, double capacity, bool disabilities);
    short int getOccupation();
    int getCapacity();
    int *getcurrentHold();
    string getType();
    int *getSpeed();
    string getOrientation();
    void setOrientation();
    vector<int> *getRoute();
    bool getDisabilities();
    unsigned int *getZoneCount();
    int getID();
    bool getBack();
    void setBack(bool);
    void setAsistance(bool);
    bool getAsistance();
};

class Bus : public Transport
{
public:
    Bus(int ID, double capacity, bool disabilities);
    short int getOccupation();
    int *getcurrentHold();
    int getCapacity();
    string getType();
    int *getSpeed();
    string getOrientation();
    void setOrientation();
    vector<int> *getRoute();
    bool getDisabilities();
    unsigned int *getZoneCount();
    int getID();
    bool getBack();
    void setBack(bool);
    void setAsistance(bool);
    bool getAsistance();
};

class CTP
{
private:
    vector<Transport *> vehicles;

public:
    vector<Transport *> getVehicles();
    void addVehicle(Transport *);
    void removeVehicle(int ID);
    void generateRoutes(vector<vector<int>> *, int);
    int choose(vector<vector<int>> *, int);
    int makeRoute(vector<vector<int>> *, Transport *, int, int, vector<int> &, int);
    void printRoutes();
    void printAll();
};

class Map
{
private:
    vector<vector<int>> adjency;

public:
    int maxNode, sqrtNode;
    Map(int a);
    bool checkAdjency(int, vector<int> *);
    int createMap(int);
    vector<vector<int>> *getAdjency();
    void printMap();
};

// ------------

Tram::Tram(int ID, double capacity, bool disabilities)
{
    this->ID = ID;
    this->capacity = capacity;
    this->disabilities = disabilities;
    this->type = "Tram";
    currentHold = speed = zoneCount = 0;
    orientation = "None";
    this->zoneCount = 0;
    this->goBack = false;
    needAsistance = false;
}

Bus::Bus(int ID, double capacity, bool disabilities)
{
    this->ID = ID;
    this->capacity = capacity;
    this->disabilities = disabilities;
    this->type = "Bus";
    currentHold = speed = zoneCount = 0;
    orientation = "None";
    this->zoneCount = 0;
    this->goBack = false;
    needAsistance = false;
}

bool Tram::getAsistance() { return this->needAsistance; }
bool Bus::getAsistance() { return this->needAsistance; }

void Tram::setAsistance(bool asistance) { this->needAsistance = asistance; }
void Bus::setAsistance(bool asistance) { this->needAsistance = asistance; }

int Tram::getCapacity() { return capacity; }
int Bus::getCapacity() { return capacity; }

int *Tram::getcurrentHold() { return &currentHold; }
int *Bus::getcurrentHold() { return &currentHold; }

void Tram::setBack(bool state) { this->goBack = state; }
void Bus::setBack(bool state) { this->goBack = state; }

bool Tram::getBack() { return goBack; }
bool Bus::getBack() { return goBack; }

short int Tram::getOccupation() { return (short int)((currentHold / capacity) * 100); }
short int Bus::getOccupation() { return (short int)((currentHold / capacity) * 100); }

int Tram::getID() { return ID; }
int Bus::getID() { return ID; }

string Tram::getType() { return type; }
string Bus::getType() { return type; }

int *Tram::getSpeed() { return &speed; }
int *Bus::getSpeed() { return &speed; }

void Tram::setOrientation()
{
    orientation.clear();
    orientation += "Stationary";
    if (goBack == false && zoneCount != route.size() && zoneCount != 0)
    {
        orientation.clear();
        int current = route[zoneCount], previous = route[zoneCount - 1];
        if (current - previous == 1)
            orientation += "East";
        else if (current - previous > 1)
            orientation += "North";
        else if (previous - current == 1)
            orientation += "West";
        else if (previous - current > 1)
            orientation += "South";
    }
    else if (goBack == true && zoneCount != 0 && zoneCount != route.size() - 1)
    {
        orientation.clear();
        int current = route[zoneCount], previous = route[zoneCount + 1];
        if (previous - current == 1)
            orientation += "West";
        else if (previous - current > 1)
            orientation += "South";
        else if (current - previous == 1)
            orientation += "East";
        else if (current - previous > 1)
            orientation += "North"; // west north north east
    }
    // if (zoneCount == 0 || zoneCount == route.size() + 1 || zoneCount == route.size())
    //     orientation += "Stationary";
}
string Tram::getOrientation()
{
    Tram::setOrientation();
    return orientation;
}

void Bus::setOrientation()
{
    orientation.clear();
    orientation += "Stationary";
    if (goBack == false && zoneCount != route.size() && zoneCount != 0)
    {
        orientation.clear();
        int current = route[zoneCount], previous = route[zoneCount - 1];
        if (current - previous == 1)
            orientation += "East";
        else if (current - previous > 1)
            orientation += "North";
        else if (previous - current == 1)
            orientation += "West";
        else if (previous - current > 1)
            orientation += "South";
    }
    else if (goBack == true && zoneCount != 0 && zoneCount != route.size() - 1)
    {
        orientation.clear();
        int current = route[zoneCount], previous = route[zoneCount + 1];
        if (previous - current == 1)
            orientation += "West";
        else if (previous - current > 1)
            orientation += "South";
        else if (current - previous == 1)
            orientation += "East";
        else if (current - previous > 1)
            orientation += "North"; // west north north east
    }
    // if (zoneCount == 0 || zoneCount == route.size() + 1 || zoneCount == route.size())
    //     orientation += "Stationary";
}

string Bus::getOrientation()
{
    Bus::setOrientation();
    return orientation;
}

vector<int> *Tram::getRoute() { return &route; }
vector<int> *Bus::getRoute() { return &route; }

bool Tram::getDisabilities() { return disabilities; }
bool Bus::getDisabilities() { return disabilities; }

unsigned int *Tram::getZoneCount() { return &zoneCount; }
unsigned int *Bus::getZoneCount() { return &zoneCount; }

void CTP::printRoutes()
{
    for (int i = 0; i < vehicles.size(); i++)
    {
        cout << "Ruta " << vehicles[i]->getID() << ": ";
        for (int j = 0; j < vehicles[i]->getRoute()->size(); j++)
            cout << vehicles[i]->getRoute()->at(j) << " ";
        cout << endl;
    }
}

vector<Transport *> CTP::getVehicles() { return vehicles; }
void CTP::addVehicle(Transport *transport) { vehicles.push_back(transport); }
void CTP::removeVehicle(int ID)
{
    for (int i = 0; i < vehicles.size(); i++)
    {
        if (vehicles[i]->getID() == ID)
        {
            vehicles.erase(vehicles.begin() + i);
            break;
        }
    }
}

void CTP::printAll()
{
    for (int i = 0; i < vehicles.size(); i++)
    {
        cout << vehicles[i]->getType() << " " << vehicles[i]->getID() << endl;
    }
}

int CTP::choose(vector<vector<int>> *list, int sqrtNode)
{
    short int random = rand() % 3; // 0 = primul nod valabil, 1 = ultimul nod valabil, 2 = interior
    switch (random)
    {
    case 0:
    {
        int j = 0;
        while ((*list)[j].size() == 1) // primul nod cu minim un vecin
            ++j;
        return j;
        break;
    }
    case 1:
    {
        int j = (*list).size() - 1;
        while ((*list)[j].size() == 1) //ultimul nod cu minim un vecin
            --j;
        return j;
        break;
    }
    case 2:
    {
        bool choice = rand() % 2;
        int size = (*list).size();
        int min = sqrtNode, max = size - sqrtNode;
        if (choice == 0)
        {
            while ((*list)[min].size() == 1 && min < max)
                ++min;
            if (min <= max)
                return min;
            else
                return 0; // returnam nodul 0 daca nu gasim un nod in interior
        }
        else
        {
            while ((*list)[max].size() == 1 && min < max)
                --max;
            if (min <= max)
                return max;
            else
                return 0; // returnam nodul 0 daca nu gasim un nod in interior
        }
        break;
    }
    default:
        break;
    }
    return 0;
}

int CTP::makeRoute(vector<vector<int>> *list, Transport *vehicle, int current, int length, vector<int> &visited, int sqrtNode)
{
    if (length == 0)
        return 0;
    if (current == sqrtNode && vehicle->getRoute()->size() == 0)
    {
        int random = rand() % ((*list)[current].size() - 2);
        random += 2;
        int aux = (*list)[current][random];
        visited.push_back(aux);
        vehicle->getRoute()->push_back(aux);
        makeRoute(list, vehicle, aux, length - 1, visited, sqrtNode); // evitam ruta de o zona (5 -> 0, 0->nicaieri)
        return 0;
    }
    int optionalChoice;
    int choice = rand() % (*list)[current].size();
    if (choice == 0)
        ++choice;
    if (find(visited.begin(), visited.end(), (*list)[current][choice]) != visited.end())
    {
        optionalChoice = (choice == 1 ? 2 : 1);
        while (find(visited.begin(), visited.end(), (*list)[current][optionalChoice]) != visited.end() && optionalChoice < (*list)[current].size())
        {
            ++optionalChoice;
            if (optionalChoice == choice)
                ++optionalChoice;
        }
        choice = optionalChoice;
    }
    if (choice == (*list)[current].size())
        return 0;
    else
    {
        int aux = (*list)[current][choice];
        visited.push_back(aux);
        vehicle->getRoute()->push_back(aux);
        makeRoute(list, vehicle, aux, length - 1, visited, sqrtNode);
        return 0;
    }
}

void CTP::generateRoutes(vector<vector<int>> *list, int sqrtNode)
{
    for (int i = 0; i < vehicles.size(); i++)
    {
        int startNode = choose(list, sqrtNode);
        vector<int> visited;
        visited.push_back(startNode);
        // (*this->getVehicles()[i]->getRoute()).push_back(startNode);
        int length = 5 + rand() % 10; //minim
        makeRoute(list, vehicles[i], startNode, length, visited, sqrtNode);
    }
}

Map::Map(int a) : maxNode(a)
{
    sqrtNode = sqrt(maxNode);
    for (int i = 0; i < maxNode; i++)
        adjency.push_back({i});
};

bool Map::checkAdjency(int nodeNumber, vector<int> *adjencyList)
{
    if (find(adjencyList->begin(), adjencyList->end(), nodeNumber) == adjencyList->end())
        return true;
    return false;
}

int Map::createMap(int nodeNumber)
{
    if (nodeNumber <= maxNode)
    {
        int paths = rand() % 4;
        if (paths == 0)
            ++paths;
        switch (paths)
        {
        case 1:
        {
            if (nodeNumber + sqrtNode < maxNode && checkAdjency(nodeNumber + sqrtNode, &adjency[nodeNumber]) == true)
            {
                adjency[nodeNumber].push_back(nodeNumber + sqrtNode);
                adjency[nodeNumber + sqrtNode].push_back(nodeNumber); // going North
                createMap(nodeNumber + sqrtNode);
            }
            break;
        }
        case 2:
        {
            if (nodeNumber + sqrtNode < maxNode && checkAdjency(nodeNumber + sqrtNode, &adjency[nodeNumber]) == true)
            {
                adjency[nodeNumber].push_back(nodeNumber + sqrtNode);
                adjency[nodeNumber + sqrtNode].push_back(nodeNumber); // going North
                createMap(nodeNumber + sqrtNode);
            }
            if (nodeNumber + 1 < maxNode && (nodeNumber % sqrtNode) < ((nodeNumber + 1) % sqrtNode) && checkAdjency(nodeNumber + 1, &adjency[nodeNumber]) == true)
            {
                adjency[nodeNumber].push_back(nodeNumber + 1);
                adjency[nodeNumber + 1].push_back(nodeNumber); // going East
                createMap(nodeNumber + 1);
            }
            break;
        }
        case 3:
        {
            if (nodeNumber + sqrtNode < maxNode && checkAdjency(nodeNumber + sqrtNode, &adjency[nodeNumber]) == true)
            {
                adjency[nodeNumber].push_back(nodeNumber + sqrtNode);
                adjency[nodeNumber + sqrtNode].push_back(nodeNumber); // going North
                createMap(nodeNumber + sqrtNode);
            }
            if (nodeNumber + 1 < maxNode && (nodeNumber % sqrtNode) < ((nodeNumber + 1) % sqrtNode) && checkAdjency(nodeNumber + 1, &adjency[nodeNumber]) == true)
            {
                adjency[nodeNumber].push_back(nodeNumber + 1);
                adjency[nodeNumber + 1].push_back(nodeNumber); // going East
                createMap(nodeNumber + 1);
            }
            if (nodeNumber - sqrtNode > 0 && checkAdjency(nodeNumber - sqrtNode, &adjency[nodeNumber]) == true)
            {
                adjency[nodeNumber].push_back(nodeNumber - sqrtNode);
                adjency[nodeNumber - sqrtNode].push_back(nodeNumber); // going South
                createMap(nodeNumber - sqrtNode);
            }
            break;
        }
        default:
            break;
        }
    }
    return 0;
}

vector<vector<int>> *Map::getAdjency() { return &adjency; }

void Map::printMap()
{
    for (int i = 0; i < maxNode; i++)
    {
        cout << "Nodul " << adjency[i][0] << " : ";
        for (int j = 1; j < adjency[i].size(); j++)
            cout << adjency[i][j] << " ";
        cout << endl;
    }
}

struct tempCTP
{
    int lgBus;
    vector<Bus> buses;
    int lgTram;
    vector<Tram> trams;
};

void serialization(CTP vehicles, char *stringSerial)
{
    string text;
    for (int i = 0; i < vehicles.getVehicles().size(); i++)
    {
        for (int j = 0; j < vehicles.getVehicles().at(i)->getRoute()->size(); j++)
        {
            text += std::to_string(vehicles.getVehicles()[i]->getRoute()->at(j)); // ruta
            text += ',';
        }
        text += '|';
        text += vehicles.getVehicles()[i]->getType(); //Type
        text += '|';
        text += vehicles.getVehicles()[i]->getOrientation(); // Orientarea
        text += '|';
        text += std::to_string(vehicles.getVehicles()[i]->getDisabilities()); // disabilitati
        text += '|';
        text += std::to_string(vehicles.getVehicles()[i]->getBack()); // merge inapoi? 0/1
        text += '|';
        text += std::to_string(*vehicles.getVehicles()[i]->getZoneCount()); // zoneCount
        text += '|';
        text += std::to_string(vehicles.getVehicles()[i]->getID()); //ID
        text += '|';
        text += std::to_string(*vehicles.getVehicles()[i]->getSpeed()); // speed
        text += '|';
        text += std::to_string(*vehicles.getVehicles()[i]->getcurrentHold()); // currentHold
        text += '|';
        text += std::to_string(vehicles.getVehicles()[i]->getCapacity()); //Capacity
        text += ']';
    }
    strcpy(stringSerial, text.c_str());
}

void add_set(int fd, fd_set *fds, int *max_fd)
{
    FD_SET(fd, fds);
    if (fd > *max_fd)
        *max_fd = fd;
}

void remove_set(int fd, fd_set *fds, int *max_fd)
{
    FD_CLR(fd, fds);
    if (fd == *max_fd)
        (*max_fd)--;
}

int startingPoint(int sqrtNode)
{
    return 300 - 15 * sqrtNode;
}

int minusHeight(int sqrtNode) { return 60 - (sqrtNode - 5) * 4; }

double horizontalX(int sqrtNode)
{
    switch (sqrtNode)
    {
    case 7:
        return 0.34;
    case 8:
        return 0.3;
    case 9:
        return 0.25;
    case 10:
        return 0.23;
    case 11:
        return 0.21;
    case 12:
        return 0.21;
    case 13:
        return 0.19;
    default:
        return 0.34 - (sqrtNode - 7) * 0.04;
    }
}

double verticalY(int sqrtNode)
{
    switch (sqrtNode)
    {
    case 7:
        return 0.19;
    case 8:
        return 0.16;
    case 9:
        return 0.15;
    case 10:
        return 0.13;
    case 11:
        return 0.11;
    case 12:
        return 0.11;
    case 13:
        return 0.1;
    default:
        return 0.19 - (sqrtNode - 7) * 0.02;
    }
}

void minusX(int sqrtNode, int &x, int &y)
{
    if (sqrtNode == 7 || sqrtNode == 8)
    {
        x = 50;
        y = 30;
    }
    else
    {
        x = 30;
        y = 20;
    }
}
// ---------------------------------------------------------------

int main()
{
    srand(time(NULL));
    CTP vehicles;
    bool choice;
    char IP[20] = "127.0.0.1";
    Map myMap(100);
    int sd, selectStatus, startNodeClient = -1;
    struct sockaddr_in server;

    myMap.createMap(0);
    vehicles.addVehicle(new Tram(30, 30, 1));
    vehicles.addVehicle(new Tram(31, 20, 1));
    vehicles.addVehicle(new Bus(32, 40, 1));
    vehicles.addVehicle(new Bus(33, 40, 1));
    vehicles.addVehicle(new Bus(34, 60, 1));
    vehicles.generateRoutes(myMap.getAdjency(), myMap.sqrtNode);

    if ((sd = socket(AF_INET, SOCK_STREAM, 6)) == -1)
    {
        perror("Eroare: ");
        exit(2);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(3000);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        printf("Eroare la conectare server\n");
        exit(3);
    }

    // for (int i = 0; i < vehicles.getVehicles().size(); i++)
    //     cout << vehicles.getVehicles().at(i)->getRoute()->size() << endl;

    fd_set readfds;
    struct timeval tv;
    int maxFD = sd;

    int w = 3024, h = 1964;
    RenderWindow window;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(VideoMode(w, h, desktop.bitsPerPixel), "Public Mobility Advisor");

    Texture dot, greenDot, horizontalLine, verticalLine, bus, tram, wallpaper;
    Sprite dotSprite, dotGreenSprite, horizontalLineSprite, verticalLineSprite, bus1Sprite, bus2Sprite, bus3Sprite, tram1Sprite, tram2Sprite, wallpaperSprite;
    dot.loadFromFile("red-dot.png");
    greenDot.loadFromFile("greenDot.png");
    horizontalLine.loadFromFile("horizontalLine.png");
    verticalLine.loadFromFile("verticalLine.png");
    bus.loadFromFile("bus.png");
    tram.loadFromFile("tram.png");
    wallpaper.loadFromFile("wallpaper.jpeg");
    dotSprite.setTexture(dot);
    dotGreenSprite.setTexture(greenDot);
    horizontalLineSprite.setTexture(horizontalLine);
    verticalLineSprite.setTexture(verticalLine);
    bus1Sprite.setTexture(bus);
    bus2Sprite.setTexture(bus);
    bus3Sprite.setTexture(bus);
    tram1Sprite.setTexture(tram);
    tram2Sprite.setTexture(tram);
    wallpaperSprite.setTexture(wallpaper);
    dotSprite.setScale(0.05, 0.05);
    dotGreenSprite.setScale(0.07, 0.07);
    wallpaperSprite.setScale(0.8, 0.8);
    wallpaperSprite.setPosition(0, 0);
    window.clear();

    int countI = 0, countJ, counter = myMap.maxNode, counterDot = 0;
    vector<vector<int>> coords;
    for (int i = 0; i < myMap.maxNode; i++)
        coords.push_back({i});

    horizontalLineSprite.setColor(Color::Blue);
    verticalLineSprite.setColor(Color::Blue);
    for (int i = startingPoint(myMap.sqrtNode); i <= h; i += h / myMap.sqrtNode - minusHeight(myMap.sqrtNode))
    {
        countI++;
        countJ = 0;
        int aux = counter - myMap.sqrtNode;
        for (int j = startingPoint(myMap.sqrtNode); j <= w; j += w / myMap.sqrtNode)
        {
            coords[aux].push_back(j);
            coords[aux].push_back(i); //luam coordonatele punctelor  (x,y)
            ++aux;
            countJ++;
            if (countJ >= myMap.sqrtNode) //crearea punctelor
                break;
        }
        if (countI >= myMap.sqrtNode)
            break;
        counter -= myMap.sqrtNode;
    }

    horizontalLineSprite.setScale(horizontalX(myMap.sqrtNode), 0.6); // 9 -> 0.25/0.6
    verticalLineSprite.setScale(0.9, verticalY(myMap.sqrtNode));     // 9->0.9/0.15

    bus1Sprite.setScale(0.1, 0.1);
    bus2Sprite.setScale(0.1, 0.1);
    bus3Sprite.setScale(0.1, 0.1);
    tram1Sprite.setScale(0.35, 0.35);
    tram2Sprite.setScale(0.35, 0.35);

    int minusHorizontalX, minusVerticalY;
    minusX(myMap.sqrtNode, minusHorizontalX, minusVerticalY);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        while (1)
        {
            window.clear();
            window.draw(wallpaperSprite);
            for (int i = 0; i < myMap.maxNode; i++)
            {

                for (int j = 1; j < (*myMap.getAdjency())[i].size(); j++)
                {
                    if ((*myMap.getAdjency())[i][j] - i == 1 && (i + 1) % myMap.sqrtNode != 0)
                    {
                        horizontalLineSprite.setPosition(coords[i][1] - minusHorizontalX, coords[i][2] - 120); //-30, -120
                        window.draw(horizontalLineSprite);
                    }
                    if ((i - (*myMap.getAdjency())[i][j]) % myMap.sqrtNode == 0 && i >= myMap.sqrtNode)
                    {
                        verticalLineSprite.setPosition(coords[i][1] - 190, coords[i][2] - minusVerticalY);
                        window.draw(verticalLineSprite);
                    }
                }
            }

            countI = 0;
            int countNodeClient = myMap.maxNode;
            for (int i = startingPoint(myMap.sqrtNode); i <= h; i += h / myMap.sqrtNode - minusHeight(myMap.sqrtNode))
            {
                countNodeClient -= myMap.sqrtNode;
                countI++;
                countJ = 0;
                for (int j = startingPoint(myMap.sqrtNode); j <= w; j += w / myMap.sqrtNode)
                {
                    if (countNodeClient == startNodeClient && counterDot < 5)
                    {
                        dotGreenSprite.setPosition(j, i);
                        window.draw(dotGreenSprite);
                        ++counterDot;
                    }
                    else
                    {
                        dotSprite.setPosition(j, i);
                        window.draw(dotSprite);
                    }
                    countNodeClient++;
                    countJ++;
                    if (countJ >= myMap.sqrtNode) //crearea punctelor
                        break;
                }
                if (countI >= myMap.sqrtNode)
                    break;
                countNodeClient -= myMap.sqrtNode;
            }
            if (counterDot == 5)
            {
                startNodeClient = -1;
                counterDot = 0;
            }
            int number = (*vehicles.getVehicles()[0]->getRoute())[*vehicles.getVehicles()[0]->getZoneCount()];
            bus1Sprite.setPosition(coords[number][1] - 70, coords[number][2] - 40);

            number = (*vehicles.getVehicles()[1]->getRoute())[*vehicles.getVehicles()[1]->getZoneCount()];
            bus2Sprite.setPosition(coords[number][1] - 70, coords[number][2] - 40);

            number = (*vehicles.getVehicles()[2]->getRoute())[*vehicles.getVehicles()[2]->getZoneCount()];
            bus3Sprite.setPosition(coords[number][1] - 70, coords[number][2] - 40);

            number = (*vehicles.getVehicles()[3]->getRoute())[*vehicles.getVehicles()[3]->getZoneCount()];
            tram1Sprite.setPosition(coords[number][1] - 140, coords[number][2] - 120);

            number = (*vehicles.getVehicles()[4]->getRoute())[*vehicles.getVehicles()[4]->getZoneCount()];
            tram2Sprite.setPosition(coords[number][1] - 140, coords[number][2] - 120);

            window.draw(bus1Sprite);
            window.draw(bus2Sprite);
            window.draw(bus3Sprite);
            window.draw(tram1Sprite);
            window.draw(tram2Sprite);

            window.display();
            FD_ZERO(&readfds);
            add_set(sd, &readfds, &maxFD);
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            for (int i = 0; i < vehicles.getVehicles().size(); i++)
            {
                choice = rand() % 2;
                unsigned int *aux = vehicles.getVehicles().at(i)->getZoneCount();
                bool goingBack = vehicles.getVehicles().at(i)->getBack();
                if (goingBack == false)
                {
                    if (choice == 1)
                    {
                        (*vehicles.getVehicles().at(i)->getSpeed()) = rand() % 50;
                        int goingIn = rand() % 10, goingOut = rand() % 5, *occupation = vehicles.getVehicles().at(i)->getcurrentHold();
                        if ((*occupation) + goingIn - goingOut >= 0 && (*occupation) + goingIn - goingOut <= vehicles.getVehicles().at(i)->getCapacity())
                            (*occupation) += goingIn - goingOut;
                        if ((*aux) < vehicles.getVehicles().at(i)->getRoute()->size())
                            ++(*aux);
                        if ((*aux) == vehicles.getVehicles().at(i)->getRoute()->size())
                        {
                            vehicles.getVehicles().at(i)->setOrientation();
                            vehicles.getVehicles().at(i)->setBack(true);
                            --(*aux);
                        }
                        else
                            vehicles.getVehicles().at(i)->setOrientation();
                        if (vehicles.getVehicles().at(i)->getOrientation() == "Stationary")
                        {
                            (*occupation) = (*occupation) / 2;
                            *vehicles.getVehicles().at(i)->getSpeed() = 0;
                        }
                    }
                }

                else

                    if (choice == 1)
                {
                    *vehicles.getVehicles().at(i)->getSpeed() = rand() % 50;
                    int goingIn = rand() % 15, goingOut = rand() % 5, *occupation = vehicles.getVehicles().at(i)->getcurrentHold();
                    if ((*occupation) + goingIn - goingOut >= 0 && (*occupation) + goingIn - goingOut <= vehicles.getVehicles().at(i)->getCapacity())
                        (*occupation) += goingIn - goingOut;
                    if ((*aux) > 0)
                        --(*aux);

                    if ((*aux) == 0)
                    {
                        vehicles.getVehicles().at(i)->setOrientation();
                        vehicles.getVehicles().at(i)->setBack(false);
                    }
                    else
                        vehicles.getVehicles().at(i)->setOrientation();
                    if (vehicles.getVehicles().at(i)->getOrientation() == "Stationary")
                    {
                        (*occupation) = (*occupation) / 2;
                        *vehicles.getVehicles().at(i)->getSpeed() = 0;
                    }
                }

                // vehicles.printRoutes();
                std::vector<int> station = *vehicles.getVehicles()[i]->getRoute();
                int stationNumber = station[*vehicles.getVehicles()[i]->getZoneCount()];
                cout
                    << "ID : " << vehicles.getVehicles().at(i)->getID() << ", statia " << *aux << "(" << stationNumber
                    << ")"
                    << ", orientare: "
                    << vehicles.getVehicles().at(i)->getOrientation()
                    << ", persoane: " << *vehicles.getVehicles().at(i)->getcurrentHold() << ", asistance: " << vehicles.getVehicles()[i]->getAsistance() << endl;
            }
            cout << "--------------\n";
            if (select(maxFD + 1, &readfds, NULL, NULL, &tv) < 0)
                perror("select");
            if (FD_ISSET(sd, &readfds))
            {
                char aux[100];
                string s;
                read(sd, &aux, sizeof(aux));
                int i = 0;
                while (aux[i] != '|')
                {
                    s += aux[i];
                    ++i;
                }
                int trans = std::stoi(s);
                s.clear();
                ++i;
                while (aux[i] != '|')
                {
                    s += aux[i];
                    ++i;
                }
                startNodeClient = std::stoi(s);
                i = 0;
                while (i < vehicles.getVehicles().size())
                {

                    if (vehicles.getVehicles().at(i)->getID() == trans)
                    {
                        vehicles.getVehicles().at(i)->setAsistance(true);
                    }
                    ++i;
                }
            }
            vehicles.printRoutes();
            char full[1000];
            serialization(vehicles, full);
            write(sd, full, sizeof(full));
            sleep(2);
        }
    }
}
