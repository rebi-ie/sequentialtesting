#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <dirent.h>
#include <ctime>
using namespace std;

struct Component
{
	int id;
	double cost;
	double probablity;
};

struct Test
{
	int id;
	double cost;
	double probability;
	vector<int> testarray;
};

struct Candidate
{
	int num_of_ones;
	vector<int> meta_tests;
	vector<int> tested_comps;
};

struct FeasibleSet
{
	double cost;
	vector<Test> tests;
};

int NUM_OF_COMPONENTS;
int NUM_OF_TESTS;
double BETA;
vector<Component> COMPONENTS;
vector<Test> META_TESTS;
vector<FeasibleSet> FEASIBLES;
FeasibleSet BEST;


//TODO : tidy up this function
void CreateFeasibleSets(){
	//initialize candidate list of 2 independent meta tests
	//going through all tests is not possible.
	vector<Candidate> candidates;
	for (int i = 0; i < META_TESTS.size(); i++)
	{
		for (int j = i; j < META_TESTS.size(); j++)
		{
			bool does_not_overlap = true;
			int count_i = 0;
			int count_j = 0;
			vector<int> tests(NUM_OF_COMPONENTS);
			for (int k = 0; k < NUM_OF_COMPONENTS; k++)//tek tek elemanlarına bak ve karşılaştır
			{
				if (META_TESTS[i].testarray[k] && META_TESTS[j].testarray[k])
				{
					does_not_overlap = false;
					break;
				}
				if (META_TESTS[i].testarray[k])
				{
					tests[k] = 1;
					count_i++;
				}
				if (META_TESTS[j].testarray[k])
				{
					tests[k] = 1;
					count_j++;
				}
			}





			if (does_not_overlap)
			{
				Candidate cand;
				cand.meta_tests.push_back(i + 1);
				cand.meta_tests.push_back(j + 1);
				cand.tested_comps = tests;
				cand.num_of_ones = count_i + count_j;
				if (cand.num_of_ones == NUM_OF_COMPONENTS)
				{//feasible bir set oluştu.
					FeasibleSet x;
					vector<Test> feasibleset;
					for (int l = 0; l < cand.meta_tests.size(); l++)
					{
						Test t;
						t.id = cand.meta_tests[l];
						for (int m = 0; m < META_TESTS.size(); m++)
						{
							if (t.id == META_TESTS[m].id)
							{
								t.cost = META_TESTS[m].cost;
								t.probability = META_TESTS[m].probability;
								break;
							}
						}
						feasibleset.push_back(t);
					}
					x.tests = feasibleset;
					FEASIBLES.push_back(x);
				}
				else
				{
					candidates.push_back(cand);//daha tüm elemanların testi tamamlanmadı.
				}
			}
		}

	}






	vector<int> tests(NUM_OF_COMPONENTS);
	//use candidate list entries
	while (candidates.size()>0){
		for (int i = 0; i<META_TESTS.size(); i++){
			if (candidates.size() == 0){
				break;
			}
			int say = 0;
			int count_i = 0;
			int count_j = 0;
			
			std::fill(tests.begin(), tests.end(), 0);
			Candidate * thecand = &candidates.back();
			for (int k = 0; k<NUM_OF_COMPONENTS; k++){//tek tek elemanlarına bak ve karşılaştır
				if (META_TESTS[i].testarray[k]  && thecand->tested_comps[k] ){
					say++;
					break;
				}
				if (META_TESTS[i].testarray[k]){
					tests[k] = 1;
					count_i++;
				}
				if (thecand->tested_comps[k]){
					tests[k] = 1;
					count_j++;
				}
			}
			if (say == 0){
				thecand->meta_tests.push_back(i + 1);
				thecand->num_of_ones = count_i + count_j;
				thecand->tested_comps = tests;
				if (thecand->num_of_ones == NUM_OF_COMPONENTS){//bir feasible set oluştu.
					FeasibleSet x;
					vector<Test> feasibleset;
					for (int l = 0; l<thecand->meta_tests.size(); l++){
						Test t;
						t.id = thecand->meta_tests[l];
						for (int m = 0; m<META_TESTS.size(); m++){
							if (t.id == META_TESTS[m].id){
								t.cost = META_TESTS[m].cost;
								t.probability = META_TESTS[m].probability;
								break;
							}
						}
						feasibleset.push_back(t);
					}
					x.tests = feasibleset;
					FEASIBLES.push_back(x);
					candidates.pop_back();
				}
			}
		}
	}



}

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

	// Override
	NUM_OF_TESTS = 500;

	for (int i = 0; i<NUM_OF_TESTS; i++){
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

//Calculates cost and probability of meta tests
void CalculateParameters()
{
	double cost, prob;

	for (int i = 0; i < META_TESTS.size(); i++)
	{
		cost = BETA;
		prob = 1;
		for (int j = 0; j < NUM_OF_COMPONENTS; j++)
		{
			cost += COMPONENTS[j].cost;
			prob *= COMPONENTS[j].probablity;
		}
		META_TESTS[i].cost = cost;
		META_TESTS[i].probability = prob;
	}
}

//finds the best feasible set in FEASIBLES
void FindBestFeasibleSet()
{
	BEST.cost = INFINITE;
	for each (FeasibleSet feasible in FEASIBLES)
	{
		double cost = 0;
		double prob = 1;
		for each (Test test in feasible.tests)
		{
			cost = cost + test.cost*prob;
			prob = prob * test.probability;
		}
		feasible.cost = cost;
		if (cost < BEST.cost)
		{
			BEST = feasible;
			
		}
	}
}

int main(){
	ofstream Result;
	string datadir = "C:\\Users\\Rebi\\Desktop\\Research\\data\\";
	string resultfile = "C:\\Users\\Rebi\\Desktop\\Research\\results\\Feasible_Results.txt";
	Result.open(resultfile);
	Result << "Feasible_Set_Results" << endl;
	DIR *dir;
	
	if (dir = opendir(datadir.c_str()))// open data directory
	{
		struct dirent *ent;
		while (ent = readdir(dir))// read files in directory
		{
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)// while file is valid (a test file)
			{
				double solving_time = 0;
				time_t start, end;
				time(&start);

				COMPONENTS.clear();
				META_TESTS.clear();
				FEASIBLES.clear();

				ReadFromFile(datadir + (string)ent->d_name);
				CalculateParameters();//Meta testlerin cost ve probabilitylerini hesaplar
				CreateFeasibleSets();//Feasible setleri oluşturur
				FindBestFeasibleSet(); // min cost'a sahip seti bulur

				time(&end); //time_limit check
				solving_time = difftime(end, start);
	
				Result << (string)ent->d_name << "\t";
				Result << fixed << BEST.cost << "\t";
				cout << (string)ent->d_name << "\t";
				cout << " cost=\t" << BEST.cost << "\t";
				cout << " time=\t" << solving_time << "\t";

				for each (Test test in BEST.tests)
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
	system("pause");
	return 0;
}