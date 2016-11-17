#include "prebuiltlevel.h"

//!default constructor for testing purposes only
PreBuiltLevel::PreBuiltLevel()
{
	this->_player = nullptr;
}

//!parameterized constructor sets the path from which the pre built map needs to be loaded
PreBuiltLevel::PreBuiltLevel(std::string path, Fighter* player)
{
	this->_player = player;
	this->_directory_path_for_level_file_text_file = path;
}

//!loads the user created level in private member variables
void PreBuiltLevel::loadUserCreatedLevel(std::string path)
{
	/* CHANGED TO USE NEW MAPS
	//load the level and component path
	this->_level = SingletonFilePathAndFolderManager::getInstance()->loadUserGeneratedLevel(path, &this->_environmentComponentPath);
	*/

	// NEW MAP FUNCTIONALITY SECTION ---------------------------------------------------------------------------
	// create new map
	Map * tempMap = new Map();

	// fuck windows
	wchar_t *wpath = new wchar_t[strlen(path.c_str()) + 1]; //memory allocation
	mbstowcs(wpath, path.c_str(), strlen(path.c_str()) + 1);
	// end fuck windows

	// 
	CFile fileL;
	if (!fileL.Open(wpath, CFile::modeRead))
	{
		std::cout << "Unable to open input file" << std::endl;
	}
	else {
		CArchive arLoad(&fileL, CArchive::load);
		tempMap->Serialize(arLoad);
		arLoad.Close();
	}
	fileL.Close();

	// set the map to be this loaded map, important to do this through
	// the function setMap() so that this->_level is also properly set
	// due to our dirty hacks
	this->setMap(tempMap);

	// fuck windows a bit more
	delete []wpath;
	// end fuck windows again
	// END NEW MAP FUNCTIONALITY SECTION -----------------------------------------------------------------------

	//load the component names (name of theme assets. The path _environmentComponentPath is saved in Map, and assigned when the map is set)
	this->_environmentComponentNames = SingletonInputOutputManager::getInstance()->readFileLineByLine(this->_environmentComponentPath + SingletonFilePathAndFolderManager::getInstance()->getNameofFileContainingAssetNames());

}

//!sets the level text vector that would be used to set the level on the target window
void PreBuiltLevel::setLevelOnTargetWindow()
{
	// I don't think that this function is relevant anymore, seems to be identical in function to
	// the previous function loadUserCreatedLevel()., but without parameters and doesn't set the environment component path.
	// Will leave here for compatibility, and simply call loadUserCreatedLevel() providing a path.
	//this->_level = SingletonInputOutputManager::getInstance()->readFileLineByLine(_directory_path_for_level_file_text_file);
	loadUserCreatedLevel(_directory_path_for_level_file_text_file);
}

//!sets the pointer rect to values from current rects in map
void PreBuiltLevel::getDestRectsForObserver(SDL_Rect* dest1, SDL_Rect* dest2)
{
	*dest1 = this->_dest1ForObserver;
	*dest2 = this->_dest2ForObserver;
}

//!first creates a level window based on the level text vector
//!then calls parent function to setup the artwork on the window
void PreBuiltLevel::createLevelForTargetWindow()
{
	//make level window based on loaded text vector
	this->_level_window = new LevelWindow("PreBuilt", this->_level[0].length(), this->_level.size());

	//call parent function
	Level::createLevelForTargetWindow();

	//create button destinations
	this->_level_window->setFontType(9);
	this->_level_window->setTitleFontSize(72);

	this->_inventoryPaneDest.x = (this->_level_window->getGamePlay_X_Grids() * this->_level_window->getGridX_Length()) + (this->_level_window->getWindowWidth() - (this->_level_window->getGamePlay_X_Grids() * this->_level_window->getGridX_Length())) * 0.1;
	this->_inventoryPaneDest.y = this->_level_window->getWindowHeight() * 0.05;
	this->_inventoryPaneDest.w = (this->_level_window->getWindowWidth() - (this->_level_window->getGamePlay_X_Grids() * this->_level_window->getGridX_Length())) * 0.8;
	this->_inventoryPaneDest.h = this->_level_window->getWindowHeight() * 0.05;
	this->_level_window->addButton("Player Status", 255, 0, 0, this->_inventoryPaneDest);

	this->_movePlayerDest.x = this->_inventoryPaneDest.x;
	this->_movePlayerDest.w = this->_inventoryPaneDest.w;
	this->_movePlayerDest.h = this->_inventoryPaneDest.h;
	this->_movePlayerDest.y = this->_inventoryPaneDest.y + this->_inventoryPaneDest.h * 2;
	this->_level_window->addButton("Move Player", 255, 0, 0, this->_movePlayerDest);

	this->_interact.x = this->_inventoryPaneDest.x;
	this->_interact.w = this->_inventoryPaneDest.w;
	this->_interact.h = this->_inventoryPaneDest.h;
	this->_interact.y = this->_movePlayerDest.y + this->_inventoryPaneDest.h * 2;
	this->_level_window->addButton("Interact", 255, 0, 0, this->_interact);

	this->_attack.x = this->_inventoryPaneDest.x;
	this->_attack.w = this->_inventoryPaneDest.w;
	this->_attack.h = this->_inventoryPaneDest.h;
	this->_attack.y = this->_interact.y + this->_inventoryPaneDest.h * 2;
	this->_level_window->addButton("Attack", 255, 0, 0, this->_attack);

	this->_savePlayer.x = this->_inventoryPaneDest.x;
	this->_savePlayer.w = this->_inventoryPaneDest.w;
	this->_savePlayer.h = this->_inventoryPaneDest.h;
	this->_savePlayer.y = this->_attack.y + this->_inventoryPaneDest.h * 2;
	this->_level_window->addButton("Save Player", 255, 0, 0, this->_savePlayer);

	this->_exitPlay.x = this->_inventoryPaneDest.x;
	this->_exitPlay.w = this->_inventoryPaneDest.w * 0.8;
	this->_exitPlay.h = this->_inventoryPaneDest.h;
	this->_exitPlay.y = this->_savePlayer.y + this->_inventoryPaneDest.h * 2;
	this->_level_window->addButton("Exit play", 255, 0, 0, this->_exitPlay);

	//setup the menu component
	this->_level_window->setMenuOnRenderer();
}

/* NO LONGER NEEDED WITH MAP CLASS
//!sets up container coordinates on map
void PreBuiltLevel::setupContainersOnMap()
{
	ContainerOnMap* temp;
	//first find howmany an get there coordinates
	for (int y = 0; y < this->_level.size(); y++)
	{
		for (int x = 0; x < this->_level[y].size(); x++)
		{
			if (_level[y].at(x) == SimplifiedMapSymbols::_BasicContainer_)
			{
				temp = new ContainerOnMap();
				temp->stringIndex = y;
				temp->charIndex = x;
				temp->container = ContainerGenerator::generateContainer(this->_player);
				this->_containersOnMap.push_back(temp);
			}
		}
	}
}

//sets up enemy coordinates on map
void PreBuiltLevel::setupEnemiesOnMap()
{
	EnemiesOnMap* temp;
	//first find howmany an get there coordinates
	for (int y = 0; y < this->_level.size(); y++)
	{
		for (int x = 0; x < this->_level[y].size(); x++)
		{
			if (_level[y].at(x) == SimplifiedMapSymbols::_Enemies_)
			{
				temp = new EnemiesOnMap();
				temp->stringIndex = y;
				temp->charIndex = x;
				temp->monster = MonsterFactory::createMonster(this->_player);
				this->_enemisOnMap.push_back(temp);
			}
		}
	}
}
*/
//!function to update destination rectangles for the observer
void PreBuiltLevel::setDestRectsForObserver(SDL_Rect dest1, SDL_Rect dest2)
{
	this->_dest1ForObserver = dest1;
	this->_dest2ForObserver = dest2;
}

/* NO LONGER NEEDED WITH MAP CLASS
//!container coordinate accessor
std::vector<ContainerOnMap*> PreBuiltLevel::getContainersOnMap()
{
	return this->_containersOnMap;
}

//!enemy coordiante accessor
std::vector<EnemiesOnMap*> PreBuiltLevel::getEnemiesOnMap()
{
	return this->_enemisOnMap;
}
*/

//!environment component path accessor
std::string PreBuiltLevel::getEnvironmentComponentsPath() const
{
	return this->_environmentComponentPath;
}

//!level text file path accessor
std::string PreBuiltLevel::getDirectoryPathForLevelTextFile() const
{
	return this->_directory_path_for_level_file_text_file;
}

//!path for player image accessor
std::string PreBuiltLevel::getPlayerPath() const
{
	return this->_playerPath;
}

//!enemy file path accessor
std::string PreBuiltLevel::getEnemyPath() const
{
	return this->_enemyPath;
}

//!level text vector accessor
std::vector<std::string> PreBuiltLevel::getBuiltLevelFile() const
{
	return this->_level;
}

//!fighter accessor
Fighter* PreBuiltLevel::getPlayer()
{
	return this->_player;
}

//!map menu buttons accessor 
std::vector<SDL_Rect> PreBuiltLevel::getAllButtonDestinations()
{
	std::vector<SDL_Rect> temp;
	temp.push_back(this->_inventoryPaneDest);
	temp.push_back(this->_movePlayerDest);
	temp.push_back(this->_exitPlay);
	return temp;
}


//!local destructor destroys the positions for enemies and containers
PreBuiltLevel::~PreBuiltLevel()
{
	/* NO LONGER NEEDED WITH MAP CLASS
	for (int x = 0; x < this->_containersOnMap.size(); x++)
	{
		if (this->_containersOnMap[x] != nullptr)
		{
			delete this->_containersOnMap[x];
			this->_containersOnMap[x] = nullptr;
		}	
	}
	for (int x = 0; x < this->_enemisOnMap.size(); x++)
	{
		if (this->_enemisOnMap[x] != nullptr)
		{
			delete this->_enemisOnMap[x];
			_enemisOnMap[x] = nullptr;
		}
	}
	*/
}