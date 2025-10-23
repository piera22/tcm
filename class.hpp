#ifndef CLASS_HPP
#define CLASS_HPP

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TRandom.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <vector>
#include <numeric>

class Simulation {
  int N;
  int bins;
  float k;
  float k_err;
  float phi;
  float phi_err;
  float b;
  float b_err;
  TF1* function;
  TF1* norm_function;
  TH1F* histo_theory;
  TCanvas* canvas;

 public:
  Simulation(int N_, int bins_);
  double get_mean(std::vector<double> vec);
  double get_std_dev(std::vector<double> vec);
  std::vector<double> error_propagation();
  double random_extraction();  // estrae un numero in base alla funzione
  TH1F accumulate_random();
  //TF1* chi_confrontation(int N, int bins);
  TF1* scale_function_to_histogram();
  void draw_all();
  void regen_unc(int N_regen);
  void bin_smeering(int N_regen);
};

#endif