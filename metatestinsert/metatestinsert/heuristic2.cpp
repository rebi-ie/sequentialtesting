#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <numeric>
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
	vector<int> testarray;
};

//Globals
double BETA;
double BEST_COST;
int NUM_OF_COMPONENTS;
int NUM_OF_TESTS;
vector<Test> META_TESTS;
vector<Test> INDIVIDUAL_TESTS(NUM_OF_COMPONENTS);
vector<Component> COMPONENTS;
vector<Test> BEST;



//Declarations
void ReadFromFile(string filename);
void CalculateParameters();
bool Comparison(Test a, Test b);
void FindBestSequence();

bool Comparison(Test a, Test b){
	return a.ratio < b.ratio;
}



/// <summary>
/// Calculates the parameters.
/// </summary>
void CalculateParameters(){


	for (vector<Test>::iterator it_test = INDIVIDUAL_TESTS.begin(); it_test != INDIVIDUAL_TESTS.end(); it_test++){
		double cost_i = BETA;
		double prob = 1;
		for (int j = 0; j < it_test->testarray.size(); j++){
			if (it_test->testarray[j]){
				cost_i = cost_i + (COMPONENTS[j].cost);
				prob = prob*COMPONENTS[j].probablity;
			}
		}
		it_test->cost = cost_i;
		it_test->probability = prob;
		it_test->ratio = (it_test->cost / (1 - it_test->probability));
	}
	//meta testlerin cost'larını, probability'lerini ve ratiolarını hesapla
	for (vector<Test>::iterator it_test = META_TESTS.begin(); it_test != META_TESTS.end(); it_test++){
		double cost_i = BETA;
		double prob = 1;
		for (int j = 0; j < it_test->testarray.size(); j++){
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
	for (int i = 0; i < NUM_OF_COMPONENTS; i++){
		Component comp;
		Source >> comp.id;
		Source >> comp.probablity;
		Source >> comp.cost;
		COMPONENTS.push_back(comp);
	}
	Source >> NUM_OF_TESTS;
	for (int i = 0; i<NUM_OF_COMPONENTS; i++){
		Test t;
		Source >> t.id;
		vector<int> test_array(NUM_OF_COMPONENTS);
		for (int j = 0; j<NUM_OF_COMPONENTS; j++){
			Source >> test_array[j];
		}
		t.testarray = test_array;
		INDIVIDUAL_TESTS.push_back(t);
	}
	for (int i = NUM_OF_COMPONENTS; i<NUM_OF_TESTS; i++){
		Test t;
		Source >> t.id;
		vector<int> test_array(NUM_OF_COMPONENTS);
		for (int j = 0; j<NUM_OF_COMPONENTS; j++){
			Source >> test_array[j];
		}
		t.testarray = test_array;
		META_TESTS.push_back(t);
	}
}

void heuristic()
{
	// start with all individual solution

	vector<Test> currentsolution;
	sort(INDIVIDUAL_TESTS.begin(), INDIVIDUAL_TESTS.end(), Comparison);
	
	currentsolution = INDIVIDUAL_TESTS;
	
	// we have tests 1,2 ... number of comps which are individual tests

	//compute current cost
	double cost = 0;
	double prob = 1;
	for (int i = 0; i < currentsolution.size(); i++)
	{
		cost = cost + prob*currentsolution[i].cost;
		prob = prob*currentsolution[i].probability;

	}


	// for all meta tests that do not intersect
	vector<Test> bestsln = currentsolution;
	vector<Test> metas;
	vector<int> meta_comps(NUM_OF_COMPONENTS, 0);
	BEST_COST = cost;

	vector<Test> best_metas;
	vector<int> best_meta_comps(NUM_OF_COMPONENTS, 0);

	
	bool improved;
	do
	{
		improved = false;
		for each (Test test in META_TESTS)
		{
			vector<Test> currentsolution = best_metas;
			currentsolution.push_back(test);
			
			vector<Test>currentmetas = best_metas;
			currentmetas.push_back(test);

			vector<int> current_meta_comps(NUM_OF_COMPONENTS);
			for (int i = 0; i < NUM_OF_COMPONENTS; i++)
			{
				current_meta_comps[i] = best_meta_comps[i] + test.testarray[i];
			}

			// add other individual tests to fill tests not in 
			for (int i = 0; i < INDIVIDUAL_TESTS.size(); i++)
			{
				if (current_meta_comps[i] == 0)
				{
					currentsolution.push_back(INDIVIDUAL_TESTS[i]);
				}
			}

			sort(currentsolution.begin(), currentsolution.end(), Comparison);
			double cost = 0;
			double prob = 1;
			for (int i = 0; i < currentsolution.size(); i++)
			{
				cost = cost + prob*currentsolution[i].cost;
				prob = prob*currentsolution[i].probability;
			}

			if (cost < BEST_COST)
			{
				bestsln = currentsolution;
				BEST_COST = cost;
				metas = currentmetas;
				meta_comps = current_meta_comps;
				improved = true;
			}


		}
		best_metas = metas;
		best_meta_comps = meta_comps;
		// update META TESTs to remove intersecting
		vector<Test> REM_METAS;
		for each (Test test in META_TESTS)
		{
			bool no_conflicts = true;
			for (int i = 0; i < NUM_OF_COMPONENTS; i++)
			{
				if (test.testarray[i] && best_meta_comps[i])
				{
					no_conflicts = false;
					break;
				}
			}

			if (no_conflicts)
			{
				REM_METAS.push_back(test);
			}
		}

		META_TESTS = REM_METAS;
		
	} while (improved);


	BEST = bestsln;

	//currentsolution.clear();
	//bestsln.clear();
	//metas.clear();
	//best_metas.clear();
	//meta_comps.clear();
	//best_meta_comps.clear();






}

/// <summary>
/// Mains this instance.
/// </summary>
int main()
{
	ofstream Result;
	string datadir = "C:\\Users\\Rebi\\Desktop\\Research\\data\\";
	string resultfile = "C:\\Users\\Rebi\\Desktop\\Research\\results\\Heuristic2_Results.txt";
	Result.open(resultfile);
	Result << "Heuristic2_Results" << endl;

	DIR *dir;
	struct dirent *ent;

	if (dir = opendir(datadir.c_str()))
	{
		while (ent = readdir(dir))
		{
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			{
				META_TESTS.clear();
				INDIVIDUAL_TESTS.clear();
				COMPONENTS.clear();
				BEST.clear();

				ReadFromFile(datadir + (string)ent->d_name);
				CalculateParameters();//Meta testlerin cost, probabilitylerini ve ratiolarını hesaplar
				heuristic();
				
				//FindBestSequence(); //Setlerin costlarını hesaplar ve min cost'a sahip seti bulur, ekrana ve dosyaya yazdırır.

				Result << (string)ent->d_name << "\t";
				cout.precision(8);
				cout.width(15);
				Result << fixed << BEST_COST << "\t";
				cout << (string)ent->d_name << " cost" << BEST_COST << endl;

				for each (Test test in BEST)
				{
					Result << test.id << " ";
					cout << test.id << " ";
				}
				cout << endl;
				Result << endl;
			}
		}
	}
	Result.close();
	return 0;
}