#pragma once
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <random>
#include <cstdarg>
#include <ncurses.h>

using namespace std; //Boo hiss

class Map {
	vector<vector<char>> map;
	default_random_engine gen;
	public:
	//Write a getter and a setter to get/set the characters in the map
	char getTile(int x, int y) const {
		return map.at(y).at(x);
	}
	void setTile(int x, int y, char type) {
		map.at(y).at(x) = type;
	}
	//TODO: Write a function to save the map and reload the map
	void save_map(){                                                                                                                                                    
        	ofstream newFile;                                                                                                                                               
        	newFile.open ("save.txt");                                                                                                                                      
        	for (size_t i = 0; i < SIZE; i++) {                                                                                                                             
            		for (size_t j = 0; j < SIZE; j++) {                                                                                                                         
                		newFile << map.at(i).at(j)  << endl;                                                                                                                    
            		}                                                                                                                                                           
        	}                                                                                                                                                               
    	}                                                                                                                                                                   
                                                                                                                                                                        
    	void load_map(){                                                                                                                                                    
        	ifstream readfile;                                                                                                                                              
        	char type;                                                                                                                                                      
        	readfile.open ("save.txt");                                                                                                                                     
        	for (size_t i = 0; i < SIZE; i++) {                                                                                                                             
            		for (size_t j = 0; j < SIZE; j++) {                                                                                                                         
                		readfile >> map.at(i).at(j);                                                                                                                            
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
	static const size_t DISPLAY = 20; //Show a 20x20 area at a time
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
	Map() {
		init_map();
	}
};

class Menu {
	static const size_t DISPLAY = 20; //Show a 20x20 area at a time
	vector<const char*> options;
	vector<int> colors;
	public:
	Menu(int opt, ...) {
		options.clear();
		va_list valist;
		va_start(valist, opt);
		for (int i = 0; i < opt; i++)
			options.push_back(va_arg(valist, const char*));
		va_end(valist);
	}
	void change_option(int opt, const char* text) {
		options.at(opt) = text;
	}
	void add_color(int opt, ...) {
		colors.clear();
		va_list valist;
		va_start(valist, opt);
		for (int i = 0; i < opt; i++)
			colors.push_back(va_arg(valist, int));
		va_end(valist);
	}
	void draw(int option=1) const {
		for (size_t i = 0; i < DISPLAY; i++) {
			if (i != DISPLAY - 1)
				mvaddch(0,i+1,'_');
			mvaddch(DISPLAY,i,'_');
			mvaddch(i+1,0,'|');
			mvaddch(i+1,DISPLAY,'|');
		}
		if (!options.empty()) {
			attron(COLOR_PAIR(colors.at(0)));
			mvprintw(2, (DISPLAY-strlen(options.at(0)))/2, options.at(0));
			attroff(COLOR_PAIR(colors.at(0)));
			for (size_t i=1; i < options.size(); i++) {
				attron(COLOR_PAIR(colors.at(i)));
				mvprintw(i*3+3, (DISPLAY-strlen(options.at(i)))/2, options.at(i));
				attroff(COLOR_PAIR(colors.at(i)));
			}
			mvaddch(option*3+3, (DISPLAY-strlen(options.at(option)))/2-1, '>');
		}
	}
};
