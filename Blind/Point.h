#pragma once

#include <iostream>

struct Point
{
    float x = 0;
    float y = 0;
    Point() = default;
    Point(float x, float y):x(x), y(y) {}

    float get_magnitude() const;
    Point get_heading() const;
    Point get_normal() const;
    Point get_unit() const;
    void rotate(float angle);

	Point* operator+=(const Point &p);
	Point* operator-=(const Point &p);
	Point* operator*=(float s);
	Point* operator/=(float s);

    friend std::ostream& operator<<(std::ostream& os, const Point &p);
};

bool operator==(const Point &l, const Point &r);
bool operator!=(const Point &l, const Point &r);

Point operator+(const Point &l, const Point &r);
Point operator-(const Point &l, const Point &r);
Point operator-(const Point &l);
Point operator*(const Point &l, float s);
Point operator*(float s, const Point &r);
Point operator/(const Point &l, float s);
Point operator/(float s, const Point &r);

float dotProduct(const Point &l, const Point &r);
float crossProduct(const Point &l, const Point &r);
float distance(const Point &l, const Point &r);
float distanceSqrd(const Point &l, const Point &r);
bool collinear(Point const& a, Point const& b, Point const& c);
bool isAlmostEqual(Point const& p1, Point const& p2, float epsilon=0.01);
