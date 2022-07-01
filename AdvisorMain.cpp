#include "AdvisorMain.h"
#include "OrderBookEntry.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "CSVReader.h"

AdvisorMain::AdvisorMain() {

}

void AdvisorMain::init() { // initializes the program
	std::string input; 
	currentTime = orderBook.getEarliestTime(); // user starts at the first timestamp in the dataset
	printMenu();

	while (true) { // constant while loop while program is running, awaiting the user's input
		input = getUserOption();
		processUserOption(input);
	}
}

void AdvisorMain::printMenu() { // prints introduction to the advisorbot when the program first initializes
	std::cout << "Welcome to Advisorbot. Advisorbot is a command line program that can carry out various" << std::endl;
	std::cout << "tasks to help a cryptocurrency investor analyse the data available on an exchange." << std::endl;
	std::cout << "======================================================================================" << std::endl;
	std::cout << "To begin, please enter a command, or type 'help' for a list of commands" << std::endl;
}

void AdvisorMain::printHelp(std::string userOption) { //print help function, when prints all available commands, and also prints each command's use and purpose
	if (userOption == "help") {
		std::cout << "The available commands are: help, help <cmd>, prod, min, max, avg, predict, liquidity, time, step <no>" << std::endl;
		std::cout << "======================================================================================================" << std::endl;
	} else if (userOption == "help prod") {
		std::cout << "Command: prod" << std::endl;
		std::cout << "Purpose: List all available products." << std::endl;
		std::cout << "Example: user> help prod" << std::endl;
		std::cout << "         advisorbot> ETH/BTC, DOGE/BTC etc." << std::endl;
	} else if (userOption == "help min") {
		std::cout << "Command: min product bid/ask" << std::endl;
		std::cout << "Purpose: Find the minimum bid or ask for product in current time step" << std::endl;
		std::cout << "Example: user> min ETH/BTC ask" << std::endl;
		std::cout << "         advisorbot> The min ask for ETH/BTC is 0.0248261" << std::endl;
	} else if (userOption == "help max") {
		std::cout << "Command: max product bid/ask" << std::endl;
		std::cout << "Purpose: Find the maximum bid or ask for product in current time step" << std::endl;
		std::cout << "Example: user> max ETH/BTC ask" << std::endl;
		std::cout << "         advisorbot> The max ask for ETH/BTC is 0.0251581" << std::endl;
	} else if (userOption == "help avg") {
		std::cout << "Command: avg product ask/bid timesteps" << std::endl;
		std::cout << "Purpose: Compute the average ask or bid for the sent product over the sent number of time steps" << std::endl;
		std::cout << "Example: user> avg ETH/BTC ask 10" << std::endl;
		std::cout << "         advisorbot> The average ETH/BTC ask price over the last 10 timesteps was 0.0249612" << std::endl;
	} else if (userOption == "help predict") {
		std::cout << "Command: predict max/min product ask/bid" << std::endl;
		std::cout << "Purpose: Predict the max or min ask or bid for the sent product for the next time step, requires user to be at minimum the 5th timestamp" << std::endl;
		std::cout << "Example: user> predict max BTC/USDT bid" << std::endl;
		std::cout << "         advisorbot> The predicted max ask price for ETH/BTC is 0.0222814 for the next time frame" << std::endl;
	} else if (userOption == "help liquidity") {
		std::cout << "Command: liquidity product" << std::endl;
		std::cout << "Purpose: Averages the liquidity of product for the past 10 timesteps, uses bid-ask spread as the measure" << std::endl;
		std::cout << "Example: user> liquidity DOGE/BTC" << std::endl;
		std::cout << "         advisorbot> The average liquidity of DOGE/BTC for the previous 10 steps is 4.07%" << std::endl;
	} else if (userOption == "help time") {
		std::cout << "Command: time" << std::endl;
		std::cout << "Purpose: State current time in dataset, i.e. which timeframe are we looking at" << std::endl;
		std::cout << "Example: user> time" << std::endl;
		std::cout << "         advisorbot> Current time is 2020/03/17 17:01:24, timestamp: 5" << std::endl;
	} else if (userOption == "help step") {
		std::cout << "Command: step <no.>" << std::endl;
		std::cout << "Purpose: Moves to the next specified amount of timesteps, defaults to 1" << std::endl;
		std::cout << "Example: user> step" << std::endl;
		std::cout << "         advisorbot> Now at 2020/03/17 17:01:30" << std::endl;
	} else { // if none of the above was matched, returns invalid input statement and tells user to type help for all valid commands
		std::cout << "Invalid input. Type 'help' to display all valid commands" << std::endl;
	}
}

void AdvisorMain::printProducts() { // 'prod' command, prints out all available products in the dataset
	bool first = true;
	std::cout << "Known products: " << "";
	for (std::string const& p : orderBook.getKnownProducts()) { // Gets all known products from the orderbook using getKnownProducts function
		if (!first) {
			std::cout << "," << p << ""; // separates the products with commas to be printed
		} else {
			std::cout << p << "";
			first = false;
		}
	}
	std::cout << std::endl;
}

void AdvisorMain::printMinMax(std::string userOption) { // Minimum / Maximum command which returns the min/max as/bid of the product the user has input 

	std::vector<std::string> userOptionLine = userOptionTokenise(userOption);

	int valid = 0;
	for (std::string const& p : orderBook.getKnownProducts()) { // counts the number of products in the current timeframe to be used for validation
		valid++;
	};

	if (userOptionLine.size() != 3) { // user input must be a line which can be separated into 3 individual strings
		std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
	} else {

		std::string type = userOptionLine[2];
		std::string product = userOptionLine[1];

		if (product.rfind("DOGE", 0) == 0) { // Changing precision when printing DOGE products to show more accurately their value rather than scientific notations
			std::cout.precision(10);
			std::cout << std::fixed;
		}
		else { // If user is analysing other products, change back the precision to the default
			std::cout.precision(-1);
			std::cout << std::defaultfloat;
		}

		for (std::string const& p : orderBook.getKnownProducts()) { // loops through the known products to match whichever product the user has input
			if ((type == "bid" || type == "ask") && product == p) { // validates if their input contains bid/ask and also matches the product to their input 
				std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime); //gets the order book entry depending on the bid/ask which the user input
				if (userOptionLine[0] == "min") { // matches if the user wanted to search for min
					std::cout << "The min " << type << " for " << product << " is " << OrderBook::getLowPrice(entries) << std::endl;
				}
				else if (userOptionLine[0] == "max") {// matches if the user wanted to search for max
					std::cout << "The max " << type << " for " << product << " is " << OrderBook::getHighPrice(entries) << std::endl;
				}
			}
			else {
				valid--; // How many ever products is in the timestamp, the for loop should match 1 out of x products and have valid equal to 1
			}
		}
		if (valid != 1) { // If 1 line of data is not matched in the for loop, the input is not valid and prints wrong line input
			std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
		}
	}

}

void AdvisorMain::printAvg(std::string userOption) { // Average command. Averages the ask/bid of a product for the x past numbers of timesteps

	std::vector<std::string> userOptionLine = userOptionTokenise(userOption);

	// variables needed for the average function
	int valid = 0;
	for (std::string const& p : orderBook.getKnownProducts()) { // counts the number of products in the current timeframe to be used for validation
		valid++;
	};
	double avg = 0;
	double sum = 0;
	unsigned int timesteps; // How many past timesteps (including current) the user wants to average
	unsigned int userTimeStamp; // Which timestamp the user is current at
	bool first = true;

	if (userOptionLine.size() != 4) { // user input must be a line which can be separated into 4 individual strings from a vector
		std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
	}
	else {
		std::string type = userOptionLine[2];
		std::string product = userOptionLine[1];

		try {
			timesteps = std::stoi(userOptionLine[3]); // convert user's input from string to int
		} catch (const std::exception& e) {
			std::cout << "Please input a number for your timesteps" << std::endl;
			timesteps = 0;
			return; // If user inputs non int for timestep value, return without executing any more code
		}

		try {
			if (type != "ask" && type != "bid") { // validation to check if user inputed a valid type
				std::cout << "Wrong line input, please check order of commands" << std::endl;
				return;
			}
		} catch (const std::exception& e) {
			//
		}

		if (product.rfind("DOGE", 0) == 0) { // Changing precision when printing DOGE products to show more accurately their value rather than scientific notations
			std::cout.precision(10);
			std::cout << std::fixed;
		}
		else { // If user is analysing other products, change back the precision to the default
			std::cout.precision(-1);
			std::cout << std::defaultfloat;
		}

		userTimeStamp = timeStepsTaken + 1; // timestepstaken is how many time steps the user taken which starts at 0, usertimestamp starts at 1
		
		if (timesteps == userTimeStamp && timesteps > 1) { // if the user wants to analyse all the timestamps they have passed through, minus
			timesteps--;								   // timesteps to go back by 1 so that they dont go 1 further back
		}

		if (timesteps > userTimeStamp) { // validation if the user inputs more timesteps to analyse than their current timestamp number
			std::cout << "You entered a greater number of timesteps to your current timestamp, please enter a timestep equal or less than your timestamp" << std::endl;
			return;
		} else if (timesteps == 0) {
			std::cout << "Please enter a number greater than 0" << std::endl;
			return;
		}

		for (std::string const& p : orderBook.getKnownProducts()) {

			if ((type == "bid" || type == "ask") && product == p && userOptionLine[0] == "avg" && userTimeStamp >= timesteps) {
				if (userTimeStamp == 1) { // If the user has not taken any timesteps; they are on the first time stamp, do not go back any timestamps
					std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
					sum += OrderBook::getAvgPrice(entries);
				}
				else {
					for (int i = 0; i < timesteps; i++) {

						std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime); 
						sum += OrderBook::getAvgPrice(entries); // sums the average price each loop for current time

						gotoPrevTimeFrame(); // go back 1 timestamp

						// For the last iteration of the loop, add to the sum of average prices but do no go back 1 timestamp
						if (i == (timesteps - 1) && userTimeStamp == stoi(userOptionLine[3])) { 
							std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
							sum += OrderBook::getAvgPrice(entries);
						}
					}
				}

				avg = sum / stoi(userOptionLine[3]); // the average past x timestamps is the sum of entries average divided by timesteps

				std::cout << "The average " << product << " " << type << " price over the last " << stoi(userOptionLine[3]) << " timesteps was " << avg << std::endl;

			} else {
				valid--;
			}

		}
		if (valid != 1) {
			std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
		}
		else {
			if (timesteps == 1) { // if user only wants to avg current timestamp values, moves user forward only 1 timestamp as above function they are sent back once
				for (int i = 0; i < 1; i++) {
					if (userTimeStamp != 1) {
						gotoNextTimeFrame("return"); // returns the user to their original timestamp
					}
				}
			}
			else {
				// this for loop returns the user's time step to wherever they stepped to
				for (int i = 0; i < timesteps; i++) {
					if (userTimeStamp != 1) { // if user has not taken any timesteps, they can only avg the first timestep
						gotoNextTimeFrame("return");
					}
				}
			}
		}
		//std::cout << "Current time stamp is " << currentTime << " Timestamp: " << timeStepsTaken + 1 << std::endl;
	}

}


void AdvisorMain::printPredict(std::string userOption) { // Predict function uses 4 steps exponential moving average to predict the next min/max ask/bid for the product

	std::vector<std::string> userOptionLine = userOptionTokenise(userOption);

	//variables needed for the function
	int valid = 0;
	for (std::string const& p : orderBook.getKnownProducts()) { // counts the number of products in the current timeframe to be used for validation
		valid++;
	};
	double sum = 0;
	double EMA, SMA, CurrentPrice;
	unsigned int timesteps = 4; // Using 4 step moving average as predictor, EMA requires the SMA of the previous step

	//predict max/min product ask/bid
	//   0	     1	     2	     3
	// EMA = Current price * (2/5) + SMA of previous * (3/5)

	if (userOptionLine.size() != 4) { // user input must be a line which can be separated into 4 individual strings
		std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
	} else if (timeStepsTaken < 4) { // Function works by using past 5 timesteps data so can only use this command on the 5th timestamp onwards
		std::cout << "Predict can only be used on the 5th timestamp onwards as it uses historical data" << std::endl;
	} else {

		std::string type = userOptionLine[3];
		std::string product = userOptionLine[2];
		std::string minmax = userOptionLine[1];

		try {  // validation for ask/bid and min/max if they are input in the correct order of commands
			if (type != "ask" && type != "bid") {
				std::cout << "Wrong line input, please check order of commands" << std::endl;
				return;
			}
			else if (minmax != "max" && minmax != "min") {
				std::cout << "Wrong line input, please check order of commands" << std::endl;
				return;
			}
		}
		catch (const std::exception& e) {
			//
		}

		if (product.rfind("DOGE", 0) == 0) { // Changing precision when printing DOGE products to show more accurately their value rather than scientific notations
			std::cout.precision(10);
			std::cout << std::fixed;
		}
		else { // If user is analysing other products, change back the precision to the default
			std::cout.precision(-1);
			std::cout << std::defaultfloat;
		}

		for (std::string const& p : orderBook.getKnownProducts()) {

			if ((type == "ask" || type == "bid") && product == p) { // matches the product
				for (int i = 0; i < timesteps; i++) {
					if (minmax == "min") { // matches if they user wants to analyse min
						if (i == 0) {
							std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
							CurrentPrice = OrderBook::getLowPrice(entries);
						}
						else {
							std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
							sum += OrderBook::getLowPrice(entries);
						}
						gotoPrevTimeFrame();
						if (i == 3) { // on i = 3 iteration, gotoPrevTimeFrame() has been executed 4 times, meaning the user has went back to the 5th time stamp
							std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
							sum += OrderBook::getLowPrice(entries); // gets the value of the 5th step value as it is required for SMA, 
						}
					}
					else if (minmax == "max") { // matches if they user wants to analyse max
						if (i == 0) {
							std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
							CurrentPrice = OrderBook::getHighPrice(entries);
						}
						else {
							std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
							sum += OrderBook::getHighPrice(entries);
						}
						gotoPrevTimeFrame();
						if (i == 3) {
							std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookEntry::stringToOrderBookType(type), p, currentTime);
							sum += OrderBook::getHighPrice(entries); 
						}
					}
				}
				SMA = sum / 4; // formula for Simple moving average, sum of 5th to 2nd timestamps divided by 4
				EMA = CurrentPrice * (2.0 / 5.0) + SMA * (3.0 / 5.0); // formula for Exponential moving average, which uses the SMA of previous step and current step price
				std::cout << "The " << minmax << " " << type << " for " << product << " might be " << EMA << " for the next timestep" << std::endl;
			} else {
				valid--;
			}
		}
		if (valid != 1) {
			std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
		}
		else {
			// this for loop returns the user's time step to wherever they stepped to
			for (int i = 0; i < 4; i++) {
				if (timeStepsTaken >= 4) { // Function will not execute if users have not taken more than 4 steps
					gotoNextTimeFrame("return");
				}

			}
		}

		//std::cout << "Current time stamp is " << currentTime << " Timestamp: " << timeStepsTaken + 1 << std::endl;
	}

}

void AdvisorMain::printLiquidity(std::string userOption) { // Liquidity function takes the product's average bid-ask spread of the 10 previous steps in %

	std::vector<std::string> userOptionLine = userOptionTokenise(userOption);

	// variables needed for liquidty function
	int valid = 0;
	for (std::string const& p : orderBook.getKnownProducts()) { // counts the number of products in the current timeframe to be used for validation
		valid++;
	};
	double sumOfLiquidity = 0;
	double avgOfLiquidity = 0;
	double BidAskSpread;
	double liquidity;
	unsigned int timesteps = 10; // Using 10 step average of liquidity%, as some day

	//liquidity product 
	//   0	       1
	// Bid ask spread = min ask – max bid
	// Liquidity % = (bid ask spread / lowest ask price)* 100

	if (userOptionLine.size() != 2) { // user input must be a line which can be separated into 2 individual strings
		std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
	}
	else if (timeStepsTaken < timesteps) { // User has to be on 11th timestamp onwards to use this function
		std::cout << "Liquidity can only be used on the 11th timestamp onwards as it uses historical data" << std::endl;
	}
	else {

		std::string product = userOptionLine[1];

		if (product.rfind("DOGE", 0) == 0) { // Changing precision when printing DOGE products to show more accurately their value rather than scientific notations
			std::cout.precision(10);
			std::cout << std::fixed;
		}
		else { // If user is analysing other products, change back the precision to the default
			std::cout.precision(-1);
			std::cout << std::defaultfloat;
		}

		for (std::string const& p : orderBook.getKnownProducts()) {
			if (product == p) { // matches user's product input to the dataset's product
				for (int i = 0; i < timesteps; i++) { 
					std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
					std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, p, currentTime);
					BidAskSpread = OrderBook::getLowPrice(askEntries) - OrderBook::getHighPrice(bidEntries);
					liquidity = (BidAskSpread / OrderBook::getLowPrice(askEntries)) * 100;
					sumOfLiquidity += liquidity;
					gotoPrevTimeFrame();
					if (i == timesteps - 1) { // on the last iteration of the loop, gets the value of the 10th day
						std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
						std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, p, currentTime);
						BidAskSpread = OrderBook::getLowPrice(askEntries) - OrderBook::getHighPrice(bidEntries);
						liquidity = (BidAskSpread / OrderBook::getLowPrice(askEntries)) * 100;
						sumOfLiquidity += liquidity;
					}
				}
				avgOfLiquidity = sumOfLiquidity / timesteps; // Formula for average of liquidity for the last 10 days
				std::cout << std::setprecision(2) << "The average liquidity of " << product << " for the previous 10 steps is " << avgOfLiquidity << "%" << std::endl;
			}
			else {
				valid--;
			}

		}

		if (valid != 1) {
			std::cout << "Wrong line input, type 'help <cmd>' for a valid command input" << std::endl;
		}
		else {

			// this for loop returns the user's time step to wherever they stepped to
			for (int i = 0; i < timesteps; i++) {
				if (timeStepsTaken >= timesteps) { // Function will not execute if users have not taken more than 10 steps
					gotoNextTimeFrame("return");
				}

			}
		}

		//std::cout << "Current time stamp is " << currentTime << " Timestamp: " << timeStepsTaken + 1 << std::endl;
	}


}



void AdvisorMain::gotoNextTimeFrame(std::string userOption) {
	std::string original = userOption;
	std::vector<std::string> userOptionLine = userOptionTokenise(userOption);

	signed int steps; // If unsigned int is used, user inputting negative number will crash the program
	if (original == "step") { // 'step' defaults to advancing 1 time step
		currentTime = orderBook.getNextTime(currentTime);
		std::cout << "Now at " << currentTime << std::endl;
		timeStepsTaken++; // adds 1 to the timestepstaken
	} else if (original == "return") { // used for returning to timestamp after calculating commands, will not add to timestepstaken
		currentTime = orderBook.getNextTime(currentTime);
		//std::cout << "Now at " << currentTime << std::endl;
	} else if (userOptionLine.size() == 2) { // 'step <no>' users can type how many steps they want to advance, and for loop will advance x numbers of step
		try {
			steps = std::stoi(userOptionLine[1]);
			if (!(steps <= 0)) {
				for (int i = 0; i < steps; i++) {
					currentTime = orderBook.getNextTime(currentTime);
					if (i == steps - 1) {
						std::cout << "Now at " << currentTime << std::endl;
					}
					timeStepsTaken++;
				}
			} else {
				std::cout << "Please enter a step greater than 0" << std::endl;
			}
		}
		catch (const std::exception& e) { // validation
			std::cout << "Invalid input. Type 'help' to display all valid commands" << std::endl;
		}
	} else { // validation
		std::cout << "Invalid input. Type 'help' to display all valid commands" << std::endl;
	}


}

void AdvisorMain::gotoPrevTimeFrame() { // Function to send user back 1 time step, for the different commands to get past timesteps data for calculation

	currentTime = orderBook.getPrevTime(currentTime); 
	//std::cout << "Now at " << currentTime << std::endl;

}

std::string AdvisorMain::getUserOption() { // Takes the user's input using cin and returns a string line
	std::string userOption;
	std::string line;
	std::getline(std::cin, line);
	try {
		userOption = std::string(line);
	} catch (const std::exception& e){
		//
	}
	return userOption;
}

std::vector<std::string> AdvisorMain::userOptionTokenise(std::string userOption) { // tokenise user's input into a vector
	std::vector<std::string> userOptionLine;
	signed int start, end;
	std::string token;
	start = userOption.find_first_not_of(" ", 0);
	do {
		end = userOption.find_first_of(" ", start);
		if (start == userOption.length() || start == end) break;
		if (end >= 0) token = userOption.substr(start, end - start);
		else token = userOption.substr(start, userOption.length() - start);
		userOptionLine.push_back(token);
		start = end + 1;
	} while (end > 0);
	return userOptionLine;
}

void AdvisorMain::processUserOption(std::string userOption) { // Processes the user's input and uses rfind to match which command the user has input

	if (userOption.rfind("help", 0) == 0) { //Print all commands and their uses
		printHelp(userOption);
	} else if (userOption == "prod") { // Displays all products
		printProducts();
	} else if (userOption.rfind("min", 0) == 0 || userOption.rfind("max", 0) == 0) { // Displays min/max ask/bid for product
		printMinMax(userOption);
	} else if (userOption.rfind("avg", 0) == 0) { // Displays avg ask/bid for product
		printAvg(userOption);
	} else if (userOption.rfind("predict", 0) == 0) { // Displays prediction for max/min product ask/bid using weighted moving avg
		printPredict(userOption);
	} else if (userOption == "time") { // Displays current time frame
		std::cout << "Current time is " << currentTime << " Timestamp: " << timeStepsTaken + 1 << std::endl;
	} else if (userOption.rfind("step", 0) == 0) { // Progresses to the next time frame
		gotoNextTimeFrame(userOption);
	} else if (userOption.rfind("liquidity", 0) == 0) {
		printLiquidity(userOption);
	} else { //If none of the valid commands are typed, invalid input and prompts user to type help
		std::cout << "Invalid input. Type 'help' to display all valid commands" << std::endl;
	}

}