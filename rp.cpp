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
		
	} else {	
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
	for(int i=0; i<data.size()-1; i++) {
		istringstream parser(data.at(i + 1));
		parser >> temp >> tr.at(i) >> st.at(i) >> qu.at(i) >> pr.at(i) >> trT.at(i) >> fe.at(i);
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
	vector<string> trader(size - 1, "");
	vector<string> stockCode(size - 1, "");
	vector<int> quantity(size - 1, 0);
	vector<double> price(size -1 , 0.0);
	vector<string> tradeType(size -1 , "");
	vector<double> fee(size -1 , 0.0); 
	parseTSVLine(data, trader, stockCode, quantity, price, tradeType, fee);
	
	///*	Just for testing
	cout << "After parsing:  " << endl;
	for(int i=0; i<size-1; i++) {
		cout << trader.at(i) << "\t" << stockCode.at(i) << "\t" << quantity.at(i) << "\t" << price.at(i) << "\t" << tradeType.at(i) << "\t" << fee.at(i) << endl;
	}
	//*/
	
	return 0;
	
}
	

