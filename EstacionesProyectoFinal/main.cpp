//
//  main.cpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/28/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#include <stdlib.h>
#include <iostream>
#include <cppconn/exception.h>
#include "database_connector.hpp"
#include "matrices.hpp"
#include "estaciones.hpp"
#include "pathsolver.hpp"

using namespace std;

int main(void) {
    
    try {
        //Create connection class instance, connect to database, and save connection pointer
        Connector connect;
        
        //Create matrix instance and solve floyd matrix
        Matrices matrix = *new Matrices(connect.getConnection());
        cout << "************************************************" << endl;
        cout << "*           Proyecto Lineas de Tren            *" << endl;
        cout << "*         Elaborado por: Juan Barajas          *" << endl;
        cout << "************************************************" << endl;
        cout << endl;
        
        int input = 0;
        do{
            
            
            string inicio, fin;
            cout << "Indicar estacion de inicio: ";
            getline(cin, inicio);
            cout << "Indicar estacion final: ";
            getline(cin, fin);
            
            
            //Create session
            Pathsolver session = Pathsolver(connect.getConnection(), inicio, fin, matrix.getMatrizM(), matrix.getMatrizT());
            
            cout << endl;
            cout << "Desea calcular otra ruta? (1. Si, 2. No): ";
            cin >> input;
            cout << endl << endl;
            cin.ignore();
        }while(input == 1);
         
        
       
        
    } catch (sql::SQLException &e) {
        cout << "SQL Error: " << e.getErrorCode() << endl;
    }
    
    cout << endl;
    return EXIT_SUCCESS;
}
