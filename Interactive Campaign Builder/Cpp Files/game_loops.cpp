#include "game_loops.h"


//!primary driver of the driver class game loops
//!managest the next destination based on the destination values returned in
//!the main while loop
void GameLoops::loopManager()
{
	SDLSetup setup = SDLSetup();
	setup.SDL_Initialize_All_Functionalities();

	//instantiate signleton object, create the menu and set it up once
	SingletonDefaultMapsMenu::getInstance()->setupMenu();
	SingletonDefaultMapsMenu::getInstance()->hideMenu();

	bool quit = false;
	int destination;

	char* mappath = new char[100];
	char* campaignname = new char[100];
	std::string temp;
	destination = _MainMenu_;
	while(quit == false)
	{
		switch(destination)
		{
		case _MainMenu_:
			destination = assignmentTWOLoop(mappath, campaignname);
			break;

		case _NewMapCreation_:
			destination = createNewMap(mappath, campaignname);
			SingletonInputsAndStringManager::getInstance()->assignStringToCharPointer(campaignname, this->_currentCampaignTracker);
			break;

		case _GameLevel_:
			gameLevelLoop();
			break;

		case _EditExistingMap_:
			destination = editExistingMap(mappath, campaignname);
			SingletonInputsAndStringManager::getInstance()->assignStringToCharPointer(campaignname, this->_currentCampaignTracker);
			break;

		case _CampaignManagerMenu_:
			destination = campaignManagerLoop(mappath, campaignname);
			break;

		case _EditExistingCampaign_:
			destination = editExistingCampaignLoop(mappath, campaignname);
			break;

		case _ExitToCommandPrompt_:
			quit = true;
		}
	}

	delete mappath;
	delete campaignname;
	mappath = nullptr;
	campaignname = nullptr;
}

//!this is the loop that runs when a user created map is loaded
//!this funtion also will be responsible for the main gameplay loop
//!PLEASE NOTE, I have left everything commented out in the submission for
//!this assignment becuase that code fragment works, an I will use that as 
//!template in assignment 3, interim and final project
int GameLoops::gameLevelLoop()
{
	/*int destinationInt;
	bool quit = false;

	PreBuiltLevel* l = new PreBuiltLevel();
	l->loadUserCreatedLevel(_userCreatedLevelFileName);
	l->createLevelForTargetWindow();
	l->renderAndDisplayLevel();
	destinationInt = -1;

	while(quit==false)
	{
		while(SDL_PollEvent(&_event) != 0)
		{
			if(_event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}

	l->getLevelWindow()->hideWindow();
	delete l;
	l=nullptr;*/
	return _ExitToCommandPrompt_;
}

//!used to create a new map
int GameLoops::createNewMap(char* path, char* campaign)
{
	//take a valid user input first
	system("cls");
	int input;
	bool valid = false;
	std::cout<<"DUNGEONS AND DRAGONS MAP CREATION ENGINE"<<std::endl<<std::endl;
	std::cout<<"Please input the number of rows and columns you wish in your map."<<std::endl<<std::endl;
	std::cout<<"Minimum: 5 X 5"<<std::endl;
	std::cout<<"Maximum: 50 X 50"<<std::endl;
	std::cout<<"Optimal 25 X 15"<<std::endl<<std::endl;

	int totalhorizontalGameGrids = SingletonInputsAndStringManager::getInstance()->takeIntInput("Please input the number of rows minimum 5 and maximum 50: ", 5,50);
	int totalVerticalGameGrids = SingletonInputsAndStringManager::getInstance()->takeIntInput("Please input the number of columns minimum 5 and maximum 50: ", 5,50);

	system("cls");

	std::string title = "test";
	LevelWindow* window = new LevelWindow(title.c_str(), totalhorizontalGameGrids, totalVerticalGameGrids);
	int destinationInt =-1;
	window->displayWindow();

	//create leveleditor object
	LevelEditor* level = new LevelEditor(window,1);
	level->createLevelForTargetWindow();
	level->renderAllNonGameplayGridPortions();
	level->getLevelWindow()->displayWindow();

	SDL_SetRenderDrawBlendMode(window->getRenderer(), SDL_BLENDMODE_BLEND);
	std::vector<Environment*> envcomponents = level->getEnvironmentComponents();

	int succuess = levelEditorLoop(level, path, campaign);
	
	//if success is negative then it means user did not complete creating the map
	//create an object with same file name and delete the last values loaded
	if(succuess < 0)
	{
		CampaignMapManagers* manager = new CampaignMapManagers("assignment2", this->_currentCampaignTracker);
		std::cout<<this->_currentCampaignTracker<<std::endl;
		manager->sucessfullMapCreated(false);
		manager->hideMenu();
		delete manager;
	}

	return _EditExistingCampaign_;
}

//!edits an existing map
int GameLoops::editExistingMap(char* path, char* campaign)
{
	//get a valid map name from the maps already in file
	PreBuiltLevel* l = new PreBuiltLevel();
	//l->loadUserCreatedLevel(_userCreatedLevelFileName);
	l->loadUserCreatedLevel(SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(path));
	l->createLevelForTargetWindow();

	LevelWindow* temp = new LevelWindow(l->getLevelWindow()->getWindowTitle(), l->getLevelWindow()->getGamePlay_X_Grids(), l->getLevelWindow()->getGamePlay_Y_Grids());
	LevelEditor* level = new LevelEditor(temp, -1);
	level->takeComponentsFromPreBuiltLevel(l);
	level->createLevelForTargetWindow();
	level->renderAllNonGameplayGridPortions();
	level->getLevelWindow()->displayWindow();
	l->getLevelWindow()->hideWindow();
	l->destroyLevel();


	SDL_SetRenderDrawBlendMode(temp->getRenderer(), SDL_BLENDMODE_BLEND);

	//delete the temporary pre built level
	delete l;
	l = nullptr;
	
	//DO NOT delete the level editor here, the level editor map is taking care of it
	levelEditorLoop(level, path, campaign);

	return _EditExistingCampaign_;
}

//!lets user create a map
int GameLoops::levelEditorLoop(LevelEditor* level, char* path, char* campaign)
{
	level->renderAndDisplayLevel();
	int destinationInt = -1;
	LevelWindow* const window = level->getLevelWindow();
	MapEditorEngine* m = new MapEditorEngine(level);
	
	while(m->getQuit()==false)
	{
		while(SDL_PollEvent(&_event) != 0)
		{
			SDL_GetMouseState(&this->mouse_X_cor, &this->mouse_Y_cor);

			//this is to ensure if left mouse button was clicked in gameplay grid and released elsewhere,
			//then when the mouse is taken back to gameplay grid nothing renders
			if(m->getRenderingOnGameplayGrids() == true && (_event.type==SDL_MOUSEBUTTONUP))
				m->setRenderingOnGameplayGrids(false);

			//quit if user presses red on top right
			if(_event.type == SDL_QUIT)
			{
				m->setQuit(true);
				destinationInt = -1;
			}

			//for right button click target selected is false all parameters get reset
			if((_event.type == SDL_MOUSEBUTTONUP)&&(_event.button.button == SDL_BUTTON_RIGHT) && (m->getTargetSelected() == true))
			{
				m->setTargetSelected(false);
				m->setTargetIsPlayer(false);
				m->setTargetTexture(nullptr);
				m->setOnTargetFirstTime(false);
				level->renderAllNonGameplayGridPortions();
				window->displayWindow();
			}

			//for gameplaygrid alphablend on select grids NOTE the -1 as are to give a 1pixel line buffer between the gameplay grid rectangle and rest of the window
			if((mouse_Y_cor<=(((window->getGamePlay_Y_Grids())*window->getGridY_Length()-1))) && (mouse_X_cor<=(((window->getGamePlay_X_Grids())*window->getGridX_Length())-1)))
			{
				m->onGameplayGrids(&_event); 
			}

			//check if mouse is in bottom grid section only if a target is not selected
			else if((mouse_Y_cor>(window->getGamePlay_Y_Grids())*window->getGridY_Length()) && (m->getTargetSelected() == false))
			{
				//check if mouse is on environment grid
				if(mouse_X_cor < level->getEnvironmentPortionEnd_X_Xor())
					m->onEnvironmentGrid(&_event);

				//check if mouse is on character grid
				else if(mouse_X_cor < level->getCharacterPortionEnd_X_Cor())
				{
					level->renderEnvironmentAtBottom();
					m->onCharacterGrid(&_event);
				}
			}

			//check is mouse is on right hand menu
			else if(mouse_X_cor > (window->getGamePlay_X_Grids() * window->getGridX_Length()) && (m->getTargetSelected() == false))
			{
				destinationInt = m->onRightHandMenuGrid(&_event);

				switch(destinationInt)
				{
				//main menu
				case 0:
					destinationInt = -1;
					m->setQuit(true);
					break;

				//instructions
				case 1:
					level->getLevelWindow()->hideWindow();
					system("cls");
					std::cout<<"MAP CREATION INSTRUCTIONS"<<std::endl<<std::endl;
					std::cout<<"1. Left click on components at bottom of window to select, left click on gameplay grid canvas to set the grids with selected component."<<std::endl;
					std::cout<<"2. Right click to deselect a selected component, and repeat the process."<<std::endl;
					std::cout<<"3. When no component is selected, hover on top of grid that is already filled and right click to delete."<<std::endl;
					std::cout<<"4. One and only one player can be put on the map."<<std::endl;
					std::cout<<"5. One and only one exit door can be put on the map."<<std::endl;
					std::cout<<"6. All grids have to be filled for the map to be valid."<<std::endl;
					std::cout<<"7. There has to be a completely unobstructed from from player to exit door for map to be valid."<<std::endl;
					std::cout<<"8. Click on 'Accept Map' on right hand side menu to save map, invalid map will have no affect on button click."<<std::endl;
					std::cout<<"9. When a component is selected, the bottom rightmost rectangle gives the component name and weather it is obstruction or not."<<std::endl;
					std::cout<<"10. Once a valid map is accepted you will be in main menu where you can click on 'Load Map' to load the last valid accepted map."<<std::endl;
					std::cout<<"11. Click on main menu or the cross button at any point to quit, the incomplete map will NOT be saved."<<std::endl<<std::endl;
					std::cout<<"Press any key to continue map creation."<<std::endl;
					_getch();
					system("cls");
					level->getLevelWindow()->unHideWindow();
					break;
				
				//validate map
				case 2:
					if(m->validateMap() == true)
						m->setQuit(true);
					else
					{
						level->getLevelWindow()->hideWindow();
						system("cls");
						std::cout<<"This map is invalid, it could be for one of the following reasons.\n\n";
						std::cout<<"1. Not all the cells have a component, please not for our project, a cell with a component named 'floor' is like an empty cell where a player can traverse freely\n";
						std::cout<<"2. There is not one and one only player\n";
						std::cout<<"3. There is not one and one only exit door\n";
						std::cout<<"4. There is no direct unobstructed route from the player to the exit door. Please not enemies, items and containers do count as obstruction during map creation\n\n";
						std::cout<<"Press any key to continue editing map.";
						getch();
						level->getLevelWindow()->unHideWindow();
					}
				}
			}
		}
	}

	//if main menu was quit without a valid level then, the level will not be saved
	if(destinationInt != -1)
	{
		std::vector<std::string> templevel = m->getEnvironmentTextLevel();
		std::cout<<std::endl<<std::endl;

		//save the level on text file
		level->setEnvironmentTextVector(templevel);
		level->saveLevel(SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(path));

		//InputOutput inop;

		delete level;
		level = nullptr;
	
		delete m;
		m= nullptr;
		return _EditExistingCampaign_;
	}

	delete level;
	level = nullptr;
	
	delete m;
	return -1;
}

//!manages campaign editor menu, and driver for assignment 2 specifications
int GameLoops::assignmentTWOLoop(char* path, char* campaign)
{
	Assignment2Menu* menu = new Assignment2Menu("Assignment2 Menu");
	menu->setupMenu();
	menu->displayMenu();
	
	MenuEngine* engine = new MenuEngine(menu, this->_event);
	int destination = engine->runEngine();
	destination = menu->destinationMap(destination);

	menu->hideMenu();
	delete engine;
	engine = nullptr;
	delete menu;
	menu = nullptr;
	return destination;
}

//!launches the campaign editor main menu loop
int GameLoops::campaignManagerLoop(char* path, char* campaign)
{
	CampaignManager* manager = new CampaignManager("assignment2");
	manager->setupMenu();
	manager->displayMenu();
	MenuEngine* engine = new MenuEngine(manager, this->_event);
	
	int destination = engine->runEngine(campaign);

	if(destination >=0)
		this->_currentCampaignTracker = SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(campaign);

	destination = manager->destinationMap(destination, campaign);
	
	
	manager->hideMenu();
	delete engine;
	engine = nullptr;
	delete manager;
	manager = nullptr;
	return destination;
}

//!once a campaign is selected, this function facilitates editing
int GameLoops::editExistingCampaignLoop(char* path, char* campaign)
{
	START_AFTER_SEQUENCE_CHANGE:
	std::cout<<path<<std::endl;
	CampaignMapManagers* manager = new CampaignMapManagers("assignment2", this->_currentCampaignTracker);
	manager->setupMenu();
	manager->displayMenu();

	MenuEngine* engine = new MenuEngine(manager, this->_event);

	int destination;

	ABORT_SEQUENCE_CHANGE:
	destination = engine->runEngine(path);
	
	//first check if sequence change button or choose from default map was clicked
	//if not then continue as normal
	if((SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(path) != SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(Others::_SequenceChangeButton_))&&(SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(path) != SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(Others::_DefaultMapSelectionButton_)))
	{
		destination = manager->destinationMap(destination, path);
		SingletonInputsAndStringManager::getInstance()->assignStringToCharPointer(campaign, manager->getCampaignNameOfMap());
	}

	//otherwise do a sequence change
	else if(SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(path) == SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(Others::_SequenceChangeButton_))
	{
		//first take input for which map needs to change, ignoring render for sequence change button
		int index1, index2;

		//first get index 1, destination holds index of change sequence
		index1 = engine->runEngine(destination, -1, path);
		if(index1 == -1)
			goto ABORT_SEQUENCE_CHANGE;

		index2 = engine->runEngine(destination, index1, path);
		if(index2 == -1)
			goto ABORT_SEQUENCE_CHANGE;

		manager->changeSequence(index1, index2);

		manager->hideMenu();
		delete engine;
		engine = nullptr;
		delete manager;
		manager = nullptr;
		goto START_AFTER_SEQUENCE_CHANGE;
	}
	
	//other wise get and input for map selection
	else if((SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(path) == SingletonInputsAndStringManager::getInstance()->convertCharPointerToString(Others::_DefaultMapSelectionButton_)))
	{
		SingletonDefaultMapsMenu::getInstance()->getMenuWindow()->unHideWindow();
		MenuEngine* defaultmaps = new MenuEngine(SingletonDefaultMapsMenu::getInstance(), this->_event);
		destination = defaultmaps->runEngine(path);
		SingletonDefaultMapsMenu::getInstance()->getMenuWindow()->hideWindow();
		destination = manager->destinationMap(_MaximumMapsPerCampaign_+1, path);
		SingletonDefaultMapsMenu::getInstance()->getMenuWindow()->hideWindow();
		delete defaultmaps;
		defaultmaps = nullptr;
	}

	manager->hideMenu();
	delete engine;
	engine = nullptr;
	delete manager;
	manager = nullptr;
	return destination;
}

