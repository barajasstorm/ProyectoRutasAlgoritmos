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
#include "linked_list.hpp"
#include "estaciones.hpp"
#include "pathsolver.hpp"

enum option { AGAIN = 1, EXIT };

int main(void) {
    
    try {
        //Create connection class instance, connect to database, and save connection pointer
        Connector connect;
        sql::Connection *connection = connect.getConnection();
        
        //Create matrix instance and solve floyd matrix
        Matrices matrix = *new Matrices(connection);

        std::cout << "************************************************" << std::endl;
        std::cout << "*           Proyecto Lineas de Tren            *" << std::endl;
        std::cout << "*          Elaborado por: John Poker           *" << std::endl;
        std::cout << "************************************************" << std::endl;
        
        std::cout << std::endl;
        
        do{
            //Input station values
            std::string inicio, fin;
            std::cout << "Indicar estacion de inicio: ";
            std::getline(std::cin, inicio);
            std::cout << "Indicar estacion final: ";
            std::getline(std::cin, fin);
            
            //Create test station
            Pathsolver session = Pathsolver(connection, inicio, fin, matrix.getMatrizM(), matrix.getMatrizT());
        }while();
         
        
        

        
        
        
        
        
        
        
        
        
        
        delete connection;
        //Deconstruct all objects and pointers
        
    } catch (sql::SQLException &e) {
        cout << "SQL Error: " << e.getErrorCode() << endl;
    }
    
    cout << endl;
    return EXIT_SUCCESS;
}
