/*
./mydisambig $1 $2 $3 $4
$1 segemented file to be decoded
$2 ZhuYin-Big5 mapping
$3 language model
$4 output file
*/
#include "Ngram.h"
#include <iostream>
//#include "Vocab.h"
//#include "File.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
class Duck {
private:
  char zhu[37][2];
  ifstream fin;
  void build_zhu() {
    for (int i = 0; i < 37; i++) {
      zhu[i][0] = char(0xA3);
      if (i >= 0 && i < 11) {
        zhu[i][1] = char(0x74 + i);
      } else if (i >= 11 && i < 26) {
        zhu[i][1] = char(0xA1 + i - 11);
      } else if (i >= 26 && i < 37) {
        zhu[i][1] = char(0xB0 + i - 26);
      }
      // fout << zhu[i][0]<<zhu[i][1]<<endl;
    }
  }
  Vocab voc;
  Ngram lm;
  bool czhu(char c1, char c2) {
    for (int i = 0; i < 37; i++) {
      if (c1 == zhu[i][0] && c2 == zhu[i][1]) {
        return true;
      }
    }
    return false;
  }
  vector<string> backtrack(vector<vector<LogP>> delta, vector<int> state_len,
                           vector<vector<string>> chinese,
                           vector<vector<int>> optimal, int size) {
    vector<string> str;
    str.reserve(size);
    int pre = 0;
    for (int i = size - 1; i >= 0; i--) { // backtrack from last bit

      str.push_back(chinese[i][pre]);
      // cout << str[size - 1 - i] << " ";
      pre = optimal[i][pre];
      // if (pre == -1)
    }
    // cout << endl;
    return str;
  }
  VocabIndex getindex(VocabIndex wid) {
    if (wid == Vocab_None) {
      // replace OOV with <unk>
      wid = voc.getIndex(Vocab_Unknown);
    }
    return wid;
  }

public:
  char **_argv;
  Duck() : lm(voc, 2) {}
  // Duck(char **argv){
  /*_argv = argv;
  char* a= argv[3];
  Ngram lm(voc,2);
  File lmFile( a, "r" );                                  //open lm file and
  read prob lm.read(lmFile); lmFile.close(); build_zhu(); //bulid zhu
  fin.open(argv[2],ios::in);*/                              //open map
  //}
  ~Duck() {}
  void initial(char **argv) {
    _argv = argv;
    char *a = argv[3];
    // Ngram * lm(voc,2);
    File lmFile(a, "r");
    lm.read(lmFile); //
    lmFile.close();
    build_zhu();
    // fin.open(argv[2], ios::in);
  }
  // void terminate(){// fin.close(); }

  vector<string> viterbi(string s) { // real len is i/2!!!

    vector<vector<LogP>> delta;
    vector<vector<int>> optimal;
    vector<vector<string>> chinese;
    vector<int> state_len;
    state_len.reserve(s.size() / 2 + 2);
    delta.reserve(s.size() / 2 + 2);
    chinese.reserve(s.size() / 2 + 2);
    optimal.reserve(s.size() / 2 + 2);
    // s=0
    vector<string> schinese;
    vector<int> soptimal;
    vector<LogP> sdelta;
    schinese.emplace_back("<s>");
    chinese.push_back(schinese);
    sdelta.push_back(0);
    delta.push_back(sdelta);
    state_len.push_back(1);
    soptimal.push_back(-1);
    optimal.push_back(soptimal);

    // sentence
    for (int i = 0; i < s.size(); i += 2) { // cal delta bigram

      vector<string> zhuchinese;
      vector<int> zhuoptimal;
      vector<LogP> zhudelta;
      if (czhu(s[i], s[i + 1])) { // current is zhu
        fin.open(_argv[2], ios::in);
        // string zhuchinese;                            //store every possible
        // chinese
        string buffer;
        while (getline(fin, buffer, '\n')) { // read .map to find zhu
          if (s[i] == buffer[0] && s[i + 1] == buffer[1]) { // first char is zhu
            istringstream temp(buffer);
            string data;
            getline(temp, data, ' ');          // skip first zhu
            while (getline(temp, data, ' ')) { // put chinese into vec
              zhuchinese.push_back(data);
              // cout << data << " ";
            }
            // cout << endl;
            break; // only one zhu is match
          }
        }
        /*store para to global*/
        // cout << zhuchinese.size() << endl;
        chinese.push_back(zhuchinese);
        // push every possible zhuchinese to chinese
        zhudelta.reserve(zhuchinese.size()); // bulid probably
        zhuoptimal.reserve(zhuchinese.size());
        // store # of chinese map to this zhu
        state_len.push_back(zhuchinese.size());
        /*2 case analysis*/
        if (i >= 2 && czhu(s[i - 2], s[i - 1])) {          // both two are zhu
          for (int k = 0; k < state_len[i / 2 + 1]; k++) { // k for current zhu
            for (int j = 0; j < state_len[i / 2 + 1 - 1]; j++) {
              // pre are zhu (j for state len of pre)
              VocabIndex context[] = // qt-1
                  {getindex(voc.getIndex(chinese[i / 2 - 1 + 1][j].c_str())),
                   Vocab_None};
              LogP temp;
              temp = delta[i / 2 - 1 + 1][j] +
                     lm.wordProb(
                         getindex(voc.getIndex(chinese[i / 2 + 1][k].c_str())),
                         context);
              if (j == 0) {
                zhudelta.push_back(temp);
                zhuoptimal.push_back(j);
              } else if (temp > zhudelta[k]) {
                zhudelta[k] = temp;
                zhuoptimal[k] = j;
              }
            }
            // cout << optimal[i / 2 + 1][k] << endl;
          }
        } else {                 // only current are zhu
          VocabIndex context[] = // qt-1(pre are chinese)
              {getindex(voc.getIndex(chinese[i / 2 - 1 + 1][0].c_str())),
               Vocab_None};
          for (int k = 0; k < state_len[i / 2 + 1]; k++) {
            // every zhu has it's delta
            zhudelta.push_back(delta[i / 2 - 1 + 1][0] +
                               lm.wordProb(getindex(voc.getIndex(
                                               chinese[i / 2 + 1][k].c_str())),
                                           context));
            zhuoptimal.push_back(0);
            // cout << optimal[i / 2 + 1][k] << endl;
            // cout << delta[i / 2 + 1][k] << endl;
          }
        }
        fin.close();
      } else if (i >= 2 && czhu(s[i - 2], s[i - 1])) { // only pre are zhu
        zhuchinese.emplace_back(s, i, 2);              // put word into vec
        chinese.push_back(zhuchinese);
        state_len.push_back(1); // len 1
        LogP temp;
        for (int j = 0; j < state_len[i / 2 + 1 - 1]; j++) { // j for pre len
          VocabIndex context[] =                             // qt-1
              {getindex(voc.getIndex(chinese[i / 2 + 1 - 1][j].c_str())),
               Vocab_None};
          temp =
              delta[i / 2 + 1 - 1][j] // delta
              +
              lm.wordProb(getindex(voc.getIndex(chinese[i / 2 + 1][0].c_str())),
                          context);
          if (j == 0) {
            zhudelta.push_back(temp);
            zhuoptimal.push_back(j);
          }

          else if (temp > zhudelta[0]) { // max
            zhudelta[0] = temp;
            zhuoptimal[0] = j;
            // cout << "j: " << j << endl;
          }
        }
        // backtrack
      } else {                  // word & s /s
        state_len.push_back(1); // len 1
        // maintain
        // string combine = string() + s[i] + s[i+1];        //2 big5 char into
        // word
        zhuchinese.emplace_back(s, i, 2); // put into vec
        chinese.push_back(zhuchinese);
        VocabIndex context[] = {
            getindex(voc.getIndex(chinese[i / 2 - 1 + 1][0].c_str())),
            Vocab_None};
        zhudelta.push_back(
            delta[i / 2 + 1 - 1][0] +
            lm.wordProb(getindex(voc.getIndex(chinese[i / 2 + 1][0].c_str())),
                        context));
        zhuoptimal.push_back(0);
      }
      optimal.push_back(zhuoptimal);
      delta.push_back(zhudelta);
    }
    // s=end
    vector<string> echinese;
    vector<int> eoptimal;
    vector<LogP> edelta;
    echinese.emplace_back("</s>");
    chinese.push_back(echinese);
    state_len.push_back(1);
    for (int i = 0; i < state_len[s.size() / 2]; i++) {
      VocabIndex context[] = {
          getindex(voc.getIndex(chinese[s.size() / 2][i].c_str())), Vocab_None};
      LogP temp;
      temp =
          delta[s.size() / 2][i] + lm.wordProb(voc.getIndex("</s>"), context);
      if (i == 0) {
        edelta.push_back(temp);
        eoptimal.push_back(i);
      } else if (temp > edelta[0]) {
        edelta[0] = temp;
        eoptimal[0] = i;
        // cout << "i: " << i << endl;
      }
    }
    delta.push_back(edelta);
    optimal.push_back(eoptimal);
    // end
    // cout << "fuck" << endl;
    // sentence
    // fout.close();

    return backtrack(delta, state_len, chinese, optimal, s.size() / 2 + 2);
  }
};

int main(int argc, char *argv[]) {

  ifstream fin;
  ofstream fout("./buffer.txt");
  fin.open(argv[1], ios::in);
  string line;
  while (getline(fin, line, '\n')) {
    string data;
    istringstream temp(line);
    while (getline(temp, data, ' ')) {
      fout << data;
    }
    fout << '\n';
  }

  fin.close();
  fout.close();
  // cout << "2";
  Duck duck;
  duck.initial(argv);

  fin.open("./buffer.txt");
  ofstream fout2(argv[4]);
  while (getline(fin, line, '\n')) {
    vector<string> res = duck.viterbi(line);
    for (int i = res.size() - 1; i >= 0; i--) { // print sectence
      fout2 << res[i];
      if (i == 0) {
        fout2 << '\n';
      } else {
        fout2 << " ";
      }
    }
  }
  // duck.terminate();
  fin.close();
  fout2.close();
}