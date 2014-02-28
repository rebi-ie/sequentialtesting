#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <list>
#include <iterator>
#include <bitset>
#include <dirent.h>



using namespace std;

 int NUM_OF_COMPONENTS;
 int NUM_OF_TESTS;

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


list<Test> META_TESTS;
vector<Component> COMPONENTS;
vector<unsigned int> BEST;
double BETA;
double BEST_COST;
stringstream FILENAME;

//Declarations
void ReadFromFile(string filename);
void CalculateParameters();
bool Comparison(Test a, Test b);
void FindBestSequence();


/// <summary>
/// Comparisons the specified a.
/// </summary>
/// <param name="a">A.</param>
/// <param name="b">The b.</param>
/// <returns></returns>
bool Comparison(Test a, Test b){
	return a.ratio<b.ratio;
}

/// <summary>
/// Finds the best sequence.
/// </summary>
/// <returns></returns>
void FindBestSequence(){
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
			
			if ((it_test->testarray & scheduled.testarray).any() )
			{
				it_test = META_TESTS.erase(it_test);
			}
			else
			{
				it_test++;
			}
		}
	}
	BEST_COST = cost;
}


/// <summary>
/// Calculates the parameters.
/// </summary>
void CalculateParameters(){
	//meta testlerin cost'larını, probability'lerini ve ratiolarını hesapla
	for (list<Test>::iterator it_test = META_TESTS.begin(); it_test != META_TESTS.end(); it_test++){
		double cost_i = BETA;
		double prob = 1;
		for (int j = 0; j< it_test->testarray.size(); j++){
			if (it_test->testarray[j]){
				cost_i = cost_i + (COMPONENTS[j].cost);
				prob = prob*COMPONENTS[j].probablity;
			}
		}
		it_test->cost = cost_i;
		it_test->probability = prob;
		it_test->ratio = (it_test->cost / (1 - it_test->probability));
	}
}

/// <summary>
/// Reads from file.
/// </summary>
void ReadFromFile(string filename){
	ifstream Source;
	
	Source.open(filename);
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

/// <summary>
/// Mains this instance.
/// </summary>
/// <returns></returns>
int main()
{
	ofstream Result;
	string datadir = "C:\\Users\\Rebi\\Desktop\\Research\\data\\";
	string resultfile = "C:\\Users\\Rebi\\Desktop\\Research\\results\\Ratio_Heuristic_Results.txt";
	Result.open(resultfile);
	Result << "Ratio_Heuristic_Results" << endl;

	DIR *dir;
	struct dirent *ent;
	
	if (dir=opendir(datadir.c_str()))
	{
		while (ent = readdir(dir))
		{
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			{
				META_TESTS.clear();
				COMPONENTS.clear();
				BEST.clear();			
				
				ReadFromFile( datadir + (string) ent->d_name);
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
		}
	}
	Result.close();
	system("pause");
	return 0;
}