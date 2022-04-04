class Player {
	int hp;
	int speed;

	public:
	void GetHP() {
		return hp;
	}
};

class Hero: public Player {
};

class EarthWizard: public Hero {
};

class Villain: public Player {
};
