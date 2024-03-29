#include "fileIOmanager.h"

//!static singleton instance
SingletonInputOutputManager* SingletonInputOutputManager::_IOManagerInstance = nullptr;

//!get instance of the singleton class, the instance is only created if it has not been created
SingletonInputOutputManager* SingletonInputOutputManager::getInstance()
{
	if(SingletonInputOutputManager::_IOManagerInstance == nullptr)
		SingletonInputOutputManager::_IOManagerInstance = new SingletonInputOutputManager();
	return SingletonInputOutputManager::_IOManagerInstance;
}

//!default constructor
SingletonInputOutputManager::SingletonInputOutputManager()
{
}

//!this functions reads a file word by word based on the path that is passed
std::vector<std::string> SingletonInputOutputManager::readFileWordByWord(std::string path)
{
		
	std::ifstream inputfile;
	inputfile.open(path);
	if(inputfile.is_open() == false)
	{
		std::cout<<"Could not open file, "<<path<<" exiting program."<<std::endl;
		system("pause");
		exit(-1);
	}
	std::vector<std::string> lines;
	std::string currentinput;
	int counter = 0;
	while(inputfile >> currentinput)
	{
		lines.push_back("X");
		lines[counter] = currentinput;
		counter++;
	}
	inputfile.close();
	return lines;
}

//!this function reads a file line by line based on the path that is passed
std::vector<std::string> SingletonInputOutputManager::readFileLineByLine(std::string path)
{
	std::ifstream inputfile;
	inputfile.open(path);
	if(inputfile.is_open() == false)
	{
		std::cout<<"Could not open file, "<<path<<" exiting program."<<std::endl;
		system("pause");
		exit(-1);
	}
	std::vector<std::string> lines;
	std::string currentinput;
	int counter = 0;
	while(std::getline(inputfile, currentinput))
	{
		lines.push_back("X");
		lines[counter] = currentinput;
		counter++;
	}
	inputfile.close();
	return lines;
}

//!this funtion writes a string vector to a file based on the path that is passed
void SingletonInputOutputManager::writeToFile(std::string path, std::vector<std::string> lines)
{
	std::ofstream write;
	write.open(path);
	for(int x=0; x< lines.size(); x++)
		write<<lines[x]<<std::endl;
	write.close();
}

//!this functions splits a string delimited by space and returns a vector of strings
std::vector<std::string> SingletonInputOutputManager::splitString(std::string toBeSplit)
{
	std::vector<std::string> final;
	std::string temp = "";

	for(int x=0; x<=toBeSplit.length(); x++)
	{
		if(x == toBeSplit.length())
		{
			final.push_back(temp);
			temp = "";
		}
		else if((toBeSplit.at(x) == ' '))
		{
			final.push_back(temp);
			temp = "";
		}
		else
		{
			temp = temp + toBeSplit.at(x);
		}
	}
	return final;
}



//!load the list of all saved campaign names and returns the vector
std::vector<std::string> SingletonInputOutputManager::loadAllCampaign()
{
	return readFileLineByLine(SingletonFilePathAndFolderManager::getInstance()->_path_to_savedCampaignsList);
}

//!loads the all map names and maps for a specific campaign
std::vector<std::string> SingletonInputOutputManager::loadONECampaign(std::string NAME)
{
	return readFileLineByLine(SingletonFilePathAndFolderManager::getInstance()->_path_to_campaigns_folder + NAME + ".txt");
}

//!destructor destroys the singleton object
SingletonInputOutputManager::~SingletonInputOutputManager()
{
	delete SingletonInputOutputManager::_IOManagerInstance;
	SingletonInputOutputManager::_IOManagerInstance = nullptr;
}