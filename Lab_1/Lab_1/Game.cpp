#include "Game.h"
#include <algorithm>
#include <iostream>
#include "Strings.h"
#include <string>
#include <fstream>


using namespace messages;

std::random_device rd;
std::mt19937 gen(rd());

GameData::GameData(int year, int citizens, int weath, int lands)
	: year(year), citizens(citizens), weath(weath), lands(lands)
{
}

GameData::GameData()
{
}

std::string GameData::ToString()
{
	std::string gameData = std::to_string(year) + " " 
		+ std::to_string(citizens) + " "
		+ std::to_string(weath) + " "
		+ std::to_string(lands) + " "
		+ std::to_string(acrePrice) + " "
		+ std::to_string(deaths) + " ";
	return gameData;
}

std::string TurnData::ToString()
{
	std::string turnData = std::to_string(turnDeaths) + " "
		+ std::to_string(newCitizens) + " "
		+ std::to_string(isPlague) + " "
		+ std::to_string(weathIncome) + " "
		+ std::to_string(weathPerAcre) + " "
		+ std::to_string(eatenByRats) + " "
		+ std::to_string(isGameOver) + " ";
	return turnData;
}

Game::Game()
{
}

void Game::StartGame()
{
	std::cout << messages::loadGame;
	int input = UserInput<bool>(false);

	if(!input)
	{
		gameState = GameData();
		LoadGame();
		GameCycleHandle();
	}
	else
	{
		gameState = GameData();
		GameCycleHandle();
	}
}

bool Game::SaveGame()
{
	std::ofstream os (savePath,std::ios::out | std::ios::trunc);
	if(!os.is_open())
	{
		std::cout << saveFail;
		os.close();
		return false;
	}

	os << gameState.ToString();
	os << turnData.ToString();

	if(os.fail())
	{
		std::cout << saveFail;
		os.close();
		return false;
	}

	std::cout << saveSucc;
	os.close();
	return true;

}
	
bool Game::LoadGame()
{
	std::ifstream is (savePath);
	if(!is.is_open())
	{
		std::cout << loadFail;
		is.close();
		return false;
	}

	is >> gameState.year >> gameState.citizens >> gameState.weath >> gameState.lands 
	>> gameState.acrePrice >> gameState.deaths >> 
	turnData.turnDeaths >> turnData.newCitizens >> turnData.isPlague >> turnData.weathIncome
	>> turnData.weathPerAcre >> turnData.eatenByRats >> turnData.isGameOver;

	std::cout << loadSucc;
	return true;
}

void Game::GameCycleHandle()
{
	while (gameState.year <= 10)
	{
		gameState.acrePrice = RandRange(17, 26);
		TurnInfo();

		std::cout << saveGame;
		if (UserInput<bool>(false))
		{
			SaveGame();
		}

		turnData = TurnData();

		int toBuy = 0;
		int toSell = 0;
		int toEat = 0;
		int toSow = 0;

		int weathTemp = gameState.weath;
		int landsTemp = gameState.lands;

		while (!UserTurnInputHandle(toBuy, toSell, toEat, toSow, weathTemp, landsTemp))
		{
			toBuy = 0;
			toSell = 0;
			toEat = 0;
			toSow = 0;
			weathTemp = gameState.weath;
			landsTemp = gameState.lands;
		};

		SowHandle(weathTemp, toSow);
		RatsHandle(weathTemp);
		HungerHandle(weathTemp, toEat);
		if (turnData.isGameOver)
		{
			return;
		}
		CitizensIncomeHandle();
		PlagueHandle();

		gameState.weath = weathTemp;
		gameState.lands = landsTemp;

		++gameState.year;
	}

	GameResults();
	return;
}

void Game::SowHandle(int& weathTemp, int toSow)
{
	turnData.weathPerAcre = RandRange(1, 6);
	turnData.weathIncome = toSow * turnData.weathPerAcre;
	weathTemp += turnData.weathIncome;
}

void Game::RatsHandle(int& weathTemp)
{
	turnData.eatenByRats = static_cast<int>(std::ceil(weathTemp * RandRange(.01f, .07f)));
	weathTemp -= turnData.eatenByRats;
}

void Game::HungerHandle(int& weathTemp, int toEat)
{
	if (toEat - gameState.citizens * kCitizenMaintenance < 0)
	{
		int hungeryCitizens = -static_cast<int>(std::ceil(toEat / kCitizenMaintenance - gameState.citizens));
		const float deathRate = static_cast<float>(hungeryCitizens) / static_cast<float>(gameState.citizens);
		if (deathRate > 0.45f)
		{
			GameOver();
			return;
		}
		gameState.citizens -= hungeryCitizens;
		gameState.deaths += hungeryCitizens;
		turnData.turnDeaths = hungeryCitizens;
	}
}

void Game::CitizensIncomeHandle()
{
	int newCitizens = std::clamp(turnData.turnDeaths / 2 + (5 - turnData.weathPerAcre) * gameState.weath / 600 + 1, 0, 50);
	turnData.newCitizens = newCitizens;
	gameState.citizens += newCitizens;
}

void Game::PlagueHandle()
{
	if (RandRange(0.f, 1.f) <= kPlagueChance)
	{
		gameState.deaths += gameState.citizens / 2;
		gameState.citizens /= 2;
		turnData.isPlague = true;
	}
	else
	{
		turnData.isPlague = false;
	}
}


bool Game::UserTurnInputHandle(int& toBuy, int& toSell, int& toEat, int& toSow, int& weathAfterInput, int& landsAfterInput)
{
	std::cout << wish;

	std::cout << buy << std::endl;
	toBuy = UserInput<int>(true, 0, INT_MAX);
	if (weathAfterInput < toBuy * gameState.acrePrice)
	{
		PrintWarningMessage();
		return false;
	}
	else
	{
		weathAfterInput -= toBuy * gameState.acrePrice;
		landsAfterInput += toBuy;
	}

	std::cout << sell << std::endl;
	toSell = UserInput<int>(true, 0, INT_MAX);
	if (landsAfterInput < toSell)
	{
		PrintWarningMessage();
		return false;
	}
	else
	{
		weathAfterInput += toSell * gameState.acrePrice;
		landsAfterInput -= toSell;
	}

	std::cout << eat << std::endl;
	toEat = UserInput<int>(true, 0, INT_MAX);
	if(weathAfterInput < toEat)
	{
		PrintWarningMessage();
		return false;
	}
	else
	{
		weathAfterInput -= toEat;
	}

	std::cout << sow << std::endl;
	toSow = UserInput<int>(true, 0, INT_MAX);
	if (weathAfterInput < static_cast<int>(toSow * kSowMaintenance) || gameState.citizens * kCitizenProductivity < toSow || toSow > landsAfterInput)
	{
		PrintWarningMessage();
		return false;
	}
	else
	{
		weathAfterInput -= static_cast<int>(toSow * kSowMaintenance);
	}

	return true;
}

void Game::PrintWarningMessage()
{
	std::cout << spare1 << gameState.citizens << spare2 << gameState.weath 
	<< spare3 << gameState.lands << spare4 << std::endl;
}

void Game::TurnInfo()
{
	std::cout << myLord << year1 << gameState.year << year2;

	if (turnData.turnDeaths > 0 || turnData.newCitizens > 0)
	{
		std::cout << "\t" << turnData.turnDeaths << citizens1 << turnData.newCitizens << citizens2 << std::endl;
	}
	if(turnData.isPlague)
	{
		std::cout << "\t" << plague << std::endl;
	}

	std::cout << "\t" << weathIncome1 << turnData.weathIncome << weathIncome2 << turnData.weathPerAcre << weathIncome3;
	std::cout << "\t" << weathResidue1 << turnData.eatenByRats << weathResidue2 << gameState.weath << weathResidue3;
	std::cout << "\t" << city1 << gameState.lands << city2;
	std::cout << "\t" << acrePrice1 << gameState.acrePrice << acrePrice2;
	std::cout << std::endl;
}


void Game::GameResults()
{
	float P = gameState.deaths / (gameState.deaths + gameState.citizens);
	float L = gameState.lands / gameState.citizens;

	if( P > 0.33 && L < 7)
	std::cout << badEnding << std::endl;
	else if(P > 0.1 && L < 9)
	std::cout << midEnding << std::endl;
	else if(P > 0.03 && L < 01)
	std::cout << goodEnding << std::endl;
	else 
	std::cout << excellentEnding << std::endl;

	StartGame();
}

void Game::GameOver()
{
	std::cout << badEnding << std::endl;
	StartGame();
}

#pragma region Utils
int Game::RandRange(int min, int max)
{
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}

float Game::RandRange(float min, float max)
{
	std::uniform_real_distribution<> dist(min, max);
	return dist(gen);
}
template<typename T>
T Game::UserInput(bool hasMaxAndMin, T min /*= std::numeric_limits<T>::min()*/, T max /*= std::numeric_limits<T>::max()*/)
{
	T input;
	bool inLimits = !hasMaxAndMin;

	std::cin >> input;
	if (!std::cin || !inLimits)
	{
		inLimits = (input >= min) && (input <= max);

		while (!std::cin || !inLimits)
		{
			std::cerr << "Incorrect input data! Try to write value between " << min << " and " << max << "\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin >> input;

			if (!hasMaxAndMin) continue;

			inLimits = (input >= min) && (input <= max);
		}
	}

	return input;
}
#pragma endregion Utils

