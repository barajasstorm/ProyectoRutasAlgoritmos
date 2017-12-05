//
//  pathsolver.cpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#include "pathsolver.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "interest_points.hpp"



using namespace std;

Pathsolver::Pathsolver(sql::Connection *connection) : connection(connection) {
    loadIndividualLines();
    loadAllStations(&allStations);
    loadAllStations(&allStationsOrdered);
    quickSort(&allStationsOrdered, 0, (int)allStationsOrdered.size() - 1);
}

Pathsolver::Pathsolver(sql::Connection *connection, string inicio, string fin,int** matrizM,int** matrizT) : connection(connection) {
    bool failure = false;
    
    this->matrizM = matrizM;
    this->matrizT = matrizT;
    
    loadIndividualLines();
    loadAllStations(&allStations);
    loadAllStations(&allStationsOrdered);
    quickSort(&allStationsOrdered, 0, (int)allStationsOrdered.size() - 1);

    do{
        if(binarySearch(allStationsOrdered, inicio) != -1) {
            int index = binarySearch(allStationsOrdered, inicio);
            if (allStationsOrdered.at(index).getTipo() == -1) {
                
                //If station is not a connection point
                this->inicio.setPk_estacionid(allStationsOrdered.at(index).getPk_estacionid());
                this->inicio.setLinea(allStationsOrdered.at(index).getLinea());
                this->inicio.setNumero(allStationsOrdered.at(index).getNumero());
                this->inicio.setNombre(allStationsOrdered.at(index).getNombre());
                this->inicio.setStatus(allStationsOrdered.at(index).getStatus());
                this->inicio.setTipo(-1);
            } else {
                this->inicio.setPk_estacionid(allStationsOrdered.at(index).getPk_estacionid());
                this->inicio.setLinea(allStationsOrdered.at(index).getLinea());
                this->inicio.setNumero(allStationsOrdered.at(index).getNumero());
                this->inicio.setNombre(allStationsOrdered.at(index).getNombre());
                this->inicio.setStatus(allStationsOrdered.at(index).getStatus());
                this->inicio.setTipo(allStationsOrdered.at(index).getTipo());
                
            }
            failure = false;
        } else {
            cout << "Punto de inicio no encontrado. Favor de intentar de nuevo." << endl;
            failure = true;
            cout << "Indicar estacion de incio: ";
            getline(cin, inicio);
            cout << endl;
        }
    }while(failure == true);
    
    do{
        if(binarySearch(allStationsOrdered, fin, 0, (int)allStationsOrdered.size() - 1) != -1) {
            int index = binarySearch(allStationsOrdered, fin, 0, (int)allStationsOrdered.size() - 1);
            if (allStationsOrdered.at(index).getTipo() == -1) {
                
                //If station is not a connection point
                this->fin.setPk_estacionid(allStationsOrdered.at(index).getPk_estacionid());
                this->fin.setLinea(allStationsOrdered.at(index).getLinea());
                this->fin.setNumero(allStationsOrdered.at(index).getNumero());
                this->fin.setNombre(allStationsOrdered.at(index).getNombre());
                this->fin.setStatus(allStationsOrdered.at(index).getStatus());
                this->fin.setTipo(-1);
            } else {
                //If it is a connection point
                this->fin.setPk_estacionid(allStationsOrdered.at(index).getPk_estacionid());
                this->fin.setLinea(allStationsOrdered.at(index).getLinea());
                this->fin.setNumero(allStationsOrdered.at(index).getNumero());
                this->fin.setNombre(allStationsOrdered.at(index).getNombre());
                this->fin.setStatus(allStationsOrdered.at(index).getStatus());
                this->fin.setTipo(allStationsOrdered.at(index).getTipo());
                
            }
            failure = false;
        } else {
            cout << "Punto de fin no encontrado. Favor de intentar de nuevo." << endl;
            failure = true;
            cout << "Indicar estacion final: ";
            getline(cin, fin);
            cout << endl;
        }
    }while(failure == true);
    
    //Solve best route
    loadInterestPoints();
    solve();
    
}

void Pathsolver::solve() {
    
    //If they are both on the same line
    if(sameLineVerify()) {
        
        int start = inicio.getPk_estacionid();
        int end = fin.getPk_estacionid();
        bool floydNeeded = false;
        bool reverse = ((start < end) ? false : true);

        //Add stations to ruta list
        floydNeeded = sameLineSolve(&this->ruta, &start, &end, &reverse);

        //If floyd is needed, then call floyd function
        if(floydNeeded) {
            //If connection point is the same
            if(start == end) {
                pointerRuta = ruta.begin();
                for(int i = 0; i < insertIndex; i++) pointerRuta++;
                ruta.insert(--pointerRuta, allStations.at(start));
            } else {
                 floyd(&this->ruta, start, end, &this->insertIndex);
            }
        }
        printBestRoute(&this->ruta);
    } else {
        //If they are on different lines
        beginLeftFinalCount[0] = 0;
        beginLeftFinalCount[1] = 0;
        beginRightFinalCount[0] = 0;
        beginRightFinalCount[1] = 0;
        list<Estaciones> rutaOptima = *differentLineSolve();
        printBestRoute(&rutaOptima);
    }
    
}

list<Estaciones>* Pathsolver::differentLineSolve() {
    beginRoute[0] = false;
    beginRoute[1] = false;
    endRoute[0] = false;
    endRoute[1] = false;
    Estaciones beginRouteFloyd[2];
    Estaciones endRouteFloyd[2];

    if(inicio.getTipo() == -1) {
        
        /* --------------------- Begin left -------------------------  */
        int start = inicio.getPk_estacionid();
        while(allStations.at(start).getTipo() == -1 && allStations.at(start).getLinea() == inicio.getLinea()) {
            beginLeftFinal[0].push_back(allStations.at(start));
            beginLeftFinal[1].push_back(allStations.at(start));
            beginLeftFinalCount[0]++;
            beginLeftFinalCount[1]++;
            start--;
            if(start < 0 || start > allStations.size()) break;
            if(allStations.at(start).getTipo() != -1) beginRoute[0] = true;
        }
        
        //If begin left is a valid route, then save left connection point
        if(beginRoute[0]) beginRouteFloyd[0] = allStations.at(start);

        /* --------------------  Begin Right ------------------------  */
        start = inicio.getPk_estacionid();
        while(allStations.at(start).getTipo() == -1 && allStations.at(start).getLinea() == inicio.getLinea()) {
            beginRightFinal[0].push_back(allStations.at(start));
            beginRightFinal[1].push_back(allStations.at(start));
            beginRightFinalCount[0]++;
            beginRightFinalCount[1]++;
            start++;
            if(start < 0 || start > allStations.size()) break;
            if(allStations.at(start).getTipo() != -1) beginRoute[1] = true;
            
        }
        
        //If begin right is a valid route, then save right connection point
        if(beginRoute[1]) beginRouteFloyd[1] = allStations.at(start);
        
        /* ---------------------- End Left --------------------------- */
        //Create insertion pointers for End left
        indexone = (int)beginLeftFinal[0].size();
        pointerRuta = beginLeftFinal[0].begin();
        for(int i = 0; i < beginLeftFinal[0].size(); i++) pointerRuta++;
        pointerRuta++;
        
        indexthree = (int)beginRightFinal[0].size();
        pointerRutaSecond = beginRightFinal[0].begin();
        for(int i = 0; i < beginRightFinal[0].size(); i++) pointerRutaSecond++;
        pointerRutaSecond++;
        
        //Insert end left route to beginleft and begin right
        int end = fin.getPk_estacionid();
        while(allStations.at(end).getTipo() == -1 && allStations.at(end).getLinea() == fin.getLinea()) {
            if(beginRoute[0]) beginLeftFinal[0].insert(--pointerRuta, allStations.at(end));
            if(beginRoute[1]) beginRightFinal[0].insert(--pointerRutaSecond, allStations.at(end));
            beginLeftFinalCount[0]++;
            beginRightFinalCount[0]++;
            end--;
            if(end < 0 || end > allStations.size()) break;
            if(allStations.at(end).getTipo() != -1) endRoute[0] = true;
        }
        
        //If end left is a valid route, then save left connection point
        if(endRoute[0]) endRouteFloyd[0] = allStations.at(end);
        
        /* ---------------------  End Right  --------------------------  */
        //Create insertion pointers for End Right
        indextwo = (int)beginLeftFinal[1].size();
        pointerRuta = beginLeftFinal[1].begin();
        for(int i = 0; i < beginLeftFinal[1].size(); i++) pointerRuta++;
        pointerRuta++;
        
        indexfour = (int)beginRightFinal[1].size();
        pointerRutaSecond = beginRightFinal[1].begin();
        for(int i = 0; i < beginRightFinal[1].size(); i++) pointerRutaSecond++;
        pointerRutaSecond++;
        
        //End Right
        end = fin.getPk_estacionid();
        while(allStations.at(end).getTipo() == -1 && allStations.at(end).getLinea() == fin.getLinea()) {
            if(beginRoute[0]) beginLeftFinal[1].insert(--pointerRuta, allStations.at(end));
            if(beginRoute[1]) beginRightFinal[1].insert(--pointerRutaSecond, allStations.at(end));
            beginLeftFinalCount[1]++;
            beginRightFinalCount[1]++;
            end++;
            if(end < 0 || end > allStations.size()) break;
            if(allStations.at(end).getTipo() != -1) endRoute[1] = true;
        }
        
        //If end right is a valid route, then save right connection point
        if(endRoute[1]) endRouteFloyd[1] = allStations.at(end);
        /*--------------------------------------------------------------------*/
        
        
        //Do floyd for all valid routes
        //end left / begin left
        if(endRoute[0] && beginRoute[0]) {
            if(endRouteFloyd[0].getTipo() == beginRouteFloyd[0].getTipo()) {
                //Create pointer for insertion point
                pointerRuta = beginLeftFinal[0].begin();
                for(int i = 0; i < indexone; i++) pointerRuta++;
                pointerRuta++;
                
                //Insert floyd route into list
                beginLeftFinal[0].insert(--pointerRuta, endRouteFloyd[0]);
                beginLeftFinalCount[0]++;
                
            } else {
                floyd(&beginLeftFinal[0], beginRouteFloyd[0].getPk_estacionid(), endRouteFloyd[0].getPk_estacionid(), &indexone, &beginLeftFinalCount[0]);
            }
        }
        //end right / begin left
        if(endRoute[1] && beginRoute[0]) {
            if(endRouteFloyd[1].getTipo() == beginRouteFloyd[0].getTipo()) {
                //Create pointer for insertion point
                pointerRuta = beginLeftFinal[1].begin();
                for(int i = 0; i < indextwo; i++) pointerRuta++;
                pointerRuta++;
                
                //Insert floyd route into list
                beginLeftFinal[1].insert(--pointerRuta, endRouteFloyd[1]);
                beginLeftFinalCount[1]++;
                
            } else {
                floyd(&beginLeftFinal[1], beginRouteFloyd[0].getPk_estacionid(), endRouteFloyd[1].getPk_estacionid(), &indextwo, &beginLeftFinalCount[1]);
            }
        }
        //end left / begin right
        if(endRoute[0] && beginRoute[1]) {
            if(endRouteFloyd[0].getTipo() == beginRouteFloyd[1].getTipo()) {
                //Create pointer for insertion point
                pointerRuta = beginRightFinal[0].begin();
                for(int i = 0; i < indexthree; i++) pointerRuta++;
                pointerRuta++;
                
                //Insert floyd route into list
                beginRightFinal[0].insert(--pointerRuta, endRouteFloyd[0]);
                beginRightFinalCount[0]++;
                
            } else {
                floyd(&beginRightFinal[0], beginRouteFloyd[1].getPk_estacionid(), endRouteFloyd[0].getPk_estacionid(), &indexthree, &beginRightFinalCount[0]);
            }
        }
        //end right / begin right
        if(endRoute[1] && beginRoute[1]) {
            if(endRouteFloyd[1].getTipo() == beginRouteFloyd[1].getTipo()) {
                //Create pointer for insertion point
                pointerRuta = beginRightFinal[1].begin();
                for(int i = 0; i < indexfour; i++) pointerRuta++;
                pointerRuta++;
                
                //Insert floyd route into list
                beginRightFinal[1].insert(--pointerRuta, endRouteFloyd[1]);
                beginRightFinalCount[1]++;
                
            } else {
                floyd(&beginRightFinal[1], beginRouteFloyd[1].getPk_estacionid(), endRouteFloyd[1].getPk_estacionid(), &indexthree, &beginRightFinalCount[1]);
            }
        }
        
        
    }
    
    
    
    //Create minimum variable
    int minimo = 99;
    
    if(beginRoute[0] && endRoute[0]) {
        if(beginLeftFinalCount[0] < minimo) {
            rutaOptima = beginLeftFinal[0];
            minimo = beginLeftFinalCount[0];
        }
    }
    
    if(beginRoute[1] && endRoute[0]) {
        if(beginRightFinalCount[0] < minimo) {
            rutaOptima = beginRightFinal[0];
            minimo = beginRightFinalCount[0];
        }
    }
    
    if(beginRoute[0] && endRoute[1]) {
        if(beginLeftFinalCount[1] < minimo) {
            rutaOptima = beginLeftFinal[1];
            minimo = beginLeftFinalCount[1];
        }
    }
    
    if(beginRoute[1] && endRoute[1]) {
        if(beginRightFinalCount[1] < minimo) {
            rutaOptima = beginRightFinal[1];
        }
    }
    
    return &rutaOptima;
}

bool Pathsolver::sameLineSolve(list<Estaciones>* lista, int* start, int* end, bool* reverse) {
    bool floydFlag = false;
    int startLocal = *start;
    int endLocal = *end;
    insertIndex = 0;

    while ((startLocal != endLocal) && (floydFlag == false)) {
        //If it is not a connection point
        if(allStations.at(startLocal).getTipo() == -1) {
            
            //Insert station into list
            lista->push_back(allStations.at(startLocal));
            insertIndex++;
            
            //Add / Subtract from position
            *start = ((*reverse) ? --*start : ++*start);
            startLocal = ((*reverse) ? --startLocal : ++startLocal);
            
        } else {
            //If it is a connection point
            floydFlag = true;
        }
    }
    //insertIndex = insertIndex + cont;
    if(floydFlag) {
        //Create pointer to insertion point
        pointerRuta = lista->begin();
        for(int i = 0; i < insertIndex; i++) pointerRuta++;
        pointerRuta++;
        
        //Start traveling the opposite direction until finding crosspoint
        bool connectionPoint = false;
        while ((endLocal != startLocal) && (!connectionPoint)) {
            if(allStations.at(endLocal).getTipo() == -1) {
                
                //Insert station into list
                lista->insert(--pointerRuta, allStations.at(endLocal));

                //Add / Subtract position
                *end = ((!*reverse) ? --*end : ++*end);
                endLocal = ((!*reverse) ? --endLocal : ++endLocal);
                
            } else {
                connectionPoint = true;
            }
        }
    }
    //insertIndex = insertIndex - cont;
    return floydFlag;
}

//Return traveling direction
string Pathsolver::getDirection(int linea, int start, int end) {
    string direction;
    
        if (linea == 1) direction = (start > end) ? linea1.front().getNombre() : linea1.back().getNombre();
        if (linea == 2) direction = (start > end) ? linea2.front().getNombre() : linea2.back().getNombre();
        if (linea == 3) direction = (start > end) ? linea3.front().getNombre() : linea3.back().getNombre();
        if (linea == 4) direction = (start > end) ? linea4.front().getNombre() : linea4.back().getNombre();
        if (linea == 5) direction = (start > end) ? linea5.front().getNombre() : linea5.back().getNombre();
        if (linea == 6) direction = (start > end) ? linea6.front().getNombre() : linea6.back().getNombre();

    return direction;
}

//Verifies if both stations belong to the same line
bool Pathsolver::sameLineVerify() {
    //Function variables
    bool sameline = false;
    bool inicioConnection = false;
    bool finConnection = false;
    vector<Estaciones> startVector;
    vector<Estaciones> endVector;
    
    //If one of the two is a connection point, then
    if(inicio.getTipo() != -1 || fin.getTipo() != -1) {
        
        //Verify if start or end, or both are connection points and assign to variable
        if(inicio.getTipo() != -1) inicioConnection = true;
        if(fin.getTipo() != -1) finConnection = true;
        
        //Create vectors to be able to compare stations
        if(inicioConnection) {
            for(Estaciones estacion : allStationsOrdered) {
                if(inicio.getTipo() == estacion.getTipo()) {
                    startVector.push_back(estacion);
                }
            }
        } else {
            startVector.push_back(inicio);
        }
        
        if(finConnection) {
            for(Estaciones estacion : allStationsOrdered) {
                if(fin.getTipo() == estacion.getTipo()) {
                    endVector.push_back(estacion);
                }
            }
        } else {
            endVector.push_back(fin);
        }
        
        //Compare stations, set them to the matching lane and return true if they are on same line
        for(Estaciones start : startVector) {
            for(Estaciones end : endVector) {
                if(start.getLinea() == end.getLinea()) {
                    if(inicioConnection) inicio = start;
                    if(finConnection) fin = end;
                    sameline = true;
                }
            }
        }
    } else {
        //If none of the two are connection points
        if(inicio.getLinea() == fin.getLinea()) {
            sameline = true;
        }
    }
        return sameline;
}

//Print route to screen
void Pathsolver::printBestRoute(list<Estaciones>* lista) {

    //Variables
    bool cambiodelinea = false;
    int count = 0;
    int transbordos = 0;
    int lineaactual = inicio.getLinea();
    int nextline = 0;
    int nextnumber = 0;
    int nextnextnumber = 0;
    
    //Create pointer for first item
    pointerRuta = lista->begin();
    pointerRutaSecond = lista->begin();
    pointerRutaThird = lista->begin();
    pointerRutaSecond++;
    pointerRutaThird++;
    pointerRutaThird++;


    vector<Estaciones> firstVec;
    vector<Estaciones> secondVec;
    
    
    if(pointerRuta->getTipo() != -1) {
        for(Estaciones estacion : allStations) {
            if(estacion.getTipo() == pointerRuta->getTipo()) {
                firstVec.push_back(estacion);
            }
        }
    } else {
        firstVec.push_back(*pointerRuta);
    }
    
    
    if(pointerRutaSecond->getTipo() != -1) {
        for(Estaciones estacion : allStations) {
            if(estacion.getTipo() == pointerRutaSecond->getTipo()) {
                secondVec.push_back(estacion);
            }
        }
    } else {
        secondVec.push_back(*pointerRutaSecond);
    }
    
    for(Estaciones estacion1 : firstVec) {
        for(Estaciones estacion2 : secondVec) {
            if(estacion1.getLinea() == estacion2.getLinea()) {
                nextline = estacion2.getLinea();
                nextnumber = estacion1.getNumero();
                nextnextnumber = estacion2.getNumero();
            }
        }
    }
    
    //Presentation to start traveling if start is not a connection point
    cout << endl;
    cout << "\nTomar la linea " << nextline << ", estacion " << pointerRuta->getNombre() << ", con direccion a ";
    cout << getDirection(nextline, pointerRuta->getNumero(), pointerRutaSecond->getNumero()) << endl;
    count++;
    cout << "Pasara por las estaciones: " << endl;
    
    lineaactual = nextline;

    while(pointerRuta != lista->end()) {
        
        if(cambiodelinea) {
            cout << "\nBajar en la estacion " << pointerRuta->getNombre() << endl;
            printInterestPoints(pointerRuta->getPk_estacionid());
            cout << "Transbordar a la linea " << nextline << " con direccion ";
            cout << getDirection(nextline, nextnumber, nextnextnumber) << endl << endl;
            lineaactual = nextline;
            transbordos++;
        }
        
            
        vector<Estaciones> firstVec;
        vector<Estaciones> secondVec;
        
        
        if(pointerRutaSecond->getTipo() != -1) {
            for(Estaciones estacion : allStations) {
                if(estacion.getTipo() == pointerRutaSecond->getTipo()) {
                    firstVec.push_back(estacion);
                }
            }
        } else {
            firstVec.push_back(*pointerRutaSecond);
        }

        
        if(pointerRutaThird->getTipo() != -1) {
            for(Estaciones estacion : allStations) {
                if(estacion.getTipo() == pointerRutaThird->getTipo()) {
                    secondVec.push_back(estacion);
                }
            }
        } else {
            secondVec.push_back(*pointerRutaThird);
        }

    
        for(Estaciones estacion1 : firstVec) {
            for(Estaciones estacion2 : secondVec) {
                if(estacion1.getLinea() == estacion2.getLinea()) {
                    nextline = estacion2.getLinea();
                    nextnumber = estacion1.getNumero();
                    nextnextnumber = estacion2.getNumero();
                }
            }
        }
        
        if(!cambiodelinea) {
            if(lineaactual != nextline) {
                cout << "*" << pointerRuta->getNombre() << endl;
                printInterestPoints(pointerRuta->getPk_estacionid());
                cambiodelinea = true;
                
            } else {
                cout << "*" << pointerRuta->getNombre() << endl;
                printInterestPoints(pointerRuta->getPk_estacionid());
                cambiodelinea = false;
            }
        } else {
            if(lineaactual != nextline) {
                cambiodelinea = true;
                
            } else {
                cambiodelinea = false;
            }
        }
        
        if(pointerRuta->getTipo() != -1 && pointerRutaSecond->getTipo() != -1) {
            count = count + matrizM[pointerRuta->getTipo()][pointerRutaSecond->getTipo()];
        } else {
            count++;
        }
        
        pointerRuta++;
        pointerRutaSecond++;
        pointerRutaThird++;

    } // end while looop
    cout << "Bajar en la estacion " << pointerRuta.operator--()->getNombre() << endl;
    cout << "Has llegado a tu destino." << endl;
    cout << "\nTiempo estimado: " << count << endl;
    cout << "Numero de transbordos: " << transbordos << endl;
}

//Function to print all interest points
void Pathsolver::printInterestPoints(int fk_id) {
    vector<InterestPoints> vector;
    for(InterestPoints punto : puntosInteresLista) {
        if(punto.getFk_estacionid() == fk_id) {
            vector.push_back(punto);
        }
    }
    if(vector.size() > 0) {
        cout << endl;
        cout << "\tPuntos de Interes: " << endl;
        for(InterestPoints punto : vector) {
            cout << "\t*" << punto.getNombre() << endl;
        }
        cout << endl;
    }
}

//Quick sort function
void Pathsolver::quickSort(vector<Estaciones>* vector, int start, int end) {
    int i = start;
    int k = end;
    if (end - start >= 1) {
        Estaciones pivot = vector->at(start);
        while (k > i) {
            while (vector->at(i).getNombre().compare(pivot.getNombre()) <= 0 && i <= end && k > i) {
                i++;
            }
            while (vector->at(k).getNombre().compare(pivot.getNombre()) > 0 && k >= start && k >= i) {
                k--;
            }
            if (k > i) {
                swap(vector, i, k);
            }
        }
        swap(vector, start, k);
        quickSort(vector, start, k - 1);
        quickSort(vector, k + 1, end);
    }
}

//Swap function used by the quick sort
void Pathsolver::swap(vector<Estaciones>* vector, int index1, int index2) {
    Estaciones temp = vector->at(index1);
    vector->at(index1) = vector->at(index2);
    vector->at(index2) = temp;
}

//Binary search function to search by station, returns pk_estacionid of found station, else returns -1
int Pathsolver::binarySearch(vector<Estaciones> list, string value, int min, int max) {
    if (min > max) {
        return -1;
    }
    
    int mid = (max + min) / 2;
    
    if (list.at(mid).getNombre() == value) {
        return mid;
    } else if (list.at(mid).getNombre() > value) {
        return binarySearch(list, value, min, mid - 1);
    } else {
        return binarySearch(list, value, mid + 1, max);
    }
}

//Binary search overload function
int Pathsolver::binarySearch(vector<Estaciones> list, string value) {
    return binarySearch(list, value, 0, (int)list.size() - 1);
}

void Pathsolver::loadInterestPoints() {
    try {
        //Create SQL variables
        sql::Statement *statement;
        sql::ResultSet *resultSet;
        statement = connection->createStatement();
        resultSet = statement->executeQuery("SELECT * FROM puntosinteres ");
        
        //Iterate through result set
        while(resultSet->next()) {
            int pkid = resultSet->getInt("fk_estacionid");
            string nombre = resultSet->getString("nombre");

            //Create new station from result set
            InterestPoints punto = *new InterestPoints(pkid, nombre);
            
            //Insert station into list
            this->puntosInteresLista.push_back(punto);
        }
        //Delete pointers
        delete resultSet;
        delete statement;
        
    } catch (sql::SQLException &e) {
        cout << "SQL Error: " << e.getErrorCode() << endl;
    }
}


void Pathsolver::loadAllStations(vector<Estaciones>* list) {
    try {
        //Create SQL variables
        sql::Statement *statement;
        sql::ResultSet *resultSet;
        statement = connection->createStatement();
        resultSet = statement->executeQuery("SELECT * FROM estaciones ");
        
        //Iterate through result set
        while(resultSet->next()) {
            int pkid = resultSet->getInt("pk_estacionid");
            int linea = resultSet->getInt("linea");
            int numero = resultSet->getInt("numero");
            string nombre = resultSet->getString("nombre");
            int tipo = resultSet->getInt("tipo");
            int status = resultSet->getInt("status");
            
            //Create new station from result set
            Estaciones estacion = *new Estaciones(pkid,linea,numero,nombre,tipo,status);
            
            //Insert station into list
            list->push_back(estacion);
        }
        //Delete pointers
        delete resultSet;
        delete statement;
        
    } catch (sql::SQLException &e) {
        cout << "SQL Error: " << e.getErrorCode() << endl;
    }
}

void Pathsolver::loadIndividualLines() {
    //Create array to iterate through different lists
    string listas[] = {"1", "2", "3", "4", "5", "6"};
    
    for(int i = 0; i < 6; i++) {
        try {
            //Create SQL variables
            sql::Statement *statement;
            sql::ResultSet *resultSet;
            statement = connection->createStatement();
            resultSet = statement->executeQuery("SELECT * FROM estaciones WHERE linea = " + listas[i]);
            
            //Iterate through resultset to load all stations
            while(resultSet->next()) {
                
                int pkid = resultSet->getInt("pk_estacionid");
                int linea = resultSet->getInt("linea");
                int numero = resultSet->getInt("numero");
                string nombre = resultSet->getString("nombre");
                int tipo = resultSet->getInt("tipo");
                int status = resultSet->getInt("status");
                
                //Create new station from result set
                Estaciones estacion = *new Estaciones(pkid,linea,numero,nombre,tipo,status);
                
                //Insert into appropriate list
                if(i == 0) linea1.push_back(estacion);
                if(i == 1) linea2.push_back(estacion);
                if(i == 2) linea3.push_back(estacion);
                if(i == 3) linea4.push_back(estacion);
                if(i == 4) linea5.push_back(estacion);
                if(i == 5) linea6.push_back(estacion);
                
            }
            //Delete pointers
            delete resultSet;
            delete statement;
            
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.getErrorCode() << endl;
        }
    }
}

void Pathsolver::readRecursiveMatrixT(int i, int j) {
    if (matrizT[i][j] == 99) {
        
    } else {
        //Find position of insertion
        pointerFloyd = floydRuta.begin();
        while(*pointerFloyd != i) {
            pointerFloyd++;
        }
        pointerFloyd++;
    
        //Insert value and call function again
        floydRuta.insert(pointerFloyd, matrizT[i][j]);
        readRecursiveMatrixT(i, matrizT[i][j]);
        readRecursiveMatrixT(matrizT[i][j], j);
    }
}

void Pathsolver::readRecursiveMatrixT(int i, int j, int* count) {
    if (matrizT[i][j] == 99) {
        //Add cost to route total cost
        *count = *count + matrizM[i][j];
    } else {
        //Find position of insertion
        pointerFloyd = floydRuta.begin();
        while(*pointerFloyd != i) {
            pointerFloyd++;
        }
        pointerFloyd++;
        
        //Add cost to route total cost
        *count = *count + matrizM[i][j];
        
        //Insert value and call function again
        floydRuta.insert(pointerFloyd, matrizT[i][j]);
        readRecursiveMatrixT(i, matrizT[i][j]);
        readRecursiveMatrixT(matrizT[i][j], j);
    }
}

//Function to solve floyd route
void Pathsolver::floyd(list<Estaciones>* lista, int start, int end, int* insertindex) {
    //Add start and end to list
    floydRuta.push_back(allStations.at(start).getTipo());
    floydRuta.push_back(allStations.at(end).getTipo());
    
    //Call recursive matrix reader
    readRecursiveMatrixT(allStations.at(start).getTipo(), allStations.at(end).getTipo());
        
    //Insert floyd stations into route
    floydInsert(lista, insertindex);
}

void Pathsolver::floyd(list<Estaciones>* lista, int start, int end, int* insertindex, int* count) {
    //Add start and end to list
    floydRuta.push_back(allStations.at(start).getTipo());
    floydRuta.push_back(allStations.at(end).getTipo());
    *count = *count + 2;
    
    //Call recursive matrix reader
    readRecursiveMatrixT(allStations.at(start).getTipo(), allStations.at(end).getTipo(), count);
    
    //Insert floyd stations into route
    floydInsert(lista, insertindex);
}

//Function to insert floyd route into route list
void Pathsolver::floydInsert(list<Estaciones>* lista, int* insertindex) {
    //Create pointer to insertion point on
    pointerRuta = lista->begin();
    for(int i = 0; i < *insertindex; i++) pointerRuta++;
    
    //Obtain each station and add to list
    for (int estacion : floydRuta) {
        try {
            //Create SQL variables
            sql::Statement *statement;
            sql::ResultSet *resultSet;
            statement = connection->createStatement();
            resultSet = statement->executeQuery("SELECT * FROM estaciones WHERE tipo = " + to_string(estacion));
            
            //Advance resultset
            resultSet->next();
            
            //If result is not the same to start or end, then
            if(resultSet->getInt("tipo") != inicio.getTipo() && resultSet->getInt("tipo") != fin.getTipo()) {
                int pkid = resultSet->getInt("pk_estacionid");
                int linea = resultSet->getInt("linea");
                int numero = resultSet->getInt("numero");
                string nombre = resultSet->getString("nombre");
                int tipo = resultSet->getInt("tipo");
                int status = resultSet->getInt("status");
                
                //Create new station from result and insert into ruta
                Estaciones estacion = *new Estaciones(pkid,linea,numero,nombre,tipo,status);
 
                lista->insert(pointerRuta, estacion);
       
            
            }
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.getErrorCode() << endl;
        }
    }
    floydRuta.clear();
}

//Function to print stations when needed
void Pathsolver::printStations() {
    for(Estaciones estacion : linea1) {
        cout << estacion.getPk_estacionid() << " " << estacion.getLinea() << " " << estacion.getNumero() << " " << estacion.getNombre() << " " << estacion.getTipo() << " " << estacion.getStatus() << endl;
    }
    
    for(Estaciones estacion : allStations) {
        cout << estacion.getPk_estacionid() << " " << estacion.getLinea() << " " << estacion.getNumero() << " " << estacion.getNombre() << " " << estacion.getTipo() << " " << estacion.getStatus() << endl;
    }
    
    for(Estaciones estacion : allStationsOrdered) {
        cout << estacion.getPk_estacionid() << " " << estacion.getLinea() << " " << estacion.getNumero() << " " << estacion.getNombre() << " " << estacion.getTipo() << " " << estacion.getStatus() << endl;
    }
}
