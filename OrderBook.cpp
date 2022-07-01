#include "OrderBook.h"
#include "CSVReader.h"
#include <iostream>
#include <map>
#include <string>
#include <algorithm>


OrderBook::OrderBook(std::string filename) {
	orders = CSVReader::readCSV(filename);

}


std::vector<std::string> OrderBook::getKnownProducts() { // Loops through the dataset and maps true value to the products
	std::vector<std::string> products;					 // Stores all the available products into a vector of strings and is returned
	std::map<std::string, bool> prodMap;

	for (OrderBookEntry& e : orders) {
		prodMap[e.product] = true;
	}

	for (auto const& e : prodMap) {
		products.push_back(e.first);
	}

	return products;
}


std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, std::string product, std::string timestamp) {
	std::vector<OrderBookEntry> orders_sub;
	for (OrderBookEntry& e : orders) {
		if (e.orderType == type &&
			e.product == product &&
			e.timestamp == timestamp) {
			orders_sub.push_back(e);
		}
	}
	return orders_sub;
}

double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders) {
	if (orders.size() == 0) { //if entries for product is empty, print out line
		std::cout << "This product has no entries" << std::endl;
		return 0;
	}
	else {
		double max = orders[0].price;
		for (OrderBookEntry e : orders) {
			if (e.price > max)max = e.price;
		}
		return max;
	}

}

double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders) {
	if (orders.size() == 0) { //if entries for product is empty, print out line
		std::cout << "This product has no entries" << std::endl;
		return 0;
	}
	else {
		double min = orders[0].price;
		for (OrderBookEntry e : orders) {
			if (e.price < min)min = e.price;
		}
		return min;
	}

}

double OrderBook::getAvgPrice(std::vector<OrderBookEntry>& orders) {
	double sum = 0;
	double avg = 0;
	for (OrderBookEntry e : orders) {
		sum = sum + e.price;
		avg = sum / orders.size();
	}

	return avg;
}


std::string OrderBook::getEarliestTime() {
	return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp) {
	std::string next_timestamp = "";
	for (OrderBookEntry& e : orders) {
		if (e.timestamp > timestamp) {
			next_timestamp = e.timestamp;
			break;
		}
	}
	if (next_timestamp == "") {

		next_timestamp = orders[0].timestamp;
	}
	return next_timestamp;
}

std::string OrderBook::getPrevTime(std::string timestamp) {
	std::string prev_timestamp = "";
	int index = 0;
	for (OrderBookEntry& e : orders) {
		if (e.timestamp == timestamp && timestamp != orders[0].timestamp) { // matches current timestep to dataset's timestep & not the first timestep
			prev_timestamp = orders[index-1].timestamp; // minus 1 to the index to get the previous timestep
			break;
		} else {
			index++;
		}
	}

	if (prev_timestamp == "") {

		prev_timestamp = orders[0].timestamp;
	}
	return prev_timestamp;
}



