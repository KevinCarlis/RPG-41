//Put your name(s) here:
//What bullet points did you do:
#include "map.h"
#include "circsl.h"
#include <unistd.h>
#include <string>
#include <memory>
#include <time.h>

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

void fight_menu(shared_ptr<Hero> h, int &option) {
	Menu fight;
	fight.add_option(h->name, h->color);
	fight.add_option(h->print(),1,3);
	fight.add_option("Attack", 1, 6);
	fight.add_option(h->move1, 1, 9);
	fight.add_option(h->move2, 1, 12);
	fight.add_option("Pass", 1, 15);
	fight.add_option("Party", 1, 18);
	fight.draw(option, 5);
	while (true) {
		int ch = getch();
		if (ch == DOWN || ch == UP) {
			fight.control(ch, option, 5);
		}
		usleep(1'000'000/MAX_FPS);
		if (ch == ENTER) {
			return;
		}
	}
}

template <class T>
void actor_select(vector<shared_ptr<T>> &v, int &option=1) {
	Menu select;
	select.add_option("Select Target");
	for (int i = 0; i < v.size(); i++) {
		select.add_option(v.at(i)->name, 1, i*3+4);
		select.add_option(v.at(i)->print(),   1, i*3+5);
	}
	select.add_option("Cancel", 1, 19);
	select.draw(option*2-1);
	while (true) {
		int ch = getch();
		if (ch == DOWN || ch == UP) {
			select.actor_control(ch, option);
		}
		else if (ch == ENTER) {
			return;
		}
		usleep(1'000'000/MAX_FPS);
	}
}

void shop_menu(vector<shared_ptr<Hero>> &v, int &cash, int &bounty, bool &k) {
	int option = 1;
	Menu shop;
	shop.add_option("Megalo Mart");
	shop.add_option("Heal party", 5, 5);
	shop.add_option("50 gold", 1, 6);
	shop.add_option("Restore Mana", 2, 8);
	shop.add_option("50 gold", 1, 9);
	if (!k) {
		shop.add_option("Important Key", 3, 11);
		shop.add_option("20 Skulls", 1, 12);
	}
	else {
		shop.add_option("Protein Powder", 3, 11);
		shop.add_option("10 Skulls", 1, 12);
	}
	shop.add_option("Leave", 1, 15);
	shop.draw(option*2-1);
	while (true) {
		int ch = getch();
		if (ch == DOWN || ch == UP) {
			shop.actor_control(ch, option);
		}
		else if (ch == ENTER) {
			if (option == 1 && cash >= 50) {
				cash -= 50;
				for (auto h: v)
					h->hp.regen();
				return;
			}
			else if (option == 2 && cash >= 50) {
				cash -= 50;
				for (auto h: v)
					h->mana.regen();
				return;
			}
			else if (option == 3 && bounty >= 10 && !k) {
				bounty -= 20;
				k = true;
			}
			else if (option == 3 && bounty >= 10 && k) {
				bounty -= 10;
				for (auto h: v)
					h->power += 1;
				return;
			}
			else if (option == 4) {
				return;
			}
		}
		usleep(1'000'000/MAX_FPS);
	}
}

void party_menu(vector<shared_ptr<Hero>> &v) {
	Menu list;
	list.add_option("Party");
	for (int i = 0; i < v.size(); i++) {
		list.add_option(v.at(i)->name, 1, i*3+5);
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
		if (h->move1=="Ice Lance")
			out << "Frost" << '\t';
		else if (h->move1=="Boulder Barrage")
			out << "Earth" << '\t';
		else if (h->move1=="Self Lighteous")
			out << "Light" << '\t';
		else if (h->move1=="Firestorm")
			out << "Fire" << '\t';
		else //if (h->move1=="Arcane Missiles")
			out << "Arcane" << '\t';
		out << h->name << '\t';
		out << h->GetHP() << '\t';
		out << h->GetShield() << '\t';
		out << h->GetMana() << '\t';
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
		if (type == "Frost")
			hero = make_shared<FrostWizard>(name, currhp, temphp, mana);
		else if (type == "Earth")
			hero = make_shared<EarthWizard>(name, currhp, temphp, mana);
		else if (type == "Light")
			hero = make_shared<LightWizard>(name, currhp, temphp, mana);
		else if (type == "Fire")
			hero = make_shared<FireWizard>(name, currhp, temphp, mana);
		else //if (type == "Arcane")
			hero = make_shared<ArcaneWizard>(name, currhp, temphp, mana);
		v.push_back(hero);
	}
	return v;
}

int main() {
	turn_on_ncurses();
	turn_off_ncurses();
	turn_on_ncurses();

	srand(time(NULL));

	Map map;
	int ch = getch();
	int option = 1;
	int x = Map::SIZE / 2, y = Map::SIZE / 2; //Start in middle of the world
	int old_x = -1, old_y = -1;
	int money = 0;
	int skulls = 0;
	bool key = false;
	bool battle = false;
	bool boss = false;
	vector<shared_ptr<Hero>> heroes;
	vector<shared_ptr<Monster>> villains;

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
		in >> skulls;
		in >> x;
		in >> y;
		if (x > Map::SIZE-1 || y > Map::SIZE-1 || x < 0 || y < 0) {
			x = Map::SIZE/2;
			y = Map::SIZE/2;
		}
		if (!in) save = false;
		in.close();
	}
	if (option == 1 || !save || heroes.size() == 0) {
		heroes.clear();
		option = 1;
		select.change_option(0, "Choose your Wizards", 1);
		select.change_option(1, "Frost Wizard", 2);
		select.change_option(2, "Earth Wizard", 3);
		select.change_option(3, "Light Wizard", 4);
		select.add_option("Fire Wizard", 5);
		select.add_option("Arcane Wizard", 6);
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
				turn_off_ncurses();
				string name;
				cout << "Enter a name:" << endl;
				cin >> name;
				turn_on_ncurses();
				if (option == 1)
					heroes.push_back(make_shared<FrostWizard>(name));
				else if (option == 2)
					heroes.push_back(make_shared<EarthWizard>(name));
				else if (option == 3)
					heroes.push_back(make_shared<LightWizard>(name));
				else if (option == 4)
					heroes.push_back(make_shared<FireWizard>(name));
				else
					heroes.push_back(make_shared<ArcaneWizard>(name));
				select.draw(option);
			}
			}
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
				option = 1;
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
							out << money << '\t' << skulls << '\t' << x << '\t' << y;
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
				if (map.getTile(x,y) == Map::GATE) {
					if (key)
						map.setTile(x,y,Map::OPEN);
					else {
						x = old_x;
						y = old_y;
					}
				}
				if (map.getTile(x,y) == Map::SHOP) {
					shop_menu(heroes, money, skulls, key);
					x = old_x;
					y = old_y;
				}
				if (map.getTile(x,y) == Map::WATER || map.getTile(x,y) == Map::WALL) {
					x = old_x;
					y = old_y;
				}
				else if (map.getTile(x,y) == Map::MONSTER) {
					battle = true;
					map.setTile(x,y,Map::OPEN);
				}
				else if (map.getTile(x,y) == Map::BOSS) {
					boss = true;
					battle = true;
					map.setTile(x,y,Map::OPEN);
				}
				else {
					map.draw(x,y);
					mvprintw(DISPLAY+1,0,"X: %i Y: %i\n",x,y);
					mvprintw(DISPLAY+2,0,"Money: %i\n",money);
					mvprintw(DISPLAY+3,0,"Skulls: %i\n",skulls);
					refresh();
					old_x = x;
					old_y = y;
				}
			}
			usleep(1'000'000/MAX_FPS);
			if (battle) {
				vector<shared_ptr<Hero>> heros;
				option = 1;
				if (boss) {
					for (int i = 0; i < 2; i++)
						villains.push_back(make_shared<GrimmyGoblim>("GrimmyGoblim" + to_string(i+1), rand()%5, rand()%5, rand()%12));

					villains.push_back(make_shared<SunGuy>("SrSun", rand()%12, rand()%5, rand()%42));
				}
				else {
					for (int i = 0; i < 3+rand()%2; i++)
						villains.push_back(make_shared<GrimmyGoblim>("GrimmyGoblim" + to_string(i+1), rand()%5, rand()%5, rand()%12));
				}
				CircSLelement<shared_ptr<Actor>> *head = nullptr;
				CircSLelement<shared_ptr<Actor>> *curr;
				for (auto h: heroes) {
					head = insert_actor(head, h);
					heros.push_back(h);
				}
				for (auto v: villains)
					head = insert_actor(head, v);
				skulls += 1 + rand() % (villains.size() - 1);
				curr = head;
				int rounds = 0;
				while (battle) {
					bool end_turn = false;
					option = 1;
					vector<shared_ptr<Hero>>::iterator it;
					it = find(heros.begin(), heros.end(), curr->getValue());
					if (it != heros.end()) {
						shared_ptr<Hero> hero = heros.at(it-heros.begin());
						while (!end_turn && hero->GetHP()) {
							fight_menu(hero, option);
							if (option == 1) {
								while (!end_turn) {
									actor_select(villains, option);
									if (option-1 < villains.size())
										end_turn = hero->attack(villains.at(option-1));
									else {
										option = 1;
										break;
									}
								}
							}
							else if (option == 2) {
								end_turn = hero->use_move1();
								if (!end_turn)
									end_turn = hero->use_move1(villains);
								if (hero->move1 == "Ice Lance" || hero->move1 == "Arcane Missiles") {
									option = 1;
									actor_select(villains, option);
									end_turn = hero->use_move1(villains.at(option-1));
									option = 2;
								}
							}
							else if (option == 3) {
								end_turn = hero->use_move2();
								if (!end_turn)
									end_turn = hero->use_move2(heroes);
								if (hero->move2 == "Snowfort") {
									option = 1;
									actor_select(heroes, option);
									end_turn = hero->use_move2(heroes.at(option-1));
									option = 3;
								}
							}
							else if (option == 4) {
								end_turn = true;
							}
							else if (option == 5) {
								party_menu(heroes);
							}
						}
					}
					else {
						while(!end_turn) {
							end_turn = curr->getValue()->attack(heros.at(rand()%heros.size()));
						}
					}
					curr = curr->getNext();
					for (int i =  0; i < villains.size(); i++) {
						if (villains.at(i)->GetHP() < 1) {
							if (curr->getValue() == villains.at(i))
								curr = curr->getNext();
							head = remove_actor(head, villains.at(i));
							villains.erase(villains.begin()+i);
						}
					}
					for (int i = 0; i < heros.size(); i++) {
						if (heros.at(i)->GetHP() == 0) {
							if (curr->getValue() == heros.at(i))
								curr = curr->getNext();
							head = remove_actor(head, heros.at(i));
							heros.erase(heros.begin() + i);
						}
					}
					if (villains.empty()) {
						for (auto h: heroes) {
							h->hp.regen();
							h->mana.regen();
						}
						battle = false;
						if (boss) {
							turn_off_ncurses();
							cout << "YOU WIN" << endl;
							exit(0);
						}
					}
					if(heros.empty()) {
						turn_off_ncurses();
						cout << "YOU LOSE" << endl;
						exit(0);
					}
				}
				villains.clear();
			}
		}
		turn_off_ncurses();
	}
