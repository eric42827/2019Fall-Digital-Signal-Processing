#include "hmm.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#define T_num 50
#define N_num 6
#define O_num 6
#define Sample 10000
using namespace std;

class Calculate {
private:
  void forward();
  void backward();
  void calgamma();
  void calepos();
  void accugamma();
  void accuepos();

  HMM _hmm;
  double alpha[T_num][N_num];
  double beta[T_num][N_num];
  double gamma[T_num][N_num];
  double epos[T_num - 1][N_num][N_num];
  double sumgamma[N_num];
  double sumgamma_o[N_num][O_num];
  double sumepos[N_num][N_num];
  // cross Sample
  double pigamma[N_num];
  double tsumgamma[N_num];
  double tsumgamma_1[N_num];
  double tsumgamma_o[N_num][O_num];
  double tsumepos[N_num][N_num];

  double al_ter;
  string _s;
  char **_argv;

public:
  Calculate() {}
  Calculate(char **argv) {
    _argv = argv;
    loadHMM(&_hmm, argv[2]);
  }
  void initial() {
    for (int i = 0; i < N_num; i++) {
      tsumgamma[i] = 0;
      tsumgamma_1[i] = 0;
      pigamma[i] = 0;
      for (int j = 0; j < N_num; j++) {
        tsumepos[i][j] = 0;
        sumgamma_o[i][j] = 0;
      }
    }
  }
  void load_string(string s);
  void output();
  void Baum_Welch();
  void Build_model();
};

int main(int argc, char *argv[]) {

  Calculate cal(argv);
  // int iter = atoi(argv[1]);
  // cout  <<  iter << endl;

  // dumpHMM( stdout, &hmm_initial );
  ifstream fin;
  string seq;
  // double alpha[T_num][N_num];
  // double beta[T_num][N_num];
  // input
  for (int iter = 0; iter < atoi(argv[1]); iter++) {
    fin.open(argv[3], ios::in);

    if (fin) {
      // getline(fin, seq);
      // cal.load_string(seq);
      // cal.Baum_Welch();
      cal.initial();
      while (getline(fin, seq)) {
        cal.load_string(seq);
        cal.Baum_Welch();
      }
      cal.Build_model();
      // for(int i = 0; i <T_num;i++)
      // cout << seq[i] << " ";
      // cout << '\n';

      // double a = cal_alpha(seq,hmm_initial);
      // cout << "a: " << a;
      /*while (getline(fin, seq))
      {

          cal_alpha(seq,hmm_initial);
          //cout << seq[0];
          //cout << '\n';
      }*/
    }

    fin.close();
  }
  cal.output();

  return 0;
  // output

  //   for(int i = 0;i < argc;i++){
  //       cout << argv[i]<< endl;
  //   }
}

void Calculate::Baum_Welch() {
  forward();
  backward();
  calgamma();
  calepos();
  accugamma();
  accuepos();
}
void Calculate::load_string(string s) {
  _s = s;
  // cout << _s << '\n';
}

void Calculate::forward() {
  for (int i = 0; i < T_num; i++)
    for (int j = 0; j < _hmm.state_num; j++)
      alpha[i][j] = 0.0;
  al_ter = 0.0;
  // cout << '\n';

  for (int i = 0; i < T_num; i++) { // ocr num 50
    int ocu = _s[i] - 'A';
    // cout << ocu << ' ';
    // cout << "i: " << i << endl;
    for (int j = 0; j < _hmm.state_num; j++) { // state num  6
      if (i == 0) {
        alpha[i][j] = _hmm.initial[j] * _hmm.observation[ocu][j]; // alphai(j)
        // cout << alpha[i][j] << " ";
      } else {
        for (int k = 0; k < _hmm.state_num; k++) {
          double temp = alpha[i - 1][k] * _hmm.transition[k][j];
          alpha[i][j] += temp;
          // cout << h.transition[k][j] << " ";
        }
        // cout << '\n';
        // cout << alpha[i][j] << " ";
        alpha[i][j] *= _hmm.observation[ocu][j]; // bj(ocu)
      }
    }
    // cout << "\n";
  }
  // cout << '\n';
  for (int i = 0; i < _hmm.state_num; i++) {
    // cout << alpha[T_num][i] << " ";
    al_ter += alpha[T_num - 1][i];
    // cout << al_ter << " ";
  }
  // cout << '\n';
  // cout << "sum: ";
}

void Calculate::backward() {
  for (int tim = T_num - 1; tim >= 0; tim--)
    for (int sta = 0; sta < _hmm.state_num; sta++)
      beta[tim][sta] = 0;
  for (int sta = 0; sta < N_num; sta++) {
    beta[T_num - 1][sta] = 1;
  }
  for (int tim = T_num - 2; tim >= 0; tim--) { // time
    for (int i = 0; i < N_num; i++) {
      double sum = 0;
      for (int j = 0; j < N_num; j++) {
        sum += _hmm.transition[i][j] * _hmm.observation[_s[tim + 1] - 'A'][j] *
               beta[tim + 1][j];
      }
      beta[tim][i] = sum;
    }
  }
}

void Calculate::calgamma() {
  for (int tim = 0; tim <= T_num; tim++)
    for (int sta = 0; sta < _hmm.state_num; sta++)
      gamma[tim][sta] = 0;
  for (int tim = 0; tim < T_num; tim++) {
    double sum = 0.0;
    // cout << "t: "<<tim <<"  ";
    for (int sta = 0; sta < N_num; sta++) {
      // double temp = alpha[tim][sta] * beta[tim][sta];
      // cout << temp << " ";
      sum += (alpha[tim][sta] * beta[tim][sta]);
    }
    // cout << "sum: "<<sum <<" ";
    for (int sta = 0; sta < N_num; sta++) {
      gamma[tim][sta] = alpha[tim][sta] * beta[tim][sta] / sum;
      if (tim == 0)
        pigamma[sta] += gamma[tim][sta];
      // cout <<gamma[tim][sta];
    }
    // cout << endl;
  }
}
void Calculate::calepos() {
  for (int tim = 0; tim < T_num - 1; tim++)
    for (int sta = 0; sta < _hmm.state_num; sta++)
      for (int pst = 0; pst < _hmm.state_num; pst++)
        epos[tim][sta][pst] = 0;

  for (int tim = 0; tim < T_num - 1; tim++) {
    int pocu = _s[tim + 1] - 'A';
    double sum = 0.0;
    for (int i = 0; i < N_num; i++) {
      for (int j = 0; j < N_num; j++) {
        sum += alpha[tim][i] * _hmm.transition[i][j] *
               _hmm.observation[pocu][j] * beta[tim + 1][j];
      }
    }
    for (int i = 0; i < N_num; i++) {
      for (int j = 0; j < N_num; j++) {
        epos[tim][i][j] = alpha[tim][i] * _hmm.transition[i][j] *
                          _hmm.observation[pocu][j] * beta[tim + 1][j] / sum;
        // epos[T_num][i][j] / gamma[T_num][i]
      }
      // cout << '\n';
    }
  }
}
void Calculate::accugamma() {
  for (int i = 0; i < N_num; i++)
    sumgamma[i] = 0;
  /*for (int i = 0; i < N_num; i++)
    for (int o = 0; o < O_num; o++)
      sumgamma_o[i][o] = 0;
  */
  for (int i = 0; i < N_num; i++) {
    for (int tim = 0; tim < T_num; tim++) {
      sumgamma[i] += gamma[tim][i]; // in an ex
      sumgamma_o[i][_s[tim] - 'A'] += gamma[tim][i];
      if (tim == T_num - 2)
        tsumgamma_1[i] += sumgamma[i]; // in total ex
      /*if (tim == T_num - 1) {
        for (int j = 0; j < O_num; j++)
          tsumgamma_o[i][j] += sumgamma_o[i][j];
      }*/
    }

    tsumgamma[i] += sumgamma[i];
  }
}

void Calculate::accuepos() {
  for (int i = 0; i < N_num; i++)
    for (int j = 0; j < N_num; j++)
      sumepos[i][j] = 0;
  for (int i = 0; i < N_num; i++) {
    for (int j = 0; j < N_num; j++) {
      for (int tim = 0; tim < T_num - 1; tim++) {
        sumepos[i][j] += epos[tim][i][j];
      }
      tsumepos[i][j] += sumepos[i][j];
    }
  }
}
void Calculate::Build_model() { // TODO

  for (int i = 0; i < _hmm.state_num; i++) // pi
    _hmm.initial[i] = pigamma[i] / Sample;

  for (int i = 0; i < N_num; i++) // a
    for (int j = 0; j < N_num; j++)
      _hmm.transition[i][j] = tsumepos[i][j] / tsumgamma_1[i];

  for (int k = 0; k < _hmm.observ_num; k++) // b
    for (int i = 0; i < _hmm.state_num; i++)
      _hmm.observation[k][i] = sumgamma_o[i][k] / tsumgamma[i];
}
void Calculate::output() {
  ofstream fout(_argv[4]);
  if (fout) {
    fout << "initial: " << _hmm.state_num << '\n';
    for (int i = 0; i < _hmm.state_num - 1; i++) {
      fout << fixed << setprecision(5) << _hmm.initial[i] << " ";
    }
    fout << fixed << setprecision(5) << _hmm.initial[_hmm.state_num - 1] << '\n'
         << '\n';

    fout << "transition: " << _hmm.state_num << '\n';
    for (int i = 0; i < _hmm.state_num; i++) {
      for (int j = 0; j < _hmm.state_num - 1; j++) {
        fout << fixed << setprecision(5) << _hmm.transition[i][j] << " ";
      }
      fout << fixed << setprecision(5) << _hmm.transition[i][_hmm.state_num - 1]
           << '\n';
    }
    fout << '\n';

    fout << "observation: " << _hmm.observ_num << '\n';
    for (int i = 0; i < _hmm.observ_num; i++) {
      for (int j = 0; j < _hmm.state_num - 1; j++) {
        fout << fixed << setprecision(5) << _hmm.observation[i][j] << " ";
      }
      fout << fixed << setprecision(5)
           << _hmm.observation[i][_hmm.state_num - 1] << '\n';
    }
  }
  fout.close();
}