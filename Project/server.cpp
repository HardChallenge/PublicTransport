#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <algorithm>
using std::vector, std::string;

unsigned int counting = 0;

void get_line(char *buffer, int size)
{
    int ch = getchar();
    if (ch == '\n')
        ch = getchar();
    if (ch == EOF)
        printf("Eroare la getchar()\n");
    else
    {
        int i;
        for (i = 0; i < size - 1; i++)
        {
            if (ch == '\n' || ch == EOF)
                break;
            buffer[i] = ch;
            ch = getchar();
        }
        buffer[i] = '\0';
    }
}

struct tempCTP
{
    vector<int> route;
    string type, orientation;
    unsigned int zoneCount;
    bool disabilities, goBack;
    int ID, speed, currentHold;
    double capacity;
};

void deserialization(char *text, vector<tempCTP> &temporary)
{
    int i = 0;
    string s;
    while (text[i] != '\0')
    {
        tempCTP temp;
        while (text[i] != '|')
        {
            while (text[i] != ',')
            {
                s += text[i]; // route
                i++;
            }
            temp.route.push_back(std::stoi(s));
            s.clear();
            i += 1;
        }
        i += 1;
        s.clear();
        while (text[i] != '|')
        {
            s += text[i]; // type
            ++i;
        }
        temp.type = s;
        s.clear();
        ++i;
        while (text[i] != '|')
        {
            s += text[i];
            ++i;
        }
        temp.orientation = s; // orientarea
        ++i;
        s.clear();

        while (text[i] != '|')
        {
            s += text[i];
            ++i;
        }
        temp.disabilities = std::stoi(s); //disabilities
        ++i;
        s.clear();

        while (text[i] != '|')
        {
            s += text[i];
            ++i;
        }
        temp.goBack = std::stoi(s); //goBack
        ++i;
        s.clear();

        while (text[i] != '|')
        {
            s += text[i];
            ++i;
        }
        temp.zoneCount = std::stoi(s); //zoneCount
        ++i;
        s.clear();

        //ID
        while (text[i] != '|')
        {
            s += text[i];
            ++i;
        }
        temp.ID = std::stoi(s);
        ++i;
        s.clear();

        //Speed
        while (text[i] != '|')
        {
            s += text[i];
            ++i;
        }
        temp.speed = std::stoi(s);
        ++i;
        s.clear();

        //currentHold
        while (text[i] != '|')
        {
            s += text[i];
            ++i;
        }
        temp.currentHold = std::stoi(s);
        ++i;
        s.clear();

        //capacity
        while (text[i] != ']')
        {
            s += text[i];
            ++i;
        }
        temp.capacity = std::stoi(s);
        ++i;
        s.clear();

        temporary.push_back(temp);
    }
}

int max(int a, int b)
{
    if (a == b)
        return a;
    else if (a > b)
        return a;
    else
        return b;
}

void printTemp(vector<tempCTP> temporary)
{
    for (int i = 0; i < temporary.size(); i++)
    {
        std::cout << "ID : " << temporary[i].ID << ", orientation: " << temporary[i].orientation << ", persoane: " << temporary[i].currentHold << ", statia: " << temporary[i].zoneCount << std::endl;
    }
    printf("----------------\n");
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

void decode(char *text, int &start, int &end, bool &assistance)
{
    string s;
    int i = 0;
    while (text[i] != ',')
    {
        s += text[i];
        ++i;
    }
    start = std::stoi(s);
    s.clear();
    ++i;
    while (text[i] != ',')
    {
        s += text[i];
        ++i;
    }
    end = std::stoi(s);
    s.clear();
    ++i;
    assistance = std::atoi(&text[i]);
}

int main()
{
    // Child Transport = CT, // Child Client = CC
    pid_t childTransport, childClient;
    int ServerToCT[2], CTtoServer[2], ServerToCC[2], CCtoServer[2]; // pipes
    pipe(ServerToCT);
    pipe(CTtoServer);
    pipe(ServerToCC);
    pipe(CCtoServer);
    childTransport = fork();

    if (childTransport == 0)
    {
        // ChildTransport
        close(ServerToCC[0]);
        close(ServerToCC[1]);
        close(CCtoServer[1]);
        close(CCtoServer[0]);

        vector<tempCTP> temporary;
        int sdTransport, clientTransport = 0;
        struct sockaddr_in childServer, Transport;
        struct timeval tv;

        if ((sdTransport = socket(AF_INET, SOCK_STREAM, 6)) == -1)
        {
            printf("Eroare la creare socket.\n");
            exit(2);
        }
        int aux = 1;
        if (setsockopt(sdTransport, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(aux)) == -1)
            printf("Eroare la setsockopt()\n");
        childServer.sin_family = AF_INET;
        childServer.sin_addr.s_addr = htonl(INADDR_ANY);
        childServer.sin_port = htons(3000);
        if (bind(sdTransport, (struct sockaddr *)&childServer, sizeof(struct sockaddr)) == -1)
        {
            perror("Eroare la bind: \n");
            exit(3);
        }

        if (listen(sdTransport, 5) == -1)
        {
            printf("Eroare la listen\n");
            exit(4);
        }

        fd_set readfds;
        int max_fd = 0;

        while (1)
        {
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            FD_ZERO(&readfds);
            if (clientTransport == 0)
                add_set(sdTransport, &readfds, &max_fd);
            else
            {
                add_set(clientTransport, &readfds, &max_fd);
                add_set(ServerToCT[0], &readfds, &max_fd);
            }
            if (select(max_fd + 1, &readfds, NULL, NULL, &tv) < 0)
                perror("select");

            if (FD_ISSET(sdTransport, &readfds))
            {
                unsigned int len = sizeof(Transport);
                clientTransport = accept(sdTransport, (struct sockaddr *)&Transport, &len);
                printf("A venit primul transport\n");
                if (clientTransport < 0)
                {
                    perror("[server] Eroare la accept().\n");
                    //continue;
                }
                char fullText[1000];
                int readbytes;
                readbytes = read(clientTransport, &fullText, sizeof(fullText));
                temporary.clear();
                deserialization(fullText, temporary);
                // printTemp(temporary);
            }

            if (FD_ISSET(clientTransport, &readfds))
            {
                printf("A mai venit un transport\n");
                char fullText[1000];
                int readbytes;
                readbytes = read(clientTransport, &fullText, sizeof(fullText));
                temporary.clear();
                deserialization(fullText, temporary);
                // printTemp(temporary);
            }
            if (FD_ISSET(ServerToCT[0], &readfds))
            {
                int start, end;
                bool assistance;
                char aux[100];
                read(ServerToCT[0], &aux, sizeof(aux));
                printf("CT: Am citit mesajul -> %s\n", aux);
                decode(aux, start, end, assistance);
                int answer[temporary.size()]; // maximul de autobuze care pot fi disponibile
                vector<tempCTP> answer2;
                int count = 0;
                if (assistance == 1)
                {
                    for (int i = 0; i < temporary.size(); i++)
                        if (std::find(temporary[i].route.begin(), temporary[i].route.end(), start) != temporary[i].route.end() &&
                            std::find(temporary[i].route.begin(), temporary[i].route.end(), end) != temporary[i].route.end() &&
                            assistance == temporary[i].disabilities)
                        {
                            answer[count++] = temporary[i].ID;
                            answer2.push_back(temporary[i]);
                        }
                }
                else
                {
                    for (int i = 0; i < temporary.size(); i++)
                        if (std::find(temporary[i].route.begin(), temporary[i].route.end(), start) != temporary[i].route.end() &&
                            std::find(temporary[i].route.begin(), temporary[i].route.end(), end) != temporary[i].route.end())
                        {
                            answer[count++] = temporary[i].ID;
                            answer2.push_back(temporary[i]);
                        }
                }

                if (count == 0)
                {
                    char tempMSG[100] = "Nu exista nici un mijloc de transport disponibil.";
                    write(CTtoServer[1], &tempMSG, sizeof(tempMSG));
                }
                else if (count == 1)
                {
                    char tempMSG[100] = {'\0'};
                    string s = std::to_string(answer[0]);
                    strcat(tempMSG, s.c_str());
                    strcat(tempMSG, "|");
                    write(CTtoServer[1], &tempMSG, sizeof(tempMSG));
                    s.clear();
                    s += std::to_string(start);
                    s += "|";
                    strcat(tempMSG, s.c_str());
                    write(clientTransport, &tempMSG, sizeof(tempMSG));
                }
                else if (count > 1)
                {
                    //vedem care e mai aproape;
                    unsigned int min = 1000000, minPossible;
                    int posMin = 0;
                    for (int i = 0; i < answer2.size(); i++)
                    {
                        minPossible = -1;
                        int currentClientStation = 0;
                        int isForward = -2;
                        while (answer2[i].route[currentClientStation] != start) //
                        {
                            if (answer2[i].route[answer2[i].zoneCount] == answer2[i].route[currentClientStation])
                                isForward = -1; // mijlocul de transport e in spatele clientului (mai are pana vine la el)
                            ++currentClientStation;
                        }
                        if (isForward == -2)
                        {
                            if (currentClientStation == answer2[i].zoneCount)
                                isForward = 0;
                            else
                                isForward = 1;
                        }

                        if (isForward == 0)
                        {
                            minPossible = 0;
                        }
                        else if (isForward == -1)
                        {
                            if (answer2[i].goBack == 0)
                                minPossible = currentClientStation - answer2[i].zoneCount;
                            else
                                minPossible = answer2[i].zoneCount + (currentClientStation - answer2[i].zoneCount);
                        }
                        else
                        {
                            if (answer2[i].goBack == 0)
                                minPossible = (answer2[i].route.size() - answer2[i].zoneCount) + (answer2[i].route.size() - currentClientStation);
                            else
                            {
                                minPossible = answer2[i].zoneCount - currentClientStation;
                            }
                        }
                        if (minPossible < min)
                        {
                            min = minPossible;
                            posMin = i;
                        }
                    }

                    char tempMSG[100] = {'\0'};
                    string s = std::to_string(answer2[posMin].ID);
                    strcat(tempMSG, s.c_str());
                    strcat(tempMSG, "|");
                    write(CTtoServer[1], &tempMSG, sizeof(tempMSG));
                    s.clear();
                    s += std::to_string(start);
                    s += "|";
                    strcat(tempMSG, s.c_str());
                    write(clientTransport, &tempMSG, sizeof(tempMSG));
                }
                // Procesam ce comanda vine de la server (serverul primeste comanda de la CC)
                // Daca primim si disabilities = 1 ii scriem transportul lui tt
            }
        }
        close(sdTransport);
        //EndChildTransport
    }
    else
    {
        childClient = fork();
        if (childClient == 0)
        {
            //ChildClient
            close(CTtoServer[0]);
            close(CTtoServer[1]);
            close(ServerToCT[1]);
            close(ServerToCT[0]);
            int sdClient;
            struct sockaddr_in childClient, client;
            struct timeval tvClient;

            if ((sdClient = socket(AF_INET, SOCK_STREAM, 6)) == -1)
            {
                printf("Eroare la creare socket.\n");
                exit(2);
            }
            int aux = 1;
            if (setsockopt(sdClient, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(aux)) == -1)
                printf("Eroare la setsockopt()\n");

            childClient.sin_family = AF_INET;
            childClient.sin_addr.s_addr = htonl(INADDR_ANY);
            childClient.sin_port = htons(4000); // PORT 4000

            if (bind(sdClient, (struct sockaddr *)&childClient, sizeof(struct sockaddr)) == -1)
            {
                perror("Eroare la bind: \n");
                exit(3);
            }

            if (listen(sdClient, 5) == -1)
            {
                printf("Eroare la listen\n");
                exit(4);
            }

            fd_set readfds;
            int clientDescriptor = 0;
            int nfds = 0;

            while (1)
            {
                tvClient.tv_sec = 2;
                tvClient.tv_usec = 0;
                FD_ZERO(&readfds);
                if (clientDescriptor == 0)
                    add_set(sdClient, &readfds, &nfds);
                else
                {
                    add_set(clientDescriptor, &readfds, &nfds);
                    add_set(ServerToCC[0], &readfds, &nfds);
                }
                if (select(nfds + 1, &readfds, NULL, NULL, &tvClient) < 0)
                    perror("select");

                if (FD_ISSET(sdClient, &readfds))
                {
                    unsigned int len = sizeof(client);
                    clientDescriptor = accept(sdClient, (struct sockaddr *)&client, &len);
                    if (clientDescriptor < 0)
                    {
                        perror("[server] Eroare la accept().\n");
                    }
                    add_set(clientDescriptor, &readfds, &nfds);
                }
                if (FD_ISSET(clientDescriptor, &readfds))
                {
                    char aux[100];
                    read(clientDescriptor, &aux, sizeof(aux));
                    printf("CC : Am citit mesajul de la client %s\n", aux);
                    write(CCtoServer[1], &aux, sizeof(aux));
                }
                if (FD_ISSET(ServerToCC[0], &readfds))
                {
                    char aux[100];
                    read(ServerToCC[0], &aux, sizeof(aux));
                    if (aux[0] != 'N')
                        aux[2] = '\0';
                    write(clientDescriptor, &aux, sizeof(aux));
                    clientDescriptor = 0;
                }
            }
            //endChildClient
        }
        else
        {
            //server
            close(ServerToCT[0]);
            close(ServerToCC[0]);
            close(CTtoServer[1]);
            close(CCtoServer[1]);
            fd_set readServer;
            struct timeval tvServer;

            int maxFD = CTtoServer[0];
            while (1)
            {
                FD_ZERO(&readServer);
                add_set(CTtoServer[0], &readServer, &maxFD);
                add_set(CCtoServer[0], &readServer, &maxFD);
                tvServer.tv_sec = 3;
                tvServer.tv_usec = 0;
                int selectStatus = select(maxFD + 1, &readServer, NULL, NULL, &tvServer);
                if (selectStatus < 0)
                    printf("Eroare la select");
                if (FD_ISSET(CTtoServer[0], &readServer))
                {
                    char aux[100];
                    read(CTtoServer[0], &aux, sizeof(aux));
                    write(ServerToCC[1], &aux, sizeof(aux));
                }
                if (FD_ISSET(CCtoServer[0], &readServer))
                {
                    char aux[100];
                    read(CCtoServer[0], &aux, sizeof(aux));
                    write(ServerToCT[1], &aux, sizeof(aux));
                }
            }
            //endServer
        }
    }
}

/*



*/
