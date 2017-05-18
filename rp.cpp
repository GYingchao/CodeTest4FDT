#include<iostream>
#include <fstream>
#include<string>
#include<sstream>
#include<vector>
#include<map>
#include<algorithm>
#include "rp.h"

using namespace std;

//	Trade Type definition
enum TradeType
{
	BUY = -1,
	SELL = 1,
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
		//bool realized;

	public:
		tradeEntry(string st, int qu, double pr, string tt);
		string getStockCode();
		int getQuantity();
		void setQuantity(int v);
		double getPrice();
		TradeType getType();
		//double getFee();
		bool isRealized();
		//void setRealized(bool r);
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
		void updatePositions(tradeEntry& newOrder, double fee);
		double getProfit();
};

/*	=============================   Class tradeEntry Implementation =====================================	*/
tradeEntry::tradeEntry(string st, int qu, double pr, string tt)
{
	stockCode = st;
	quantity = qu;
	if(quantity <= 0)
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
	//fee = fe;
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
    if(quantity == 0)
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
    uint64_t tick1 = GetTimeMs64();
	realizedProfit -= fee;
	if(positions.empty())   //  Holding no stock
	{
		positions.push_back(newOrder);
	}
	else {  //  Find whether there is a same stock with different tradeType on hold
		string stockCode = newOrder.getStockCode();
		TradeType type = newOrder.getType();
		bool found = false;
		for(int i=0; i<positions.size(); i++)
		{
		    if(positions.at(i).isRealized() ) continue;
			if( ( (positions.at(i) ).getStockCode() == stockCode) && ( (positions.at(i) ).getType() * type == -1) ) // Search for buy/sell pair
			{
				//	buy/sell pair exists, needs to be realized
				found = true;
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
					break;
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
	cout << "========== up costs " << GetTimeMs64() - tick1 << " ms." << endl;
}
double trader::getProfit()
{
	return realizedProfit;
}

/*	=============================	Utilities functions =====================================	*/
int readTSV(string filePath, vector<string> &data)
{
    //uint64_t tick1 = GetTimeMs64();
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
	//cout << "========== readTSV in "<< filePath << " costs " << GetTimeMs64() - tick1 << " ms." << endl;
	return 0;
}
void parseTSVLine(vector<string> &data, vector<string> &tr, vector<string> &st, vector<int> &qu, vector<double> &pr, vector<string> &trT, vector<double> &fe)
{
    //uint64_t tick1 = GetTimeMs64();
	//	Handle tab separated string line by line
	int temp;
	for(int i=0; i<data.size()-1; i++)
	{
		istringstream parser(data.at(i+1));
		parser >> temp >> tr.at(i) >> st.at(i) >> qu.at(i) >> pr.at(i) >> trT.at(i) >> fe.at(i);
	}
	//cout << "========== parseTSVLine costs " << GetTimeMs64() - tick1 << " ms." << endl;
	return;
}
void write2TSV(vector<pair<double, string> > &result, string outPath)
{
    //uint64_t tick1 = GetTimeMs64();
	ofstream fileWriter(outPath);
	int len = result.size() - 1;
	for(int p=0; p<len; p++)
	{
		fileWriter << (result.at(p) ).second << "\t" << (result.at(p) ).first << endl;
	}
	fileWriter << (result.at(len) ).second << "\t" << (result.at(len) ).first;
	fileWriter.close();
	//cout << "========== write2TSV costs " << GetTimeMs64() - tick1 << " ms." << endl;
	return;
}
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
	//string tem1 = outPath.substr(17, 5);
	string tem2 = inPath.substr(12, 5);
	outPath.replace(17, 5, tem2);
	int a = readTSV(inPath, data);
	if(a == -1)
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
	//cout << "========== allocate vectors costs " << GetTimeMs64() - tick1 << " ms." << endl;
	parseTSVLine(data, traderID, stockCode, quantity, price, tradeType, fee);

	uint64_t tick1 = GetTimeMs64();
	//	proceed the algorithm
	map<string, trader*> book;
	map<string, trader*>::iterator it;
	for(int i=0; i<traderID.size(); i++)
	{
		string tid = traderID.at(i);
		it = book.find(tid);
		if(it != book.end())	//	exsiting
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
    vector<pair<double, string> >  formatter(book.size());      //  Restore to vector
    int f = 0;
    for(map<string, trader*>::iterator it = book.begin(); it != book.end(); ++it)
    {
        formatter.at(f) = pair<double, string>(it->second->getProfit(), it->first);
        f++;
    }
    sort(formatter.begin(), formatter.end(), comparePairs); //  Sort the pair by the first value DESC
    cout << "========== main algorithm costs " << GetTimeMs64() - tick1 << " ms." << endl;

	//	output to file
	write2TSV(formatter, outPath);
    cout << "Solution is stored on " << outPath << endl;
	return 0;
}


