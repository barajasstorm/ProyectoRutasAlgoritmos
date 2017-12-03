//
//  matrices.cpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#include "matrices.hpp"
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

Matrices::Matrices(sql::Connection *connection) : connection(connection) {
    
    matrizM = new int *[13];
    matrizT = new int *[13];
    for(int i = 0; i < 13; i++) {
        matrizM[i] = new int [13];
        matrizT[i] = new int [13];
    }
    
    try {
        sql::Statement *statement;
        sql::ResultSet *resultSet;
        
        statement = connection->createStatement();
        resultSet = statement->executeQuery("SELECT * FROM matrizm");
        int i = 0;
        while (resultSet->next()) {
            this->matrizM[i][0] = resultSet->getInt(1);
            this->matrizM[i][1] = resultSet->getInt(2);
            this->matrizM[i][2] = resultSet->getInt(3);
            this->matrizM[i][3] = resultSet->getInt(4);
            this->matrizM[i][4] = resultSet->getInt(5);
            this->matrizM[i][5] = resultSet->getInt(6);
            this->matrizM[i][6] = resultSet->getInt(7);
            this->matrizM[i][7] = resultSet->getInt(8);
            this->matrizM[i][8] = resultSet->getInt(9);
            this->matrizM[i][9] = resultSet->getInt(10);
            this->matrizM[i][10] = resultSet->getInt(11);
            this->matrizM[i][11] = resultSet->getInt(12);
            this->matrizM[i][12] = resultSet->getInt(13);
            i++;
        }
        delete resultSet;
        delete statement;
        
        statement = connection->createStatement();
        resultSet = statement->executeQuery("SELECT * FROM matrizt");
        i = 0;
        while (resultSet->next()) {
            this->matrizT[i][0] = resultSet->getInt(1);
            this->matrizT[i][1] = resultSet->getInt(2);
            this->matrizT[i][2] = resultSet->getInt(3);
            this->matrizT[i][3] = resultSet->getInt(4);
            this->matrizT[i][4] = resultSet->getInt(5);
            this->matrizT[i][5] = resultSet->getInt(6);
            this->matrizT[i][6] = resultSet->getInt(7);
            this->matrizT[i][7] = resultSet->getInt(8);
            this->matrizT[i][8] = resultSet->getInt(9);
            this->matrizT[i][9] = resultSet->getInt(10);
            this->matrizT[i][10] = resultSet->getInt(11);
            this->matrizT[i][11] = resultSet->getInt(12);
            this->matrizT[i][12] = resultSet->getInt(13);
            i++;
        }
        delete resultSet;
        delete statement;
        
    } catch (sql::SQLException &e) {
        cout << "SQL Error: " << e.getErrorCode() << endl;
    }
    
    //Solve matrix to create new values
    floydSolve();
    
}

int** Matrices::getMatrizM() {
    return matrizM;
}

int** Matrices::getMatrizT() {
    return matrizT;
}

void Matrices::floydSolve() {
    for(int k = 0; k < 13; k++) {
        for(int i = 0; i < 13; i++) {
            for(int j = 0; j < 13; j++) {
                if(matrizM[i][k] + matrizM[k][j] < matrizM[i][j]) {
                    matrizM[i][j] = matrizM[i][k] + matrizM[k][j];
                    matrizT[i][j] = k;
                }
            }
        }
    }
}

void Matrices::printAll() {
    cout << "MatrizM\n";
    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < 13; j++) {
            cout << matrizM[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
    
    cout << "MatrizT\n";
    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < 13; j++) {
            cout << matrizT[i][j] << "\t";
        }
        cout << endl;
    }
}



