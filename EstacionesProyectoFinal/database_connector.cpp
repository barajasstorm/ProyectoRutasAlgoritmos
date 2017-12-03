//
//  database_connector.cpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#include "database_connector.hpp"
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>


Connector::Connector() {
}

sql::Connection* Connector::getConnection() {
    sql::Driver *driver;
    sql::Connection *connection;
    
    driver = get_driver_instance();
    connection = driver->connect("tcp://127.0.0.1:3306", "root", "password1");
    connection->setSchema("estacionesfinal");
    
    return connection;
}
