//
//  database_connector.hpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#ifndef database_connector_hpp
#define database_connector_hpp

#include <stdio.h>
#include <string>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>

class Connector {
    
public:
    Connector();
    sql::Connection* getConnection();
    
};

#endif /* database_connector_hpp */
