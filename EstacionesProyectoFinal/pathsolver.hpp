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
#include "interest_points.hpp"


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
    list<Estaciones> rutaOptima;
    list<Estaciones> ruta;
    list<Estaciones> beginLeftFinal[2];
    list<Estaciones> beginRightFinal[2];
    list<int> floydRuta;
    list<int>::iterator pointerFloyd;
    list<Estaciones>::iterator pointerRuta;
    list<Estaciones>::iterator pointerRutaSecond;
    list<Estaciones> linea1;
    list<Estaciones> linea2;
    list<Estaciones> linea3;
    list<Estaciones> linea4;
    list<Estaciones> linea5;
    list<Estaciones> linea6;
    list<InterestPoints> puntosInteresLista;
    vector<Estaciones> allStations;
    vector<Estaciones> allStationsOrdered;
    bool beginRoute[2];
    bool endRoute[2];
    int beginLeftFinalCount[2];
    int beginRightFinalCount[2];
    int insertIndex;
    int indexone;
    int indextwo;
    int indexthree;
    int indexfour;
    
    //Diferent Liness
    list<Estaciones>* differentLineSolve();
    
    void loadIndividualLines();
    void loadAllStations(vector<Estaciones>* list);
    void quickSort(vector<Estaciones>* vector, int start, int end);
    void swap(vector<Estaciones>* vector, int index1, int index2);
    int binarySearch(vector<Estaciones> list, string value, int min, int max);
    int binarySearch(vector<Estaciones> list, string value);
    bool sameLineVerify();
    string getDirection(int linea, int start, int end);
    void readRecursiveMatrixT(int i, int j);
    void readRecursiveMatrixT(int i, int j, int*);
    void printBestRoute(list<Estaciones>*);
    bool sameCrossPointVerify(int, int);
    bool sameLineSolve(list<Estaciones>*, int*, int*, bool*);
    void floyd(list<Estaciones>*, int, int, int*);
    void floyd(list<Estaciones>*, int, int, int*, int*);
    void floydInsert(list<Estaciones>*, int*);
    void printInterestPoints(int fk_id);
    void loadInterestPoints();
    void lineSwitch();

    
    
    
  
};

#endif /* pathsolver_hpp */
