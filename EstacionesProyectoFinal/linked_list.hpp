//
//  linked_list.hpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/30/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#ifndef linked_list_hpp
#define linked_list_hpp

#include <stdio.h>
#include "estaciones.hpp"


class Linkedlist {
    
public:
    Linkedlist();
    void createEstaciones(Estaciones);
    void display();
    Estaciones getFirst();
    Estaciones getLast();
    void insertStart(Estaciones);
    void insertPosition(int,Estaciones);
    void deleteFirst();
    void deleteLast();
    void deletePosition(int);
    
private:
    Estaciones *head;
    Estaciones *tail;
    
    
};

#endif /* linked_list_hpp */
