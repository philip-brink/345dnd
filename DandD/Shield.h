/**
 * @brief Class for shields.
 * @author Philip Brink
 * @version 0.0.1
 * @date 2016-10-20
 */

// Include guard
#ifndef Shield_H
#define Shield_H


#include <string>
#include "Item.h"
#include "Dice.h"

class Shield: public Item {
	protected:
		// Shield Attributes
		int defense;
		std::string attackDice;
		int range;
		DECLARE_SERIAL(Shield);
		// Constructors
	public:
		Shield();
		Shield(const Shield* otherShield);
		Shield(string name, int weight, int value, string image, 
				array<int, 9> enchantmentValues, int defense, std::string attackDice, int range);
		int getDefense();
		void setDefense(int defense);
		void incrementDefense();
		void decrementDefense();
		void incrementRange();
		void decrementRange();
		std::string getAttackDice();
		void setAttackDice(std::string attackDice);
		int getRange();
		void setRange(int range);
		std::string toString();
		virtual void Serialize(CArchive &ar); // Will be used to Serialize Shields
};

#endif // Include guard
