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
#include <limits>
#include <cmath>
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




struct Player
{
	Point size;

	Point position;
	Point velocity;
};

constexpr int sign(float x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
};


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
	MapData map_data;
	parser.parse("assets/tmx/map_0.tmx", map_data);

	Point tile_size(20, 20);
	vector<Line> top_lines = map_data.static_lines_top;
	Point num_tiles = map_data.num_tiles;

	Player player;
	player.size = { 1, 1 };
	player.position = map_data.location_start;

	float gravity = 20;

	
		

	sf::ContextSettings cs;
	cs.antialiasingLevel = 16;

	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!", sf::Style::Default, cs);
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);
	// print context settings
	sf::ContextSettings settings = window.getSettings();
	std::cout << "OpenGL version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;
	std::cout << "Anti-aliasing: " << settings.antialiasingLevel << std::endl;
	std::cout << "Depth Bits: " << settings.depthBits << std::endl;
	std::cout << "Stencil Bits: " << settings.stencilBits << std::endl;
	std::cout << "sRGB Capable: " << std::boolalpha << settings.sRgbCapable << std::endl;

	sf::Clock clock;
	constexpr float elapsed = 1 / 60.f;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		int h = 0;
		int vv = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			h++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			h--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			vv++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			vv--;
		}
		player.velocity.x = h * 6;
		//player.velocity.y = vv * 6;
		auto is_int = [](float x) { return int(x) == x; };

		if ((is_int(player.position.x + 0.5) ? false : map_data.tiles[player.position.x + 0.5][player.position.y + 0.51].is_solid) || map_data.tiles[player.position.x - 0.5][player.position.y + 0.51].is_solid)
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			player.velocity.y = -20;
		}

		player.velocity.y += gravity * elapsed;
		Point old_pos = player.position;
		player.position += player.velocity * elapsed;

		// get leading edge
		Point leading;
		leading.x = player.position.x + player.size.x * sign(player.velocity.x) * 0.5;
		leading.y = player.position.y + player.size.y * sign(player.velocity.y) * 0.5;
		cout << "Leading:: " << leading << '\n';
		cout << "   --old   = " << old_pos << '\n';
		cout << "   --pos   = " << player.position << '\n';
		cout << "   --vel   = " << player.velocity << '\n';
		cout << "   --sign  = " << sign(player.velocity.x) << ',' << sign(player.velocity.y) << '\n';

		

		if ((is_int(old_pos.y + 0.5) ? false : map_data.tiles[int(leading.x)][int(old_pos.y + 0.5)].is_solid) || map_data.tiles[int(leading.x)][int(old_pos.y - 0.5)].is_solid)
		{
			if (player.velocity.x > 0) player.position.x = floor(leading.x) - player.size.x * 0.5;
			if (player.velocity.x < 0) player.position.x = ceil(leading.x) + player.size.x * 0.5;

			old_pos.x = player.position.x; // idk if this works, supposed to fix collision bug when collision occurs at corners of rectangles of tile & player

			player.velocity.x = 0;
		}
		if ((is_int(old_pos.x + 0.5) ? false : map_data.tiles[int(old_pos.x+0.5)][int(leading.y)].is_solid) || map_data.tiles[int(old_pos.x - 0.5)][int(leading.y)].is_solid)
		{
			if (player.velocity.y > 0) player.position.y = floor(leading.y) - player.size.y * 0.5;
			if (player.velocity.y < 0) player.position.y = ceil(leading.y) + player.size.y * 0.5;

			player.velocity.y = 0;
		}

		



		ImGui::SFML::Update(window, clock.restart());

		ImGui::Begin("Map loader");

		if (ImGui::Combo("Map", &current_tmx_index, tmx_list.c_str()))
		{
			cout << "CHOOOOSSSE: " << tmx_files[current_tmx_index] << '\n';
			map_data = {};
			parser.parse(tmx_files[current_tmx_index].c_str(), map_data);
			top_lines = map_data.static_lines_top;
			num_tiles = map_data.num_tiles;
		}

		ImGui::End();

		window.clear(sf::Color::Black);
		sf::RectangleShape r;
		r.setSize(sf::Vector2f(tile_size.x, tile_size.y));
		r.setFillColor(sf::Color(255, 255, 255, 100));
		for (int x = 0; x < num_tiles.x; ++x)
			for (int y = 0; y < num_tiles.y; ++y)
			{
				if (map_data.tiles[x][y].is_solid) continue;
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
		for (Line & l : map_data.static_lines_bottom)
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
		for (Line & l : map_data.static_lines_left)
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
		for (Line & l : map_data.static_lines_right)
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

		// draw player
		sf::RectangleShape rect;
		rect.setPosition(player.position.x * tile_size.x, player.position.y * tile_size.y);
		rect.setSize(sf::Vector2f(player.size.x * tile_size.x, player.size.y * tile_size.y));
		rect.setOrigin(tile_size.x * 0.5, tile_size.y * 0.5);
		window.draw(rect);


		ImGui::SFML::Render(window);
		window.display();
	}

	getchar();
	return 0;
}


