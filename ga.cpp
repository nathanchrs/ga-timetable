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

using namespace std;

long long randomoffset;

int nperiodsperweek, nteachers, nrooms;
int populationsize, generationlimit;
int mutationsize;
int mutationrate;
bool elitism;

vector <string> teachers; //[MAX_TEACHERS];
map <string, int> teacherid;
vector <string> rooms; //[MAX_ROOMS];
vector < vector <int> > initial; //[MAX_PERIODS_PER_WEEK][MAX_ROOMS];
vector < vector <int> > periodcount; //[MAX_TEACHERS][MAX_ROOMS];

bool availability[MAX_TEACHERS][MAX_PERIODS_PER_WEEK][MAX_ROOMS];
bool conflicts[MAX_TEACHERS][MAX_TEACHERS];

struct individual { vector < vector <int> > timetable; double fitness;};

vector < individual > population;

int randomint(int lower, int upper){
	srand(time(0)+randomoffset);
	randomoffset = (randomoffset+1)%2823401239LL;
	if(upper<lower) return lower;
	return rand()%(upper-lower+1)+lower;
}

bool randombool(int chancepenalty){	
	if(randomint(0,chancepenalty)==0) return true; else return false;
}



int main(){
	randomoffset = 0;
	
	cin >> nperiodsperweek;
	cin >> populationsize >> generationlimit;
	cin >> mutationsize >> mutationrate;
	int tempint;
	cin >> tempint;
	if(tempint == 0) elitism = false; else elitism = true;
	
	string tempstring;
	cin >> nteachers;
	for(int i = 0; i<nteachers; i++){
		cin >> tempstring;
		teachers.push_back(tempstring);
		teacherid[tempstring]=i;
	}
	cin >> nrooms;
	for(int i = 0; i<nrooms; i++){
		cin >> tempstring;
		rooms.push_back(tempstring);
	}
	
	for(int i = 0; i<nteachers; i++){
		vector <int> nperiods;
		for(int j = 0; j<nrooms; j++){
			cin >> tempint;
			nperiods.push_back(tempint);
		}
		periodcount.push_back(nperiods);
	}
	
	for(int i = 0; i<nperiodsperweek; i++){
		vector <int> period;
		for(int j = 0; j<nrooms; j++){
			cin >> tempstring;
			if(tempstring=="_") period.push_back(-1); else period.push_back(teacherid[tempstring]);
		}
		initial.push_back(period);
	}
	
	for(int k = 0; k<nteachers; k++){
		for(int i = 0; i<nperiodsperweek; i++){
			for(int j = 0; j<nrooms; j++){
				//cin >> tempint;
				
				tempint = 1;
				
				if(tempint==0) availability[k][i][j] = false; else availability[k][i][j] = true;
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
	//INSERT TO POPULATION
	
	for(int k = 0; k<populationsize; k++){
		vector < vector <int> > individualtemp;
		for(int j = 0; j<nperiodsperweek; j++){
			vector <int> row(nrooms,-1);
			individualtemp.push_back(row);
		}
		for(int j = 0; j<nrooms; j++){
			vector <int> weekperiod;
			for(int i = 0; i<nteachers; i++) weekperiod.insert(weekperiod.end(),periodcount[i][j],i);
		
			for(int i = 0; i<nperiodsperweek; i++){
				if(initial[i][j]==-1){
					tempint = randomint(0,weekperiod.size()-1);
					individualtemp[i][j] = weekperiod[tempint];
					weekperiod.erase(weekperiod.begin()+tempint);
				} else {
					individualtemp[i][j] = initial[i][j];
				}
			}
		}
		
		individual tempindividual;
		tempindividual.timetable = individualtemp;
		tempindividual.fitness = -1;
		population.push_back(tempindividual);
	}
					
					
	//ALGORITHM
	cout << "Starting genetic algorithm..." << endl;
	
	double minfitness;
	
	int elapsedgenerations = 0;
	while(elapsedgenerations < generationlimit){	
	
		//fitness
		minfitness = POSITIVE_INFINITY;
		for(int i = 0; i<population.size(); i++){
			double tempfitness = 0;
			for(int j = 0; j<nperiodsperweek; j++){
				for(int k = 0; k<nrooms; k++){
					for(int l = 0; l<nrooms; l++){
						if(!(k==l) && conflicts[population[i].timetable[j][k]][population[i].timetable[j][l]]) tempfitness += 10;
					}
				}
			}
			population[i].fitness = tempfitness;
			if(tempfitness<minfitness){
				minfitness = tempfitness;
			}
		}

		//tournamentselection
		while(population.size()>(populationsize/2+1)){
		
			int a = randomint(0,population.size()-1);
			int b = randomint(0,population.size()-1);
		
			if(a==b){
				continue;
			}
			if(elitism){
				if((population[a].fitness==minfitness) || (population[b].fitness==minfitness)) continue;
			}
			
			if(population[a].fitness<population[b].fitness){
				population.erase(population.begin()+a);
			} else {
				population.erase(population.begin()+b);
			}
		}
		
		//crossover
		while(population.size()<populationsize){
			
			int a = randomint(0,population.size()-1);
			int b = randomint(0,population.size()-1);
			
			if(a==b) continue;
			
			individual individualtemp;
			vector < vector <int> > timetabletemp;
			
			for(int i = 0; i<nperiodsperweek; i++){
				vector <int> row(nrooms,-1);
				timetabletemp.push_back(row);
			}
			
			for(int i = 0; i<nrooms; i++){	
				vector <int> leftover;
				for(int j = 0; j<nperiodsperweek; j++){
					if(initial[j][i]==-1) leftover.push_back(population[b].timetable[j][i]);
				}
				//cout << leftover.size() << "-";
				
				for(int j = 0; j<nperiodsperweek; j++){
					if(initial[j][i]==-1){
						if(j<=nperiodsperweek/2){
							timetabletemp[j][i] = population[a].timetable[j][i];
							leftover.erase(find(leftover.begin(), leftover.end(), timetabletemp[j][i]));
						} else {
							timetabletemp[j][i] = leftover[0];
							leftover.erase(leftover.begin());
						}
					} else {
						timetabletemp[j][i] = initial[j][i];
					}
				}				
			}
			
			individualtemp.timetable = timetabletemp;
			individualtemp.fitness = -1;
			population.push_back(individualtemp);			
		}

		//swapmutation
		//ERROR CAUSE = MINFITNESS ID CHANGED BETWEEN FITNESS CALC AND MUTATION DECISION
		
		for(int i = 0; i<population.size(); i++){
			for(int j = 0; j<nrooms; j++){
				if(randombool(mutationrate)){
					for(int k = 0; k<mutationsize; k++){
						
						int a = randomint(0,nperiodsperweek-1);
						int b = randomint(0,nperiodsperweek-1);
						
						if(elitism && ((population[a].fitness==minfitness) || (population[b].fitness==minfitness))) continue;
						if((initial[a][j]!=-1) || (initial[b][j]!=-1)) continue;
						
						swap(population[i].timetable[a][j],population[i].timetable[b][j]);						
					}
				}
			}
		}
		
		elapsedgenerations++;
		cout << "Computed generation " << elapsedgenerations << ", minimum fitness value = " << minfitness << endl;
	}
	
	//output
	
	//cout << elitism << endl;
	int minfitnessid = -1;
	for(int i = 0; i<population.size(); i++){
		if(population[i].fitness==minfitness){
			minfitnessid = i;
			break;
		}
	}
	
	for(int i = 0; i<nperiodsperweek; i++){
		for(int j = 0; j<nrooms; j++){
			cout << teachers[population[minfitnessid].timetable[i][j]] << " ";
		}
		cout << endl;
	}
	
return 0;}
	
	
	
	