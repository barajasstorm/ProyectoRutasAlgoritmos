//
//  linked_list.cpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/30/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#include "linked_list.hpp"
#include <iostream>
#include "estaciones.hpp"


using std::cout;
using std::endl;

Linkedlist::Linkedlist() {
    head = NULL;
    tail = NULL;
}

void Linkedlist::createEstaciones(Estaciones estacion) {
    Estaciones *temp = new Estaciones;
    temp = &estacion;
    temp->next = NULL;
    if(head == NULL) {
        head = temp;
        tail = temp;
        temp = NULL;
    } else {
        tail->next = temp;
        tail = temp;
    }
}

void Linkedlist::display() {
    Estaciones *temp = new Estaciones();
    cout << "head: " << head << endl;
    cout << "tail: " << tail << endl;
    temp = head;
    int i = 0;
    while(temp != NULL) {
        cout << i << endl;
        cout << temp->pk_estacionid << " "
        << temp->linea << " "
        << temp->numero << " "
        << temp->nombre << " "
        << temp->tipo << " "
        << temp->status << endl;
        temp = temp->next;
        i++;
    }
}

Estaciones Linkedlist::getFirst() {
    return *head;
}

Estaciones Linkedlist::getLast() {
    return *tail;
}

void Linkedlist::insertStart(Estaciones estacion) {
    Estaciones *temp = new Estaciones;
    temp = &estacion;
    temp->next = head;
    head = temp;
}

void Linkedlist::insertPosition(int position, Estaciones estacion) {
    Estaciones *previous = new Estaciones;
    Estaciones *current = new Estaciones;
    Estaciones *temp = new Estaciones;
    current = head;
    for(int i = 0; i < position; i++) {
        previous = current;
        current = current->next;
    }
    temp = &estacion;
    previous->next = temp;
    temp->next = current;
}

void Linkedlist::deleteFirst() {
    Estaciones *temp = new Estaciones;
    temp = head;
    head = head->next;
    //delete temp;
}

void Linkedlist::deleteLast() {
    Estaciones *current = new Estaciones;
    Estaciones *previous = new Estaciones;
    current = head;
    while(current->next != NULL) {
        previous = current;
        current = current->next;
    }
    tail = previous;
    previous->next = NULL;
    //delete current;
}

void Linkedlist::deletePosition(int position)
{
    Estaciones *current = new Estaciones;
    Estaciones *previous = new Estaciones;
    current = head;
    for(int i = 1; i < position; i++) {
        previous = current;
        current = current->next;
    }
    previous->next = current->next;
}
