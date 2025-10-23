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

double Simulation::random_extraction() {
  double x{function->GetRandom()};
  return x;
};
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
    histo.Fill(random_extraction());
  }
  canvas->Update();
  return histo;
};

TF1* Simulation::scale_function_to_histogram() {
  double xmin = function->GetXmin();
  double xmax = function->GetXmax();

  // Calcola l'integrale della funzione teorica
  double integral_function = function->Integral(xmin, xmax);

  // Calcola l'integrale dell'istogramma simulato
  TH1F histo = accumulate_random();  // oppure usa un istogramma già generato
  double integral_histogram = histo.Integral();

  // Calcola il fattore di scala
  double scale_factor = integral_histogram / integral_function;

  // Crea una nuova funzione scalata
  TF1* scaled_function = new TF1("scaled_function",
                            "([3]*((cos([0]*x+[1]))^2 + [2]))", xmin, xmax);
  scaled_function->SetParameters(k, phi, b, scale_factor);
  scaled_function->SetLineColor(kMagenta);
  scaled_function->SetLineWidth(2);
  return scaled_function;
}

void Simulation::draw_all() {
  canvas->cd();
  auto histo = accumulate_random();
  histo.SetLineColor(kBlue);
  histo.SetFillColorAlpha(kBlue, 0.3);
  //histo.Draw("SAME");

  auto scaled_function = scale_function_to_histogram();  // crea la funzione scalata
  scaled_function->Draw("SAME");  // disegna la funzione scalata

  canvas->Update();
  canvas->SaveAs("histogram.png");
}

void Simulation::regen_unc(int N_regen) {  // prendo media e stddev di ogni bin
  std::vector<double> bin_means{};
  std::vector<double> bin_devstds{};
  for (int i_bin{0}; i_bin != bins; ++i_bin) {
    std::vector<double> ibin_values{};
    for (int i_gen{0}; i_gen != N_regen; ++i_gen) {
      auto hist{accumulate_random()};
      ibin_values.push_back(hist.GetBinContent(i_bin + 1));
    }
    bin_means.push_back(get_mean(ibin_values));
    bin_devstds.push_back(get_std_dev(ibin_values));
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
}

/*void Simulation::bin_smeering(int N_regen) {
  // Parametri della gaussiana con cui far fluttuare : media e radice valore
  // atteso
  auto hist{accumulate_random(N, bins)};
    // intervallo costruzione istogramma
    // intervallo integrazione smeering

  // costruzione di istogramma "teorico" per confronto:


  // smeering
  std::vector<double> std_dev_bin{};
  for (int i_bin{0}; i_bin != bins; ++i_bin) {
    std::vector<double> fluctuated_values_bin{};
    double interval_acc{
      norm_function->GetXmin()};
    for (int i_reg{0}; i_reg != N_regen; ++i) {
      auto mean{histo_theory.GetBinContent(i_bin + 1)};
      auto dev_std{
          std::sqrt(N * norm_function->Integral(
                            interval_acc,
                            interval_acc +
                                (norm_function->GetXmax()) / bins))  // Nexp
      };
      interval_acc += norm_function->GetXmax() / bins;
      auto random_gaus{
          TMath::Gaus(mean, dev_std)};  // questo è il valore del bin fluttuato
      fluctuated_values_bin.push_back(random_gaus);
    }
    std_dev_bin.push_back(
        get_std_dev(fluctuated_values_bin));  // vettore di devstd dei bin
  }
}*/

/*TF1* Simulation::chi_confrontation(int N, int bins) {
  TF1* function_fit =
      new TF1("function_fit", "((cos([0]*x+[1]))^2 + [2])", 0., 3.14 / k);
  TGraph graph = TGraph(N);
  for (int i{0}; i!=N; ++i) {
    double x = ;
    double y{};
    graph.AddPoint(x, y);
  }
  graph.Fit(function_fit);
  std::cout << "k = " << function_fit->GetParameter(0) << "\n";
  std::cout << "phi = " << function_fit->GetParameter(1) << "\n";
  std::cout << "b = " << function_fit->GetParameter(2) << "\n";
  return function_fit;
};*/
