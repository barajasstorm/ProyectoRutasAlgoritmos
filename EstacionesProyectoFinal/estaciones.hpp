//
//  estaciones.hpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#ifndef estaciones_hpp
#define estaciones_hpp

#include <stdio.h>
#include <string>

using std::string;

class Estaciones {
  
public:
    Estaciones();
    Estaciones(int pk_estacionid, int linea, int numero, string nombre, int tipo, int status);
    Estaciones(const Estaciones& object);
    Estaciones& operator=(const Estaciones& object);
    ~Estaciones();
    
    void setPk_estacionid(int);
    void setLinea(int);
    void setNumero(int);
    void setNombre(string);
    void setTipo(int);
    void setStatus(int);
    
    int getPk_estacionid();
    int getLinea();
    int getNumero();
    string getNombre();
    int getTipo();
    int getStatus();
        
private:
    int pk_estacionid;
    int linea;
    int numero;
    string nombre;
    int tipo;
    int status;
    
};

#endif /* estaciones_hpp */
