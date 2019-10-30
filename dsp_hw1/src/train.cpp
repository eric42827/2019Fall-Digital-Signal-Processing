#include <iostream>
#include "hmm.h"
#include <math.h>
#include <fstream>
#define T_num 50
#define N_num 6
using namespace std;

class Calculate
{
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
    double epos[T_num][N_num][N_num];
    double sumgamma[N_num];
    double sumepos[N_num][N_num];

    double al_ter;
    string _s;
    char **_argv;

public:
    Calculate() {}
    Calculate(char **argv)
    {
        _argv = argv;
        loadHMM(&_hmm, argv[2]);
    }
    void load_string(string s);
    void output();
    void Baum_Welch();
    void Build_model();
};

int main(int argc, char *argv[])
{

    Calculate cal(argv);
    int iter = atoi(argv[1]);
    //cout  <<  iter << endl;

    //dumpHMM( stdout, &hmm_initial );
    ifstream fin;
    string seq;
    //double alpha[T_num][N_num];
    //double beta[T_num][N_num];
    //input
    fin.open(argv[3], ios::in);
    if (fin)
    {
        while (getline(fin, seq))
        {
            cal.load_string(seq);
            cal.Baum_Welch();
        }
        cal.Build_model();
        //for(int i = 0; i <T_num;i++)
        //cout << seq[i] << " ";
        //cout << '\n';

        //double a = cal_alpha(seq,hmm_initial);
        //cout << "a: " << a;
        /*while (getline(fin, seq))
        {

            cal_alpha(seq,hmm_initial);
            //cout << seq[0];
            //cout << '\n';
        }*/
    }
    fin.close();

    cal.output();

    return 0;
    //output

    //   for(int i = 0;i < argc;i++){
    //       cout << argv[i]<< endl;
    //   }
}

void Calculate::Baum_Welch()
{
    forward();
    //backward();
    //calgamma();
    //calepos();
    //accugamma();
    //accuepos();
}
void Calculate::load_string(string s)
{
    _s = s;
    //cout << _s << '\n';
}

void Calculate::forward()
{
    for (int i = 0; i < T_num; i++)for(int j = 0; j < _hmm.state_num; j++)alpha[i][j]=0.0;
    al_ter = 0.0;
    //cout << '\n';

    for (int i = 0; i < T_num; i++)
    { //ocr num 50
        int ocu = _s[i] - 'A';
        //cout << ocu << ' ';
        //cout << "i: " << i << endl;
        for (int j = 0; j < _hmm.state_num; j++)
        { //state num  6
            if (i == 0)
            {
                alpha[i][j] = _hmm.initial[j] * _hmm.observation[ocu][j]; //alphai(j)
                //cout << alpha[i][j] << " ";
            }
            else
            {
                for (int k = 0; k < _hmm.state_num; k++)
                {
                    double temp = alpha[i - 1][k] * _hmm.transition[k][j];
                    alpha[i][j] += temp;
                    //cout << h.transition[k][j] << " ";
                }
                //cout << '\n';
                //cout << alpha[i][j] << " ";
                alpha[i][j] *= _hmm.observation[ocu][j]; //bj(ocu)
            }
        }
        //cout << "\n";
    }
    //cout << '\n';
    for (int i = 0; i < _hmm.state_num; i++)
    {
        //cout << alpha[T_num][i] << " ";
        al_ter += alpha[T_num - 1][i];
        cout << al_ter <<" ";
    }
    cout << '\n';
    //cout << "sum: ";
}

void Calculate::backward()
{
    for (int tim = T_num - 1; tim >= 0; tim--)
    { //time
        int pocu = -1;
        if (tim != T_num - 1)
            pocu = _s[tim + 1] - 'A';
        for (int sta = 0; sta < _hmm.state_num; sta++)
        { //state
            if (tim == T_num - 1)
                beta[tim][sta] = 1;
            else
            {
                for (int pst = 0; pst < _hmm.state_num; pst++)
                { //post state
                    beta[tim][sta] += (_hmm.transition[sta][pst] * _hmm.observation[pocu][pst] * beta[tim][pst]);
                }
            }
        }
    }
}

void Calculate::calgamma()
{
    for (int tim = 0; tim < T_num; tim++)
    {
        double sum;
        for (int sta = 0; sta < N_num; sta++)
        {
            sum += (alpha[tim][sta] * beta[tim][sta]);
        }
        for (int sta = 0; sta < N_num; sta++)
        {
            gamma[tim][sta] = alpha[tim][sta] * beta[tim][sta] / sum;
        }
    }
}
void Calculate::calepos()
{
    double sum;
    for (int tim = 0; tim < T_num - 1; tim++)
    {
        int pocu = _s[tim + 1];
        double sum;
        for (int i = 0; i < N_num; i++)
        {
            for (int j = 0; j < N_num; j++)
            {
                sum += alpha[tim][i] * _hmm.transition[i][j] * _hmm.observation[pocu][j] * beta[tim + 1][j];
            }
        }
        for (int i = 0; i < N_num; i++)
        {
            for (int j = 0; j < N_num; j++)
            {
                epos[tim][i][j] = alpha[tim][i] * _hmm.transition[i][j] * _hmm.observation[pocu][j] * beta[tim + 1][j] / sum;
            }
        }
    }
}
void Calculate::accugamma()
{
    for (int i; i < N_num; i++)
    {
        for (int tim = 0; tim < T_num - 1; tim++)
        {
            sumgamma[i] += gamma[tim][i];
        }
    }
}
void Calculate::accuepos()
{
    for (int i = 0; i < N_num; i++)
    {
        for (int j; j < N_num; j++)
        {
            for (int tim = 0; tim < T_num - 1; tim++)
            {
                sumepos[i][j] += epos[tim][i][j];
            }
        }
    }
}
void Calculate::Build_model()
{ //TODO
    for (int i; i < _hmm.state_num; i++)
    {
        //_hmm.initial = sumgamma[i];
    }
}
void Calculate::output()
{
    ofstream fout(_argv[4]);
    if (fout)
    {
        fout << "initial: " << _hmm.state_num << '\n';
        for (int i = 0; i < _hmm.state_num - 1; i++)
        {
            fout << _hmm.initial[i] << " ";
        }
        fout << _hmm.initial[_hmm.state_num - 1] << '\n'
             << '\n';

        fout << "transition: " << _hmm.state_num << '\n';
        for (int i = 0; i < _hmm.state_num; i++)
        {
            for (int j = 0; j < _hmm.state_num - 1; j++)
            {
                fout << _hmm.transition[i][j] << " ";
            }
            fout << _hmm.transition[i][_hmm.state_num - 1] << '\n';
        }
        fout << '\n';

        fout << "observation: " << _hmm.observ_num << '\n';
        for (int i = 0; i < _hmm.observ_num; i++)
        {
            for (int j = 0; j < _hmm.state_num - 1; j++)
            {
                fout << _hmm.observation[i][j] << " ";
            }
            fout << _hmm.observation[i][_hmm.state_num - 1] << '\n';
        }
    }
    fout.close();
}