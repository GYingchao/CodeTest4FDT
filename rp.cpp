#include<iostream>
#include <fstream>
#include<string>
#include<sstream>
#include<vector>

using namespace std;

int readTSV(string filePath, vector<string> &data) 
{
	ifstream fileReader(filePath);
	if (!fileReader) 
	{
		cerr << "unable to load file " << filePath << endl;
		return -1;
		
	}
	else {	
		string line;
		while(getline(fileReader, line))
		{
			data.push_back(line);
		}
	}
	fileReader.close();
	return 0;
}

void parseTSVLine(vector<string> &data, vector<string> &tr, vector<string> &st, vector<int> &qu, vector<double> &pr, vector<string> &trT, vector<double> &fe)
{
	//	Handle tab separated string line by line
	int temp;
	for(int i=0; i<data.size()-1; i++)
	{
		istringstream parser(data.at(i + 1));
		parser >> temp >> tr.at(i) >> st.at(i) >> qu.at(i) >> pr.at(i) >> trT.at(i) >> fe.at(i);
	}
	return; 
}

//	Trade Type definition
enum TradeType
{
	BUY = -1,
	SELL = 1,
	REALIZED = 0
};

//	Trade Entry definition
class tradeEntry
{
	private:
		//string traderID;
		string stockCode;
		int quantity;
		double price;
		TradeType tradeType;
		//double fee;
		bool realized;
		
	public:
		tradeEntry(string st, int qu, double pr, string tt);
		//~tradeEntry();
		string getStockCode();
		int getQuantity();
		void setQuantity(int v);
		double getPrice();
		TradeType getType();
		//double getFee();
		bool isRealized();
		void setRealized(bool r);
};

//	Trader class definition
class trader
{
	private:
		string traderID;
		vector<tradeEntry> positions;
		double realizedProfit;
		
	public:
		trader(string ID);
		//~trader();
		void updatePositions(tradeEntry newOrder, double fee);
		double getProfit();
};

//	class tradeEntry implementation	===============================================================
tradeEntry::tradeEntry(string st, int qu, double pr, string tt)
{
	stockCode = st;
	quantity = qu;
	price = pr;
	if(tt == "Buy") 
	{
		tradeType = BUY;
	}
	else if(tt == "Sell") {
		tradeType = SELL;
	}
	else {
		cerr << "Error ocurred in instancing tradeEntry£ºCannot decide trade type!" << endl;
		return;
	}
	//fee = fe;
	realized = false;
}
string tradeEntry::getStockCode() {
	return stockCode;
}
int tradeEntry::getQuantity()
{
	return quantity;
}
void tradeEntry::setQuantity(int v)
{
	quantity = v;
} 
double tradeEntry::getPrice()
{
	return price;
}
TradeType tradeEntry::getType()
{
	return tradeType;
}
/*
double tradeEntry::getFee()
{
	return fee;
}
*/
bool tradeEntry::isRealized()
{
	return realized;
}
void tradeEntry::setRealized(bool r)
{
	realized = r;
}

// class trader implementation	===================================================================
trader::trader(string ID)
{
	traderID = ID;
	if(!positions.empty()) 
	{
		positions.clear();
	}
	realizedProfit = 0.0;
}
void trader::updatePositions(tradeEntry newOrder, double fee) 
{
	realizedProfit -= fee;
	if(positions.empty()) 
	{
		positions.push_back(newOrder);
	}
	else {
		string stockCode = newOrder.getStockCode();
		TradeType type = newOrder.getType();
		for(int i=0; i<positions.size(); i++)
		{
			// Search for buy/sell pair
			if( (positions.at(i) ).getStockCode() == stockCode && ( (positions.at(i) ).getType() * type) == -1)
			{
				//	buy/sell pair needs to be realized, should be CAREFULLY handled here
				int resQ = (positions.at(i) ).getQuantity() - newOrder.getQuantity();
				if(resQ > 0)
				{
					//	newOrder is wholly realized by entry i
					(positions.at(i) ).setQuantity(resQ);
					realizedProfit += newOrder.getQuantity() * (newOrder.getType()*newOrder.getPrice() + (positions.at(i) ).getType()* (positions.at(i) ).getPrice() );
					break;	//	No more need to search
					
				} else if(resQ < 0) {
					//	entry i is wholly realized by newOrder
					newOrder.setQuantity(resQ);
					realizedProfit += (positions.at(i) ).getQuantity() * (newOrder.getType()*newOrder.getPrice() + (positions.at(i) ).getType()* (positions.at(i) ).getPrice() );
					(positions.at(i) ).setRealized(true);
					continue;
	
				} else {
					// Just equal and balanced
					realizedProfit += (positions.at(i) ).getQuantity() * (newOrder.getType()*newOrder.getPrice() + (positions.at(i) ).getType()* (positions.at(i) ).getPrice() );
					(positions.at(i) ).setRealized(true);
					break;
				}
			}
			else {
				//	No pair found, just append
				positions.push_back(newOrder);
			}
		}
		//	Update positions in case some entry is realized
		 for(vector<tradeEntry>::iterator it = positions.begin() ; it != positions.end(); ++it)
		{
			if( (*it).isRealized() )
			{
				positions.erase(it);
			}
		}
	}
}

int main()
{
	vector<string> data;
	//	Get access to the input .tsv file
	string filePath = "../in1.tsv";
	readTSV(filePath, data);
	/*	Just for testing
	cout << "What in " << filePath << endl;
	for(int i=0; i<data.size(); i++) {
		cout << data.at(i) << endl;
	}
	*/
	
	int size = data.size();
	//vector<int> orderId(size - 1, 0);
	vector<string> traderID(size - 1, "");
	vector<string> stockCode(size - 1, "");
	vector<int> quantity(size - 1, 0);
	vector<double> price(size -1 , 0.0);
	vector<string> tradeType(size -1 , "");
	vector<double> fee(size -1 , 0.0); 
	parseTSVLine(data, traderID, stockCode, quantity, price, tradeType, fee);
	
	///*	Just for testing
	cout << "After parsing:  " << endl;
	for(int i=0; i<size-1; i++) {
		cout << traderID.at(i) << "\t" << stockCode.at(i) << "\t" << quantity.at(i) << "\t" << price.at(i) << "\t" << tradeType.at(i) << "\t" << fee.at(i) << endl;
	}
	//*/
	
	return 0;
	
}
	

