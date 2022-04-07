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

void fight_menu(int &option, shared_ptr<Hero> &h) {
	Menu fight;
	fight.add_option(h->GetName(), h->GetColor());
	fight.add_option("Attack");
	fight.add_option("Special 1");
	fight.add_option("Special 2");
	fight.add_option("Pass");
	fight.add_option("Party");
	fight.draw(option);
	while (true) {
		int ch = getch();
		if (ch == DOWN || ch == UP) {
			fight.control(ch, option);
		}
		usleep(1'000'000/MAX_FPS);
		if (ch == ENTER) {
			return;
		}
	}
}

template <class T>
shared_ptr<T> actor_select(vector<shared_ptr<T>> &v, int option=1) {
	Menu select;
	select.add_option("Select Target");
	for (int i = 0; i < v.size(); i++) {
		select.add_option(v.at(i)->GetName(), 1, i*3+5);
		select.add_option(v.at(i)->print(),   1, i*3+6);
	}
	select.draw(1);
	while (true) {
		int ch = getch();
		if (ch == DOWN || ch == UP) {
			select.actor_control(ch, option);
		}
		usleep(1'000'000/MAX_FPS);
		if (ch == ENTER) {
			return v.at(option-1);
		}
	}
}

void party_menu(vector<shared_ptr<Hero>> &v) {
	Menu list;
	list.add_option("Party");
	for (int i = 0; i < v.size(); i++) {
		list.add_option(v.at(i)->GetName(), 1, i*3+5);
		list.add_option(v.at(i)->print(), 1, i*3+6);
	}
	list.add_option("Exit", 1, 19);
	list.draw();
	mvaddch(19, 8, '>');
	while (true) {
		int ch = getch();
		usleep(1'000'000/MAX_FPS);
		if (ch == ENTER) {
			return;
		}
	}
}

void save_party(vector<shared_ptr<Hero>> &v, string file="heroes.txt") {
	ofstream out;
	out.open(file);
	for (auto h : v) {
		if (h->GetMove1()=="Boulder Barrage")
			out << "Earth" << '\t';
		out << h->GetName() << '\t';
		out << h->GetHP().GetCurr() << '\t';
		out << h->GetHP().GetTemp() << '\t';
		out << h->GetMana().GetCurr() << '\t';
		out << endl;
	}
	out.close();
}

vector<shared_ptr<Hero>> load_party(string file="heroes.txt") {
	vector<shared_ptr<Hero>> v;
	shared_ptr<Hero> hero;
	ifstream in;
	in.open(file);
	string type, name;
	int currhp, temphp, mana;
	for (int h = 0; h < 4; h++) {
		in >> type;
		if (!in) break;
		in >> name;
		in >> currhp;
		in >> temphp;
		in >> mana;
		if (type == "Earth")
			hero = make_shared<EarthWizard>(name, currhp, temphp, mana);
		v.push_back(hero);
	}
	return v;
}

int main() {
	turn_on_ncurses();
	turn_off_ncurses();
	turn_on_ncurses();
	Map map;
	int ch = getch();
	int option = 1;
	int x = Map::SIZE / 2, y = Map::SIZE / 2; //Start in middle of the world
	int old_x = -1, old_y = -1;
	int money = 0;
	bool battle = false;
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
	bool save = false;
	if (option == 2) {
		save = map.load();
		heroes = load_party();
		ifstream in;
		in.open("save.txt");
		in >> money;
		in >> x;
		in >> y;
		if (!in) save = false;
		in.close();
	}
	if (option == 1 || !save || heroes.size() == 0) {
		heroes.clear();
		option = 1;
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
		//for (auto h: heroes) {
		//	cout << h->GetName() << endl;
		//	cout << h->print() << endl << endl;
		//}
		map.init_map();
	}
	else if (option == 3) {
		turn_off_ncurses();
		exit(EXIT_SUCCESS);
	}

	while (true) {
		ch = getch(); // Wait for user input, with TIMEOUT delay
		if (ch == ERR) {;}
		else if (ch == DOWN || ch == UP || ch == LEFT || ch == RIGHT)
			map.control(ch, x, y);
		else if (ch == 'q' || ch == 'Q') {
			Menu start;
			start.add_option("Main Menu");
			start.add_option("Continue");
			start.add_option("Party");
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
						break;
					if (option == 2) {
						party_menu(heroes);
						start.draw(option);
					}
					else if (option == 3) {
						map.save();
						save_party(heroes);
						ofstream out;
						out.open("save.txt");
						out << money << '\t' << x << '\t' << y;
						out.close();
						break;
					}
					else {
						turn_off_ncurses();
						exit(EXIT_SUCCESS);
					}
				}
			}
			old_x -= 1;
		}
		if (x != old_x || y != old_y) {
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
			option = 1;
			shared_ptr<Hero> hero = heroes.at(0);
			while (battle) {
				fight_menu(option, hero);
				if (option == 2 || option == 3) {
					actor_select(heroes);
				}
				else if (option == 5) {
					party_menu(heroes);
				}
				else {
					battle = false;
				}
			}
		}
	}
	turn_off_ncurses();
}
