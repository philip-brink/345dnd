/**
*@file Fighter.h
@brief Implementation of Fighter, which extends Characters
*/
#pragma once
#include "Characters.h"
#include "Entity.h"
#include "Dwarf.h"
#include "Monster.h"

/*!
This class inherits from Characters and creates a Fighter character. All fighters are created with a level and a name. Fighters have a constant Hit_Die = d10. This value is used to calculate the hit points as follows: At level 1: HitPoints = Hit_Die_Max + CON_mod = 10 + CON_mod With each level gained: HitPoints = HitPoints + Hit_Die_Roll + CON_mod
All other information about the fighter is generated in the Characters class See Characters class for more details.

Additional functions were implemented for battle and leveling up.
Namely functions to attack a monster, to receive damage which may result in character death
(leading to Game Over and termination of program), to gain experience which
will trigger level up when applicable
*/


class Monster;

class Fighter : public Characters
{
protected:
	DECLARE_SERIAL(Fighter);
private:
	const int HIT_DIE = 10; //d10
	int hitPoints;
	Race race;
	Size size;
	string name;
	int speed;
	void detRaceTraits(Race race);

public:
	Fighter(int, Race, string);
	Fighter(); // Default constructor for Dummy Fighter
	~Fighter();


	void initHitPoints();
	void recalcHitPoints();
	void displayStats();
	void displayBattle();
	void displayLevelUp();
	void displayDeath();
	string getName() { return name; }

	//For battle
	void attack(Monster* c);
	void receiveDamage(int);

	void levelUp(int);
	void gainExperience(int);
	void dead();


	void currentState();

	//TEST
	bool validateHitPoints();
	bool validateDeath();
	bool validateGainExperience(int);

	bool validatePlayerMove(int x, int y);

	virtual void Serialize(CArchive &ar);
};
