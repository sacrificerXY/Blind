// Blind.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include <string>

#include <plf_colony.h>


#include "Point.h"
#include "tinyxml2.h"
#include "TmxParser.h"
#include "MapData.h"
#include "string_split.h"
#include "loop_utility.h"



using namespace std;
using namespace tinyxml2;

bool isNotError(XMLError error)
{
	cout << "Error code: " << static_cast<int>(error) << '\n';
	//    XMLError e = XMLError::XML_ERROR_FILE_NOT_FOUND;
	//    assert(e != XML_ERROR_FILE_NOT_FOUND);
	switch (error)
	{
	case XML_SUCCESS:
		cout << "Success!\n";
		return true;
		break;
	case XML_ERROR_FILE_NOT_FOUND:
		assert("XML error: File not found!\n");
		break;
	default:
		assert("XML error: error!\n");
	}
	return false;
}

XMLElement* getElement(const char* path)
{

}



struct NestedLoopCounter
{

};

int main()
{
	// loading assets

	TmxParser parser;
	MapData mapData;
	parser.parse("assets/tmx/test.tmx", mapData);

	XMLDocument doc;

	Point tile_size(20, 20);


	doc.LoadFile("assets/tmx/test.tmx");

	vector<Line> top_lines;



	//    cout<< doc.Value();
	XMLElement* e = doc.FirstChildElement("map");
	Point num_tiles;
	num_tiles.x = e->IntAttribute("width", 0);
	num_tiles.y = e->IntAttribute("height", 0);
	assert(e != nullptr);
	e = e->FirstChildElement("layer");
	assert(e != nullptr);
	e = e->FirstChildElement("data");
	assert(e != nullptr);
	cout << e->GetText();
	cout << "Map: " << num_tiles << '\n';
	stringstream ss;
	ss << e->GetText();
	XYLoopCounter loop(num_tiles.x, num_tiles.y, false);
	vector<vector<int>> gid(num_tiles.x, vector<int>(num_tiles.y));
	string line;
	int y = 0;
	while (getline(ss, line, '\n'))
	{
		if (line.size() == 0) continue;
		cout << "Line: " << line << '\n';
		std::vector<string> id;
		id = split(line, ',');
		assert(id.size() > 0);
		//        for (int x = 0; x < num_tiles.x; ++x)
		while (true)
		{
			int idd = std::atoi(id[loop.x].c_str());
			gid[loop.x][loop.y] = idd;
			loop.increment();
			if (loop.inner_loop_end) break;
		}
	}

	XMLPrinter printer;
	doc.Print(&printer);

	top_lines = mapData.static_lines_top;




	plf::colony<int> colony;

	sf::ContextSettings cs;
	cs.antialiasingLevel = 16;

	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!", sf::Style::Default, cs);
	// print context settings
	sf::ContextSettings settings = window.getSettings();
	std::cout << "OpenGL version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;
	std::cout << "Anti-aliasing: " << settings.antialiasingLevel << std::endl;
	std::cout << "Depth Bits: " << settings.depthBits << std::endl;
	std::cout << "Stencil Bits: " << settings.stencilBits << std::endl;
	std::cout << "sRGB Capable: " << std::boolalpha << settings.sRgbCapable << std::endl;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color::Black);
		sf::RectangleShape r;
		r.setSize(sf::Vector2f(tile_size.x, tile_size.y));
		r.setFillColor(sf::Color(255, 255, 255, 100));
		for (int x = 0; x < num_tiles.x; ++x)
			for (int y = 0; y < num_tiles.y; ++y)
			{
				if (gid[x][y] == 0) continue;
				r.setPosition(tile_size.x * x, tile_size.y * y);
				window.draw(r);
			}
		sf::VertexArray va{ sf::Lines };
		sf::VertexArray vaTriangles{ sf::Triangles };
		sf::Vertex v;
		v.color = sf::Color(255, 0, 0, 255);
		constexpr float line_thickness = 0.25;
		for (Line & l : top_lines)
		{
			v.position.x = l.p1.x * tile_size.x;
			v.position.y = l.p1.y * tile_size.y;
			vaTriangles.append(v);
			v.position.x = l.p2.x * tile_size.x;
			v.position.y = l.p2.y * tile_size.y;
			vaTriangles.append(v);

			v.position.x = (l.p1.x + l.p2.x) * 0.5 * tile_size.x;
			v.position.y = (l.p1.y + line_thickness) * tile_size.y;
			vaTriangles.append(v);
		}
		v.color = sf::Color(0, 255, 0, 255);
		for (Line & l : mapData.static_lines_bottom)
		{
			v.position.x = l.p1.x * tile_size.x;
			v.position.y = l.p1.y * tile_size.y;
			vaTriangles.append(v);
			v.position.x = l.p2.x * tile_size.x;
			v.position.y = l.p2.y * tile_size.y;
			vaTriangles.append(v);

			v.position.x = (l.p1.x + l.p2.x) * 0.5 * tile_size.x;
			v.position.y = (l.p1.y - line_thickness) * tile_size.y;
			vaTriangles.append(v);
		}

		v.color = sf::Color(0, 0, 255, 255);
		for (Line & l : mapData.static_lines_left)
		{
			v.position.x = l.p1.x * tile_size.x;
			v.position.y = l.p1.y * tile_size.y;
			vaTriangles.append(v);
			v.position.x = l.p2.x * tile_size.x;
			v.position.y = l.p2.y * tile_size.y;
			vaTriangles.append(v);

			v.position.x = (l.p1.x + line_thickness) * tile_size.x;
			v.position.y = (l.p1.y + l.p2.y) * 0.5 * tile_size.y;
			vaTriangles.append(v);
		}
		v.color = sf::Color(255, 255, 0, 255);
		for (Line & l : mapData.static_lines_right)
		{
			v.position.x = l.p1.x * tile_size.x;
			v.position.y = l.p1.y * tile_size.y;
			vaTriangles.append(v);
			v.position.x = l.p2.x * tile_size.x;
			v.position.y = l.p2.y * tile_size.y;
			vaTriangles.append(v);

			v.position.x = (l.p1.x - line_thickness) * tile_size.x;
			v.position.y = (l.p1.y + l.p2.y) * 0.5 * tile_size.y;
			vaTriangles.append(v);
		}
		//        window.draw(va);
		window.draw(vaTriangles);
		window.display();
	}

	return 0;
}


