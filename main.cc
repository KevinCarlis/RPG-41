//Put your name(s) here:
//What bullet points did you do:
#include "map.h"
#include "actor.h"
#include <unistd.h>
#include <string>
#include <memory>

const int MAX_FPS = 90; //Cap frame rate 
const unsigned int TIMEOUT = 10; //Milliseconds to wait for a getch to finish

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
	int systemRet = system("clear");
	if (systemRet == -1)
		return;
}

bool main_menu() {
	Menu start;
	start.add_option("Main Menu");
	start.add_option("Continue");
	start.add_option("Save");
	start.add_option("Quit");
	int option = 1;
	start.draw(option);
	while(true) {
		int ch = getch();
		if (ch == DOWN || ch == UP) {
			start.control(ch, option);
		}
		usleep(1'000'000/MAX_FPS);
		if (ch == ENTER) {
			if (option == 1)
				return false;
			else if (option == 2) {
				return true;
			}
			else {
				turn_off_ncurses();
				exit(EXIT_SUCCESS);
			}
		}
	}
}

int main() {
	turn_on_ncurses();
	Map map;
	int ch = getch();
	int option = 1;
	vector<shared_ptr<Hero>> heroes;

	Menu select;
	select.add_option("Game Title", 5);
	select.add_option("New Game");
	select.add_option("Load Game");
	select.add_option("Quit");
	select.draw(option);

	while (true) {
		ch = getch();
		if (ch == DOWN || ch == UP) {
			select.control(ch, option);
		}
		//if(ch != ERR)
			//mvprintw(0, 25, to_string(ch).c_str()); //figure out key value
		usleep(1'000'000/MAX_FPS);
		if (ch == ENTER) {
			break;
		}
	}
	if (option == 1) {
		select.change_option(0, "Choose your Wizards");
		select.change_option(1, "Earth Wizard", 3);
		select.change_option(2, " Wizard");
		select.change_option(3, " Wizard", 4);
		select.add_option(" Wizard", 5);
		select.add_option(" Wizard", 6);
		select.draw(option);

		int h = 0;
		while(h < 4) {
			ch = getch();
			if (ch == DOWN || ch == UP) {
				select.control(ch, option);
			}
			usleep(1'000'000/MAX_FPS);
			if (ch == ENTER) {
				h++;
				//select.change_option(0, ("Select Hero " + to_string(heroes + 1)).c_str());
				turn_off_ncurses();
				string name;
				cout << "Enter a name:" << endl;
				cin >> name;
				turn_on_ncurses();
				//if (option == 1) {
				shared_ptr<Hero> hero = make_shared<EarthWizard>(name);
				//else if (option == 2)//TODO: Add classes
				//else if (option == 3)
				//else if (option == 4)
				//else
				heroes.push_back(hero);
				select.draw(option);
			}
		}
		map.init_map();
		//turn_off_ncurses();
		//for (auto h: heroes)
			//cout << h->GetName() << endl;
	}
	else if (option == 2) {
		map.load("save.txt"); //TODO: write load functions
	}
	else {
		turn_off_ncurses();
		exit(EXIT_SUCCESS);
	}

	int x = Map::SIZE / 2, y = Map::SIZE / 2; //Start in middle of the world
	int old_x = -1, old_y = -1;
	int money = 0;
	bool battle = false;
	bool save = false;
	while (true) {
		ch = getch(); // Wait for user input, with TIMEOUT delay
		if (ch == ERR) { //No keystroke
			; //Do nothing
		}
		else if (ch == DOWN || ch == UP || ch == LEFT || ch == RIGHT)
			map.control(ch, x, y);
		else if (ch == 'q' || ch == 'Q') {
			save = main_menu();
			if (save) {
				map.save();//TODO: write save;
			}
			old_x -= 1;
		}
		//Stop flickering by only redrawing on a change
		if (x != old_x || y != old_y) {
			//clear(); //Put this in if the screen is getting corrupted
			if (map.getTile(x,y) == Map::TREASURE) {
				map.setTile(x,y,Map::OPEN);
				money += 10;
			}
			if (map.getTile(x,y) == Map::WATER || map.getTile(x,y) == Map::WALL) {
				x = old_x;
				y = old_y;
			}
			else if (map.getTile(x,y) == Map::MONSTER) {
				battle = true;
				map.setTile(x,y,Map::OPEN);
			}
			else {
				map.draw(x,y);
				mvprintw(DISPLAY+1,0,"X: %i Y: %i\n",x,y);
				mvprintw(DISPLAY+2,0,"Money: %i\n",money);
				refresh();
				old_x = x;
				old_y = y;
			}
		}
		usleep(1'000'000/MAX_FPS);
		if (battle) {
			Menu fight;
			fight.add_option("Hero", 3);
			fight.add_option("Attack");
			fight.add_option("Special 1");
			fight.add_option("Special 2");
			fight.add_option("Pass");
			fight.add_option("Run");
			option = 1;
			fight.draw(option);
			while(battle) {
				ch = getch();
				if (ch == DOWN || ch == UP) {
					fight.control(ch, option);
				}
				usleep(1'000'000/MAX_FPS);
				if (ch == ENTER) {
					battle = false;
				}
			}
		}
	}
	turn_off_ncurses();
}
