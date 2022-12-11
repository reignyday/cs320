#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

struct branch {
    unsigned long long address;
    string behavior = "";
    unsigned long long target;
};

int numofinstructions = 0;
string inputfile = "";
string outputfile = "";
ifstream file;

unsigned long long address;
string behavior = "";
unsigned long long target;

vector<branch> branches;

int alwaystakenCorrect = 0;
int alwaysnottakenCorrect = 0;
int bimodalsinglebitCorrect = 0;
int bimodaltwobitCorrect = 0;
int gshareCorrect = 0;
int tournamentCorrect = 0;
int btbCorrect = 0;

void alwaystaken (ofstream &filew, vector<branch> v){ 
  //for (int i = 0; i < v.size(); i++){
    for(branch b : v){
	if (b.behavior == "T"){
	    alwaystakenCorrect++;
	}
    }
    filew << alwaystakenCorrect << "," << v.size() << ";" << endl;
}

void alwaysnottaken(ofstream &filew, vector<branch> v){
  //for (int i = 0; i < v.size(); i++){
    for(branch b : v){
	if(b.behavior == "NT"){
	    alwaysnottakenCorrect++;
	}
    }
    filew << alwaysnottakenCorrect << "," << v.size() << ";" << endl;
}

void bimodalsinglebit(ofstream &filew, vector<branch> v, int tablesize)
{
    vector<bool> table(tablesize, true);
    int index;

    bimodalsinglebitCorrect = 0; //restart
    
    for(branch b: v){
        index = b.address % tablesize;
	if(b.behavior == "T" && table[index] == true){
	    bimodalsinglebitCorrect++;
	}
	else if(b.behavior == "T" && table[index] == false){
	    table[index] = true;
	}
	else if(b.behavior == "NT" && table[index] == false){
	    bimodalsinglebitCorrect++;
	}
	else if(b.behavior == "NT" && table[index] == true){
	    table[index] = false;
	}
    }
    if(tablesize == 2048){
      filew << bimodalsinglebitCorrect << "," << v.size() << "; " << endl;
    }
    else{
      filew << bimodalsinglebitCorrect << "," << v.size() << "; ";
    }
}
	
void bimodaltwobit(ofstream &filew, vector<branch> v, int tablesize)
{
    vector<int> table(tablesize, 3); //strongly taken 11=3, weakly taken 10 =2, weakly not taken 01=1, strongly not taken = 00=0
    int index;

    bimodaltwobitCorrect = 0; //reset everytime used
    
    for(branch b: v){
        index = b.address % tablesize;
	if(b.behavior == "T" && table[index] >= 2){
	    bimodaltwobitCorrect++;
	    if(table[index] == 2){
	        table[index]++;
	    }
	}
        else if(b.behavior == "T" && table[index] <= 1){//predicted wrong
	    if(table[index] < 3){
	        table[index]++;
	    }
	}
        else if(b.behavior == "NT" && table[index] <= 1){//predicted right
	    bimodaltwobitCorrect++;
	    if(table[index] == 1){
	        table[index]--;
	    }
	}
        else if(b.behavior == "NT" && table[index] >= 2){//predicted wrong
	    if(table[index] > 0){
	        table[index]--;
	    }
        }
    }
    if(tablesize == 2048){
        filew << bimodaltwobitCorrect << "," << v.size() << "; " << endl;
    }
    else{
        filew << bimodaltwobitCorrect << "," << v.size() << "; ";
    }
}

void gshare(ofstream &filew, vector<branch> v, int historylength){
        
    const int tablesize = 2048;
    vector<int> table(tablesize, 3);

    int index;
    int ghr = 0;
    int size = pow(2, historylength) - 1;
    
    gshareCorrect = 0;
    
    for(branch b : v){
        index = (b.address ^ (ghr & size)) % tablesize;
	if(b.behavior == "T" && table[index] >= 2){
	    gshareCorrect++;
	    if(table[index] == 2){
	        table[index]++;
	    }
	    ghr = ghr << 1;
	    ghr++;
	}
        else if(b.behavior == "T" && table[index] <= 1){//predicted wrong
	    if(table[index] < 3){
	        table[index]++;
	    }
	    ghr = ghr << 1;
	    ghr++;//taken, plus 1
	}
        else if(b.behavior == "NT" && table[index] <= 1){//predicted right
	    gshareCorrect++;
	    if(table[index] == 1){
	        table[index]--;
	    }
	    ghr = ghr << 1;
	}
        else if(b.behavior == "NT" && table[index] >= 2){//predicted wrong
	    if(table[index] > 0){
	        table[index]--;
	    }
	    ghr = ghr << 1;
        }
    }
    if(historylength == 11){
        filew << gshareCorrect << "," << v.size() << "; " << endl;
    }
    else{
        filew << gshareCorrect << "," << v.size() << "; ";
    }
}

void tournament(ofstream &filew, vector<branch> v){
    const int tablesize = 2048;

    vector<int> gsharetable(tablesize, 3); //strongly taken
    vector<int> bimodaltable(tablesize, 3); //dtrongly taken
    vector<int> selectortable(tablesize, 0); //sttrongly not take, prefer gshare

    int gshareindex;
    int sandbindex;
    
    int ghr = 0;
    int size = 2047;

    /*int sellow = 0;
    int selhigh = 0;

    int gwmain = 0;
    int bwmain = 0;
    int grmain = 0;
    int brmain = 0;
    int a = 0;

    int c1 = 0;
    int c2=0;
    int c3=0;
    int c4=0;
    int c5=0;
    int c6=0;*/
	
    for(branch b : v){
        sandbindex = b.address % tablesize;
	gshareindex = (b.address ^ (ghr & size)) % tablesize;

	if(selectortable[sandbindex] <= 1) {//gshare
	    if((gsharetable[gshareindex] >= 2) && b.behavior == "T"){
	        tournamentCorrect++;
		if(bimodaltable[sandbindex] <= 1){
		    if(selectortable[sandbindex] > 0){
		        selectortable[sandbindex]--;
		    }
	        }
	    }
	    else if((gsharetable[gshareindex] <= 1) && b.behavior == "T"){
	        if(bimodaltable[sandbindex] >= 2){
	            if(selectortable[sandbindex] < 3){
		        selectortable[sandbindex]++;
		    }
		}
	    }
	    else if((gsharetable[gshareindex] <= 1) && b.behavior == "NT"){
		tournamentCorrect++;
		if(bimodaltable[sandbindex] >= 2){
		    if(selectortable[sandbindex] > 0){
                        selectortable[sandbindex]--;
		    }
		}
	    }
	    else if((gsharetable[gshareindex] >= 2) && b.behavior == "NT"){
	        if(bimodaltable[sandbindex] <= 1){
		    if(selectortable[sandbindex] == 0 || selectortable[sandbindex] == 1 || selectortable[sandbindex] == 2){
		        selectortable[sandbindex]++;
		    }
		}
	    }
	}
	else if(selectortable[sandbindex] >= 2){//bimodal
	    if((bimodaltable[sandbindex] >= 2) && b.behavior == "T"){
	        tournamentCorrect++;
		if(gsharetable[gshareindex] <= 1){
		    if(selectortable[sandbindex] < 3){
		        selectortable[sandbindex]++;
		    }
		}
	    }
	    else if((bimodaltable[sandbindex] <= 1) && b.behavior == "T"){
	        if(gsharetable[gshareindex] >= 2){
		    if(selectortable[sandbindex] > 0){
		        selectortable[sandbindex]--;
		    }
		}
	    }
	    else if((bimodaltable[sandbindex] <= 1) && b.behavior == "NT"){
		tournamentCorrect++;
		if(gsharetable[gshareindex] >= 2){
		    if(selectortable[sandbindex] < 3){
		        selectortable[sandbindex]++;
		    }
		}
	    }
	    else if((bimodaltable[sandbindex] >= 2) && b.behavior == "NT"){
	        if(gsharetable[gshareindex] <= 1){
		    if(selectortable[sandbindex] > 0){
		        selectortable[sandbindex]--;
		    }
		}
	    }
       	}

	if(b.behavior == "T"){
	    if(gsharetable[gshareindex] < 3){
	        gsharetable[gshareindex]++;
	    }
	    if(bimodaltable[sandbindex] < 3){
	        bimodaltable[sandbindex]++;
	    }
	    ghr = ghr << 1;
	    ghr++;
	}
	else if(b.behavior == "NT"){
	    if(gsharetable[gshareindex] > 0){
	        gsharetable[gshareindex]--;
	    }
	    if(bimodaltable[sandbindex] > 0){
	        bimodaltable[sandbindex]--;
	    }
	    ghr = ghr << 1;
	}
    }
    /*else if(z==0){
	if(selectortable[sandbindex] <= 1){
            if(gsharetable[gshareindex] >= 2 && b.behavior == "T"){
                if(bimodaltable[sandbindex] <= 1){
                    if(selectortable[sandbindex] > 0){
                        selectortable[sandbindex]--;
                    }
                }
            }
            else if(gsharetable[gshareindex] <= 1 && b.behavior == "T"){
                if(bimodaltable[sandbindex] >= 2){
                   if(selectortable[sandbindex] < 3){
                        selectortable[sandbindex]++;
                    }
                }
            }
	    else if(gsharetable[gshareindex] <= 1 && b.behavior == "NT"){
	        tournamentCorrect++;
	        if(bimodaltable[sandbindex] >= 2){
		     if(selectortable[sandbindex] > 0){
                        selectortable[sandbindex]--;
                    }
                }
		grmain++;
	    }
	    else if(gsharetable[gshareindex] >= 2 && b.behavior == "NT"){
	      if(sellow == 97 || sellow==98){
		cout <<"here4"<<endl;
	      }
	        if(bimodaltable[sandbindex] <= 1){
	            if(selectortable[sandbindex] < 3){
                        selectortable[sandbindex]++;
                    }

		    c6++;
                }
		gwmain++;
	    }
	    if(sellow>91 && sellow < 100){
	      cout << "g:"<< gsharetable[gshareindex] << " b:" << bimodaltable[sandbindex]<< " beh:" << b.behavior<< endl;
	      cout << c1 << " " << c2<< " "<< c3 << " " << c4<< " " << c5 <<" " << c6<< "??" << bimodaltable[1957]<< endl;
			}
	}
        else if(selectortable[sandbindex] >= 2){ //bimodal
	  	   selhigh++;
	   if(bimodaltable[sandbindex] >= 2 && b.behavior == "T"){
	       tournamentCorrect++;
	       if(gsharetable[gshareindex] <= 1){
                   if(selectortable[sandbindex] < 3){
                       selectortable[sandbindex]++;
                   }
               }
	       brmain++;
            }
            else if(bimodaltable[sandbindex] <= 1 && b.behavior == "T"){
                if(gsharetable[gshareindex] >= 2){
                   if(selectortable[sandbindex] > 0){
                        selectortable[sandbindex]--;
                    }
                }
		bwmain++;
            }
	    else if(bimodaltable[sandbindex] <= 1 && b.behavior == "NT"){
	        tournamentCorrect++;
	        if(gsharetable[gshareindex] >= 2){
		    if(selectortable[sandbindex] < 3){
                        selectortable[sandbindex]++;
                    }
                }
		brmain++;
	    }
	    else if(bimodaltable[sandbindex] >= 2 && b.behavior == "NT"){
	        if(gsharetable[gshareindex] <= 1){
	            if(selectortable[sandbindex] > 0){
                        selectortable[sandbindex]--;
                    }
                }
		bwmain++;
	    }
	   }
if(sellow == 97 || sellow==98){
		cout <<  " 1bb:" << bimodaltable[sandbindex]<< endl;
	      }
	if(b.behavior == "T"){
	      if(sellow == 97 || sellow==98){
		cout <<  " tbb:" << bimodaltable[sandbindex]<< endl;
	      }
	    if(gsharetable[gshareindex] < 3){
	        gsharetable[gshareindex]++;
	    }
	    if(bimodaltable[gshareindex] < 3){
	        bimodaltable[sandbindex]++;
	    }
	    ghr = ghr << 1;
	    ghr++;
	}
	else if(b.behavior == "NT"){
	  if(sellow == 97 || sellow==98){
		cout <<  " ntbb:" << bimodaltable[sandbindex]<< endl;
	      }
	    if(gsharetable[gshareindex] > 0){
	        gsharetable[gshareindex]--;
	    }
	    if(bimodaltable[gshareindex] > 0){
	        bimodaltable[sandbindex]--;
	    }
	    ghr = ghr << 1;
        }
	if(sellow == 97 || sellow==98){
		cout <<  " 2bb:" << bimodaltable[sandbindex]<< endl;
	}
	}
	}*/
    filew << tournamentCorrect << "," << v.size() << "; " << endl;
    //cout << sellow << " -" << selhigh << endl;
    //cout << "gr:" << grmain << " gw:" << gwmain << " br:" << brmain << " bw:"<< bwmain << endl;
    // cout << "a:" << a<< endl;
    //cout << c1 << " " << c2<< " "<< c3 << " " << c4<< " " << c5 <<" " << c6<< endl;*/
}

void btb(ofstream &filew, vector<branch> v){
    int accessnum = 0;
    const int entrynum = 512;
    int btbindex;
    int index;
    
    vector<bool> table(entrynum, true);
    vector<pair<unsigned long long, unsigned long long>> buff(128);

    for(branch b: v){
        btbindex = b.address % 128;
	index = b.address % 512;

	if(table[index] == true){
	    //if(buff[btbindex].first == b.address){
	    if(b.behavior == "NT"){
	        table[index] = false;
	    }
	    if(buff[btbindex].first == b.address && buff[btbindex].second == b.target){
	        btbCorrect++;
	    }
	    else{
	        buff[btbindex].first = b.address;
	        buff[btbindex].second = b.target;
	    }
	    accessnum++;
	}
	else if(table[index] == false){
	    if(b.behavior == "T"){
	        table[index] = true;
	    }
	}
    }
    filew << btbCorrect << "," << accessnum << ";" << endl;
}
	
      

int main (int argc, char *argv[]){
    
    if (argc != 3){
        cerr << "Usage: ./predictor <input_file.txt> <output_file.txt>\n";
        exit(1);
    }
    
    //infile = argv[1];
    //outfile = argv[2];
  //ifstream filer(infile);
    ifstream infile(argv[1]);
    if(infile){

    while(infile >> std::hex >> address >> behavior >> std::hex >> target){
        branch b; //create new branch that'll come from input file
	b.address = address; //put new branch info in each struct
       	b.behavior = behavior;
	b.target = target;
	branches.push_back(b);//each file one trace
	numofinstructions++;//IC in trace
    }
    
    infile.close();
    
    //totalnum += branches.size();
    ofstream filew(argv[2]);
    
    alwaystaken(filew, branches);
    alwaysnottaken(filew, branches);
    bimodalsinglebit(filew, branches, 16);
    bimodalsinglebit(filew, branches, 32);
    bimodalsinglebit(filew, branches, 128);
    bimodalsinglebit(filew, branches, 256);
    bimodalsinglebit(filew, branches, 512);
    bimodalsinglebit(filew, branches, 1024);
    bimodalsinglebit(filew, branches, 2048);

    bimodaltwobit(filew, branches, 16);
    bimodaltwobit(filew, branches, 32);
    bimodaltwobit(filew, branches, 128);
    bimodaltwobit(filew, branches, 256);
    bimodaltwobit(filew, branches, 512);
    bimodaltwobit(filew, branches, 1024);
    bimodaltwobit(filew, branches, 2048);

    gshare(filew, branches, 3);
    gshare(filew, branches, 4);
    gshare(filew, branches, 5);
    gshare(filew, branches, 6);
    gshare(filew, branches, 7);
    gshare(filew, branches, 8);
    gshare(filew, branches, 9);
    gshare(filew, branches, 10);
    gshare(filew, branches, 11);

    tournament(filew, branches);
    // tournament(filew, branches, 0);

    btb(filew, branches);

    return 0;
    }
    else{
        cout << "Sorry! Your input file doesn't exist!" << endl;
        exit(1);
    }
}
