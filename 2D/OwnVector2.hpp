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

class OwnVector2
{
public:
    double x, y = 0.0;

    OwnVector2(double _x, double _y)
    {
        x = _x;
        y = _y;
    }

    // surcharge de l'opérateur +
    OwnVector2 operator+(OwnVector2 const &obj)
    {
        OwnVector2 res(x + obj.x, y + obj.y);
        return res;
    }

    // surcharge de l'opérateur -
    OwnVector2 operator-(OwnVector2 const &obj)
    {
        OwnVector2 res(x - obj.x, y - obj.y);
        return res;
    }

    // surcharge de l'opérateur *
    OwnVector2 operator*(double const &obj)
    {
        OwnVector2 res(x * obj, y * obj);
        return res;
    }

    // surcharge de l'opérateur /
    OwnVector2 operator/(double const &obj)
    {
        OwnVector2 res(x / obj, y / obj);
        return res;
    }

    // surcharge de l'opérateur =
    OwnVector2 operator=(OwnVector2 const &obj)
    {
        x = obj.x;
        y = obj.y;
        return *this;
    }

    // surcharge de l'opérateur +=
    OwnVector2 operator+=(OwnVector2 const &obj)
    {
        x += obj.x;
        y += obj.y;
        return *this;
    }

    // surcharge de l'opérateur -=
    OwnVector2 operator-=(OwnVector2 const &obj)
    {
        x -= obj.x;
        y -= obj.y;
        return *this;
    }

    // surcharge de l'opérateur *=
    OwnVector2 operator*=(double const &obj)
    {
        x *= obj;
        y *= obj;
        return *this;
    }

    // surcharge de l'opérateur /=
    OwnVector2 operator/=(double const &obj)
    {
        x /= obj;
        y /= obj;
        return *this;
    }

    // surcharge de l'opérateur ==
    bool operator==(OwnVector2 const &obj)
    {
        return (x == obj.x && y == obj.y);
    }

    // surcharge de l'opérateur !=
    bool operator!=(OwnVector2 const &obj)
    {
        return (x != obj.x || y != obj.y);
    }

    Vector2 toVector2()
    {
        return Vector2{(float)x, (float)y};
    }
};