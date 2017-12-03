//
//  matrices.hpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#ifndef matrices_hpp
#define matrices_hpp

#include <stdio.h>
#include <mysql_connection.h>

class Matrices {
    
public:
    Matrices(sql::Connection *connection);
    void printAll();
    void floydSolve();
    int** getMatrizM();
    int** getMatrizT();
    
private:
    int** matrizM = NULL;
    int** matrizT = NULL;
    sql::Connection *connection;
    
};


#endif /* matrices_hpp */
