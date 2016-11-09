#if !defined (GAMEPLAY_ENGINE_H)
#define GAMEPLAY_ENGINE_H
#include "prebuiltlevel.h"
#include <vector>
class GamePlayEngine
{
private:
	PreBuiltLevel* _level;
	SDL_Event* _event;
	bool _moveSelect;
	bool _moveValidityTracker;
	bool _firstTimeGridTarget;
	SDL_Rect _currentGrid;
	SDL_Rect _lastGrid;
	int _gridIndex;
	int _lastButtonIndex;
	std::vector<SDL_Rect> _buttons;

public:
	GamePlayEngine();
	void attachLevel(PreBuiltLevel* level, SDL_Event* event_);
	void detachLevel();
	int runEngine();
	void onGameplayGrids();
	int onRIghtHandMenu();
	void movePlayer();
	SDL_Rect checkMousePosition(std::vector<SDL_Rect> components, int* targetIndex);
};

#endif