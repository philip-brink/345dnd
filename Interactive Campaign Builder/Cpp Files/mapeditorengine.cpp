#include "mapeditorengine.h"

//!default oncstructor sets the temp level vector for the cpp unit test
MapEditorEngine::MapEditorEngine()
{
	this->player = nullptr;
	for(int x=0; x<10; x++)
	{
		templevel.push_back("");
		for(int y=0; y<10; y++)
			templevel[x] = templevel[x] + ".";
	}
}

//!parameterized oncstructor takes in a level editor object
//!and initializes all booleans to false.
//!also initializes other components of the leveleditor object
//!that it is ob serving as its own
MapEditorEngine::MapEditorEngine(LevelEditor* level_)
{
	this->level = level_;
	lastButtonIndex = -1;
	targetIndex = 0;
	target.x = 0;
	target.y = 0;
	playercounter = 0;
	exitdoorcounter = 0;
	currentenvcomponent = nullptr;
	targetTexture = nullptr;
	quit = false;
	ontargetfirsttime = false;
	targetselected = false;
	wasAtGamePlayGrids = false;
	canDelete = false;
	renderingOnGameplayGrids = false;
	targetisplayer = false;
	this->components = this->level->getGameplayGridsRects();
	this->characterTarget.push_back(this->level->getPlayerDestinationAtBottomRect());
	this->player = this->level->getPlayerComponent();
	this->envcomponents = this->level->getEnvironmentComponents();


	//if theme index is -1, it means a prebuilt level was loaded
	if(this->level->getThemeIndex() == -1)
	{
		this->playercounter = 1;
		this->exitdoorcounter = 1;
		this->templevel = this->level->getLevelMapVector();
	}

	else
	{
		for(int y=0; y<this->level->getLevelWindow()->getGamePlay_Y_Grids(); y++)
		{
			templevel.push_back("");
			for(int x=0; x<this->level->getLevelWindow()->getGamePlay_X_Grids(); x++)
				templevel[y] = templevel[y] + "."; 
		}
	}
}

//!fills a cell on the map with a character component
void MapEditorEngine::fillCell(int x, int y, char component)
{
	templevel[y].at((x)) = component;
}

//!this executes when the mouse cursor is on the canvas where gameplay grid cells and be filled
void MapEditorEngine::onGameplayGrids(SDL_Event* _event)
{
	this->level->getLevelWindow()->displayWindow();
	wasAtGamePlayGrids = true;
	//for no target selected just do a green alphablend
	if (targetselected == false)
	{
		//if no target was selected first look for a right button click down to make can delete true
		if ((_event->type == SDL_MOUSEBUTTONDOWN) && (_event->button.button == SDL_BUTTON_RIGHT))
			canDelete = true;

		//since here I already no target is selected and mouse is on gameplay grid,
		//check for right button click to delete an existing texture
		if ((_event->type == SDL_MOUSEBUTTONUP) && (_event->button.button == SDL_BUTTON_RIGHT) && (canDelete == true))
		{
			target = getRectGridOfMouse(this->level->getLevelWindow());
			if (templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) != '.')
			{
				//check to see if exit door is being deleted
				for (int x = 0; x<envcomponents.size(); x++)
				{
					if (envcomponents[x]->getComponentName() == "exit")
						if (envcomponents[x]->getComponentChar() == templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())))
							exitdoorcounter--;
				}

				//check to see if player is being deleted
				if (player->getComponentChar() == templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())))
				{
					playercounter--;
					std::cout << "Player counter after deletion: " << playercounter << std::endl;
				}

				templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) = '.';
				SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 0, 0, 255);
				SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
				this->level->getLevelWindow()->displayWindow();
			}
		}
		//check if cursor is already on a target
		if (ontargetfirsttime == true)
		{
			//check if moved away from that target
			if (checkIfMouseOnARectangle(&target) == false)
			{
				//moved away from target render the target rect back to old state
				if (templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) == '.')
				{
					SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 0, 0, 255);
					SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
					this->level->getLevelWindow()->displayWindow();
					ontargetfirsttime = false;
				}
			}
		}
		//by here it means at a new target on gameplaygrid
		else
		{
			components = level->getGameplayGridsRects();

			target = checkMousePosition(components, &targetIndex);

			if (target.x >= 0)
			{
				//alphablend only if the grid does not have an exsting texture
				if (templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) == '.')
				{
					ontargetfirsttime = true;
					SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 255, 0, 75);
					SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
					this->level->getLevelWindow()->displayWindow();
				}
			}
		}
	}

	//if a target is selected render the texture on target rectangles
	else
	{
		//look for a left button click
		if ((_event->type == SDL_MOUSEBUTTONDOWN) && (_event->button.button == SDL_BUTTON_LEFT))
			renderingOnGameplayGrids = true;

		if (renderingOnGameplayGrids == true)
		{
			target = getRectGridOfMouse(this->level->getLevelWindow());
			//only if a target is not alreaady rendered there
			if (templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) == '.')
			{
				if ((targetisplayer == true) && (playercounter == 0))
				{
					playercounter++;
					std::cout << "Targert player: " << targetisplayer << std::endl;
					std::cout << "Player counter: " << playercounter << std::endl;
					SDL_RenderCopy(this->level->getLevelWindow()->getRenderer(), targetTexture, nullptr, &target);
					fillCell(target.x / this->level->getLevelWindow()->getGridX_Length(), target.y / this->level->getLevelWindow()->getGridY_Length(), player->getComponentChar());
					this->level->getLevelWindow()->displayWindow();
				}
				else if ((envcomponents[targetIndex]->getComponentName() == "exit") && (exitdoorcounter == 0) && (targetisplayer == false))
				{
					std::cout << "Targert player rendered from exitdoor: " << targetisplayer << std::endl;
					exitdoorcounter++;
					SDL_RenderCopy(this->level->getLevelWindow()->getRenderer(), targetTexture, nullptr, &target);
					fillCell(target.x / this->level->getLevelWindow()->getGridX_Length(), target.y / this->level->getLevelWindow()->getGridY_Length(), envcomponents[targetIndex]->getComponentChar());
					this->level->getLevelWindow()->displayWindow();
				}

				else if ((targetisplayer == false) && (envcomponents[targetIndex]->getComponentName() != "exit"))
				{
					SDL_RenderCopy(this->level->getLevelWindow()->getRenderer(), targetTexture, nullptr, &target);
					fillCell(target.x / this->level->getLevelWindow()->getGridX_Length(), target.y / this->level->getLevelWindow()->getGridY_Length(), envcomponents[targetIndex]->getComponentChar());
					this->level->getLevelWindow()->displayWindow();
				}
			}
		}

		if ((_event->type == SDL_MOUSEBUTTONUP) && (_event->button.button == SDL_BUTTON_LEFT))
			renderingOnGameplayGrids = false;
	}
}

//!a test function for cpp unit test
bool MapEditorEngine::isPlayerOnCell(int x, int y)
{
	if (this->templevel[y].at(x) == SimplifiedMapSymbols::_Player_)
		return true;
	return false;
}

//!a test function for cpp unit test
//!test if the cell has an obstruction
bool MapEditorEngine::isCellAnObstruction(int x, int y)
{
	if (this->templevel[y].at(x) == SimplifiedMapSymbols::_Obstruction_)
		return true;
	return false;
}

//!this executes when the mouse cursor is on the bottom left rectangle where the environment
//!components can be selected
void MapEditorEngine::onEnvironmentGrid(SDL_Event* _event)
{
	//first correctly render back the last gameplay grid the mouse cursor was on
	if (wasAtGamePlayGrids == true)
	{
		wasAtGamePlayGrids = false;
		ontargetfirsttime = false;
		if (templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) == '.')
		{
			SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 0, 0, 255);
			SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
			this->level->getLevelWindow()->displayWindow();
		}
	}

	if (targetselected == false)
	{
		components = level->getLevelComponentRects();
		//check on which rect
		target = checkMousePosition(components, &targetIndex);
		//if mouse is on target check to see for a click event
		if (target.x >= 0)
		{
			//render description when on target first time
			if (ontargetfirsttime == false)
			{
				//render description at bottom right
				this->level->renderDescriptionAtBottomRIght(envcomponents[targetIndex]);

				//now alphablend the target on environment portion
				ontargetfirsttime = true;
				SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 255, 0, 75);
				SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
				this->level->getLevelWindow()->displayWindow();
			}

			//lookout for a click event for target
			if ((_event->type == SDL_MOUSEBUTTONUP) && (_event->button.button == SDL_BUTTON_LEFT))
			{
				targetselected = true;
				canDelete = false;
				targetTexture = envcomponents[targetIndex]->getImageDetails()->getImageTexture();
			}
		}
		//if mouse is not on a target then render the bottom fresh
		else
		{
			this->level->renderDescriptionAtBottomRIght(nullptr);
			ontargetfirsttime = false;
			level->renderEnvironmentAtBottom();
			this->level->getLevelWindow()->displayWindow();
		}
	}
}

//!this executes when the mouse cursor is on the rectangle at bottom where the character component can be selected
void MapEditorEngine::onCharacterGrid(SDL_Event* _event)
{
	//first correctly render back the last gameplay grid the mouse cursor was on
	if (wasAtGamePlayGrids == true)
	{
		wasAtGamePlayGrids = false;
		ontargetfirsttime = false;
		if (templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) == '.')
		{
			SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 0, 0, 255);
			SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
		}
	}

	if (targetselected == false)
	{
		//check on which rect
		target = checkMousePosition(characterTarget, &targetIndex);
		//if mouse is on character target check to see for a click event
		if (target.x >= 0)
		{
			if (ontargetfirsttime == false)
			{
				this->level->renderDescriptionAtBottomRIght(this->level->getPlayerComponent());
				ontargetfirsttime = true;
				SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 255, 0, 75);
				SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
				this->level->getLevelWindow()->displayWindow();
			}

			//lookout for a click event for target
			if ((_event->type == SDL_MOUSEBUTTONUP) && (_event->button.button == SDL_BUTTON_LEFT))
			{
				targetselected = true;
				targetisplayer = true;
				canDelete = false;
				targetTexture = player->getImageDetails()->getImageTexture();
			}
		}
		//if mouse is not selected by character targer and not on character rectangle then render the bottom fresh
		else if(targetselected == false && ontargetfirsttime == false)
		{
			this->level->renderDescriptionAtBottomRIght(nullptr);
			ontargetfirsttime = false;
			level->renderCharactersAtBottom();
			this->level->getLevelWindow()->displayWindow();
		}
	}
}

//!given a vector of rectangles, this function returns the exact rectangle the mouse could be on
//!along with the index of the vector that is change on the reference pass.
//!if mouse is not on a rectangle then a rectangle with negative dimensions is returned
SDL_Rect MapEditorEngine::checkMousePosition(std::vector<SDL_Rect> components, int* targetIndex)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	for (int z = 0; z<components.size(); z++)
	{
		if ((x >= components[z].x) && (x <= components[z].x + components[z].w)
			&& (y >= components[z].y) && (y <= components[z].y + components[z].h))
		{
			*targetIndex = z;
			return components[z];
		}
	}
	SDL_Rect d;
	d.x = -5;
	d.y = -5;
	d.h = -5;
	d.w = -5;
	return d;
}

//!checks if a cell is occupied or not
bool MapEditorEngine::isOccupied(int x, int y)
{
	if (templevel[y].at(x) == '.')
		return false;
	return true;
}

//!checks if mouse is on a target rectangle
bool MapEditorEngine::checkIfMouseOnARectangle(SDL_Rect* target)
{
	int mouse_X_cor, mouse_Y_cor;
	SDL_GetMouseState(&mouse_X_cor, &mouse_Y_cor);
	if ((mouse_X_cor >= target->x) && (mouse_X_cor <= (target->x + target->w))
		&& (mouse_Y_cor >= target->y) && (mouse_Y_cor <= (target->y + target->h)))
		return true;
	return false;
}

//!if mouse is on gameplay canvas then this function will return
//!which grid the mouse is on
SDL_Rect MapEditorEngine::getRectGridOfMouse(LevelWindow* window)
{
	int mouse_X_cor, mouse_Y_cor;
	SDL_GetMouseState(&mouse_X_cor, &mouse_Y_cor);
	SDL_Rect dest;
	dest.x = mouse_X_cor - (mouse_X_cor % window->getGridX_Length());
	dest.y = mouse_Y_cor - (mouse_Y_cor % window->getGridY_Length());
	dest.h = window->getGridY_Length();
	dest.w = window->getGridX_Length();
	return dest;
}

//!validates the map
bool MapEditorEngine::validateMap()
{
	//if player is nullptr it means we are running a cpp unit test
	if(this->player == nullptr)
	{
		this->simplemap = this->templevel;
		return SingletonRouting::getInstance()->routeValidity(simplemap);
	}
	
	bool condition = false;

	//first check if 1 player and 1 exit
	if ((this->exitdoorcounter == 0) || (this->playercounter == 0))
		return false;

	//first check if all grids are filled
	for (int x = 0; x<this->templevel.size(); x++)
		for (int y = 0; y<this->templevel[x].length(); y++)
			if (this->templevel[x].at(y) == '.')
				return false;

	//now convert the temp level map into a simplistic map
	this->simplemap = this->templevel;
	char tempchar;
	for (int x = 0; x<this->templevel.size(); x++)
	{
		for (int y = 0; y<this->templevel[x].length(); y++)
		{
			tempchar = templevel[x].at(y);
			//check if the character is player
			if (tempchar == player->getComponentChar())
			{
				simplemap[x].at(y) = player->getComponentChar();
			}

			//tempcharacter correspondes to envronment
			else
			{
				for (int j = 0; j<envcomponents.size(); j++)
				{
					if (envcomponents[j]->getComponentChar() == tempchar)
					{
						//check if exit door
						if (envcomponents[j]->getComponentName() == "exit")
						{
							simplemap[x].at(y) = _ExitDoor_;
						}

						//if not exit door then check if it is obstruction
						else if (envcomponents[j]->isComponentObstructionToPlayer() == true)
						{
							simplemap[x].at(y) = _Obstruction_;
						}

						//otherwise its a path that player can traverse
						else
						{
							simplemap[x].at(y) = _FreePath_;
						}
					}
				}
			}
		}
	}
	condition = SingletonRouting::getInstance()->routeValidity(simplemap);
	std::cout<<SingletonRouting::getInstance()->routeValidity(simplemap)<<std::endl;
	return condition;
}

//!this executes when mouse cursor is on the right hand menu rectangle
//!0 back to main menu
//!1 instructions
//!2 accept map
int MapEditorEngine::onRightHandMenuGrid(SDL_Event* _event)
{
	if (wasAtGamePlayGrids == true)
	{
		wasAtGamePlayGrids = false;
		ontargetfirsttime = false;
		if (templevel[target.y / this->level->getLevelWindow()->getGridY_Length()].at((target.x / this->level->getLevelWindow()->getGridX_Length())) == '.')
		{
			SDL_SetRenderDrawColor(this->level->getLevelWindow()->getRenderer(), 0, 0, 0, 255);
			SDL_RenderFillRect(this->level->getLevelWindow()->getRenderer(), &target);
			this->level->getLevelWindow()->displayWindow();
		}
	}

	int x_cor, y_cor;
	SDL_GetMouseState(&x_cor, &y_cor);
	int index = level->getLevelWindow()->getButtonIndexThatMouseIsOn(x_cor, y_cor);

	if (index < 0)
	{
		if (lastButtonIndex != -1)
			level->getLevelWindow()->changeButtonColor(lastButtonIndex, 255, 0, 0);
		lastButtonIndex = index;
	}
	//now if on a valid new button render new color and check for input and return corresponding index
	else
	{
		if (index != lastButtonIndex)
		{
			level->getLevelWindow()->changeButtonColor(index, 0, 255, 0);

			if (lastButtonIndex != -1)
				level->getLevelWindow()->changeButtonColor(lastButtonIndex, 255, 0, 0);
			lastButtonIndex = index;
		}
	}

	if ((_event->type == SDL_MOUSEBUTTONUP) && (_event->button.button == SDL_BUTTON_LEFT) && (index >= 0))
		return index;
	return -1;
}

//!returns the current state of the map as a string vector
std::vector<std::string> MapEditorEngine::getEnvironmentTextLevel()
{
	return this->templevel;
}

//accessors and mutators that would be relevant in the class that instantiated the map editor object
void MapEditorEngine::setQuit(bool q)
{
	this->quit = q;
}
bool MapEditorEngine::getQuit()
{
	return this->quit;
}
void MapEditorEngine::setOnTargetFirstTime(bool o)
{
	this->ontargetfirsttime = o;
}
bool MapEditorEngine::getOnTargetFirstTime()
{
	return this->ontargetfirsttime;
}
void MapEditorEngine::setTargetSelected(bool t)
{
	this->targetselected = t;
}
bool MapEditorEngine::getTargetSelected()
{
	return this->targetselected;
}
void MapEditorEngine::setWasAtGameplayGrids(bool w)
{
	this->wasAtGamePlayGrids = w;
}
bool MapEditorEngine::getWasAtGameplayGrids()
{
	return this->wasAtGamePlayGrids;
}
void MapEditorEngine::setRenderingOnGameplayGrids(bool r)
{
	this->renderingOnGameplayGrids = r;
}
bool MapEditorEngine::getRenderingOnGameplayGrids()
{
	return this->renderingOnGameplayGrids;
}
void MapEditorEngine::setCanDelete(bool c)
{
	this->canDelete =c;
}
bool MapEditorEngine::getCanDelete()
{
	return this->canDelete;
}
void MapEditorEngine::setTargetIsPlayer(bool s)
{
	this->targetisplayer = s;
}
bool MapEditorEngine::setTargetIsPlayer()
{
	return this->targetisplayer;
}
void MapEditorEngine::setTargetTexture(SDL_Texture* t)
{
	this->targetTexture = t;
}
SDL_Texture* MapEditorEngine::getTargetTexture()
{
	return this->targetTexture;
}