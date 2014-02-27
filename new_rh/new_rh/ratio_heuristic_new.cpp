#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <list>
#include <iterator>
#include <bitset>


using namespace std;

struct Component{
	int id;
	double cost;
	double probablity;
	
};

struct Test{
	int id;
	double cost;
	double probability;
	double ratio;
	bitset<50> testarray;
};


//Globals
int NUM_OF_COMPONENTS;
int NUM_OF_TESTS;
list<Test> META_TESTS;
vector<Component> COMPONENTS;
vector<unsigned int> BEST;
double BETA;
double BEST_COST;
stringstream FILENAME;

//Declarations
void ReadFromFile();
void CalculateParameters();
bool Comparison(Test a, Test b);
double FindBestSequence();


bool Comparison(Test a, Test b){
	return a.ratio<b.ratio;
}

double FindBestSequence(){
	META_TESTS.sort(Comparison);//meta testleri ratiolara göre sıraladı

	double cost = 0;
	double prob = 1;

	while (!META_TESTS.empty()){

		BEST.push_back(META_TESTS.front().id);//en iyi ratioyu vereni seçti

		cost = cost + prob*META_TESTS.front().cost;//cost güncelle
		prob = prob*META_TESTS.front().probability;//prob güncelle

		Test scheduled = META_TESTS.front();//schedule et testi

		list<Test>::iterator it_test = META_TESTS.begin();
		while (it_test != META_TESTS.end())
		{
			if ((it_test->testarray & scheduled.testarray) == 1)
			{
				it_test = META_TESTS.erase(it_test);
			}
			else
			{
				it_test++;
			}
		}
	}
	return cost;
}


void CalculateParameters(){
	//meta testlerin cost'larını, probability'lerini ve ratiolarını hesapla
	for each (Test t in META_TESTS)
	{
	double cost_i = BETA;
		double prob = 1;
		for (int i = 0; i < t.testarray.size; i++)
		{
			if (t.testarray[i])
			{
				cost_i = cost_i + (COMPONENTS[i].cost);
				prob = prob*COMPONENTS[i].probablity;
			}
		}
		t.cost = cost_i;
		t.probability = prob;
		t.ratio = (t.cost / (1 - t.probability));
	}
}

void ReadFromFile(){
	ifstream Source;
	string root;
	root = "C:\\Users\\Rebi\\Desktop\\Research\\data\\" + FILENAME.str();
	Source.open(root);
	Source >> BETA;
	Source >> NUM_OF_COMPONENTS;
	for (int i = 0; i<NUM_OF_COMPONENTS; i++){
		Component comp;
		Source >> comp.id;
		Source >> comp.probablity;
		Source >> comp.cost;
		COMPONENTS.push_back(comp);
	}
	Source >> NUM_OF_TESTS;
	for (int i = 0; i<NUM_OF_TESTS; i++){
		Test t;
		Source >> t.id;
		vector<int> test_array;
		for (int j = 0; j<NUM_OF_COMPONENTS; j++){
			bool a;
			Source >> a;
			t.testarray[j] = a;
		}
		
		META_TESTS.push_back(t);
	}
}

int main()
{
	ofstream Result;
	stringstream resultfile;
	resultfile.str("");
	resultfile.clear();
	resultfile << "Ratio_Heuristic_Results.txt";
	string root;
	root = "C:\\Users\\Rebi\\Desktop\\Research\\results\\" + resultfile.str();
	Result.open(root);
	Result << "Ratio_Heuristic_Results" << endl;

	for (int j = 1; j <= 50; j++){
		FILENAME.str("");
		FILENAME.clear();
		FILENAME << "Meta-" << j << ".txt";

		META_TESTS.clear();
		COMPONENTS.clear();
		BEST.clear();

		ReadFromFile();//Input dosyasını okur
		CalculateParameters();//Meta testlerin cost, probabilitylerini ve ratiolarını hesaplar
		FindBestSequence(); //Setlerin costlarını hesaplar ve min cost'a sahip seti bulur, ekrana ve dosyaya yazdırır.

		string strn;
		strn = FILENAME.str();
		Result << strn << "\t";
		cout.precision(8);
		cout.width(15);
		Result << fixed << BEST_COST << "\t";
		cout << strn << " cost" << BEST_COST << endl;

		for (int i = 0; i<BEST.size(); i++){
			if (i == BEST.size() - 1){
				cout << BEST[i] << "\t";
				Result << BEST[i] << "\t";
			}
			else{
				cout << BEST[i] << " ";
				Result << BEST[i] << " ";
			}
		}
		cout << endl;
		Result << endl;

	}
	Result.close();

	system("pause");
	return 0;
}