#include "class.hpp"
#include <TApplication.h>

int main() {
    int N{2000};
    int bin{100};
    int N_regen{100};
    Simulation sim(N, bin);
    sim.accumulate_random();
    sim.draw_all();
    sim.bin_smeering();
    sim.regen_unc(N_regen);
    sim.sigma_confrontation(N_regen);
    sim.regen_unc_randompars(N_regen);
    sim.bin_smeering_randompars();
    sim.sigma_confrontation_randompars(N_regen);
}