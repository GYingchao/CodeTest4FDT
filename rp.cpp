#include<iostream>
#include <fstream>
#include<string>
#include<sstream>
#include<vector>
#include<map>
#include<algorithm>
//#include "time.h"
//#include "memory.h"

using namespace std;

//	Trade Type definition
enum TradeType
{
	BUY = -1,
	SELL = 1,
};

//	Trade Entry definition, namely every order can be recorded as a tradeEntry
class tradeEntry
{
	private:
		string stockCode;
		int quantity;
		double price;
		TradeType tradeType;

	public:
		tradeEntry(string st, int qu, double pr, string tt);
		string getStockCode();
		int getQuantity();
		void setQuantity(int v);
		double getPrice();
		TradeType getType();
		bool isRealized();
};

//	Trader class definition, which has "on hold" stocks and realized profit
class trader
{
	private:
		string traderID;
		vector<tradeEntry> positions;
		double realizedProfit;

	public:
		trader(string ID);
		//  Key function in this program, which contains realized profit computing and maintaining the intermediate orders of the trader.
		void updatePositions(tradeEntry& newOrder, double fee);
		double getProfit();
};

/*	=============================   Class tradeEntry Implementation =====================================	*/
tradeEntry::tradeEntry(string st, int qu, double pr, string tt)
{
	stockCode = st;
	quantity = qu;
	if(quantity <= 0)   //  A new order should have positive quantity number of stocks
    {
        cerr << "Error ocurred in instancing tradeEntry ! Quantity is not positive value ! " << endl;
		return;
    }
	price = pr;
	if(tt == "Buy")
	{
		tradeType = BUY;
	}
	else if(tt == "Sell") {
		tradeType = SELL;
	}
	else {
		cerr << "Error ocurred in instancing tradeEntry ! Cannot decide trade type ! " << endl;
		return;
	}
}

string tradeEntry::getStockCode()
{
	return stockCode;
}

int tradeEntry::getQuantity()
{
	return quantity;
}

void tradeEntry::setQuantity(int v)
{
    if(v < 0)
    {
        cerr << "Error ocurred in setQuantity ! Quantity cannot be set negative value !" << endl;
		return;
    }
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

bool tradeEntry::isRealized()
{
    if(quantity == 0)   //  must be realized by some buy/sell entry pair
    {
        return true;
    } else {
        return false;
    }
}

/*	=============================   Class trader Implementation =====================================	*/
trader::trader(string ID)
{
	traderID = ID;
	if(!positions.empty())
	{
		positions.clear();
	}
	realizedProfit = 0.0;
}

void trader::updatePositions(tradeEntry& newOrder, double fee)
{
	realizedProfit -= fee;
	if(positions.empty())   //  Holding no stock
	{
		positions.push_back(newOrder);
	}
	else {  //  Search whether there is a same stock with different tradeType on hold, namely a buy/sell pair
		string stockCode = newOrder.getStockCode();
		TradeType type = newOrder.getType();
		bool found = false;
		for(int i=0; i<positions.size(); i++)
		{
		    if(positions.at(i).isRealized() ) continue;
			if( ( (positions.at(i) ).getStockCode() == stockCode) && ( (positions.at(i) ).getType() * type == -1) ) // Search for buy/sell pair
			{
				found = true;   //	buy/sell pair exists
				int resQ = (positions.at(i) ).getQuantity() - newOrder.getQuantity();   //  Handle quantity
				if(resQ > 0)
				{
					//	newOrder is wholly realized by entry i
					realizedProfit += newOrder.getQuantity() * (newOrder.getType()*newOrder.getPrice() + (positions.at(i) ).getType()* (positions.at(i) ).getPrice() );
					newOrder.setQuantity(0);
					(positions.at(i) ).setQuantity(resQ);
					break;	//	No more need to search

				} else if(resQ < 0) {
					//	entry i is wholly realized by newOrder
					realizedProfit += (positions.at(i) ).getQuantity() * (newOrder.getType()*newOrder.getPrice() + (positions.at(i) ).getType()* (positions.at(i) ).getPrice() );
					(positions.at(i) ).setQuantity(0);
					newOrder.setQuantity(-resQ);
					//  Search more to realize the rest newOrder
				} else {
					//  Just equal and balanced
					realizedProfit += (positions.at(i) ).getQuantity() * (newOrder.getType()*newOrder.getPrice() + (positions.at(i) ).getType()* (positions.at(i) ).getPrice() );
					(positions.at(i) ).setQuantity(0);
					newOrder.setQuantity(0);
					break;  //	No more need to search
				}
			}
		}
		if(found == false)
		{
			positions.push_back(newOrder);
		} else {
			//	Update positions for some entry is realized
			 for(int i=positions.size()-1; i>=0; i--)
			{
				if( positions.at(i).isRealized() )
				{
					positions.erase(positions.begin() + i);
				}
			}
			if(!newOrder.isRealized() )
			{
			    positions.push_back(newOrder);
			}
		}
	}
}

double trader::getProfit()
{
	return realizedProfit;
}

/*	=============================	Utilities functions =====================================	*/
int readTSV(string filePath, vector<string> &data)  //  read data from filePath and store data in a string vector
{
	ifstream fileReader(filePath);
	if (!fileReader.is_open())
	{
		cerr << "unable to open file " << filePath << endl;
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

//  Parse the string data to their appropriate type, restore them into vectors
void parseTSVLine(vector<string> &data, vector<string> &tr, vector<string> &st, vector<int> &qu, vector<double> &pr, vector<string> &trT, vector<double> &fe)
{
	//	Handle tab separated string line by line
	int temp;
	for(int i=0; i<data.size()-1; i++)
	{
		istringstream parser(data.at(i+1));
		parser >> temp >> tr.at(i) >> st.at(i) >> qu.at(i) >> pr.at(i) >> trT.at(i) >> fe.at(i);
	}
	return;
}

//  Write the result into tsv file according to the outPath.
void write2TSV(vector<pair<double, string> > &result, string outPath)
{
	ofstream fileWriter(outPath);
	int len = result.size() - 1;
	for(int p=0; p<len; p++)
	{
		fileWriter << (result.at(p) ).second << "\t" << (result.at(p) ).first << endl;
	}
	fileWriter << (result.at(len) ).second << "\t" << (result.at(len) ).first;
	fileWriter.close();
	return;
}

//  Used by sorting pairs
bool comparePairs(const std::pair<double, string>& lhs, const std::pair<double, string>& rhs)
{
  return lhs.first > rhs.first;
}

/*	=====================================	test case	======================================	*/
int main()
{
	//	Get access to the input .tsv file
	vector<string> data;
	string inPath = "";
	cout << "Please type in data file path: " << endl;
	cin >> inPath;
	string outPath = "./output/solution0.tsv";
	string tem = inPath.substr(12, 5);
	outPath.replace(17, 5, tem);
	int load = readTSV(inPath, data);
	if(load == -1)
    {
        cerr << "readTSV failed!" << endl;
        return -1;
    }

	//	parse data
	int size = data.size() - 1;
	vector<string> traderID(size, "");
	vector<string> stockCode(size, "");
	vector<int> quantity(size, 0);
	vector<double> price(size, 0.0);
	vector<string> tradeType(size, "");
	vector<double> fee(size, 0.0);
	parseTSVLine(data, traderID, stockCode, quantity, price, tradeType, fee);

	//uint64_t tick1 = GetTimeMs64(); //  for counting time
	//memoryInWin mInW;   //  for counting memory
	//mInW.init();
	//	proceed the algorithm
	map<string, trader*> book;
	map<string, trader*>::iterator it;
	for(int i=0; i<traderID.size(); i++)    //  maintain a hash table for quick find
	{
		string tid = traderID.at(i);
		it = book.find(tid);
		if(it != book.end())	//	exsiting trader, then update its positions
		{
			trader* e = it->second;
			tradeEntry newEntry(stockCode.at(i), quantity.at(i), price.at(i), tradeType.at(i));
			e->updatePositions(newEntry, fee.at(i) );
		}
		else {  //	insert a new trader if not exsiting
			trader* n = new trader(tid);
			tradeEntry newEntry(stockCode.at(i), quantity.at(i), price.at(i), tradeType.at(i));
			n->updatePositions(newEntry, fee.at(i) );
			book.insert(pair<string, trader*>(tid, n) );
		}
	}

	//  Sorting by trader's realized profit
    vector<pair<double, string> >  formatter(book.size());      //  Restore the table to vector
    int f = 0;
    for(map<string, trader*>::iterator it = book.begin(); it != book.end(); ++it)
    {
        trader* tem = it->second;
        formatter.at(f) = pair<double, string>(tem->getProfit(), it->first);
        f++;
        delete tem;
    }
    sort(formatter.begin(), formatter.end(), comparePairs); //  Sort the pair by the first value DESC
    //cout << "========== main algorithm costs " << GetTimeMs64() - tick1 << " ms." << endl;
    //cout << "========== current used VM by current process: " << mInW.getCurrentUsedVMbyMe() << "MB." << endl;
    //cout << "========== current used PhM by current process: " << mInW.getCurrentUsedPhMbyMe() << "MB." << endl;

	//	output to file
	write2TSV(formatter, outPath);
    cout << "Solution is stored on " << outPath << endl;
	return 0;
}


