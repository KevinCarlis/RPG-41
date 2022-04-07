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
	unsigned int max;
	unsigned int curr;
	unsigned int temp;
	public:
	Resource(unsigned int userMax=100, unsigned int userCurr=100, unsigned int userTemp=0) {
		SetMax(userMax);
		SetCurr(userCurr);
		SetTemp(userTemp);
	}
	unsigned int GetMax() const {
		return max;
	}
	unsigned int GetCurr() const {
		return curr;
	}
	unsigned int GetTemp() const {
		return temp;
	}
	unsigned int GetTotal() const {
		return curr + temp;
	}
	void SetMax(unsigned int userMax) {
		max = userMax;
	}
	void SetCurr(unsigned int userCurr) {
		if (userCurr > max)
			userCurr = max;
		curr = userCurr;
	}
	void SetTemp(unsigned int userTemp) {
		temp = userTemp;
	}
	void AddTemp(unsigned int userTemp) {
		temp += userTemp;
	}
	void regen(int percent=10) {
		SetCurr(curr + max*percent/100);
	}
	bool operator<(unsigned int rhs) {
		return (curr+temp < rhs) ? true: false;
	}
	Resource& operator+=(unsigned int rhs) {
		this->SetCurr(curr + rhs);
		return *this;
	}
	Resource& operator-=(unsigned int rhs) {
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
	protected:
	string name;
	int speed;
	unsigned int attack;
	Resource hp;
	int color; //1: white, 2: blue, 3: green, 4: yellow, 5: red, 6:purple

	public:
	Actor() {;}
	Actor(string userName, int userSpeed, unsigned int userAttack, unsigned int userHP, int userColor) {
		name = userName;
		speed = userSpeed;
		attack = userAttack;
		hp = Resource(userHP);
		color = userColor;
	}
	void SetName(string userName) {
		name = userName;
	}
	void SetSpeed(int userSpeed) {
		speed = userSpeed;
	}
	void SetAttack(unsigned int userAttack) {
		attack = userAttack;
	}
	void SetHP(unsigned int userMax=100, unsigned int userCurr=100, unsigned int userTemp=0) {
		hp.SetMax(userMax);
		hp.SetCurr(userCurr);
		hp.SetTemp(userTemp);
	}
	void SetColor(int userColor) {
		color = userColor;
	}
	string GetName() const {
		return name;
	}
	int GetSpeed() const {
		return speed;
	}
	unsigned int GetAttack() const {
		return attack;
	}
	Resource GetHP() const {
		return hp;
	}
	int GetColor() const {
		return color;
	}
	virtual string print() {
		if (hp.GetTemp() > 0)
			return "HP:" + to_string(hp.GetCurr()) + '+' + to_string(hp.GetTemp());
		return "HP:" + to_string(hp.GetCurr());
	}
};

class Monster: public Actor {
	public:
	Monster() {
		SetColor(5);
	}
};

class GrimmyGoblim : public Monster {
	public:
	GrimmyGoblim() {
		SetName("GrimmyGoblim");
		SetSpeed(18);
		SetAttack(15);
		SetHP(95);
	}
};

class Hero: public Actor {
	protected:
	Resource mana;
	string move1;
	string move2;

	public:
	void SetMana(unsigned int userMax=100, unsigned int userCurr=100, unsigned int userTemp=0) {
		mana.SetMax(userMax);
		mana.SetCurr(userCurr);
		mana.SetTemp(userTemp);
	}
	void SetMove1(string move) {
		move1 = move;
	}
	void SetMove2(string move) {
		move2 = move;
	}
	Resource GetMana() const {
		return mana;
	}
	string GetMove1() const {
		return move1;
	}
	string GetMove2() const {
		return move2;
	}
	virtual string print() override {
		return Actor::print() + " MP:" + to_string(mana.GetTotal());
	}
	virtual bool use_move1(){return true;};
	virtual bool use_move2(){return true;};
};

class EarthWizard: public Hero {
	public:
	EarthWizard(string userName="Frank", int currhp=120, int temphp=20, int currmana=95) {
		SetName(userName);
		SetSpeed(17);
		SetAttack(25);
		SetHP(120, currhp, temphp);
		SetMana(95, currmana);
		SetColor(3);
		SetMove1("Boulder Barrage");
		SetMove2("Rock Wall");
	}
	bool use_move1(int enemies, ...) {
		if (mana < 25) return false;
		mana -= 25;
		va_list valist;
		va_start(valist, enemies);
		for (int i = 0; i < enemies; i++)
			va_arg(valist, Monster*)->GetHP() -= 15;
		va_end(valist);
		return true;
	}
	bool use_move2() {
		if (mana < 20)
			return false;
		mana -= 20;
		hp.AddTemp(20);
		return true;
	}
};
