#include "class.hpp"

#include <cmath>
#include <iostream>
#include <vector>
Simulation::Simulation(int N_, int bins_) {
  N = N_;
  bins = bins_;

  k = 5.2;
  k_err = 0.104;
  phi = 1.8;
  phi_err = 0.09;
  b = 0.2;
  b_err = 0.002;

  function = new TF1("function", "((cos([0]*x+[1]))^2 + [2])", 0., (3.14 / k));
  function->SetParameters(k, phi, b);
  function->SetLineColor(kRed);
  function->SetLineWidth(2);
  norm_function = new TF1("norm_function", "((cos([0]*x+[1]))^2 + [2])/[3]", 0.,
                          (3.14 / k));
  norm_function->SetParameters(
      k, phi, b, function->Integral(function->GetXmin(), function->GetXmax()));

  function_random_pars = new TF1("function_random_pars",
                                 "((cos([0]*x+[1]))^2 + [2])", 0., (3.14 / k));
  canvas = new TCanvas("canvas", "Distribuzione", 0, 0, 800, 600);
  double xmin = function->GetXmin();
  double xmax = function->GetXmax();
  double bin_width = (xmax - xmin) / bins;
  double norm = function->Integral(xmin, xmax);

  histo_theory =
      new TH1F("histo_theory", "Distribuzione teorica", bins, xmin, xmax);

  for (int i = 0; i < bins; ++i) {
    double x_low = xmin + i * bin_width;
    double x_high = x_low + bin_width;
    double bin_integral = function->Integral(x_low, x_high);
    double events = N * bin_integral / norm;
    histo_theory->SetBinContent(i + 1, events);
  }
  canvas->SetGrid();
  canvas->SetTicks();
}

double Simulation::get_mean(std::vector<double> vec) {
  double N{vec.size()};
  return {std::accumulate(vec.begin(), vec.end(), 0.) / N};
};



double Simulation::get_std_dev(std::vector<double> vec) {
  double N{vec.size()};
  double mean{get_mean(vec)};
  double sq_sum = std::accumulate(vec.begin(), vec.end(), 0.0,
                    [mean](double acc, double val) {
                      return acc + (val - mean) * (val - mean);
                    });
  return std::sqrt(sq_sum / N);
}

std::vector<double> Simulation::error_propagation() {
  return {TMath::Gaus(k, k_err), TMath::Gaus(phi, phi_err),
          TMath::Gaus(b, b_err)};
}

TH1F Simulation::accumulate_random() {
  TH1F histo = TH1F("histo", "Extractions", bins, function->GetXmin(),
                    function->GetXmax());
  for (int i{0}; i != N; ++i) {
    histo.Fill(function->GetRandom());
  }
  canvas->Update();
  return histo;
};

TH1F Simulation::accumulate_random_pars() {
  function->SetParameters(gRandom->Gaus(k, k_err), gRandom->Gaus(phi, phi_err),
                          gRandom->Gaus(b, b_err));
  TH1F histo_pars = TH1F("histo", "Extractions", bins, function->GetXmin(),
                         function->GetXmax());
  for (int i{0}; i != N; ++i) {
    histo_pars.Fill(function->GetRandom());
  }
  canvas->Update();
  return histo_pars;
}

void Simulation::draw_all() {
  canvas->cd();

  double xmin = function->GetXmin();
  double xmax = function->GetXmax();

  // Istogramma simulato
  auto histo = accumulate_random();
  histo.SetLineColor(kBlue);
  histo.SetFillColorAlpha(kBlue, 0.3);
  histo.SetTitle("Distribuzione simulata e funzione teorica; x; N eventi");

  histo.Scale(1.0 / histo.Integral("width"));
  histo.Draw("HIST");  // 🔹 primo draw SENZA "SAME"

  // Calcola l'integrale della funzione teorica
  double integral_function = function->Integral(xmin, xmax);
  double integral_histogram = histo.Integral();

  double scale_factor = integral_histogram / integral_function;

  // Se vuoi, disegna anche la funzione teorica non scalata
  function->SetLineColor(kRed);
  function->SetLineStyle(2);
  norm_function->Draw("SAME");

  canvas->Update();
  canvas->SaveAs("histogram.png");
}

double Simulation::regen_unc(int N_regen) {  // prendo media e stddev di ogni bin
  std::vector<double> bin_means{};
  std::vector<double> bin_devstds{};
  for (int i_bin{0}; i_bin != bins; ++i_bin) {
    std::vector<double> ibin_values{};
    for (int i_gen{0}; i_gen != N_regen; ++i_gen) {
      auto hist{accumulate_random()};
      ibin_values.push_back(hist.GetBinContent(i_bin + 1));
    }
    bin_means.push_back(get_mean(ibin_values));
    bin_devstds.push_back(get_std_dev(ibin_values) / std::sqrt(N_regen));
  }
  std::vector<int> x_values(bins);
  std::iota(x_values.begin(), x_values.end(),
            1);  // riempie il vettore con naturali

  std::vector<double> x_double(x_values.begin(), x_values.end());
  TGraphErrors graph = TGraphErrors(bins, x_double.data(), bin_means.data(), 0,
                                    bin_devstds.data());
  canvas->cd();
  graph.Draw("APE");
  canvas->Update();
  canvas->SaveAs("tgraph.png");

  double scarts {0};
  double sum41 {0};

  for (int i = 0; i<bins; ++i){
    sum41 += pow((bin_means[i] - histo_theory->GetBinContent(i+1)),2);
  }

  double sigma {std::sqrt(sum41/N)};
  return sigma;

}

double Simulation::bin_smeering() {
  canvas->cd();
  TH1F* histo_gaus = new TH1F("histo_gaus", "Istogramma con valori fluttuati",
                              bins, function->GetXmin(), function->GetXmax());

  for (int i_bin{0}; i_bin != bins; ++i_bin) {
    auto mean{histo_theory->GetBinContent(i_bin + 1)};
    auto dev_std{sqrt(mean)};
    auto random_gaus{
        gRandom->Gaus(mean, dev_std)};  // questo è il valore del bin fluttuato

    histo_gaus->SetBinContent(i_bin + 1, random_gaus);
  }
  histo_gaus->Draw();
  canvas->Update();
  canvas->SaveAs("histogaus.png");

  double scarts {0};
  double sum41 {0};

  for (int i = 0; i<bins; ++i){
    sum41 += pow((histo_gaus->GetBinContent(i+1) - histo_theory->GetBinContent(i+1)),2);
  }

  double sigma {std::sqrt(sum41/N)};
  return sigma;
}

double Simulation::regen_unc_randompars(int N_regen) {
  function->SetParameters(gRandom->Gaus(k, k_err), gRandom->Gaus(phi, phi_err),
                          gRandom->Gaus(b, b_err));
  TH1F* histo_theory_rand =
      new TH1F("histo_theory_rand", "Istogramma teorico con parametri casuali",
               bins, function->GetXmin(), function->GetXmax());
  std::vector<double> bin_means{};
  std::vector<double> bin_devstds{};
  for (int i_bin{0}; i_bin != bins; ++i_bin) {
    std::vector<double> ibin_values{};
    for (int i_gen{0}; i_gen != N_regen; ++i_gen) {
      auto hist{accumulate_random_pars()};
      ibin_values.push_back(hist.GetBinContent(i_bin + 1));
    }
    bin_means.push_back(get_mean(ibin_values));
    bin_devstds.push_back(get_std_dev(ibin_values) / std::sqrt(N_regen));
  }
  std::vector<int> x_values(bins);
  std::iota(x_values.begin(), x_values.end(),
            1);  // riempie il vettore con naturali

  std::vector<double> x_double(x_values.begin(), x_values.end());
  TGraphErrors graph = TGraphErrors(bins, x_double.data(), bin_means.data(), 0,
                                    bin_devstds.data());
  canvas->cd();
  graph.Draw("APE");
  canvas->Update();
  canvas->SaveAs("tgraphrand.png");

  double scarts {0};
  double sum41 {0};

  for (int i = 0; i<bins; ++i){
    sum41 += pow((bin_means[i] - histo_theory_rand->GetBinContent(i+1)),2);
  }

  double sigma {std::sqrt(sum41/N)};
  return sigma;

}

double Simulation::bin_smeering_randompars() {
  canvas->cd();
  function->SetParameters(gRandom->Gaus(k, k_err), gRandom->Gaus(phi, phi_err),
                          gRandom->Gaus(b, b_err));
  TH1F* histo_theory_rand =
      new TH1F("histo_theory_rand", "Istogramma teorico con parametri casuali",
               bins, function->GetXmin(), function->GetXmax());
  double xmin = function->GetXmin();
  double xmax = function->GetXmax();
  double bin_width = (xmax - xmin) / bins;
  double norm = function->Integral(xmin, xmax);
  for (int i = 0; i < bins; ++i) {
    double x_low = xmin + i * bin_width;
    double x_high = x_low + bin_width;
    double bin_integral = function->Integral(x_low, x_high);
    double events = N * bin_integral / norm;
    histo_theory_rand->SetBinContent(
        i + 1,
        events);  // ho generato l'istogramma teorico con parametri casuali
  }
  TH1F* histo_gaus_rand =
      new TH1F("histo_gaus_rand", "Istogramma con valori fluttuati", bins,
               function->GetXmin(), function->GetXmax());

  for (int i_bin{0}; i_bin != bins; ++i_bin) {
    auto mean{histo_theory_rand->GetBinContent(i_bin + 1)};
    auto dev_std{sqrt(mean)};
    auto random_gaus{
        gRandom->Gaus(mean, dev_std)};  // questo è il valore del bin fluttuato

    histo_gaus_rand->SetBinContent(i_bin + 1, random_gaus);
  }
  histo_gaus_rand->Draw();
  canvas->Update();
  canvas->SaveAs("histogausrand.png");

  double scarts {0};
  double sum41 {0};

  for (int i = 0; i<bins; ++i){
    sum41 += pow((histo_gaus_rand->GetBinContent(i+1) - histo_theory->GetBinContent(i+1)),2);
  }

  double sigma {std::sqrt(sum41/N)};
  return sigma;
}

void Simulation::sigma_confrontation(int N_regen){
  std::cout << bin_smeering()/regen_unc(N_regen) << '\n';
}

void Simulation::sigma_confrontation_randompars(int N_regen){
  std::cout << bin_smeering_randompars()/regen_unc_randompars(N_regen) << '\n';
}


