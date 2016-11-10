#include "Characters.h"
#include "Fighter.h"
#include "Monster.h"
#include "Item.h"
#include "namespaces.h"
#include <iomanip>
using namespace std;

IMPLEMENT_SERIAL(Characters, CObject, 1);

int experience[3][20] = {
	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
	{ 0, 300, 900, 2700, 6500, 14000, 23000, 34000, 48000, 64000, 85000, 100000, 120000, 140000, 165000, 195000, 225000, 265000, 305000, 355000 },
	{ 50, 100, 200, 450, 700, 1100, 1800, 2300, 2900, 3900, 5000, 5900, 7200, 8400, 10000, 11500, 13000, 15000, 18000, 20000 }
};

/*CONSTRUCTOR*/
//!Default Constructor - Creates a dummy Lvl 1 Character
Characters::Characters()
{
	this->level = 1;
	isDead = false;
	armorClass = 0;
	inBattle = false;
	isLevelUp = false;
	_map = NULL;

	armor = new Armor();
	belt = new Belt();
	weapon = new Weapon();
	shield = new Shield();
	boots = new Boots();
	ring = new Ring();
	helmet = new Helmet();
	backpack = new Container();

	position.push_back(-1);
	position.push_back(-1);
}

Characters::Characters(int level, int STR, int DEX, int CON, int INT, int WIS, int CHA)
{
	this->level = level;
	inBattle = false;
	isDead = false;
	isLevelUp = false;
	this->armorClass = armorClass;
	scores[0][0] = STR;
	scores[0][1] = DEX;
	scores[0][2] = CON;
	scores[0][3] = INT;
	scores[0][4] = WIS;
	scores[0][5] = CHA;

	srand(time(NULL));

	armor = new Armor();
	belt = new Belt();
	weapon = new Weapon();
	shield = new Shield();
	boots = new Boots();
	ring = new Ring();
	helmet = new Helmet();

	detProficiencyBonus();
	detExp();
	abilityScoreMod();
	calcArmorClass();
	_map = NULL;


	// Sets dummy values to position so that its size is initialized
	position.push_back(-1);
	position.push_back(-1);
}

//!Parameterized Constructor for Monster class, sets level, ability scores and armor class.
Characters::Characters(int level, int STR, int DEX, int CON, int INT, int WIS, int CHA, int armorClass)
{
	this->level = level;
	inBattle = false;
	isDead = false;
	isLevelUp = false;
	this->armorClass = armorClass;
	scores[0][0] = STR;
	scores[0][1] = DEX;
	scores[0][2] = CON;
	scores[0][3] = INT;
	scores[0][4] = WIS;
	scores[0][5] = CHA;

	srand(time(NULL));

	detProficiencyBonus();
	detExp();
	abilityScoreMod();
	calcArmorClass();
	_map = NULL;

}

//!Parameterized Constructor for Fighter class, sets level.
Characters::Characters(int level)
{
	this->level = level;
	isDead = false;
	armorClass = 0;
	inBattle = false;
	//call to set proficiencyBonus and exp
	detExperience();
	detProficiencyBonus();

	srand(time(NULL));

	//call to set abilityScore and modifiers
	generateAbility();

	armor = new Armor();
	belt = new Belt();
	weapon = new Weapon();
	shield = new Shield();
	boots = new Boots();
	ring = new Ring();
	helmet = new Helmet();

	calcArmorClass();

	//calculate attackBonus and DamageBonus
	calcAttackBonus();
	calcDamageBonus();
	_map = NULL;

	
}

//!Auxiliary function used to destroy all equipment objects
void Characters::destroyObject()
{
	if (armor != nullptr)
		delete armor;
	armor = nullptr;
	if (weapon != nullptr)
		delete weapon;
	weapon = nullptr;
	if (shield != nullptr)
		delete shield;
	shield = nullptr;
	if (boots != nullptr)
		delete boots;
	boots = nullptr;
	if (ring != nullptr)
		delete ring;
	ring = nullptr;
	if (helmet != nullptr)
		delete helmet;
	helmet = nullptr;
	if (belt != nullptr)
		delete belt;
	belt = nullptr;

}
//!Destructor
Characters::~Characters()
{
	destroyObject();
	//delete _map;
}

//!Accessor to level
int Characters::getLevel()
{
	return level;
}

/*!Accessor for scores, i: 0 for ability scores values, 1 for ability score modifiers
and j: 0 for STR, 1 for DEX, 2 for CON, 3 for INT, 4 for WIS, 5 for CHA*/
int Characters::getScores(int i, int j)
{
	return scores[i][j];
}


bool Characters::getInBattle()
{
	return inBattle;
}


//!Function to simulate dice roll of i faces 
int Characters::rollDice(int i)
{
	int x = _die.roll("1d" + i);
	if (x == -1) {
		return 0;
	}
	return x;
}

//!Function that rolls 4D6 for each ability score and sets two largest values to STR and CON
void Characters::generateAbility()
{
	int temp;
	int abscor[6];
	for (int i = 0; i < MAX_NUM_SCORES; i++)
	{
		abscor[i] = _die.roll("4d6");
	}

	orderArray(abscor);

	scores[0][0] = abscor[0];
	scores[0][2] = abscor[1];
	scores[0][1] = abscor[2];

	for (int i = 3; i < MAX_NUM_SCORES; i++)
	{
		scores[0][i] = abscor[i];
	}

	abilityScoreMod();
}

//!Auxiliary funtion to reorganize the array such that the two largest values are at index 0 and 1
void Characters::orderArray(int a[])
{
	int temp;
	for (int i = 0; i < 2; i++)
	{
		for (int j = i + 1; j<MAX_NUM_SCORES; j++)
		{
			if (a[j]>a[i])
			{
				temp = a[j];
				a[j] = a[i];
				a[i] = temp;
			}
		}
	}
}

//!Function to calculate the ability score modifiers with formula floor((score-10)/2)
void Characters::abilityScoreMod()
{
	//TO BE DEFINED
	//MODIFIER = (ABILITY SCORE - 10)/2

	double modifier;
	for (int i = 0; i < MAX_NUM_SCORES; i++)
	{
		modifier = ((double)scores[0][i] - 10) / 2;
		modifier = floor(modifier);
		scores[1][i] = (int)modifier;
	}
}

//!Function that determines initial experience level of character (namely Fighter) based on level
void Characters::detExperience()
{
	exp = experience[1][level - 1];
}

/*!Function that determines initial experience level of character (namely Monster) based on level
NOTE the experience of Monster is the amount gained if fighter defeats it*/
void Characters::detExp()
{
	exp = experience[2][level - 1];
}

//!Function to determine Proficiency bonus based on the level
void Characters::detProficiencyBonus()
{
	if (level <= 4)
		proficiencyBonus = 2;
	else if (level <= 8)
		proficiencyBonus = 3;
	else if (level <= 12)
		proficiencyBonus = 4;
	else if (level <= 16)
		proficiencyBonus = 5;
	else
		proficiencyBonus = 6;
}

//!Calculates the armorclass based if armor is equipped or not (for initialialization purposes) 
void Characters::calcArmorClass()
{
	if (armor->getDefense() == 0)
		armorClass = 10 + scores[1][1];
	else
	{
		armorClass = armor->getDefense()+armor->getEnchantmentValues()[6] + scores[1][1];
	}

}

/*!Function to calculate attack bonus by adding proficiency bonus and
DEX mod if ranged weapon or STR mod if melee weapon*/
void Characters::calcAttackBonus()
{
	attackBonus = proficiencyBonus;
	if (weapon->getName().compare("None") == 0)
		attackBonus += scores[1][0];  //add str mod if no weapon
	else if (weapon->getRange() == 1)
		attackBonus += scores[1][0];  //add str mod if melee 
	else
		attackBonus += scores[1][1]; //add str mod if ranged
}

/*!Function to calculate damage bonus which is DEX mod if ranged weapon or STR mod if melee weapon*/
void Characters::calcDamageBonus()
{
	if (weapon->getName().compare("None") == 0)
		damageBonus = scores[1][0];  //add str mod if no weapon
	else if (weapon->getRange() == 1)
		damageBonus = scores[1][0];  //add str mod if melee 
	else
		damageBonus = scores[1][1]; //add str mod if ranged
}

//!Function for generic display of character's stats (shows all information)
void Characters::displayStats()
{
	cout << "Level: " << level << "	EXP: " << exp << endl;
	cout << "STR: " << scores[0][0] << "(" << scores[1][0] << ")";
	cout << "\nDEX: " << scores[0][1] << "(" << scores[1][1] << ")";
	cout << "\nCON: " << scores[0][2] << "(" << scores[1][2] << ")";
	cout << "\nINT: " << scores[0][3] << "(" << scores[1][3] << ")";
	cout << "\nWIS: " << scores[0][4] << "(" << scores[1][4] << ")";
	cout << "\nCHA: " << scores[0][5] << "(" << scores[1][5] << ")" << endl;
	cout << "Proficiency Bonus: +" << proficiencyBonus;
	cout << "\nArmor Class: " << armorClass;
	cout << "\nEquipment:" << endl;

	displayEquip();

	cout << "Attack Bonus: " << attackBonus << endl;
	cout << "Damage Bonus: " << damageBonus << endl;

	
}

//!Helper Function for equipment display
void Characters::displayEquip()
{
	printf("%-10s%-15s  %-10s%-15s\n", "Armor: ", armor->getName().c_str(), "Weapon: ", weapon->getName().c_str());

	printf("%-10s%-15s  %-10s%-15s\n", "Shield: ", shield->getName().c_str(), "Helmet: ", helmet->getName().c_str());

	printf("%-10s%-15s  %-10s%-15s \n", "Ring: ", ring->getName().c_str(), "Boots: ", boots->getName().c_str());
	
	printf("%-10s%-15s\n", "Belt: ", belt->getName().c_str());
}

//!Function for display of character's stats during battle (shows level, armor class, attack bonus and damage bonus)
void Characters::displayBattle()
{
	cout << "Level: " << level << endl;
	cout << "Armor Class: " << armorClass << endl;
	cout << "Attack Bonus: " << attackBonus << endl;
	cout << "Damage Bonus: " << damageBonus << endl;
}

//!Function for display of character death 
void Characters::displayDeath()
{
	cout << " is dead!" << endl;
}

//!Function for display of character's stats when leveling up (shows level, EXP, and ability scores and modifiers)
void Characters::displayLevelUp()
{
	cout << "Level: " << level << "	EXP: " << exp << endl;
	cout << "STR: " << scores[0][0] << "(" << scores[1][0] << ")";
	cout << "\nDEX: " << scores[0][1] << "(" << scores[1][1] << ")";
	cout << "\nCON: " << scores[0][2] << "(" << scores[1][2] << ")";
	cout << "\nINT: " << scores[0][3] << "(" << scores[1][3] << ")";
	cout << "\nWIS: " << scores[0][4] << "(" << scores[1][4] << ")";
	cout << "\nCHA: " << scores[0][5] << "(" << scores[1][5] << ")" << endl;
}




//!Helper function to update stats when de-equipping an equipment
void Characters::updateStatsDQ(Item* i)
{
	array<int, 9> ench = i->getEnchantmentValues();

	for (int j = 0; j < 6; j++)
		scores[0][j] -= ench[j];

	armorClass -= ench[6];
	attackBonus -= ench[7];
	damageBonus -= ench[8];
}

//!Helper function to update stats when equipping an equipment
void Characters::updateStatsEQ(Item* i)
{
	array<int, 9> ench = i->getEnchantmentValues();

	for (int j = 0; j < 6; j++)
		scores[0][j] += ench[j];

	armorClass += ench[6];
	attackBonus += ench[7];
	damageBonus += ench[8];

	
}

//!Function to return armor's name
string Characters::currentArmor()
{
	return armor->getName();
}
//!Function to return weapon's name
string Characters::currentWeapon()
{
	return weapon->getName();
}
//!Function to return shield's name
string Characters::currentShield()
{
	return shield->getName();
}
//!Function to return helmet's name
string Characters::currentHelmet()
{
	return helmet->getName();
}
//!Function to return boots' name
string Characters::currentBoots()
{
	return boots->getName();
}
//!Function to return ring's name
string Characters::currentRing()
{
	return ring->getName();
}

/*
FOR BATTLE
*/
//!Function to set inBattle to true and initiate battle state
void Characters::startBattle(Characters* c)
{
	this->inBattle = true;
	c->inBattle = true;
}
//!Function to set inBattle to false, ending battle state
void Characters::endBattle()
{
	inBattle = false;
}
/*//!Helper function for conversion of weapon dice of format #d# and roll dice to get dice roll value
int Characters::processWeaponDice()
{
string dice = weapon->getDice();
int damage = _die.roll(dice);
return damage;
}*/

//!Function to calculate attack roll with: d20 + attack bonus
int Characters::attackRoll()
{
	return _die.roll("1d20") + attackBonus;
}

//!Function to calculate damage roll with: weaponDice + damage bonus (if this yields value less than zero, damageRoll = 0)
int Characters::damageRoll()
{
	//	int damage = processWeaponDice() + damageBonus;
	//	if (damage < 0)
	return 0;
	//	return damage;
}

//!Function that increases experience based on specified amount gained and if experience exceeds lower limit of next level it will trigger level up.
void Characters::gainExperience(int gained)
{
	exp += gained;
	if (exp >= experience[1][level])
		gainLevel();
}

//!Function that increments level and sets isLevelUp to true
void Characters::gainLevel()
{
	level++;
	isLevelUp = true;
}

//!Function used when level up to increment an chosen ability score and end level up process.
void Characters::updateAbilityScore(int scoreIndex)
{
	int modifier;
	scores[0][scoreIndex]++;
	abilityScoreMod();
	setIsLevelUp(false);
}

void Characters::scoreIncrease(int index, int value)
{
	if (index == 6)
	{
		for (int i = 0; i < MAX_NUM_SCORES; i++)
			scores[0][i] += value;
	}
	else
	{
		scores[0][index] += value;
	}
	abilityScoreMod();
}


void Characters::setPosition(int y, int x)
{
	position.clear();
	position.push_back(y);
	position.push_back(x);
}


/*
FOR SUBJECT

*/

//!Function used to notify changes in character state and redisplay stats
void Characters::currentState()
{
	Notify();
}


/*
FOR UNIT TEST
*/

///Check to see if score values are valid (3 to 18)
bool Characters::validateNewCharacter()
{
	for (int i = 0; i < 6; i++)
	{
		if (scores[0][i]<3 || scores[0][i]>18)
			return false;
		//if (scores[1][i] != (int)floor(((double)scores[0][i] - 10) / 2))
		//	return false;
	}

	return true;
}

//Check if proficiency bonus is correct
bool Characters::validateProficiency()
{
	if (level <= 4 && proficiencyBonus != 2)
		return false;
	else if (level> 4 && level <= 8 && proficiencyBonus != 3)
		return false;
	else if (level> 8 && level <= 12 && proficiencyBonus != 4)
		return false;
	else if (level> 12 && level <= 16 && proficiencyBonus != 5)
		return false;
	else if (level > 16 && proficiencyBonus != 6)
		return false;
	else
		return true;
}

//! Allows for Characters to be saved to disk
void Characters::Serialize(CArchive &ar) {
	CObject::Serialize(ar);
	if (ar.IsStoring()) {
		calcAttackBonus();
		calcDamageBonus();
		ar << exp;
		ar << level;
		ar << proficiencyBonus;
		ar << armorClass;

		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 6; ++j) {
				ar << scores[i][j];
			}
		}

		ar << size;
		ar << inBattle;
		ar << isDead;
		ar << isLevelUp;
		ar << equiped;
		ar << attackBonus;
		ar << damageBonus;
		// If player position hasn't been set, then dummy values will be serialized
		if (position.size() == 0){
			ar << -1;
			ar << -1;
		}
		else {
			ar << (int)position[0];
			ar << (int)position[1];
		}
		armor->Serialize(ar);
		weapon->Serialize(ar);
		helmet->Serialize(ar);
		shield->Serialize(ar);
		boots->Serialize(ar);
		belt->Serialize(ar);
		ring->Serialize(ar);
	}
	else {
		ar >> exp;
		ar >> level;
		ar >> proficiencyBonus;
		ar >> armorClass;

		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 6; ++j) {
				ar >> scores[i][j];
			}
		}

		int tempSize = 0;
		ar >> tempSize;
		size = (Size)tempSize;
		ar >> inBattle;
		ar >> isDead;
		ar >> isLevelUp;
		ar >> equiped;
		ar >> attackBonus;
		ar >> damageBonus;
		if (position.size() == 0){
			int tempX = 0;
			ar >> tempX;
			position.push_back(0);
			int tempY = 0;
			ar >> tempY;
			position.push_back(0);
		}
		else {
			ar >> position[0];
			ar >> position[1];
		}
	
		armor->Serialize(ar);
		weapon->Serialize(ar);
		helmet->Serialize(ar);
		shield->Serialize(ar);
		boots->Serialize(ar);
		belt->Serialize(ar);
		ring->Serialize(ar);
	}
}

/**
* Operator overload for assignment operator. Allows the Character object to
* take on copies of the same members of another Character object.
*/
Characters& Characters::operator =(const Characters *otherChar) {
	this->level = otherChar->level;
	this->exp = otherChar->exp;
	this->level = otherChar->level;
	this->proficiencyBonus = otherChar->proficiencyBonus;
	this->armorClass = otherChar->armorClass;

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 6; ++j) {
			this->scores[i][j] = otherChar->scores[i][j];
		}
	}

	this->size = otherChar->size;
	this->inBattle = otherChar->inBattle;
	this->isDead = otherChar->isDead;
	this->isLevelUp = otherChar->isLevelUp;
	this->equiped = otherChar->equiped;
	this->attackBonus = otherChar->attackBonus;
	this->damageBonus = otherChar->damageBonus;
	this->position.at(0) = otherChar->position.at(0);
	this->position.at(1) = otherChar->position.at(1);
	this->armor = otherChar->armor;
	this->weapon = otherChar->weapon;
	this->shield = otherChar->shield;
	this->boots = otherChar->boots;
	this->ring = otherChar->ring;
	return *this;
}

/**
* Copy Constructor - creates a new Character with the same attributes as
* another Character object
*/
Characters::Characters(Characters* otherChar) {
	this->level = otherChar->level;
	this->exp = otherChar->exp;
	this->level = otherChar->level;
	this->proficiencyBonus = otherChar->proficiencyBonus;
	this->armorClass = otherChar->armorClass;

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 6; ++j) {
			this->scores[i][j] = otherChar->scores[i][j];
		}
	}

	this->size = otherChar->size;
	this->inBattle = otherChar->inBattle;
	this->isDead = otherChar->isDead;
	this->isLevelUp = otherChar->isLevelUp;
	this->equiped = otherChar->equiped;
	this->attackBonus = otherChar->attackBonus;
	this->damageBonus = otherChar->damageBonus;
	this->position.at(0) = otherChar->position.at(0);
	this->position.at(1) = otherChar->position.at(1);
	this->armor = otherChar->armor;
	this->weapon = otherChar->weapon;
	this->shield = otherChar->shield;
	this->boots = otherChar->boots;
	this->ring = otherChar->ring;
}

/**
* Sets the Character's map data to a new map, deleting the old map data in the process
*@param newMap vector<string>, where each string represents one line of the map
*/
void Characters::setMap(std::vector<std::string> *newMap){
	if (_map != NULL){
		//delete _map; // To prevent memory leaks
	}
	_map = newMap;
	for (int i = 0; i < _map->size(); i++){
		for (int j = 0; j < _map->at(i).size(); j++){
			if (_map->at(i).at(j) == SimplifiedMapSymbols::_Player_){
				position[0] = i;
				position[1] = j;
				return;
			}
		}
	}
	return;
}

bool Characters::validatePlayerMove(int x, int y){
	// Determine valid square
	if (x < 0 || x >= _map->size())
		return false;
	if (y < 0 || y >= _map->at(0).size())
		return false;
	char posInQuestion = _map->at(x).at(y);
	return _validPosition(posInQuestion);
}

bool Characters::_validPosition(char posInQuestion) {
	if (posInQuestion == SimplifiedMapSymbols::_Obstruction_)
		return false;
	if (posInQuestion == SimplifiedMapSymbols::_BasicContainer_)
		return false;
	if (posInQuestion == SimplifiedMapSymbols::_Enemies_)
		return false;
	return true;
}