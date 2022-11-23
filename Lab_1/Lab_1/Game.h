#pragma once
#include <limits>
#include <random>
#include <algorithm>

constexpr int kCitizenProductivity = 10;
constexpr int kCitizenMaintenance = 20;
constexpr double kSowMaintenance = 0.5;
constexpr float kPlagueChance = .15f;

struct GameData
{
	int year = 0;
	int citizens = 100;
	int weath = 2800;
	int lands = 1000;

	int acrePrice = 0;
	int deaths = 0;

	GameData();
	GameData(int year, int citizens, int weath, int lands);
	std::string ToString();
};

struct TurnData
{
	int turnDeaths = 0;
	int newCitizens = 0;
	bool isPlague = false;
	int weathIncome = 0;
	int weathPerAcre = 0;
	int eatenByRats = 0;
	bool isGameOver = false;

	std::string ToString();
};

class Game
{
private:
	GameData gameState;
	TurnData turnData;

public: 
	Game();
	void StartGame();

private:
	const std::string savePath = "Save.txt";

	bool SaveGame();
	bool LoadGame();

	void GameCycleHandle();
	bool UserTurnInputHandle(int& toBuy, int& toSell, int& toEat, int& toSow, int& weathAfterInput, int& landsAfterInput);
	void PrintWarningMessage();
	void TurnInfo();

	void SowHandle(int& weathTemp, int toSow);
	void RatsHandle(int& weathTemp);
	void HungerHandle(int& weathTemp, int toEat);
	void CitizensIncomeHandle();
	void PlagueHandle();

	void GameResults();
	void GameOver();

#pragma region Utils
	int RandRange(int min, int max);
	float RandRange(float min, float max);

	template<typename T>
	T UserInput(bool hasMaxAndMin, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max());
#pragma endregion Utils
	
};
