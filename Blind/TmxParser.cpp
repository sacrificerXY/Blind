#include "stdafx.h"
#include "TmxParser.h"

#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>

#include "tinyxml2.h"
#include "MapData.h"
#include "string_split.h"
#include "loop_utility.h"

using namespace std;
using namespace tinyxml2;



XMLElement* getElement(XMLElement * e, const char* path);

void generateLines(vector<vector<Tile>> const& tiles, Point num_tiles, Point line_direction, Point line_offset, int solid_bit_flag, vector<Line> & output_lines);


void TmxParser::parse(const char* tmx_path, MapData& map_data)
{
    XMLDocument doc;
    if (doc.LoadFile(tmx_path) != XMLError::XML_SUCCESS)
    {
        doc.PrintError();
    }

    XMLElement* map = doc.FirstChildElement("map");
    assert(map != nullptr);
    Point num_tiles;
    num_tiles.x = map->IntAttribute("width", 0);
    num_tiles.y = map->IntAttribute("height", 0);
    map_data.num_tiles = num_tiles;

	XMLElement* location = getElement(map, "objectgroup=obj_locations.object");
	while (location)
	{
		const char* name = location->Attribute("name");
		const char* type = location->Attribute("type");
		Point position(location->FloatAttribute("x"), location->FloatAttribute("y"));
		Point size(location->FloatAttribute("width"), location->FloatAttribute("height"));
		position /= 20;
		size /= 20;
		if (string(name) == "start") map_data.location_start = position + 0.5 * size;

		cout << "Location: " << name << " --type=" << type << '\n';
		cout << "    position  " << position << '\n';
		cout << "    size      " << size << '\n';
		location = location->NextSiblingElement("object");
	}

    XMLElement* tiles_static = getElement(map, "layer=tiles_static.data");
    stringstream ss;
    ss << tiles_static->GetText();

    vector<vector<int>> gid(map_data.num_tiles.x, vector<int>(num_tiles.y));
    vector<vector<Tile>> & tiles = map_data.tiles;
    tiles = vector<vector<Tile>>(map_data.num_tiles.x, vector<Tile>(num_tiles.y));

    string line;
    XYLoopCounter loop(num_tiles.x, num_tiles.y, false);
    while(getline(ss, line, '\n'))
    {
        if (line.size() == 0) continue;
        std::vector<string> id;
        id = split(line, ',');
        assert(id.size() > 0);
        while(true)
        {
            int x = loop.x;
            int y = loop.y;
            int idd = std::atoi(id[x].c_str());
            //cout << "ID " << x << ',' << y << ": " << idd << '\n';
            // do rotations
            // -----
            tiles[x][y].gid = idd;
            tiles[x][y].solid_flag = 0;
            if (idd == 2) tiles[x][y].is_solid = true;

            gid[loop.x][loop.y] = idd;
            loop.increment();
            if (loop.inner_loop_end) break;
        }
    }

    for (int x = 0; x < num_tiles.x; ++x)
    for (int y = 0; y < num_tiles.y; ++y)
    {
        Tile & tile = tiles[x][y];
        // set flags depending if adjacent tiles are solid
        if (y == 0 || tiles[x][y-1].is_solid)               tile.solid_flag |= TILE_SOLID_TOP;
        if (x == 0 || tiles[x-1][y].is_solid)               tile.solid_flag |= TILE_SOLID_LEFT;
        if (y == num_tiles.y-1 || tiles[x][y+1].is_solid)   tile.solid_flag |= TILE_SOLID_BOTTOM;
        if (x == num_tiles.x-1 || tiles[x+1][y].is_solid)   tile.solid_flag |= TILE_SOLID_RIGHT;
    }

    generateLines(tiles, num_tiles, {1, 0}, {0, 0}, ~TILE_SOLID_TOP, map_data.static_lines_top);
    generateLines(tiles, num_tiles, {1, 0}, {0, 1}, ~TILE_SOLID_BOTTOM, map_data.static_lines_bottom);
    generateLines(tiles, num_tiles, {0, 1}, {0, 0}, ~TILE_SOLID_LEFT, map_data.static_lines_left);
    generateLines(tiles, num_tiles, {0, 1}, {1, 0}, ~TILE_SOLID_RIGHT, map_data.static_lines_right);

}




XMLElement* getElement(XMLElement * e, const char* path)
{
    assert(e != nullptr);
    cout << "GetElement: " << path << '\n';

    vector<string> element = split(path, '.');
    reverse(element.begin(), element.end());

    while(element.size() > 0)
    {
        // check if an element needs a certain name
        vector<string> element_with_name = split(element.back(), '=');

        if (element_with_name.size() > 1) // needs a specific name, [0] == element tag, [1] = element name attribute
        {
            const char* element_tag =  element_with_name[0].c_str();
            const char* element_name =  element_with_name[1].c_str();

            e = e->FirstChildElement(element_tag);
            assert(e != nullptr && "Element tag does not exist");
            while (!e->Attribute("name", element_name))
            {
                e = e->NextSiblingElement(element_tag);
                assert(e != nullptr && "Element with given name not found!");
            }
        }
        else
        {
            e = e->FirstChildElement(element.back().c_str());
            assert(e != nullptr);
        }
        element.pop_back();
    }
    return e;
}

void generateLines(vector<vector<Tile>> const& tiles, Point num_tiles, Point line_direction, Point line_offset, int solid_bit_flag, vector<Line> & output_lines)
{
    XYLoopCounter loop(num_tiles.x, num_tiles.y, line_direction.x == 0);
    while (true)
    {
        bool is_creating_line = false;
        Point start;
        int length;


        while(true)
        {
            int x = loop.x;
            int y = loop.y;
            Tile const& tile = tiles[x][y];
            if (!is_creating_line)
            {
                if (tile.is_solid && (tile.solid_flag | solid_bit_flag) == solid_bit_flag)
                {
                    is_creating_line = true;
                    start = Point(x, y) + line_offset;
                    length = 1;
                }
            }
            else // creating line
            {
                if (tile.is_solid && (tile.solid_flag | solid_bit_flag) == solid_bit_flag) length++;
                else
                {
                    
                    output_lines.push_back(Line(start, start + line_direction * length));
                    is_creating_line = false;
                }
            }
            loop.increment();
            if (loop.inner_loop_end) break;
        }
        if (is_creating_line)
        {
            output_lines.push_back(Line(start, start + line_direction * length));
            //cout << "length: " << length << '\n';
        }
        if (loop.outer_loop_end) break;
    }
}

