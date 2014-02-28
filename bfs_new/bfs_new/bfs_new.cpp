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
#include<ctime>

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

struct Candidate{
	int num_of_ones;
	vector<int> meta_tests;
	vector<int> tested_comps;
};

struct Feasible{
	double cost;
	list<Test> feasible_set;
};


int NUM_OF_COMPONENTS;
int NUM_OF_TESTS;
vector<Test> META_TESTS;
list<Feasible> FEASIBLES;
vector<Component> COMPONENTS;
Feasible BEST;
double BETA;
stringstream FILENAME;

void CreateFeasibles(){
	//initialize candidate list
	list<Candidate> candidates;
	for (int i = 0; i<META_TESTS.size(); i++){//bir test tut
		for (int j = i; j<META_TESTS.size(); j++){//başka bir test tut
			if (i != j){
				int say = 0;
				int count_i = 0;
				int count_j = 0;
				vector<int> tests;
				tests.resize(NUM_OF_COMPONENTS, 0);
				for (int k = 0; k<NUM_OF_COMPONENTS; k++){//tek tek elemanlarına bak ve karşılaştır
					if (META_TESTS[i].testarray[k] == 1 && META_TESTS[j].testarray[k] == 1){
						say++;
						break;
					}
					if (META_TESTS[i].testarray[k] == 1){
						tests[k] = 1;
						count_i++;
					}
					if (META_TESTS[j].testarray[k] == 1){
						tests[k] = 1;
						count_j++;
					}
				}
				if (say == 0){
					Candidate cand;
					cand.meta_tests.push_back(i + 1);
					cand.meta_tests.push_back(j + 1);
					cand.tested_comps = tests;
					cand.num_of_ones = count_i + count_j;
					if (cand.num_of_ones == NUM_OF_COMPONENTS){//feasible bir set oluştu.
						//FEASIBLES.push_back(cand.meta_tests);
						Feasible x;
						list<Test> feasibleset;
						for (int l = 0; l<cand.meta_tests.size(); l++){
							Test t;
							t.id = cand.meta_tests[l];
							for (int m = 0; m<META_TESTS.size(); m++){
								if (t.id == META_TESTS[m].id){
									t.cost = META_TESTS[m].cost;
									t.probability = META_TESTS[m].probability;
									t.ratio = META_TESTS[m].ratio;
									break;
								}
							}
							feasibleset.push_back(t);
						}
						x.feasible_set = feasibleset;
						FEASIBLES.push_back(x);
					}
					else{
						candidates.push_back(cand);//daha tüm elemanların testi tamamlanmadı.
					}
				}
			}
		}
	}

	//use candidate list entries
	while (candidates.size()>0){
		for (int i = 0; i<META_TESTS.size(); i++){
			if (candidates.size() == 0){
				break;
			}
			int say = 0;
			int count_i = 0;
			int count_j = 0;
			vector<int> tests;
			tests.resize(NUM_OF_COMPONENTS, 0);
			for (int k = 0; k<NUM_OF_COMPONENTS; k++){//tek tek elemanlarına bak ve karşılaştır
				if (META_TESTS[i].testarray[k] == 1 && candidates.front().tested_comps[k] == 1){
					say++;
					break;
				}
				if (META_TESTS[i].testarray[k] == 1){
					tests[k] = 1;
					count_i++;
				}
				if (candidates.front().tested_comps[k] == 1){
					tests[k] = 1;
					count_j++;
				}
			}
			if (say == 0){
				candidates.front().meta_tests.push_back(i + 1);
				candidates.front().num_of_ones = count_i + count_j;
				candidates.front().tested_comps = tests;
				if (candidates.front().num_of_ones == NUM_OF_COMPONENTS){//bir feasible set oluştu.
					Feasible x;
					list<Test> feasibleset;
					for (int l = 0; l<candidates.front().meta_tests.size(); l++){
						Test t;
						t.id = candidates.front().meta_tests[l];
						for (int m = 0; m<META_TESTS.size(); m++){
							if (t.id == META_TESTS[m].id){
								t.cost = META_TESTS[m].cost;
								t.probability = META_TESTS[m].probability;
								t.ratio = META_TESTS[m].ratio;
								break;
							}
						}
						feasibleset.push_back(t);
					}
					x.feasible_set = feasibleset;
					FEASIBLES.push_back(x);
					list<Candidate>::iterator it = candidates.begin();
					it = candidates.erase(it);
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

void CalculateParameters(){
	//meta testlerin cost'larını ve probability'lerini hesapla
	for (int i = 0; i<META_TESTS.size(); i++){
		double cost_i = BETA;
		double prob = 1;
		for (int j = 0; j<META_TESTS[i].testarray.size(); j++){
			if (META_TESTS[i].testarray[j]){
				cost_i = cost_i + (COMPONENTS[j].cost);
				prob = prob*COMPONENTS[j].probablity;
			}
		}
		META_TESTS[i].cost = cost_i;
		META_TESTS[i].probability = prob;
		META_TESTS[i].ratio = (META_TESTS[i].cost / (1 - META_TESTS[i].probability));
	}
}

bool Comparison(Test a, Test b){
	return a.ratio<b.ratio;
}

void FindBestSequence(){
	int index = 0;
	for (list<Feasible>::iterator it = FEASIBLES.begin(); it != FEASIBLES.end(); it++){
		index++;
		(*it).feasible_set.sort(Comparison);
		double cost = 0;
		double prob = 1;
		for (list<Test>::iterator it_test = (*it).feasible_set.begin(); it_test != (*it).feasible_set.end(); it_test++){
			cost = cost + prob*(*it_test).cost;
			prob = prob*(*it_test).probability;
		}
		(*it).cost = cost;
		if (it == FEASIBLES.begin()){
			BEST = (*it);
		}
		if ((*it).cost<BEST.cost){
			BEST = (*it);
			cout << index << endl;
		}
	}

}

int main(){
	ofstream Result;
	string datadir = "C:\\Users\\Rebi\\Desktop\\Research\\data\\";
	string resultfile = "C:\\Users\\Rebi\\Desktop\\Research\\results\\Ratio_Heuristic_Results.txt";
	Result.open(resultfile);
	Result << "Feasible_Set_Results" << endl;

	DIR *dir;
	struct dirent *ent;

	if (dir = opendir(datadir.c_str()))
	{
		while (ent = readdir(dir))
		{
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			{
				double solving_time = 0;
				time_t start, end;
				time_t elapsed;
				time(&start);



				COMPONENTS.clear();
				META_TESTS.clear();
				FEASIBLES.clear();

				ReadFromFile(datadir + (string)ent->d_name);
				CalculateParameters();//Meta testlerin cost ve probabilitylerini hesaplar
				CreateFeasibles();//Feasible setleri oluşturur, setleri oluşturan testleri ratiolara göre sıralar
				FindBestSequence(); //Setlerin costlarını hesaplar ve min cost'a sahip seti bulur, ekrana ve dosyaya yazdırır.

				time(&elapsed); //time_limit check
				solving_time = difftime(elapsed, start);

				string strn;
				strn = FILENAME.str();
				Result << strn << "\t";
				Result << fixed << BEST.cost << "\t";
				cout << strn << "\t";
				cout << " cost=\t" << BEST.cost << "\t";
				cout << " solv.time=\t" << solving_time << "\t";

				for (list<Test>::iterator it = BEST.feasible_set.begin(); it != BEST.feasible_set.end(); it++){
					Result << (*it).id << " ";
					cout << (*it).id << " ";
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