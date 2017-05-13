#include<iostream>
#include <fstream>
#include<string>
#include<sstream>
#include<vector>
#include <map>

using namespace std;

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
		bool found =false; 
		for(int i=0; i<positions.size(); i++)
		{
			// Search for buy/sell pair
			if( (positions.at(i) ).getStockCode() == stockCode && ( (positions.at(i) ).getType() * type) == -1)
			{
				//	buy/sell pair needs to be realized, should be CAREFULLY handled here
				found == true;
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
		}
		if(found == false) 
		{
			positions.push_back(newOrder);
		} else {
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
}
double trader::getProfit()
{
	return realizedProfit;
}

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

/*	=============================	Utilities for IO	=====================================	*/

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

void write2TSV(map<string, trader*> &result, string outPath)
{
	ofstream fileWriter(outPath);
	for(map<string, trader*>::iterator p = result.begin(); p != result.end(); ++p)
	{
		fileWriter << p->first << "\t" << (p->second)->getProfit() << endl;
	}
	fileWriter.close();
}

/*	=====================================	test case	======================================	*/
int main()
{
	//	Get access to the input .tsv file
	vector<string> data;
	string filePath = "../in1.tsv";
	readTSV(filePath, data);
	/*	Just for testing
	cout << "What in " << filePath << endl;
	for(int i=0; i<data.size(); i++) {
		cout << data.at(i) << endl;
	}
	*/
	
	//	parse data
	int size = data.size();
	//vector<int> orderId(size - 1, 0);
	vector<string> traderID(size - 1, "");
	vector<string> stockCode(size - 1, "");
	vector<int> quantity(size - 1, 0);
	vector<double> price(size -1 , 0.0);
	vector<string> tradeType(size -1 , "");
	vector<double> fee(size -1 , 0.0); 
	parseTSVLine(data, traderID, stockCode, quantity, price, tradeType, fee);
	/*	Just for testing
	cout << "After parsing:  " << endl;
	for(int i=0; i<size-1; i++) {
		cout << traderID.at(i) << "\t" << stockCode.at(i) << "\t" << quantity.at(i) << "\t" << price.at(i) << "\t" << tradeType.at(i) << "\t" << fee.at(i) << endl;
	}
	*/
	
	//	proceed the algorithm
	map<string, trader*> book;
	map<string, trader*>::iterator it;
	for(int i=0; i<size-1; i++)
	{
		string tid = traderID.at(i);
		it = book.find(tid);
		if(it != book.end())	//	exsiting
		{
			trader* e = it->second;
			e->updatePositions(tradeEntry(stockCode.at(i), quantity.at(i), price.at(i), tradeType.at(i) ), fee.at(i) );
		}
		else {
			//	insert a new trader if not exsiting
			trader* n = new trader(tid);
			n->updatePositions(tradeEntry(stockCode.at(i), quantity.at(i), price.at(i), tradeType.at(i) ), fee.at(i) );
			book.insert(pair<string, trader*>(tid, n) );
		}
	}
	/*	test output
	for(map<string, trader*>::iterator p = book.begin(); p != book.end(); ++p)
	{
		cout << p->first << "\t" << (p->second)->getProfit() << endl;
	}
	*/
	
	//	output to file
	string outPath = "../out1.tsv";
	write2TSV(book, outPath);
	
	return 0;
}
	

