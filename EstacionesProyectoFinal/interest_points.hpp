//
//  interest_points.hpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 12/3/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#ifndef interest_points_hpp
#define interest_points_hpp

#include <stdio.h>
#include <string>

using std::string;

class InterestPoints {
    
public:
    InterestPoints();
    InterestPoints(int, string);
    int getFk_estacionid();
    string getNombre();
    
private:
    int fk_estacionid;
    string nombre;

};


#endif /* interest_points_hpp */
