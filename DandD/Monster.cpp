/**
*@class Monster
*Subclass of Characters, 'Monster' class implementation
*/

#include "Monster.h"
#include "Corpse.h"
#include "prebuiltlevel.h"

//!Default constructor - for making dummy monsters
Monster::Monster() : Characters() {
	this->name = "dummy";
	this->type = Beast;
	this->size = Medium;
	this->speed = 20;
	detHitDie();
	detHitPoints();

	this->_componentChar = SimplifiedMapSymbols::_Enemies_;
	this->_componentName = "enemy";
	this->_componentType = gameplayGridComponentTypes::enemy;
	this->_componentImage = nullptr;
	this->_image_path = SingletonFilePathAndFolderManager::getInstance()->_path_to_basic_enemy;
	this->_obstructionToPlayer = true;
}

//!Parameterized constructor allowing to set name, type, size, level, speed, scores, armor class and weapon of Monster
Monster::Monster(string name, Type type, Size size, int level, int speed, int STR, int DEX, int CON, int INT, int WIS, int CHA, int armorClass, Weapon* weapon, Characters* theFighter) : Characters(level, STR, DEX, CON, INT, WIS, CHA, armorClass)
{
	this->name = name;
	this->type = type;
	this->size = size;
	this->speed = speed;
	detHitDie();
	detHitPoints();
	equipWeapon(weapon);
	this->_componentChar = SimplifiedMapSymbols::_Enemies_;
	this->_componentName = "enemy";
	this->_componentType = gameplayGridComponentTypes::enemy;
	this->_componentImage = nullptr;
	this->_image_path = SingletonFilePathAndFolderManager::getInstance()->_path_to_basic_enemy;
	this->_obstructionToPlayer = true;

	// Setup the Strategy
	if (type == Beast){
		this->_strategy = new FriendlyStrategy(speed, theFighter);
		_charType = Friendly;
	}
	else {
		this->_strategy = new AggressorStrategy(speed, theFighter);
		_charType = Aggressor;
	}
}

//!Destructor
Monster::~Monster()
{}

/**Function to determine the hitDie of the monster based on the size.
Tiny: d4, Small: d6, Medium: d8, Large: d10, Huge: d12 and Gargantuan: d20*/
void Monster::detHitDie()
{
	switch (size)
	{
	case Tiny:
		hitDie = 4;
		hitDieString = "1d4";
	case Small:
		hitDie = 6;
		hitDieString = "1d6";
	case Medium:
		hitDie = 8;
		hitDieString = "1d8";
	case Large:
		hitDie = 10;
		hitDieString = "1d10";
	case Huge:
		hitDie = 12;
		hitDieString = "1d12";
	case Gargantuan:
		hitDie = 20;
		hitDieString = "1d20";
	}
}

//!Function to calculate HitPoints based on initialized level
void Monster::detHitPoints()
{
	int level = this->getLevel();
	//At Level 1: HP = max_HitDie + CON_mod
	hitPoints = hitDie + this->getScores(1, 2);
	//For every level: HP = HP + roll_HitDie + CON_mod
	for (int i = 2; i <= level; i++)
	{
		hitPoints += _die.roll(hitDieString) + this->getScores(1, 2);
	}
}

//!Function to equip weapon and calculate Attack and Damage Bonuses
void Monster::equipWeapon(Weapon* w)
{
	delete weapon;
	weapon = w;
	calcAttackBonus();
	calcDamageBonus();
}

//!Function to display fighter battle stats, calls parent displayBattle()
void Monster::displayBattle()
{
	cout << "\nMonster: " << name << " Type: " << typeString[type];
	cout << "\nHit Points: " << hitPoints << endl;
	this->Characters::displayBattle();
}

/**Function for attacking a Fighter, generates attack roll and checks it agains Fighter's AC
If larger than AC, damage roll is calculated and inflicted on Fighter, otherwise attack fails*/
void Monster::attack(Fighter* c)
{
	string message = "";
	int aRoll = attackRoll(), dRoll;
	string name;
	message += this->getName() + " rolled ";
	message += aRoll + " for attack!\n";
	//cout << this->getName() << " rolled " << aRoll << " for attack!" << endl;

	if (aRoll < c->getArmorClass())
	{
		message += "Attack missed!\n";
		//cout << "Attack missed!" << "\n" << endl;
	}
	else
	{
		message += "Attack was successful!\n";
		//cout << "Attack was successful!" << endl;
		dRoll = damageRoll();
		c->receiveDamage(dRoll);
	}
}

/**Function that reduces hitpoints based on damage taken,
if hitpoints reduce to 0 or less, fighter is dead. Notifies change in character stats*/
void Monster::receiveDamage(int damage)
{
	underAttack();
	string message;
	hitPoints -= damage;
	message = damage + " damage was inflicted on " + name + "!\n";
	cout << damage << " damage was inflicted on " << name << "!" << endl;
	if (hitPoints <= 0)
	{
		setIsDead(true);
		dropContainer();
		return;
	}
	currentState();
}

//!Function to display monster death, calls parent displayDeath()
void Monster::displayDeath()
{
	cout << "\n" << name;
	Characters::displayDeath();
}


//!Function for display of character death 
void Monster::dropContainer() {
	// make a container that will contain all of the characters equipment and is located at the characters location
	// first make a vector of all the items that will be included
	std::vector<Item*> items;
	if (armor != nullptr) {
		items.push_back(armor);
	}
	if (weapon != nullptr) {
		items.push_back(weapon);
	}
	if (shield != nullptr) {
		items.push_back(shield);
	}
	if (helmet != nullptr) {
		items.push_back(helmet);
	}
	if (boots != nullptr) {
		items.push_back(boots);
	}
	if (belt != nullptr) {
		items.push_back(belt);
	}
	if (ring != nullptr) {
		items.push_back(ring);
	}

	// create the container
	Corpse * corpse = new Corpse(7, items);
	corpse->setImage("assets/Containers/Corpse.png");

	// add to containerOnMap
	ContainerOnMap * containerOnMap = new ContainerOnMap();
	containerOnMap->charIndex = getCharPos();
	containerOnMap->stringIndex = getVectPos();
	containerOnMap->container = corpse;
	_subject->addContainerOnTheMap(containerOnMap);
}

void Monster::currentState()
{
	Notify();
}

//!Function to display Monster's stats (calls Character's displaystats)
void Monster::displayStats(){
	system("CLS");
	cout << "Name: " << name << endl;
	cout << "Hit Points: " << hitPoints << endl;
	this->Characters::displayStats();
}

/**
* Skeletal method for implementing attacks. Makes sure that movement strategy is set appropriately. Notifies
* observers of death if the Monster is now dead, or health loss.
*/
void Monster::underAttack(){

	// If the current strategy is Friendly, will switch to Aggressive
	if (typeid(*_strategy) == typeid(FriendlyStrategy)){
		_charType = Aggressor; // Sets the type of Monster to beast
		int vectPos = this->getVectPos();
		int charPos = this->getCharPos();
		// Create new AggressorStrategy with same properties as the other strategy
		_strategy = new AggressorStrategy(dynamic_cast<FriendlyStrategy*>(_strategy));

		// Will notify observers that type has changed
		Notify();
	}
}