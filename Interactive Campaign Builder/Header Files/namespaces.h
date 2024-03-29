#pragma once
#include <string>
/*!
*namspace that has map symbols simplifeid,
*reason for simplification is that, during the purpose
*of routing, a wal, water or enemies are all obstruction
*so a simplified map version reduces reduncies in the program
*/
namespace SimplifiedMapSymbols
{
	const char _Obstruction_ = 'o';
	const char _FreePath_ = '.';
	const char _ExitDoor_ = 'e';
	const char _Player_ = 'p';
}

/*!
*the menu classes hierarchy has a vector for color information
*this namespace keeps the indices for which index has which color constant
*/
namespace BaseMenuColorIndex
{
	const int _MenuBackground_ = 0;
	const int _YesButtonSelect_ = 1;
	const int _NoButtonSelect_ = 2;
	const int _Title_ = 3;
	const int _OtherText_ = 4;
}

/*!
*the aspects by which the monitor height and width should be divided for
*generating windows in this project. A height and width aspect of 1 will ensure the
*windows are full screen
*/
namespace WindowHeightWidthDivideAspect
{
	const double _WindowHeightDivideAspect_ = 1.2;
	const double _WindowWidthDivideAspect_ = 1.2;
}

/*!
*the loop manager communicates with many loops, these namespaces gives global
*constants for usage in the project to ensure that for a specific input at a specif part of the
*game, the loop manager can direct the game to the next portion
*/
namespace DestinationFromMenus
{
	const int _NewMapCreation_ = 0;
	const int _GameLevel_ = 1;
	const int _EditExistingMap_ = 2;
	const int _MainMenu_ = 3;
	const int _PlayCampaign_ = 4;
	const int _CreateNewCampaign_ = 5;
	const int _EditExistingCampaign_ = 6;
	const int _CampaignManagerMenu_ = 7;
	const int _ExitToCommandPrompt_ = 100;
}

/*!
*defines the maximums for campaign management
*/
namespace CampaignMaximums
{
	const int _MaximumCampaigns_ = 10;
	const int _MaximumMapsPerCampaign_ = 10;
	const int _MaximumNameStringSize_ = 10;
}

/*!
*some other very useful global constants used in the project
*/
namespace Others
{
	char* const _SequenceChangeButton_ = "Change Maps Sequence";
	const int _MaximumDefaultMaps_ = 10;
	char* const _DefaultMapSelectionAbort_ = "No Default Map Selected";
	char* const _DefaultMapSelectionButton_ = "Choose map from   default  List";
}