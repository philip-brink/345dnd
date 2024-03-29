#if !defined ( ASSIGNMENT2_H )
#define ASSIGNMENT2_H
#include "menus.h"
using namespace BaseMenuColorIndex;
using namespace DestinationFromMenus;
/*!
*The instance of this class will be the main menu for assignment 2, inherits from abstract class menus
*/
class Assignment2Menu : public Menus
{
public:
	Assignment2Menu(std::string title);
	void setupMenu();
	int destinationMap(int index);
};

#endif