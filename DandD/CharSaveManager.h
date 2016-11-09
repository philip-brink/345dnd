/**
*@file CharSaveManager.h
*@brief Provides access to the CharacterSaveManager class, which is used to manage Character save files.
*/
#pragma once
#ifndef CHARSAVEMANAGER_H
#define CHARSAVEMANAGER_H
#include "CharacterSaveMap.h" // Provides data structure for keeping track of save paths
#include "Fighter.h"
#include <string>
#include <iostream>

/**
*@class CharacterSaveManager
*@brief Provides static methods used to save, load and remove characters from disk.
*<p>Provides the ability to load a specific character, save a character, and remove a character that has been previously saved.
*<p>Also provides helper method to convert from CStrings to strings, which is quite useful for managing strings for CFile
*/
class CharacterSaveManager {
public:
	static bool saveCharacter(Fighter *theChar);
	static Fighter* loadCharacter();
	static void removeCharacter();
	static void removeCharacter(string name);
	static string cstrTostr(CString &cstr);
};
#endif