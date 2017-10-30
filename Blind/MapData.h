#pragma once

#include <vector>

#include "Point.h"

constexpr int TILE_SOLID_TOP     = 1 << 0;
constexpr int TILE_SOLID_LEFT    = 1 << 1;
constexpr int TILE_SOLID_BOTTOM  = 1 << 2;
constexpr int TILE_SOLID_RIGHT   = 1 << 3;


struct Tile
{
    int gid = 0;
    int solid_flag = 0;

    bool is_solid = false;

    bool flip_horizontal = false;
    bool flip_vertical = false;
    bool flip_diagonal = false;

};

struct Line
{
    Point p1;
    Point p2;

    Line(Point p1, Point p2):p1(p1), p2(p2) {}
    Line(float p1x, float p1y, float p2x, float p2y):p1(p1x, p1y), p2(p2x, p2y) {}
};

struct MapData
{
    Point num_tiles;

    std::vector<std::vector<Tile>> tiles;

    std::vector<Line> static_lines_top;
    std::vector<Line> static_lines_bottom;
    std::vector<Line> static_lines_left;
    std::vector<Line> static_lines_right;
};

