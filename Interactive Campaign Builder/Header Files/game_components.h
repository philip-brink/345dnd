
#pragma once
#include "image.h"
#include <vector>
enum environment                {door, wall, floors, water, entrence, door2};
enum gameplayGridComponentTypes {environment, character, items};
/*!
* A game component class that sets attributes for all game components there will be in game.
* primary attributes include an image component along with the path for the image file,
* the component name and a char value associated with the component so that it can be used
* in the text version of the map.
* The image component uses sdl library, details in header files of SDL Manager
*/
class GameComponent
{
protected:
	std::string _image_path;
	std::string _componentName;
	SDL_Image* _componentImage;
	char _componentChar;
	gameplayGridComponentTypes _componentType;

public:
	GameComponent(std::string componentname, char componentchar, gameplayGridComponentTypes componenttype, std::string imagepath);
	virtual void setupComponentOnTargetWindowRenderer(SDL_Renderer* window_renderer);
	SDL_Image* getImageDetails();
	char getComponentChar();
	std::string getComponentName();
	void destroyComponent();
	~GameComponent();
};





