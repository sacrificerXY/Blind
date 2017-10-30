// Blind.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::experimental::filesystem;

#include <plf_colony.h>


#include "Point.h"
#include "tinyxml2.h"
#include "TmxParser.h"
#include "MapData.h"
#include "string_split.h"
#include "loop_utility.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"



using namespace std;
using namespace tinyxml2;


int main()
{
	// get map list
	std::string path = "assets/tmx";
	std::string tmx_extension = ".tmx";
	vector<string> tmx_files;
	string tmx_list = "";
	int current_tmx_index = 0;
	for (auto & p : fs::directory_iterator(path))
	{
		std::string ext = p.path().extension().string();
		if (ext == tmx_extension)
		{
			std::cout << p.path().filename() << std::endl;
			tmx_files.push_back(p.path().string());
			cout << tmx_files.back() << '\n';
			tmx_list += p.path().filename().string() + '\0';
		}
	}
	tmx_list += '\0';

	// loading assets

	TmxParser parser;
	MapData mapData;
	parser.parse("assets/tmx/map_0.tmx", mapData);

	Point tile_size(20, 20);
	vector<Line> top_lines = mapData.static_lines_top;
	Point num_tiles = mapData.num_tiles;

	
		

	sf::ContextSettings cs;
	cs.antialiasingLevel = 16;

	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!", sf::Style::Default, cs);
	ImGui::SFML::Init(window);
	// print context settings
	sf::ContextSettings settings = window.getSettings();
	std::cout << "OpenGL version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;
	std::cout << "Anti-aliasing: " << settings.antialiasingLevel << std::endl;
	std::cout << "Depth Bits: " << settings.depthBits << std::endl;
	std::cout << "Stencil Bits: " << settings.stencilBits << std::endl;
	std::cout << "sRGB Capable: " << std::boolalpha << settings.sRgbCapable << std::endl;

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}
		ImGui::SFML::Update(window, clock.restart());

		ImGui::Begin("Map loader");

		if (ImGui::Combo("Map", &current_tmx_index, tmx_list.c_str()))
		{
			cout << "CHOOOOSSSE: " << tmx_files[current_tmx_index] << '\n';
			mapData = {};
			parser.parse(tmx_files[current_tmx_index].c_str(), mapData);
			top_lines = mapData.static_lines_top;
			num_tiles = mapData.num_tiles;
		}

		ImGui::End();

		window.clear(sf::Color::Black);
		sf::RectangleShape r;
		r.setSize(sf::Vector2f(tile_size.x, tile_size.y));
		r.setFillColor(sf::Color(255, 255, 255, 100));
		for (int x = 0; x < num_tiles.x; ++x)
			for (int y = 0; y < num_tiles.y; ++y)
			{
				if (mapData.tiles[x][y].is_solid) continue;
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
		ImGui::SFML::Render(window);
		window.display();
	}

	getchar();
	return 0;
}


