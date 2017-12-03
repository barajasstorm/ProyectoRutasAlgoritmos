//
//  estaciones.cpp
//  EstacionesProyectoFinal
//
//  Created by Juan Barajas on 11/29/17.
//  Copyright © 2017 Juan Barajas. All rights reserved.
//

#include "estaciones.hpp"
#include <string>

using std::string;

Estaciones::Estaciones() {
}

Estaciones::Estaciones(const Estaciones& object) : pk_estacionid(object.pk_estacionid), linea(object.linea), numero(object.numero), nombre(object.nombre), tipo(object.tipo), status(object.status) {

}

Estaciones& Estaciones::operator=(const Estaciones& object) {
    pk_estacionid = object.pk_estacionid;
    linea = object.linea;
    numero = object.numero;
    nombre = object.nombre;
    tipo = object.tipo;
    status = object.status;
    return *this;
}

Estaciones::Estaciones(int pk_estacionid, int linea, int numero, string nombre, int tipo, int status) {
    this->pk_estacionid = pk_estacionid;
    this->linea = linea;
    this->numero = numero;
    this->nombre = nombre;
    this->tipo = tipo;
    this->status = status;
}

Estaciones::~Estaciones()
{
}

void Estaciones::setPk_estacionid(int pk_estacionid) {
    this->pk_estacionid = pk_estacionid;
}


void Estaciones::setLinea(int linea) {
    this->linea = linea;
}


void Estaciones::setNumero(int numero) {
    this->numero = numero;
}

void Estaciones::setNombre(string nombre) {
    this->nombre = nombre;
}

void Estaciones::setTipo(int tipo) {
    this->tipo = tipo;
}

void Estaciones::setStatus(int status) {
    this->status = status;
}

int Estaciones::getPk_estacionid() {
    return pk_estacionid;
}

int Estaciones::getLinea() {
    return linea;
}

int Estaciones::getNumero() {
    return numero;
}
string Estaciones::getNombre() {
    return nombre;
}
int Estaciones::getTipo() {
    return tipo;
}
int Estaciones::getStatus() {
    return status;
}
