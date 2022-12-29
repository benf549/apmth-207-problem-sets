#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <cmath>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::stringstream;

void vprint(const vector<double>& v){
    cout << "[";
    for (auto i: v) {
        cout << i << ", ";
    }
    cout << "]" << endl;
}

void vprint(const vector<int>& v) {
    cout << "[";
    for (auto i: v) {
        cout << i << ", ";
    }
    cout << "]" << endl;
}

vector<double> compute_propensity_vector(const vector<double>& consts, const vector<int>& pop) {
    // Defines the system of equations presented in the problem statement.
    vector<double> v(4);

    // compute h*c for each according to equations.
    v[1] = (double) pop[0] * (double) pop[2] * consts[0];
    v[2] = pop[1] * consts[1];
    v[3] = pop[2] * consts[2];

    // compute a_0 from other indices.
    v[0] = v[1] + v[2] + v[3];

//    cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << endl;
    return v;
}

int compute_mu(const vector<double>& propensities, const double& r2) {
    // Returns integer in range [1, 3]
    double cum_sum = 0;
//    cout << r2 * propensities[0] << " " << r2 << " " << propensities[0] << " " << propensities[1] << " " << propensities[2] << " " << propensities[3] << endl;
    for (int i = 1; i < propensities.size(); i++) {
        cum_sum += propensities[i];
        if (r2 * propensities[0] <= cum_sum) {
            return i;
        }
    }
//    return -1; // This should never happen. Exception thrown in update_population if it does.
    return -1;
}

void update_population(vector<int>& population, int rxn) {
    // S E I R
    vector<int> r1{-1, 1, 0, 0};
    vector<int> r2{0, -1, 1, 0};
    vector<int> r3{0, 0, -1, 1};
    switch(rxn) {
        case 1:
            for (int idx = 0; idx < r1.size(); idx++)
                population[idx] += r1[idx];
            break;
        case 2:
            for (int idx = 0; idx < r2.size(); idx++)
                population[idx] += r2[idx];
            break;
        case 3:
            for (int idx = 0; idx < r3.size(); idx++)
                population[idx] += r3[idx];
            break;
        default:
            throw std::exception();
    }
}

void run_seir_ssa(stringstream& fpath, const int N, const int N_i) {
    const double tmax = 150.0;
    const vector<double> consts{0.5 / N, 0.05, 0.05};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> uniform(0.0, 1.0);

    std::ofstream output_file;
    cout << "writing: " << fpath.str() << endl;
    output_file.open(fpath.str());

    // [S, E, I, R]
    vector<int> population{N - N_i, 0, N_i, 0};
    vector<double> propensities = compute_propensity_vector(consts, population);

    int rxn_ctr = 0;
    double t = 0;
    while (t < tmax) {
        output_file << t << "," << population[0] << "," << population[1] << "," << population[2] << "," << population[3] << endl;
        double r1 = uniform(gen);
        double r2 = uniform(gen);

        //compute time-step tau = 1/a_0 ln(1/r1)
        double tau = (1/propensities[0]) * log(1/r1);

        // Select reaction to perform from {1, 2, 3}
        int mu = compute_mu(propensities, r2);

        // Perform reaction on population.
        update_population(population, mu);

        // Update propensities
        propensities = compute_propensity_vector(consts, population);

        // Increment time by time-step
        t += tau;
    }
    output_file << t << "," << population[0] << "," << population[1] << "," << population[2] << "," << population[3] << endl;
    output_file.close();
}

int ssa_main(int argc, char *argv[]) {
    int N;
    int N_i;
    if (argc <= 1) {
        N = 2000;
        N_i = 1;
        for (int i = 1; i <= 50; i++) {
            stringstream file_path;
            file_path << "SEIR_Ni_" << N_i << "_run_" << i << ".csv";
            run_seir_ssa(file_path, N, N_i);
        }
    } else {
        stringstream inp(argv[1]);
        inp >> N;
        if (!N) {
            return 1;
        }
        N_i = (int) (0.025 * N);
        for (int i = 1; i <= 10; i++) {
            stringstream file_path;
            file_path << "SEIR_N_" << N << "_run_" << i << ".csv";
            run_seir_ssa(file_path, N, N_i);
        }
    }

    return 0;
}

vector<double> compute_mean_vector(const vector<double>& propensities, const double tau) {
    vector<double> means(propensities.size() - 1);
    for (int i = 1; i < propensities.size(); i++) {
        means[i - 1] = propensities[i] * tau;
    }
    return means;
}

vector<int> compute_num_elapsed_reactions(const vector<double>& means, std::mt19937& gen) {
    vector<int> elapsed_rxns(means.size());
    for (int i = 0; i < means.size(); i++) {
        std::poisson_distribution<int> poisson(means[i]);
        elapsed_rxns[i] = poisson(gen);
    }
    return elapsed_rxns;
}

void update_population(vector<int>& population, const vector<int>& elapsed_rxns) {
    vector<int> tmp(population.size());
    // Loop over each index of the elapsed reactions vector.
    for (int rxn = 0; rxn < elapsed_rxns.size(); rxn++) {
        // Perform the reaction corresponding to this index the specified number of times.
        for (int r = 0; r < elapsed_rxns[rxn]; r++) {
            // Create a temporary copy of the population vector that we can apply the reaction to and see if any
            //  reactants go below zero.
            std::copy(population.begin(), population.end(), tmp.begin());
            update_population(tmp, rxn + 1);

            // Check if any of the species fall below zero in population.
            bool pop_went_negative = false;
            for (int x: tmp) {
                if (x < 0) {
                    pop_went_negative = true;
                }
            }
            if (pop_went_negative) {
                // If a population would be reduced below zero by this reaction, do nothing.
                continue;
            } else {
                // Otherwise keep the update by copying the updated temporary vector to the population vector.
                population = tmp;
            }
        }
    }
}

int tau_leaping_ssa_main(const double tau) {
    const double tmax = 150.0;
    const int N = 500;
    const int N_i = 12; // 0.025% infected.
    const vector<double> consts{0.5 / N, 0.05, 0.05};

    std::random_device rd;
    std::mt19937 gen(rd());

    double elapsed_time = 0;
    vector<int> population{N - N_i, 0, N_i, 0};
    vector<double> propensities = compute_propensity_vector(consts, population);

    stringstream fpath;
    fpath << "tau_" << tau << "_leaping.csv";

    std::ofstream output_file;
    cout << "writing: " << fpath.str() << endl;
    output_file.open(fpath.str());

    while (elapsed_time <= tmax) {
        output_file << elapsed_time << "," << population[0] << "," << population[1] << "," << population[2] << ","  << population[3] << endl;
        vector<double> means = compute_mean_vector(propensities, tau);
        vector<int> elapsed_rxns = compute_num_elapsed_reactions(means, gen);
        update_population(population, elapsed_rxns);
        propensities = compute_propensity_vector(consts, population);
        elapsed_time += tau;
    }
    output_file << elapsed_time << "," << population[0] << "," << population[1] << "," << population[2] << ","  << population[3] << endl;
    output_file.close();
    return 0;
}

int main(int argc, char *argv[]) {
    int out;
    if (argc < 3) {
        out = ssa_main(argc, argv);
    } else {
        vector<double> taus{0.001, 0.01, 0.1, 1, 10};
        for (auto tau: taus) {
            out = tau_leaping_ssa_main(tau);
        }
    }
    return out;
}
