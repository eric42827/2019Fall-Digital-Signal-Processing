#include <iostream>
#include "hmm.h"
#include <math.h>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]){
    
    int iter = atoi(argv[1]);
    //cout  <<  iter << endl;

    HMM hmm_initial;
    loadHMM( &hmm_initial, argv[2] );
    //dumpHMM( stdout, &hmm_initial );
    ifstream fin;
    string seq;
    fin.open(argv[3] , ios::in);
    if(fin){
        while( getline(fin,seq) ){
            cout << seq ;
            cout << '\n';
        }
        
    }
    fin.close();





    ofstream fout(argv[4]);
    if(fout){
        fout << "initial: " << hmm_initial.state_num << '\n';
        for(int i = 0;i < hmm_initial.state_num-1;i++){
            fout << hmm_initial.initial[i] << " ";
        }
        fout << hmm_initial.initial[hmm_initial.state_num-1] << '\n' << '\n';

        fout << "transition: " << hmm_initial.state_num << '\n';
        for(int i = 0;i < hmm_initial.state_num;i++){
            for(int j = 0;j < hmm_initial.state_num-1;j++){
                fout << hmm_initial.transition[i][j] << " ";
            }
             fout << hmm_initial.transition[i][hmm_initial.state_num-1] << '\n' ;
        }
        fout << '\n';

        fout << "observation: " << hmm_initial.observ_num << '\n';
        for(int i = 0;i < hmm_initial.observ_num;i++){
            for(int j = 0;j < hmm_initial.state_num-1;j++){
                fout << hmm_initial.observation[i][j] << " ";
            }
             fout << hmm_initial.observation[i][hmm_initial.state_num-1] << '\n' ;
        }

    }
    fout.close();



//   for(int i = 0;i < argc;i++){
//       cout << argv[i]<< endl;
//   }

    return 0;
}