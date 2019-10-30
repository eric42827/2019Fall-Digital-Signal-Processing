#include <iostream>
#include "hmm.h"
#include <math.h>
#include <fstream>
#define T_num 50 
#define N_num 6 


using namespace std;
void  cal_alpha(string s,double**alpha );
int main(int argc, char *argv[]){
    
    int iter = atoi(argv[1]);
    //cout  <<  iter << endl;

    HMM hmm_initial;
    loadHMM( &hmm_initial, argv[2] );
    //dumpHMM( stdout, &hmm_initial );
    ifstream fin;
    string seq;
    double alpha[T_num][N_num];
    double beta[T_num][N_num];


    fin.open(argv[3] , ios::in);
    if(fin){
        while(getline(fin,seq) ){
            
            cout << seq[0] ;
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

double cal_alpha(string s,HMM h){
    double alpha [h.observ_num][h.state_num];
    for(int i = 0;i < h.observ_num;i++){//ocr num 50
        for(int j = 0; j < h.state_num;j++){//state num  6
            int ocu = s[i] - 'A';
            if(i ==1){
                alpha[i][j] = h.initial[j]*h.observation[ocu][j];
            }
            else if (i < h.observ_num-1)
            {
                for(int k = 0;k < h.state_num;k++){
                    alpha[i][j] += (alpha[i-1][k]*h.transition[k][j]);
                    alpha[i][j] *= 
                }
                
            }
            else{

            }
        }
    }

}