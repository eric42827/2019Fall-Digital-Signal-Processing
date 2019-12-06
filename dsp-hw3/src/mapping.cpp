#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

int main(int argc, char *argv[]){
    //ifstream fin;
    ofstream fout(argv[2]);
    ifstream fin;
    //fin.open(argv[1],ios::in);

    string line;
    char zhu[37][2];
    for (int i = 0;i<37;i++){
        zhu[i][0]=char(0xA3);
        if(i>=0&&i<11){
            zhu[i][1]=char(0x74+i);
        }
        else if(i>=11&&i<26){
            zhu[i][1]=char(0xA1+i-11);
        }
        else if(i>=26&&i<37){
            zhu[i][1]=char(0xB0+i-26);
        }
        //fout << zhu[i][0]<<zhu[i][1]<<endl;
    }
    
    for(int i = 0;i<37;i++){
        fin.open(argv[1],ios::in);
        bool check=false;
        vector<string> total;
        while(getline(fin,line,'\n')){
            string data;
            string word;
            istringstream temp(line);
            getline(temp,data,' ');
            word = data;
            //fout << data << '\n';
            while(getline(temp,data,'/')){
                if(data[0]==zhu[i][0]&&data[1]==zhu[i][1]){
                    if(check==false){
                        check=true;
                        fout << zhu[i][0] << zhu[i][1];
                    }                    
                    fout <<" "<< word;
                    total.push_back(word);
                }
            }
        }
        if(check==true)
            fout << endl;
        int len = 0; 
        while(len < total.size()){
            fout << total[len] << " " << total[len] << endl;
            len++;
        }
        fin.close();
    }    
    
//    ostringstream out;
    //string data;
   // getline(temp,data,' ');
 //   getline(temp,data,'/');
        //fin.get(*seq);
    //fout << data;
        
    fin.close();
    fout.close();
    //temp.close();
}