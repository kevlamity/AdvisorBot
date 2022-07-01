#pragma once
#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"

class AdvisorMain {

	public:
		AdvisorMain();
		/** Call this to start the sim*/
		void init();
		static std::vector<std::string> userOptionTokenise(std::string userOption);
		
	private:
		void printMenu();
		void printHelp(std::string userOption);
		void printProducts();
		void printMinMax(std::string userOption);
		void printAvg(std::string userOption);
		void printPredict(std::string userOption);
		void gotoNextTimeFrame(std::string userOption);
		void gotoPrevTimeFrame();
		std::string getUserOption();
		void processUserOption(std::string userOption);
		void printLiquidity(std::string userOption);
		unsigned int timeStepsTaken; 

		std::string currentTime;
	
		OrderBook orderBook{"20200601.csv"};

};

