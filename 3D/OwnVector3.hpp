////////////////////////////////////////////////////////////////////////////////////
//
// ---------------------------------------------------------------------------------
//   _____      _            _ _
//  |  _  |    | |          | (_)
//  | |/' | ___| |_ ___   __| |_  ___ ___
//  |  /| |/ __| __/ _ \ / _` | |/ __/ _ \
//  \ |_/ / (__| || (_) | (_| | | (_|  __/
//   \___/ \___|\__\___/ \__,_|_|\___\___|
//
// ---------------------------------------------------------------------------------
//
//  Project for the course "Animation et Simulation" at the University Gustave Eiffel
//  ~ Thomas DUMONT A.K.A 0ctodice
//
// ---------------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <raylib.h>
#include <raymath.h>

class OwnVector3
{
public:
    double x, y, z = 0.0;

    OwnVector3(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    // surcharge de l'opérateur +
    OwnVector3 operator+(OwnVector3 const &obj)
    {
        OwnVector3 res(x + obj.x, y + obj.y, z + obj.z);
        return res;
    }

    // surcharge de l'opérateur -
    OwnVector3 operator-(OwnVector3 const &obj)
    {
        OwnVector3 res(x - obj.x, y - obj.y, z - obj.z);
        return res;
    }

    // surcharge de l'opérateur *
    OwnVector3 operator*(double const &obj)
    {
        OwnVector3 res(x * obj, y * obj, z * obj);
        return res;
    }

    // surcharge de l'opérateur /
    OwnVector3 operator/(double const &obj)
    {
        OwnVector3 res(x / obj, y / obj, z / obj);
        return res;
    }

    // surcharge de l'opérateur =
    OwnVector3 operator=(OwnVector3 const &obj)
    {
        x = obj.x;
        y = obj.y;
        z = obj.z;
        return *this;
    }

    // surcharge de l'opérateur +=
    OwnVector3 operator+=(OwnVector3 const &obj)
    {
        x += obj.x;
        y += obj.y;
        z += obj.z;
        return *this;
    }

    // surcharge de l'opérateur -=
    OwnVector3 operator-=(OwnVector3 const &obj)
    {
        x -= obj.x;
        y -= obj.y;
        z -= obj.z;
        return *this;
    }

    // surcharge de l'opérateur *=
    OwnVector3 operator*=(double const &obj)
    {
        x *= obj;
        y *= obj;
        z *= obj;
        return *this;
    }

    // surcharge de l'opérateur /=
    OwnVector3 operator/=(double const &obj)
    {
        x /= obj;
        y /= obj;
        z /= obj;
        return *this;
    }

    // surcharge de l'opérateur ==
    bool operator==(OwnVector3 const &obj)
    {
        return (x == obj.x && y == obj.y && z == obj.z);
    }

    // surcharge de l'opérateur !=
    bool operator!=(OwnVector3 const &obj)
    {
        return (x != obj.x || y != obj.y || z != obj.z);
    }

    Vector3 operator!(void)
    {
        return {(float)x, (float)y, (float)z};
    }

    OwnVector3 normalized()
    {
        double norm = sqrt(x * x + y * y + z * z);
        return {x / norm, y / norm, z / norm};
    }
};
