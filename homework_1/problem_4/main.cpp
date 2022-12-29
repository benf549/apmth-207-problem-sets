#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <format>
#include <string>
#include <vector>
#include <cmath>


void print_vector(std::vector<double>& v) {
    for (double x: v) {
        std::cout << x << ", ";
    }
    std::cout << "\n";
}

double compute_cube_volume(int d, double edge_length) {
    return std::pow((double)edge_length, d);
}

double sample_uniform(int interval_max, int interval_min, std::mt19937& gen) {
    uint32_t rand_num = gen();
    uint32_t min = gen.min();
    uint32_t max = gen.max();

    // Generate a pseudorandom number between 0 and 1 using the mt19937 generator. Cast to double.
    double rand_01 = ((double) rand_num - min) / (max - min);
    double rand_interval = (rand_01 * (interval_max - interval_min)) - interval_max;
    return rand_interval;
}


std::vector<double> generate_uniform_sample_vector(int d, int interval_max, int interval_min, std::mt19937& gen) {
    std::vector<double> sample_vec(d);

    for (auto& x: sample_vec) {
        x = sample_uniform(interval_max, interval_min, gen);
    }

    return sample_vec;
}

std::vector<double> generate_gaussian_sample_vector(int d, std::normal_distribution<double>& gaussian,  std::mt19937& gen) {
    std::vector<double> sample_vec(d);

    for (auto& x: sample_vec) {
        x = gaussian(gen);
    }

    return sample_vec;
}

double function_to_integrate(std::vector<double>& x, int d) {
    double output = 0.0;
    for (int i = 0; i < d; i++) {
        output += (x[i] * x[i]);
    }
    return std::pow(2, -1 * d) * output;
}

double monte_carlo_integration(int d, int M, int interval_max, int interval_min) {

    const double edge_length = interval_max - interval_min;
    const double V = compute_cube_volume(d, edge_length);

    std::random_device rd;  
    std::mt19937 gen(rd());

    std::vector<double> sv; 
    double output_sum = 0.0;
    for (int i = 0; i < M; i++) {
        sv = generate_uniform_sample_vector(d, interval_max, interval_min, gen);
        output_sum += function_to_integrate(sv, d);
    }

    return (V / M) * output_sum;
}

double g_function(std::vector<double>& sv, int interval_max, int interval_min, int d) {

    for (auto x: sv) {
        if (!(interval_min <= x && x <= interval_max)) {
            // If any element of x is not between the interval min and max, return 0 as the vector is not in the volume.
            return 0.0;
        }
    }

    // The vector fell within the volume so return function value.
    return function_to_integrate(sv, d);
}

double gaussian_pdf(double mu, double sigma, double x) {
    double pi = 3.14159265358979323846;
    // 1/(math.sqrt(2 * math.pi) * sigma) * math.exp(-1/2 * ((x - mu)/sigma)**2)
    double fraction = 1.0 / (std::sqrt(2 * pi) * sigma);
    double exponential = std::exp( (-1/2) * std::pow((x - mu)/sigma, 2) );
    return fraction * exponential;
}

double importance_sampling_integration(int d, int M, int interval_max, int interval_min) {

    std::random_device rd;  
    std::mt19937 gen(rd());
    double mu = 0.0;
    double sigma = 0.5;
    std::normal_distribution<double> gaussian(mu, sigma);

    double sum = 0.0;
    int accepted = 0;
    while (accepted < M) {

        std::vector<double> sv = generate_gaussian_sample_vector(d, gaussian, gen);
        double g = g_function(sv, interval_max, interval_min, d);

        accepted += 1;

        double product = 1.0;
        for (int i = 0; i < d; i++) {
            product *= gaussian_pdf(mu, sigma, sv[i]);
        }

        sum += (g / product);
    }
    
    return sum / M;
}

double q_function(std::vector<double>& sv, int interval_max, int interval_min, int d) {
    double C = 1.0; // TODO: figure out what to do with this.
    for (auto x: sv) {
        if (!(interval_min <= x && x <= interval_max)) {
            // If any element of x is not between the interval min and max, return 0 as the vector is not in the volume.
            return 0.0;
        }
    }

    // The vector fell within the volume so return 1.
    return C * (0.1 + function_to_integrate(sv, d));
}

double rejection_sampling_integration(int d, int M, int interval_max, int interval_min) {
    std::random_device rd;  
    // std::mt19937 gen(3456789);
    std::mt19937 gen(rd());
    std::normal_distribution<double> gaussian(0.0, 0.5);

    double edge_length = interval_max - interval_min;
    double V = compute_cube_volume(d, edge_length);

    double num_sum = 0.0;
    double den_sum = 0.0;
    int accepted = 0;

    while (accepted < M) {
        std::vector<double> sv = generate_gaussian_sample_vector(d, gaussian, gen);
        double q = q_function(sv, interval_max, interval_min, d);
        double g = g_function(sv, interval_max, interval_min, d);
        if (g == 0.0) {
            continue;
        } else {
            accepted += 1;
            double w_i = 1.0 / q;
            num_sum += g * w_i;
            den_sum += w_i;
        }
    }
    return V * num_sum / den_sum;
}

int main(int argc, char **argv) {

    std::vector<int> d = {1, 2, 4, 8, 16};
    std::vector<int> ms = {1, 10, 100, 1'000, 10'000, 100'000, 1'000'000};
    int n_trials = 100;

    const int interval_max = 1;
    const int interval_min = -1;


    for (int m_i: ms) {
        std::cout << rejection_sampling_integration(16, m_i, interval_max, interval_min) << std::endl;
    }
    // for (int d_i: d) {
        // std::cout << "d: " << d_i << std::endl;
        // for (int m_i: ms) {
            // std::cout << "m: " << m_i << std::endl;
            // for (int n = 0; n < n_trials; n++) {
                // int d = d_i;
                // int m = m_i;

                // double mc_int = monte_carlo_integration(d, m, interval_max, interval_min);
                // mc_file << mc_int << std::endl;
                // double is_int = importance_sampling_integration(d, m, interval_max, interval_min);
                // is_file << is_int << std::endl;
                // double rs_int = rejection_sampling_integration(d, m, interval_max, interval_min);
                // rs_file << rs_int << std::endl;

                // // std::cout << "analytical solution: " << ((double) d / 3) << std::endl;
                // // std::cout << "monte carlo integral: " << mc_int << std::endl;
                // // std::cout << "importance sampling integral: " << is_int << std::endl;
                // // std::cout << "rejection sampling integral: " << rs_int << std::endl;
            // }
        // }
        // std::cout << std::endl;
    // }
    return 0;
}
// int main(int argc, char **argv) {

    // std::vector<int> d = {1, 2, 4, 8, 16};
    // std::vector<int> Ms = {1, 10, 100, 1'000, 10'000, 100'000, 1'000'000};
    // int n_trials = 100;

    // const int interval_max = 1;
    // const int interval_min = -1;


    // for (int d_i: d) {
        // std::cout << "d: " << d_i << std::endl;
        // for (int m_i: Ms) {
            // std::ofstream mc_file;
            // std::stringstream mc_path;
            // mc_path << "mc_" << d_i << "_" << m_i << "_100_trials.csv";
            // mc_file.open(mc_path.str());

            // std::ofstream is_file;
            // std::stringstream is_path;
            // is_path << "is_" << d_i << "_" << m_i << "_100_trials.csv";
            // is_file.open(is_path.str());

            // std::ofstream rs_file;
            // std::stringstream rs_path;
            // rs_path << "rs_" << d_i << "_" << m_i << "_100_trials.csv";
            // rs_file.open(rs_path.str());
            
            // std::cout << "M: " << m_i << std::endl;
            // for (int n = 0; n < n_trials; n++) {
                // int d = d_i;
                // int M = m_i;

                // double mc_int = monte_carlo_integration(d, M, interval_max, interval_min);
                // mc_file << mc_int << std::endl;
                // double is_int = importance_sampling_integration(d, M, interval_max, interval_min);
                // is_file << is_int << std::endl;
                // double rs_int = rejection_sampling_integration(d, M, interval_max, interval_min);
                // rs_file << rs_int << std::endl;

                // // std::cout << "Analytical Solution: " << ((double) d / 3) << std::endl;
                // // std::cout << "Monte Carlo Integral: " << mc_int << std::endl;
                // // std::cout << "Importance Sampling Integral: " << is_int << std::endl;
                // // std::cout << "Rejection Sampling Integral: " << rs_int << std::endl;
            // }
            // mc_file.close();
            // is_file.close();
            // rs_file.close();
        // }
        // std::cout << std::endl;
    // }
    // return 0;
// }
