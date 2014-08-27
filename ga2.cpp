#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#define MAX_PERIODS_PER_WEEK 1100
#define MAX_ROOMS 50
#define MAX_TEACHERS 150
#define POSITIVE_INFINITY 99999999999
#define EMPTY -1

using namespace std;

long long randomoffset;
int elapsedgenerations;

int nperiodsperweek, nteachers, nrooms;
int populationsize, generationlimit;
int tournamentsize;
double mutationrate;
bool elitism;

vector <string> teachers;
map <string, int> teacherid;


class individual {
public:
	int table[MAX_ROOMS][MAX_PERIODS_PER_WEEK];
	double fitness;
	
	individual () { fitness = 0; }
};

int initial[MAX_ROOMS][MAX_PERIODS_PER_WEEK];
int availability[MAX_TEACHERS][MAX_ROOMS][MAX_PERIODS_PER_WEEK];
int periodcount[MAX_ROOMS][MAX_TEACHERS];
bool conflicts[MAX_TEACHERS][MAX_TEACHERS];
individual elite;
vector <individual> population;

int randomint(int lower, int upper){
	srand(time(0)+randomoffset);
	randomoffset = (randomoffset+1)%2823401239LL;
	if(upper<lower) return lower;
	return rand()%(upper-lower+1)+lower;
}
bool randombool(double chance){	
	if(randomint(0,1000000) < (long long) 1000000*chance) return true; else return false;
}


int getminfitnessid(){
	double minvalue = POSITIVE_INFINITY;
	int minid = 0;
	for(int i = 0; i<population.size(); i++){
		double tempfitness = 0;
		
		//calculate conflicts
		for(int j = 0; j<nperiodsperweek; j++){
			for(int k = 0; k<nrooms; k++){
				for(int l = 0; l<nrooms; l++){
					if(k!=l){
						if(conflicts[population[i].table[k][j]][population[i].table[l][j]] != 0) tempfitness += 10;
					}
				}
			}
		}
		
		population[i].fitness = tempfitness;
		if(tempfitness<minvalue){
			minvalue = tempfitness;
			minid = i;
		}
	}
	return minid;
}
int tournamentselection(){
	double tournamentminfitness = POSITIVE_INFINITY;
	int tournamentwinnerid = 0;
	int tempint;
	for(int i = 0; i<tournamentsize; i++){
		tempint = randomint(0,population.size()-1);
		if(population[tempint].fitness < tournamentminfitness){
			tournamentminfitness = population[tempint].fitness;
			tournamentwinnerid = tempint;
		}
	}
	return tournamentwinnerid;
}

individual crossover(int a, int b){
	individual offspring;
	for(int i = 0; i<nrooms; i++){
	
		vector <int> weekperiod;
		for(int j = 0; j<nperiodsperweek; j++){
			if(initial[i][j] == EMPTY){
				weekperiod.push_back(population[b].table[i][j]);
			}
		}
		
		for(int j = 0; j<nperiodsperweek; j++){
			if(initial[i][j] != EMPTY){
				offspring.table[i][j] = initial[i][j];
			} else {
				if(j<nperiodsperweek/2){
					offspring.table[i][j] = population[a].table[i][j];
					weekperiod.erase(find(weekperiod.begin(),weekperiod.end(),offspring.table[i][j]));
				} else {
					offspring.table[i][j] = weekperiod[0];
					weekperiod.erase(weekperiod.begin());
				}
			}
		}
		
	}
	return offspring;
}


int main(){
	randomoffset = 0;
	
	string tempstring;
	int tempint;
	
	cin >> nperiodsperweek;
	cin >> populationsize >> generationlimit;
	cin >> tournamentsize;
	cin >> mutationrate;
	cin >> tempint;
	if(tempint == 0) elitism = false; else elitism = true;

	cin >> nteachers >> nrooms;
	for(int i = 0; i<nteachers; i++){
		cin >> tempstring;
		teachers.push_back(tempstring);
		teacherid[tempstring]=i;
		
		for(int j = 0; j<nrooms; j++){
			cin >> periodcount[j][i];
		}
	}
	
	for(int i = 0; i<nperiodsperweek; i++){
		for(int j = 0; j<nrooms; j++){
			cin >> tempstring;
			if(tempstring=="_") initial[j][i] = EMPTY; else initial[j][i] = teacherid[tempstring];
		}
	}
	
	for(int i = 0; i<nteachers; i++){
		for(int j = 0; j<nperiodsperweek; j++){
			for(int k = 0; k<nrooms; k++){
				//cin >> availability[i][k][j];
				availability[i][k][j] = 1;
			}
		}
	}
	
	for(int i = 0; i<nteachers; i++){
		for(int j = 0; j<nteachers; j++){
			cin >> tempint;
			if(tempint==0) conflicts[i][j] = false; else conflicts[i][j] = true;
		}
	}
	
	cout << "Finished input" << endl;
	
	//insert to population
	for(int i = 0; i<populationsize; i++){
		
		individual newindividual;
		vector <int> weekperiod;
		for(int j = 0; j<nrooms; j++){
			for(int k = 0; k<nteachers; k++){
				weekperiod.insert(weekperiod.end(), periodcount[j][k],k);
			}
			
			for(int k = 0; k<nperiodsperweek; k++){
				if(initial[j][k]==EMPTY){
					tempint = randomint(0,weekperiod.size()-1);
					newindividual.table[j][k] = weekperiod[tempint];
					weekperiod.erase(weekperiod.begin()+tempint);
				} else {
					newindividual.table[j][k] = initial[j][k];
				}
			}	
		}
		population.push_back(newindividual);
	}
		
	//algorithm
	cout << "Starting genetic algorithm..." << endl;
		
	elapsedgenerations = 0;
	int elitismoffset = 0;
	if(elitism) elitismoffset = 1;
	
	while(elapsedgenerations < generationlimit){
		vector <individual> newpopulation;
		
		
		//compute fitness, find minimum
		int minid = getminfitnessid();
		double minvalue = population[minid].fitness;
		if(elitism){
			newpopulation.push_back(population[minid]);
		}
				
		//crossover;
		for(int i = elitismoffset; i<population.size(); i++){
			int a = tournamentselection();
			int b = tournamentselection();
			individual offspring = crossover(a,b);
			newpopulation.push_back(offspring);			
		}

		
		
		//mutate;
		for(int i = elitismoffset; i<population.size(); i++){
			for(int j = 0; j<nrooms; j++){
				if(randombool(mutationrate)){
					int a, b;
					do {
						a = randomint(0,nperiodsperweek-1);
						b = randomint(0,nperiodsperweek-1);
					} while((initial[j][a]!=EMPTY) || (initial[j][b]!=EMPTY));
					swap(population[i].table[j][a],population[i].table[j][b]);
				}
			}
		}
		
		population = newpopulation;
		
		elapsedgenerations++;
		cout << "Computed generation " << elapsedgenerations << ", minimum fitness at start: " << minvalue << endl;
	}
	
	int minid = getminfitnessid();
	cout << endl << "RESULT (fitness: " << population[minid].fitness << ")" << endl;
	for(int i = 0; i<nperiodsperweek; i++){
		for(int j = 0; j<nrooms; j++){
			cout << teachers[population[minid].table[j][i]] << " ";
		}
		cout << endl;
	}
	
return 0;}
	