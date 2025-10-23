#include "class.hpp"

int main() {
    int N{2000};
    int bin{50};
    int N_regen{100};
    Simulation sim(N, bin);
    sim.accumulate_random();
    sim.draw_all();
    //sim.bin_smeering(N, bin);
    //sim.regen_unc(N_regen, N, bin);
}