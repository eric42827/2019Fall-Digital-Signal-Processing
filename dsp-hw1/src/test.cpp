#include "hmm.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <string.h>
#define T_num 50
#define M_num 5
#define N_num 6
#define O_num 6
#define Sample 2500
using namespace std;
int main(int argc, char *argv[]) {
  char *model_list_name = argv[1];
  char *testing_data_name = argv[2];
  char *result_name = argv[3];
  HMM models[M_num];
  int num_models = load_models(model_list_name, models, M_num);
  ifstream fin;
  ofstream fout;
  string seq;
  fin.open(testing_data_name, ios::in);
  fout.open(result_name);
  if (fin) {
    while (getline(fin, seq)) {
      int obs[T_num];
      for (int t = 0; t < T_num; t++)
        obs[t] = seq[t] - 'A';
      // Record which model has max. likelihood.
      int max_likelihood_model_index = -1;
      // Record max. likelihood.
      double max_likelihood = 0;
      double delta[T_num][N_num];

      // Test
      for (int model_index = 0; model_index < num_models; model_index++) {
        HMM model = models[model_index];

        // Initial
        for (int i = 0; i < model.state_num; i++) {
          delta[0][i] = model.initial[i] * model.observation[obs[0]][i];
        }

        // Recursion
        for (int t = 1; t < T_num; t++) {
          for (int j = 0; j < model.state_num; j++) {
            // find max
            double max_path_prob = 0.0;
            for (int i = 0; i < model.state_num; i++) {
              // Calculate path probability of next time step.
              double path_prob = delta[t - 1][i] * model.transition[i][j];
              // Replace max
              if (max_path_prob < path_prob)
                max_path_prob = path_prob;
            }
            // Update delta with max. path probability.
            delta[t][j] = max_path_prob * model.observation[obs[t]][j];
          }
        }

        // Termination
        double max_path_prob = 0.0;
        for (int i = 0; i < model.state_num; i++) {
          // Replace max prob
          if (max_path_prob < delta[T_num - 1][i])
            max_path_prob = delta[T_num - 1][i];
        }

        // Update max. likelihood and model index.
        if (max_likelihood < max_path_prob) {
          max_likelihood = max_path_prob;
          max_likelihood_model_index = model_index;
        }
      }
      fout << models[max_likelihood_model_index].model_name << " "
           << max_likelihood << '\n';
    }
  }

  return 0;
}