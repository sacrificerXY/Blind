#include "stdafx.h"
#include "Point.h"
#include <cmath>

using namespace std;

float Point::get_magnitude() const
{
    return sqrt(x * x + y * y);
}
Point Point::get_heading() const
{
    float mag = get_magnitude();
    return Point(x/mag, y/mag);
}
Point Point::get_normal() const
{
    return Point(-y, x);
}
Point Point::get_unit() const
{
    return get_heading();
}
void Point::rotate(float angle)
{
    // (c) - http://gamedev.stackexchange.com/questions/86755/how-to-calculate-corner-positions-marks-of-a-rotated-tilted-rectangle
    // by Aholio

    float rotatedX = x*cos(angle) - y*sin(angle);
    float rotatedY = x*sin(angle) + y*cos(angle);
    x = rotatedX;
    y = rotatedY;
}

Point* Point::operator+=(const Point &p) { x += p.x; y += p.y; return this; }
Point* Point::operator-=(const Point &p) { x -= p.x; y -= p.y; return this; }
Point* Point::operator*=(float s) { x *= s; y *= s; return this; }
Point* Point::operator/=(float s) { x /= s; y /= s; return this; }

bool operator==(const Point &l, const Point &r) { return l.x == r.x && l.y == r.y; }
bool operator!=(const Point &l, const Point &r) { return !(l == r); }

Point operator+(const Point &l, const Point &r) { return Point(l.x + r.x, l.y + r.y); }
Point operator-(const Point &l, const Point &r) { return Point(l.x - r.x, l.y - r.y); }
Point operator-(const Point &l) { return Point(-l.x, -l.y); }
Point operator*(const Point &l, float s) { return Point(l.x * s, l.y * s); }
Point operator*(float s, const Point &r) { return Point(r.x * s, r.y * s); }
Point operator/(const Point &l, float s) { return Point(l.x / s, l.y / s); }
Point operator/(float s, const Point &r) { return Point(r.x / s, r.y / s); }

float dotProduct(const Point &l, const Point &r) { return l.x * r.x + l.y * r.y; }
float crossProduct(const Point &l, const Point &r) { return l.x * r.y - l.y * r.x; }
float distance(const Point &l, const Point &r)
{
    float dx = l.x - r.x;
    float dy = l.y - r.y;
    return sqrt(dx * dx + dy * dy);
}
float distanceSqrd(const Point &l, const Point &r)
{
    float dx = l.x - r.x;
    float dy = l.y - r.y;
    return dx * dx + dy * dy;
}
bool collinear(Point const& a, Point const& b, Point const& c)
{
    return (abs(a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y*b.y)) < 0.000001);
}

bool isAlmostEqual(Point const& p1, Point const& p2, float epsilon)
{
    if (abs(p1.x-p2.x) < epsilon && abs(p1.y - p2.y) < epsilon) return true;
    return false;
}

ostream& operator<<(ostream& os, const Point &p)
{
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}


