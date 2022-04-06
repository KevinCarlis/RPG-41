#pragma once
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <random>
#include <cstdarg>
#include <ncurses.h>
using namespace std; //Boo hiss
	
const int UP = 65; //Key code for up arrow
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;
const int ENTER = 10;
const size_t DISPLAY = 20; //Show a 20x20 area at a time

class Map {
	vector<vector<char>> map;
	default_random_engine gen;
	public:
	char getTile(int x, int y) const {
		return map.at(y).at(x);
	}
	void setTile(int x, int y, char type) {
		map.at(y).at(x) = type;
	}
	void save(string filename="save.txt") {
		ofstream outfile;
		outfile.open(filename);
		for (size_t i = 0; i < SIZE; i++) {
			for (size_t j = 0; j < SIZE; j++) {
				outfile << map.at(i).at(j); 
			}
			outfile << endl;
		}
	}
	void load(string filename="save.txt") {
		ifstream infile;
		infile.open(filename);
		map.clear();
		map.resize(SIZE);
		for (auto &v : map) v.resize(SIZE,'.');
		for (size_t i = 0; i < SIZE; i++) {
			for (size_t j = 0; j < SIZE; j++) {
				infile >> map.at(i).at(j);
			}
		}
	}

	static const char HERO     = 'H';
	static const char MONSTER  = 'M';
	static const char WALL     = '#';
	static const char WATER    = '~';
	static const char OPEN     = '.';
	static const char TREASURE = '$';
	static const size_t SIZE = 100; //World is a 100x100 map
	//Randomly generate map
	void init_map() {
		uniform_int_distribution<int> d100(1,100);
		map.clear();
		map.resize(SIZE); //100 rows tall
		for (auto &v : map) v.resize(SIZE,'.'); //100 columns wide
		for (size_t i = 0; i < SIZE; i++) {
			for (size_t j = 0; j < SIZE; j++) {
				//Line the map with walls
				if (i == 0 or j == 0 or i == SIZE-1 or j == SIZE-1) 
					map.at(i).at(j) = WALL;
				else if (i == SIZE/2 and j == SIZE/2) 
					map.at(i).at(j) = OPEN;
				else {
					//5% chance of monster
					if (d100(gen) <= 5) {
						map.at(i).at(j) = MONSTER;
					}
					else if (d100(gen) <= 3) {
						map.at(i).at(j) = TREASURE;
					}
					else if (d100(gen) <= 10) { //10% each spot is wall
						map.at(i).at(j) = WALL;
					}
					else if (d100(gen) <= 3) { //3% each spot is water
						map.at(i).at(j) = WATER;
					}
					else if (d100(gen) <= 20) { //20% chance of water near other water
						if (map.at(i-1).at(j) == WATER or map.at(i+1).at(j) == WATER or map.at(i).at(j-1) == WATER or map.at(i).at(j+1) == WATER)
							map.at(i).at(j) = WATER;
					}
				}
			}
		}
	}
	//Draw the DISPLAY tiles around coordinate (x,y)
	void draw(int x, int y) const {
		int start_x = x - DISPLAY/2;
		int end_x = x + DISPLAY/2;
		int start_y = y - DISPLAY/2;
		int end_y = y + DISPLAY/2;

		//Bounds check to handle the edges
		if (start_x < 0) {
			end_x = end_x - start_x;
			start_x = 0;
		}
		if (end_x > SIZE-1) {
			start_x = start_x - (end_x - (SIZE-1));
			end_x = SIZE-1;
		}
		if (start_y < 0) {
			end_y = end_y - start_y;
			start_y = 0;
		}
		if (end_y > SIZE-1) {
			start_y = start_y - (end_y - (SIZE-1));
			end_y = SIZE-1;
		}

		//Now draw the map using NCURSES
		for (size_t i = start_y; i <= end_y; i++) {
			for (size_t j = start_x; j <= end_x; j++) {
				if (i == y && j == x) {
					attron(A_UNDERLINE | A_BOLD);
					mvaddch(i-start_y,j-start_x,'H');
					attroff(A_UNDERLINE | A_BOLD);
				}
				else {
					int color = 1;
					if (map.at(i).at(j) == WALL)
						color = 5;
					else if (map.at(i).at(j) == WATER)
						color = 2;
					else if (map.at(i).at(j) == HERO)
						color = 3;
					else if (map.at(i).at(j) == TREASURE)
						color = 4;
					else if (map.at(i).at(j) == MONSTER)
						color = 6;

					attron(COLOR_PAIR(color));
					mvaddch(i-start_y,j-start_x,map.at(i).at(j));
					attroff(COLOR_PAIR(color));
				}
			}
		}
	}
	void control(int ch, int &x, int &y) {
		if (ch == RIGHT) {
			if (x < SIZE)
				x++;
		}
		else if (ch == LEFT) {
			if (x > 0)
				x--;
		}
		else if (ch == UP) {
			if (y > 0)
				y--;
		}
		else if (ch == DOWN) {
			if (y < SIZE)
				y++;
		}
	}
	Map() {;}
	Map(string savefile) {
		load(savefile);
	}
};

class Menu {
	vector<string> options;
	vector<int> colors;
	vector<int> rows;
	public:
	void add_option(string opt, int color=1) {
		options.push_back(opt);
		colors.push_back(color);
		rows.push_back(options.size()*3-1);
	}
	void add_option(string opt, int color, int row) {
		options.push_back(opt);
		colors.push_back(color);
		rows.push_back(row);
	}
	void change_option(int opt, const char* text) {
		options.at(opt) = text;
	}
	void change_option(int opt, const char* text, int color) {
		options.at(opt) = text;
		change_color(opt, color);
	}
	void change_color(int opt, int color) {
		colors.at(opt) = color;
	}
	void draw(int option=-1) const {
		clear();
		for (size_t i = 0; i < DISPLAY; i++) {
			mvaddch(0,i+1,'_');
			mvaddch(DISPLAY,i,'_');
			mvaddch(i+1,0,'|');
			mvaddch(i+1,DISPLAY,'|');
		}
		mvaddch(0, DISPLAY, ' ');
		if (!options.empty()) {
			for (size_t i=0; i < options.size(); i++) {
				attron(COLOR_PAIR(colors.at(i)));
				mvprintw(rows.at(i), (DISPLAY-options.at(i).length())/2+1, options.at(i).c_str());
				attroff(COLOR_PAIR(colors.at(i)));
			}
			if (options.size() > 1 && option != -1)
				mvaddch(rows.at(option), (DISPLAY-options.at(option).length())/2, '>');
		}
		refresh();
	}
	void control(int ch, int &option) {
		if (ch == DOWN) {
			mvaddch(rows.at(option), (DISPLAY-options.at(option).length())/2, ' ');
			if (option < options.size() - 1)
				option++;
			else
				option = 1;
		}
		else if (ch == UP) {
			mvaddch(rows.at(option), (DISPLAY-options.at(option).length())/2, ' ');
			if (option > 1)
				option--;
			else
				option = options.size() - 1;
		}
		mvaddch(rows.at(option), (DISPLAY-options.at(option).length())/2, '>');
		refresh();
	}
};
