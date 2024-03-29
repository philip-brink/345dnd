#include "Characters.h"
#include "Fighter.h"
#include "Monster.h"
#include "Equipment.h"
#include <iomanip>
using namespace std;

int experience[3][20] = {
	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 },
	{ 0, 300, 900, 2700, 6500, 14000, 23000, 34000, 48000, 64000, 85000, 100000, 120000, 140000, 165000, 195000, 225000, 265000, 305000, 355000 },
	{ 50, 100, 200, 450, 700, 1100, 1800, 2300, 2900, 3900, 5000, 5900, 7200, 8400, 10000, 11500, 13000, 15000, 18000, 20000}
};

/*CONSTRUCTOR*/
//!Default Constructor
Characters::Characters()
{
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

	armor = new Armor();
	shield = new Shield();
	boots = new Boots();
	ring = new Ring();
	helmet = new Helmet();

	detProficiencyBonus();
	detExp();
	abilityScoreMod();
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
	if (level == 1)
	{
		armor = new Armor();
		weapon = new Weapon("Longsword", "1d8","melee");
	}
	else
	{
		armor = new Armor("Padded Armor", 11);
		weapon = new Weapon("Light Crossbow", "1d8", "ranged");
	}
	shield = new Shield();
	boots = new Boots();
	ring = new Ring();
	helmet = new Helmet();

	calcArmorClass();

	//calculate attackBonus and DamageBonus
	calcAttackBonus();
	calcDamageBonus();


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

}
//!Destructor
Characters::~Characters()
{
	destroyObject();
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
	return rand() % i + 1;
}

//!Rolls 4 dice and returns sum of 3 largest values (used for ability scores)
int Characters::roll4D6()
{
	int temp;
	int rolls[4];
	for (int i = 0; i <= 3; i++)
	{
		rolls[i] = rollDice(6);

	}

	for (int i = 1; i <= 3; i++)
	{
		if (rolls[i] > rolls[i - 1])
		{
			temp = rolls[i];
			rolls[i] = rolls[i - 1];
			rolls[i - 1] = temp;
		}
	}

	return rolls[0] + rolls[1] + rolls[2];
}

//!Function that rolls 4D6 for each ability score and sets two largest values to STR and CON
void Characters::generateAbility()
{
	int temp;
	int abscor[6];
	for (int i = 0; i < MAX_NUM_SCORES; i++)
	{
		abscor[i] = roll4D6();
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

	//If no equipment with ACBonus is equipped 
	if (armor->compareName("NONE"))
		armorClass = 10;
	else
	{
		armorClass = armor->getACBonus();
	}

	armorClass += scores[1][1];
}

/*!Function to calculate attack bonus by adding proficiency bonus and 
DEX mod if ranged weapon or STR mod if melee weapon*/
void Characters::calcAttackBonus()
{
	attackBonus = proficiencyBonus;
	if (weapon->compareType("ranged"))
		attackBonus += scores[1][1];  //add dex mod if ranged weapon
	else 
		attackBonus += scores[1][0]; //add str mod if melee or no weapon
}

/*!Function to calculate damage bonus which is DEX mod if ranged weapon or STR mod if melee weapon*/
void Characters::calcDamageBonus()
{
	if (weapon->compareType("ranged"))
		damageBonus = scores[1][1];
	else
		damageBonus = scores[1][0];
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
	cout << "\nCHA: " << scores[0][5] << "(" << scores[1][5] << ")"<< endl;
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

/*EQUIP FUNCTIONS: 
Allows to equip a new Armor, Weapon, Helmet, Boots, Ring and Shield
//If wish to de-equip use default constructor of these Classes
*/

//!Function to equip armor. Previous AC bonus is removed and recalculate the AC based on new armor. Triggers redisplay of stats
void Characters::equip(Armor* a)
{
	//Remove bonus
	if (armor->compareName("NONE"))
		armorClass -= 10;
	else
		armorClass -= armor->getACBonus();
	armor = a;
	//Add Bonus
	if (armor->compareName("NONE"))
		armorClass += 10;
	else
		armorClass += armor->getACBonus();

	currentState();
}

//!Function to equip weapon. Recalculates damage bonus and attack bonus based on weapon equipped. Triggers redisplay of stats
void Characters::equip(Weapon* w)
{
	weapon = w;
	calcDamageBonus();
	calcAttackBonus();
	currentState();
}

//!Function to equip helmet. Previous bonus is removed and recalculate the bonus based on new helmet. Triggers redisplay of stats.
void Characters::equip(Helmet* h)
{
	int bonus = helmet->getBonus();
	if (helmet->getBonusType() == 1) //INT
		updateStatsDQ(3, bonus);
	else if (helmet->getBonusType() == 2) //WIS
		updateStatsDQ(4, bonus);
	else if (helmet->getBonusType() == 3)  //AC
		armorClass -= bonus;
	
	helmet = h;
	bonus = helmet->getBonus();
	if (helmet->getBonusType() == 1) //INT
		updateStatsEQ(3, bonus);
	else if (helmet->getBonusType() == 2) //WIS
		updateStatsEQ(4, bonus);
	else if (helmet->getBonusType() == 3)  //AC
		armorClass += bonus;

	currentState();
}
//!Function to equip boots. Previous bonus is removed and recalculate the bonus based on new boots. Triggers redisplay of stats.
void Characters::equip(Boots* b)
{
	int bonus = boots->getBonus();
	if (boots->getBonusType() == 1) //DEX
		updateStatsEQ(1, bonus);
	if (boots->getBonusType() == 2) //AC
		armorClass -= bonus;

	boots = b;
	bonus = boots->getBonus();

	if (boots->getBonusType() == 1) //DEX
		updateStatsEQ(1, bonus);
	if (boots->getBonusType() == 2) //AC
		armorClass += bonus;

	currentState();
}

//!Function to equip ring. Previous bonus is removed and recalculate the bonus based on new ring. Triggers redisplay of stats.
void Characters::equip(Ring* r)
{
	int bonus = ring->getBonus();
	if (ring->getBonusType() == 1) //STR
		updateStatsDQ(0, bonus);
	else if (ring->getBonusType() == 2) //CON
		updateStatsDQ(2, bonus);
	else if (ring->getBonusType() == 3) //WIS
		updateStatsDQ(4, bonus);
	else if (ring->getBonusType() == 3) //CHA
		updateStatsDQ(5, bonus);
	else if (ring->getBonusType() == 4) //AC
		armorClass -= bonus;
	
	ring = r;

	bonus = ring->getBonus();
	if (ring->getBonusType() == 1) //STR
	{
		updateStatsEQ(0, bonus);
	}
	else if (ring->getBonusType() == 2) //CON
		updateStatsEQ(2, bonus);
	else if (ring->getBonusType() == 3) //WIS
		updateStatsEQ(4, bonus);
	else if (ring->getBonusType() == 3) //CHA
		updateStatsEQ(5, bonus);
	else if (ring->getBonusType() == 4) //AC
		armorClass += bonus;

	currentState();
}

//!Function to equip shield. Previous AC bonus is removed and recalculate the AC based on new shield. Triggers redisplay of stats
void Characters::equip(Shield* s)
{
	if (!shield->compareName("NONE"))
		armorClass -= shield->getACBonus();

	shield = s;
	
	armorClass += shield->getACBonus();

	currentState();
}



//!Helper function to update stats when de-equipping an equipment
void Characters::updateStatsDQ(int scoreIndex, int removeBonus)
{
	scores[0][scoreIndex] -= removeBonus;
}

//!Helper function to update stats when equipping an equipment
void Characters::updateStatsEQ(int scoreIndex, int addBonus)
{
	scores[0][scoreIndex] += addBonus;
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
//!Helper function for conversion of weapon dice of format #d# and roll dice to get dice roll value
int Characters::processWeaponDice()
{
	string dice = weapon->getDice();
	
	int numFace = dice.at(2) - '0';
	int numDice = dice.at(0)-'0';
	int damage = 0;
	for (int i = 0; i < numDice; i++)
	{
		damage += rollDice(numFace);
	}
	return damage;
}

//!Function to calculate attack roll with: d20 + attack bonus
int Characters::attackRoll()
{
	return rollDice(20) + attackBonus;
}

//!Function to calculate damage roll with: weaponDice + damage bonus (if this yields value less than zero, damageRoll = 0)
int Characters::damageRoll()
{
	int damage = processWeaponDice() + damageBonus;
	if (damage < 0)
		return 0;
	return damage;
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
			scores[0][i]+=value;
	}
	else
	{
		scores[0][index]+= value;
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

