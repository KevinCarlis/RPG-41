#include <memory>

/* Resource usage:
   Resource hp(150);
   hp -= 10;
   hp.SetTemp(20);
   hp -= 125;
   hp += 5;
   hp.regen(25);
   if (hp < 20)
   hp.regen();
   cout << hp.GetCurr() << " + " << hp.GetTemp() << " = " << hp.GetTotal() << endl;
   */
class Resource {
	int max;
	int curr;
	int temp;
	public:
	Resource(int userMax=100, int userCurr=100, int userTemp=0) {
		SetMax(userMax);
		SetCurr(userCurr);
		SetTemp(userTemp);
	}
	int GetMax() const {
		return max;
	}
	int GetCurr() const {
		return curr;
	}
	int GetTemp() const {
		return temp;
	}
	int GetTotal() const {
		return curr + temp;
	}
	void SetMax(int userMax) {
		max = userMax;
	}
	void SetCurr(int userCurr) {
		if (userCurr > max)
			userCurr = max;
		curr = userCurr;
	}
	void SetTemp(int userTemp) {
		temp = userTemp;
	}
	void AddTemp(int userTemp) {
		temp += userTemp;
	}
	void regen(int percent=10) {
		SetCurr(curr + max*percent/100);
	}
	bool operator<(int rhs) {
		return (curr+temp < rhs) ? true: false;
	}
	Resource& operator+=(int rhs) {
		this->SetCurr(curr + rhs);
		return *this;
	}
	Resource& operator-=(int rhs) {
		if (this->temp > 0) {
			if (this->temp < rhs) {
				rhs -= this->temp;
				this->temp = 0;
			}
			else {
				this->temp -= rhs;
				rhs = 0;
			}
		}
		if (rhs > 0) {
			if (this->curr < rhs)
				this->curr = 0;
			else
				this->curr -= rhs;
		}
		return *this;
	}
};

class Actor {
	public:
		string name;
		int speed;
		int power;
		Resource hp;
		int color; //1: white, 2: blue, 3: green, 4: yellow, 5: red, 6:purple
		string move1;
		Actor() {;}
		Actor(string userName, int userSpeed, int userPower, int userHP, int userColor) {
			name = userName;
			speed = userSpeed;
			power = userPower;
			hp = Resource(userHP);
			color = userColor;
		}
		void SetHP(int userMax, int userCurr=-1, int userTemp=0) {
			hp.SetMax(userMax);
			if (userCurr <= 0)
				hp.SetCurr(userMax);
			else
				hp.SetCurr(userCurr);
			hp.SetTemp(userTemp);
		}
		int GetHP() const {
			return hp.GetCurr();
		}
		int GetShield() const {
			return hp.GetTemp();
		}
		template <typename T>
			string attack(const shared_ptr<T> &other) const {
				if (other->GetHP() <= 0)
					return "FAIL: " + other->name + " is dead";
				other->damage(power);
				return other->name + "\ntook " + to_string(power) + " damage!";
			}
		void damage(int d) {
			hp -= d;
		}
		virtual string print() {
			if (hp.GetTemp() > 0)
				return "HP:" + to_string(GetHP()) + '+' + to_string(GetShield());
			return "HP:" + to_string(GetHP());
		}
};

class Monster: public Actor {
	public:
		Monster() {
			Actor::color = 5;
		}
};
class SkinnySkeleton : public Monster {
	public:
		SkinnySkeleton(string userName="SkinnySkeleton", int bonusSpeed=0, int bonusPower=0, int bonusHP=0) {			        name = userName;
			speed = 14 + bonusSpeed;
			power = 7 + bonusPower;
			SetHP(45+bonusHP);
		}
};
class GrimmyGoblim : public Monster {
	public:
		GrimmyGoblim(string userName="GrimmyGoblim", int bonusSpeed=0, int bonusPower=0, int bonusHP=0) {
			name = userName;
			speed = 14 + bonusSpeed;
			power = 10 + bonusPower;
			SetHP(60+bonusHP);
		}
};
class BashfulBeast : public Monster {
	public:
		BashfulBeast(string userName="BashfulBeast", int bonusSpeed=0, int bonusPower=0, int bonusHP=0) {
			name = userName;
			speed = 16 + bonusSpeed;
			power = 14 + bonusPower;
			SetHP(150+bonusHP);
		}
};
class DangerousDragon : public Monster {
	public:
		DangerousDragon(string userName="DangerousDragon", int bonusSpeed=0, int bonusPower=0, int bonusHP=0) {
			name = userName;
			speed = 15 + bonusSpeed;
			power = 18 + bonusPower;
			SetHP(220+bonusHP);
		}
};
class SunGuy : public Monster {
	public:
		SunGuy(string userName="SunGuy", int bonusSpeed=0, int bonusPower=0, int bonusHP=0) {
			name = userName;
			speed = 16 + bonusSpeed;
			power = 22 + bonusPower;
			SetHP(420+bonusHP);
		}
};

class Hero: public Actor {
	public:
		Resource mana;
		string move2;
		void SetMana(int userMax=100, int userCurr=100, int userTemp=0) {
			mana.SetMax(userMax);
			mana.SetCurr(userCurr);
			mana.SetTemp(userTemp);
		}
		int GetMana() const {
			return mana.GetCurr();
		}
		virtual string print() override {
			return Actor::print() + " MP:" + to_string(mana.GetTotal());
		}
		virtual string use_move1() {
			return "FAIL: No move";
		}
		virtual string use_move1(shared_ptr<Monster> &enemy) {
			return "FAIL: No move";
		}
		virtual string use_move1(vector<shared_ptr<Monster>> &enemies) {
			return "FAIL: No move";
		}
		virtual string use_move2() {
			return "FAIL: No move";
		}
		virtual string use_move2(shared_ptr<Hero> &ally) {
			return "FAIL: No move";
		}
		virtual string use_move2(vector<shared_ptr<Hero>> &allies) {
			return "FAIL: No move";
		}
};
class FrostWizard: public Hero {
	public:
		FrostWizard(string userName="Hoj", int currhp=105, int temphp=15, int currmana=108) {
			name = userName;
			speed = 16;
			power = 21;
			SetHP(105, currhp, temphp);
			SetMana(108, currmana);
			color = 2;
			move1 = "Ice Lance";
			move2 = "Snowfort";
		}
		virtual string use_move1(shared_ptr<Monster> &enemy) override {
			if (mana < 29) return "FAIL: Not enough mana.";
			mana -= 29;
			enemy->damage(45);
			return enemy->name + "\n took 45 damage!";
		}
		virtual string use_move2(shared_ptr<Hero> &ally) override {
			if (mana < 22)
				return "FAIL: Not enough mana.";
			mana -= 22;
			ally->hp.AddTemp(35);
			return ally->name + "\n gained 35 shield";
		}
};
class EarthWizard: public Hero {
	public:
		EarthWizard(string userName="Ted", int currhp=120, int temphp=20, int currmana=95) {
			name = userName;
			speed = 15;
			power = 25;
			SetHP(120, currhp, temphp);
			SetMana(95, currmana);
			color = 3;
			move1 = "Boulder Barrage";
			move2 = "Rock Wall";
		}
		virtual string use_move1(vector<shared_ptr<Monster>> &enemies) override {
			if (mana < 25) "FAIL: Not enough mana.";
			mana -= 25;
			for (auto m : enemies)
				m->damage(19);
			return "All enemies\ntook 19 damage!";
		}
		virtual string use_move2() override {
			if (mana < 20)
				return "FAIL: Not enough mana.";
			mana -= 20;
			hp.AddTemp(45);
			return name + "\ngained 45 shield.";
		}
};
class LightWizard: public Hero {
	public:
		LightWizard(string userName="Liam", int currhp=140, int temphp=10, int currmana=105) {
			name = userName;
			speed = 17;
			power = 19;
			SetHP(140, currhp, temphp);
			SetMana(105, currmana);
			color = 4;
			move1 = "Self Lighteous";
			move2 = "Healing Light";
		}
		virtual string use_move1() override {
			if (mana < 30) "FAIL: Not enough mana.";
			mana -= 30;
			hp += 40;
			return name + " \ngained 40HP.";
		}
		virtual string use_move2(vector<shared_ptr<Hero>> &allies) override {
			if (mana < 20)
				return "FAIL: Not enough mana.";
			mana -= 20;
			for (auto h : allies)
				if (h->name != name && h->GetHP() != GetHP())
					h->hp += 20;
			return "Allies\nhealed 20HP.";
		}
};
class FireWizard: public Hero {
	public:
		FireWizard(string userName="Frank", int currhp=90, int temphp=35, int currmana=112) {
			name = userName;
			speed = 20;
			power = 24;
			SetHP(90, currhp, temphp);
			SetMana(112, currmana);
			color = 5;
			move1 = "Firestorm";
			move2 = "Smokescreen";
		}
		virtual string use_move1(vector<shared_ptr<Monster>> &enemies) override {
			if (mana < 32) return "FAIL: Not enough mana.";
			mana -= 32;
			for (auto m : enemies)
				m->damage(22);
			return "All enemies\ntook 22 damage!";
		}
		virtual string use_move2(vector<shared_ptr<Hero>> &allies) override {
			if (mana < 22)
				return "FAIL: Not enough mana.";
			mana -= 22;
			for (auto h: allies)
				h->hp.AddTemp(12);
			return "All allies\ngained 12 shield.";
		}
};
class ArcaneWizard: public Hero {
	public:
		ArcaneWizard(string userName="Steve", int currhp=110, int temphp=0, int currmana=125) {
			name = userName;
			speed = 19;
			power = 22;
			SetHP(120, currhp, temphp);
			SetMana(125, currmana);
			color = 6;
			move1 = "Arcane Missiles";
			move2 = "Mana Surge";
		}
		virtual string use_move1(shared_ptr<Monster> &enemy) override {
			if (mana < 30) return "FAIL: Not enough mana.";
			mana -= 30;
			enemy->damage(40);
			return enemy->name + "\ntook 40 damage!";
		}
		virtual string use_move2(vector<shared_ptr<Hero>> &allies) override {
			if (mana < 20) return "FAIL: Not enough mana.";
			mana -= 20;
			for (auto h : allies)
				if (h->name != name && h->GetHP() != GetHP())
					h->mana.AddTemp(15);
			return "All allies\ngained 25 mana.";
		}
};
