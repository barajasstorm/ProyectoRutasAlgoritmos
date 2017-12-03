//
//  pathsolver.hpp
//  EstacionesProyectoFinal
//
//  Created by Juan Bar*ajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#ifndef pathsolver_hpp
#define pathsolver_hpp

#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include <mysql_connection.h>
#include "estaciones.hpp"


using namespace std;

class Pathsolver {
    
public:
    Pathsolver(sql::Connection *connection);
    Pathsolver(sql::Connection *connection, string inicio, string fin,int** matrizM,int** matrizT);
    void printStations();
    void solve();
    
    
private:
    sql::Connection *connection;
    int** matrizM;
    int** matrizT;
    Estaciones inicio;
    Estaciones fin;
    list<Estaciones> ruta;
    list<int> floydRuta;
    list<int>::iterator pointerFloyd;
    list<Estaciones>::iterator pointerRuta;
    list<Estaciones> linea1;
    list<Estaciones> linea2;
    list<Estaciones> linea3;
    list<Estaciones> linea4;
    list<Estaciones> linea5;
    list<Estaciones> linea6;
    vector<Estaciones> allStations;
    vector<Estaciones> allStationsOrdered;
    int insertIndex;
    
    void loadIndividualLines();
    void loadAllStations(vector<Estaciones>* list);
    void quickSort(vector<Estaciones>* vector, int start, int end);
    void swap(vector<Estaciones>* vector, int index1, int index2);
    int binarySearch(vector<Estaciones> list, string value, int min, int max);
    int binarySearch(vector<Estaciones> list, string value);
    bool sameLineVerify();
    string getDirection(int linea, int start, int end);
    void readRecursiveMatrixT(int i, int j);
    void printBestRoute();
    bool sameCrossPointVerify(int, int);
    bool sameLineSolve(int*, int*, bool*);
    void floyd(int, int, bool);
    void floydInsert(bool);
    

    
    
    
  
};

#endif /* pathsolver_hpp */
