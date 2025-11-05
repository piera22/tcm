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
  TF1* function_random_pars;
  TH1F* histo_theory;
  TCanvas* canvas;

 public:
  Simulation(int N_, int bins_);
  double get_mean(std::vector<double> vec);
  double get_std_dev(std::vector<double> vec);
  std::vector<double> error_propagation();
  TH1F accumulate_random();
  TH1F accumulate_random_pars();
  //TF1* chi_confrontation(int N, int bins);
  void draw_all();
  double regen_unc(int N_regen);
  double bin_smeering();
  double regen_unc_randompars(int N_regen);
  double bin_smeering_randompars();
  void sigma_confrontation(int N_regen);
  void sigma_confrontation_randompars(int N_regen);
};

#endif