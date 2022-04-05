//Put your name(s) here:
//What bullet points did you do:
#include "map.h"
#include <unistd.h>

const int MAX_FPS = 90; //Cap frame rate 
const unsigned int TIMEOUT = 10; //Milliseconds to wait for a getch to finish
const int UP = 65; //Key code for up arrow
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;

//Turns on full screen text mode
void turn_on_ncurses() {
	initscr();//Start curses mode
	start_color(); //Enable Colors if possible
	init_pair(1,COLOR_WHITE,COLOR_BLACK); //Set up some color pairs
	init_pair(2,COLOR_CYAN,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_YELLOW,COLOR_BLACK);
	init_pair(5,COLOR_RED,COLOR_BLACK);
	init_pair(6,COLOR_MAGENTA,COLOR_BLACK);
	clear();
	noecho();
	cbreak();
	timeout(TIMEOUT); //Set a max delay for key entry
}

//Exit full screen mode - also do this if you ever want to use cout or gtest or something
void turn_off_ncurses() {
	clear();
	endwin(); // End curses mode
	system("clear");
}

int main() {
	turn_on_ncurses();
	Map map;
	int x = Map::SIZE / 2, y = Map::SIZE / 2; //Start in middle of the world
	int old_x = -1, old_y = -1;
	int money = 0;
	bool battle = false;
	while (true) {
		int ch = getch(); // Wait for user input, with TIMEOUT delay
		if (ch == 'q' || ch == 'Q') break;
		else if (ch == RIGHT) {
			x++;
			if (x >= Map::SIZE) x = Map::SIZE - 1; //Clamp value
		}
		else if (ch == LEFT) {
			x--;
			if (x < 0) x = 0;
		}
		else if (ch == UP) {
			y--;
			if (y < 0) y = 0;
		}
		else if (ch == DOWN) {
			y++;
			if (y >= Map::SIZE) y = Map::SIZE - 1; //Clamp value
		}
		else if (ch == ERR) { //No keystroke
			; //Do nothing
		}
		//Stop flickering by only redrawing on a change
		if (x != old_x or y != old_y) {
			//clear(); //Put this in if the screen is getting corrupted
			if (map.getTile(x,y) == Map::MONSTER) {
				battle = true;
			}
			if (map.getTile(x,y) == Map::TREASURE) {
				map.setTile(x,y,Map::OPEN);
				money += 10;
			}
			if (map.getTile(x,y) == Map::WATER || map.getTile(x,y) == Map::WALL) {
				x = old_x;
				y = old_y;
			}
			else {
				map.draw(x,y);
				mvprintw(Map::DISPLAY+1,0,"X: %i Y: %i\n",x,y);
				mvprintw(Map::DISPLAY+2,0,"Money: %i\n",money);
				refresh();
				old_x = x;
				old_y = y;
			}
		}
		if (battle) {
			Menu fight(6,"Hero", "Attack", "Special 1", "Special 2", "Pass", "Run");
			fight.add_color(6,1,2,3,4,5,6);
			int option = 1, old_option=0;
			while(true) {
				ch = getch();
				if (ch == DOWN) {
					if (option < 5)
						option++;
				}
				else if (ch == UP) {
					if (option > 1)
						option--;
				}
				if (option != old_option) {
					clear();
					fight.draw(option);
					refresh();
					old_option = option;
				}
			}
		}
		usleep(1'000'000/MAX_FPS);
	}
	turn_off_ncurses();
}
