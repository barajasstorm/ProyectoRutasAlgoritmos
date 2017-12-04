//
//  interest_points.cpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 12/3/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#include "interest_points.hpp"
#include <string>

using std::string;

InterestPoints::InterestPoints() {
}

InterestPoints::InterestPoints(int fk_estacionid, string nombre) : fk_estacionid(fk_estacionid), nombre(nombre){
}

int InterestPoints::getFk_estacionid() {
    return fk_estacionid;
}

string InterestPoints::getNombre() {
    return nombre;
}
