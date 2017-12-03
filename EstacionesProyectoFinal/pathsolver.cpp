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
    solve();
    
}

void Pathsolver::solve() {
    
    //If they are both on the same line
    if(sameLineVerify()) {

        int start = inicio.getPk_estacionid();
        int end = fin.getPk_estacionid();
        bool floydNeeded = false;
        bool reverse = ((start < end) ? false : true);
        int* startPointer = &start;
        int* endPointer = &end;

        //Add stations to ruta list
        floydNeeded = sameLineSolve(startPointer, endPointer, &reverse);

        //If floyd is needed, then call floyd function
        if(floydNeeded) {
            //If connection point is the same
            if(start == end) {
                pointerRuta = ruta.begin();
                for(int i = 0; i < insertIndex; i++) pointerRuta++;
                ruta.insert(--pointerRuta, allStations.at(start));
            } else {
                 floyd(start, end, reverse);
            }
        }
    } else {
   //If they are on different lines
        
        
        
        
    }
    
    printBestRoute();
}

bool Pathsolver::sameLineSolve(int* start, int* end, bool* reverse) {
    bool floydFlag = false;
    int startLocal = *start;
    int endLocal = *end;
    int cont = -1;
    insertIndex = 0;

    while ((startLocal != endLocal) && (floydFlag == false)) {
        //If it is not a connection point
        if(allStations.at(startLocal).getTipo() == -1) {
            if (allStations.at(startLocal).getPk_estacionid() != inicio.getPk_estacionid()) {
                ruta.push_back(allStations.at(startLocal));
            }
            *start = ((*reverse) ? --*start : ++*start);
            startLocal = ((*reverse) ? --startLocal : ++startLocal);
            cont++;
            insertIndex++;
        } else {
            //If it is a connection point
            floydFlag = true;
        }
    }
    insertIndex = insertIndex + cont;
    if(floydFlag) {
        //Create pointer to insertion point
        pointerRuta = ruta.begin();
        for(int i = 0; i < insertIndex; i++) pointerRuta++;
    
        
        //Start traveling the opposite direction until finding crosspoint
        bool connectionPoint = false;
        while ((endLocal != startLocal) && (!connectionPoint)) {
            if(allStations.at(endLocal).getTipo() == -1) {
                if(allStations.at(endLocal).getPk_estacionid() != fin.getPk_estacionid()) {
                    ruta.insert(pointerRuta, allStations.at(endLocal));
                    pointerRuta--;
                }
                *end = ((!*reverse) ? --*end : ++*end);
                endLocal = ((!*reverse) ? --endLocal : ++endLocal);
                
            } else {
                connectionPoint = true;
            }
        }
    }
    insertIndex = insertIndex - cont;
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
void Pathsolver::printBestRoute() {
    //Presentation to start traveling if start is not a connection point
    if(inicio.getTipo() == -1) {
        cout << "Tomar la linea " << inicio.getLinea() << ", estacion " << inicio.getNombre() << ", con direccion a ";
        cout << getDirection(inicio.getLinea(), inicio.getNumero(), fin.getNumero()) << endl;
        cout << "Pasara por las estaciones: " << endl;
    } else {
        cout << "Tomar la linea " << ruta.front().getLinea() << ", estacion " << inicio.getNombre() << ", con direccion a ";
        //direction to
        cout << "Pasara por las estaciones: " << endl;
    }
    
    for(Estaciones estacion : ruta) cout << estacion.getNombre() << endl;
    
    cout << "Bajar en la estacion " << allStations.at(fin.getPk_estacionid()).getNombre() << endl;;

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
        //Create pointer for insertion point
        pointerFloyd = floydRuta.begin();
        ++pointerFloyd;
        
        //Insert value and call function again
        floydRuta.insert(pointerFloyd, matrizT[i][j]);
        readRecursiveMatrixT(i, matrizT[i][j]);
        readRecursiveMatrixT(matrizT[i][j], j);
    }
}

void Pathsolver::floyd(int start, int end, bool reverse) {
    //Add start and end to list
    floydRuta.push_back(allStations.at((reverse) ? end : start).getTipo());
    floydRuta.push_back(allStations.at((reverse) ? start : end).getTipo());
    //Call recursive matrix reader
    readRecursiveMatrixT(allStations.at(start).getTipo(), allStations.at(end).getTipo());
    

    //Insert floyd stations into route
    floydInsert(reverse);

}

void Pathsolver::floydInsert(bool reverse) {
    //Create pointer to insertion point on
    insertIndex--;
    pointerRuta = ruta.begin();
    for(int i = 0; i < insertIndex; i++) pointerRuta++;
    if(reverse) pointerRuta++;

    
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
                if(reverse) {
                    ruta.insert(--pointerRuta, estacion);
                } else {
                    ruta.insert(pointerRuta, estacion);
                }
            
            }
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.getErrorCode() << endl;
        }
    }
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
