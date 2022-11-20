#pragma once

#include <iostream>

namespace messages
{
	//##############################Last Round Results########################################
	std::string loadGame = "Do you want to load game or start new? \n 0 - Load game \n 1 - Start new game \n";
	std::string saveGame = "Do you want to save game and have a rest? \n 0 - No \n 1 - Yes \n";
	
	std::string myLord = "\nMy lord, deign to tell you\n";

	std::string year1 = "In the ";
	std::string year2 = " year of your highest dominion: \n";

	std::string citizens1 = " citizens dies form starvation and ";
	std::string citizens2 = " has come to our great city \n";

	std::string plague = "plague wiped out half the population \n";

	std::string weathIncome1 = "We harvested ";
	std::string weathIncome2 = " bushels of wheat, ";
	std::string weathIncome3 = " bushels per acre \n";

	std::string weathResidue1 = "Rats destroyed ";
	std::string weathResidue2 = " bushels of wheat, leaving ";
	std::string weathResidue3 = " bushels in barns \n";

	std::string city1 = "The city now covers ";
	std::string city2 = " acres \n";

	std::string acrePrice1 = "1 acre of land is now worth ";
	std::string acrePrice2 = " bushels.\n";

	//##############################Turn commands############################################
	std::string wish = "What do you wish, lord?";
	std::string buy = "\nHow many acres of land do you command to buy? ";
	std::string sell = "\nHow many acres of land do you command to sell? ";
	std::string eat = "\nHow many bushels of wheat do you command to eat? ";
	std::string sow = "\nHow many acres of land do you command to sow? ";

	std::string spare1 = "\nOh lord, spare us! We have only ";
	std::string spare2 = " people, ";
	std::string spare3 = " bushels of wheat and ";
	std::string spare4 = " acres of land!";

	//##############################End phrases############################################
	std::string badEnding = "\nBecause of your incompetence in management, the people staged a riot, and expelled you from their city.Now you are forced to eke out a miserable existence in exile";
	std::string midEnding = "\nYou ruled with an iron hand, like Nero and Ivan the Terrible. The people have breathed a sigh of relief, and no one wants to see you as a ruler anymore";
	std::string goodEnding = "\nYou did quite well, of course, you have detractors, but many would like to see you at the head of the city again";
	std::string excellentEnding = "\nFantastic! Charlemagne, Disraeli and Jefferson couldn't have done better together";

	//##############################Save###################################################
	std::string saveSucc = "Saved successfully!\n";
	std::string saveFail = "Unable to save game\n";

	std::string loadSucc = "Loaded  successfully!\n";
	std::string loadFail = "Unable to load game\n";
}