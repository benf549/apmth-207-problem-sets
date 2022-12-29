#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <random>


// Define aliases to save some typing.
using std::cout, std::endl, std::vector, std::string, std::stringstream, std::ofstream;
using vec_2d = vector<vector<double>>;
using vec = vector<double>;

// Define useful global constants.
const int NUM_PEDESTRIANS = 10;
const int NUM_OBSTACLES = 5;

// Helper print functions for debugging.
void print(const vec& vec) {
    cout << "[";
    for (const auto& i: vec) {
        cout << i << ", ";
    }
    cout << "]" << endl;
}

void print(const vector<int>& vec) {
    cout << "[";
    for (const auto& i: vec) {
        cout << i << ", ";
    }
    cout << "]";
}

void print(const vec_2d& vec) {
    cout << "[ ";
    for (const auto& i : vec) {
        cout << "(";

        int ctr = 0;
        for (auto j : i){
            if (ctr == 0) {
                cout << j << ",";
            } else {
                cout << j;
            }
            ctr += 1;
        }

        cout << "), ";
    }
    cout << "] " << endl;
}

double distance(const vec& v1, const vec& v2) {
    double out = sqrt( pow(v1[0] - v2[0], 2) + pow(v1[1] - v2[1], 2));
    return out;
}

double sq_dist(const vec& v1, const vec& v2) {
    double out =  pow(v1[0] - v2[0], 2) + pow(v1[1] - v2[1], 2);
    return out;
}

double x_displacement(const vec& v1, const vec& v2) {
    return abs(v1[0] - v2[0]);
}

vec_2d add_vecs(const vec_2d& v1, const vec_2d& v2) {
    // Sanity check that vectors are same size.
    assert(v1.size() == v2.size());
    // Initialize memory for output vector.
    vec_2d output (v1.size(), {0,0});
    // Add every element of both vectors, save result in output.
    for (int i = 0; i < v1.size(); i++) {
        output[i] = {v1[i][0] + v2[i][0], v1[i][1] + v2[i][1]};
    }
    return output;
}

vec_2d sub_vecs(const vec_2d& v1, const vec_2d& v2) {
    // Sanity check that vectors are same size.
    assert(v1.size() == v2.size());
    // Initialize memory for output vector.
    vec_2d output (v1.size(), {0,0});
    // Add every element of both vectors, save result in output.
    for (int i = 0; i < v1.size(); i++) {
        output[i] = {v1[i][0] - v2[i][0], v1[i][1] - v2[i][1]};
    }
    return output;
}

vec_2d scalar_mult_vecs(const double scalar, const vec_2d& v1) {
    vec_2d output(v1.size(), {0.0, 0.0});
    for (int i = 0; i < v1.size(); i++) {
        output[i] = { scalar * v1[i][0], scalar * v1[i][1]};
    }
    return output;
}

vec_2d compute_force_from_trajectories(const vec_2d& velocities) {
    // Trajectory Force Computation Constants
    const double tau     =  0.2;
    const vec_2d v_0s {
            {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0},
            {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}
    };

    // Subtract velocities and apply scalar multiplication.
    vec_2d vec_diff = sub_vecs(v_0s, velocities);
    vec_diff = scalar_mult_vecs((1/tau), vec_diff);

    return vec_diff;
}

vec_2d compute_forces(const vec_2d& positions, const vec_2d& obstacle_positions, const vec_2d& velocities) {
    const double A       = 20.0;
    const double B       =  0.5;
    const double C       = 10.0;
    const double D       =  0.6;

    vec_2d forces (NUM_PEDESTRIANS, {0.0, 0.0});

    // Compute force from trajectories.
    forces = add_vecs(forces, compute_force_from_trajectories(velocities));

    for (int i = 0; i < NUM_PEDESTRIANS; i++) {
        // Compute force from other pedestrians.
        for (int j = i + 1; j < NUM_PEDESTRIANS; j++) {
            double r_ij = distance(positions[i], positions[j]);
            double force_coeff = A * exp( -1 * (r_ij / B));
            double x_update = force_coeff * (positions[i][0] - positions[j][0]) / r_ij;
            double y_update = force_coeff * (positions[i][1] - positions[j][1]) / r_ij;
            forces[i][0] += x_update;
            forces[i][1] += y_update;
            forces[j][0] += -1 * x_update;
            forces[j][1] += -1 * y_update;
        }

        // Compute force from obstacles.
        for (int k = 0; k < NUM_OBSTACLES; k++) {
            double r_ik = distance(positions[i], obstacle_positions[k]);
            double force_coeff = C * exp(-1 * (r_ik / D));
            forces[i][0] += force_coeff * (positions[i][0] - obstacle_positions[k][0]) / r_ik;
            forces[i][1] += force_coeff * (positions[i][1] - obstacle_positions[k][1]) / r_ik;
        }
    }
    return forces;
}

vec_2d enforce_boundaries(const vec_2d& positions) {
    const double xmax =  30.0;
    const double xmin = -30.0;
    const double ymax =   3.0;
    const double ymin =  -3.0;

    vec_2d bounded_pos(NUM_PEDESTRIANS, {0.0, 0.0});

    for (int i = 0; i < NUM_PEDESTRIANS; i++) {
        bounded_pos[i][0] = fmin(fmax(positions[i][0], xmin), xmax);
        bounded_pos[i][1] = fmin(fmax(positions[i][1], ymin), ymax);
    }

    return bounded_pos;
}

void apply_time_step(vec_2d& positions, const vec_2d& obstacle_positions, vec_2d& velocities, const double delta_t) {
    // Step positions in direction of velocities.
    positions = add_vecs(positions, scalar_mult_vecs(delta_t, velocities));
    positions = enforce_boundaries(positions);

    // Step velocities in direction of forces.
    vec_2d forces = compute_forces(positions, obstacle_positions, velocities);
    velocities = add_vecs(velocities, scalar_mult_vecs(delta_t, forces));
}

void log_positions(const vec_2d& positions, bool is_first_line = false) {
    ofstream output_file;

    for (int i = 0; i < NUM_PEDESTRIANS; i++) {
        stringstream fname;
        fname << "particle_" << i << ".log";

        if (is_first_line) {
            // Deletes file from previous run.
            output_file.open(fname.str());
        } else {
            // Append if not first line.
            output_file.open(fname.str(), std::ios_base::app);
        }

        output_file << positions[i][0] << "," << positions[i][1] << endl;
        output_file.close();
    }
}

double run_model(const vec_2d& obstacle_positions, bool log_pos = false) {
    // Setup Constants
    const double max_time = 50.0;
    const double delta_t = 0.05;
    const vec_2d initial_positions {
            {-28.5, 0}, {-27.0, 0}, {-25.5, 0}, {-24.0, 0}, {-22.5, 0},
            { 22.5, 0}, { 24.0, 0}, { 25.5, 0}, { 27.0, 0}, { 28.5, 0},
    };

    // Initialize vector of positions to initial positions.
    vec_2d positions(initial_positions);
    // Initialize velocities to ideal velocity.
    vec_2d velocities {
            {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0},
            {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}
    };

    // Run simulation for 60.00 / 0.05 time steps.
    double curr_time = 0;
    if (log_pos) {
        log_positions(positions, true);
    }
    while (curr_time < max_time) {
        apply_time_step(positions, obstacle_positions, velocities, delta_t);
        if (log_pos) {
            log_positions(positions);
        }
        curr_time += delta_t;
    }

    // Compute average displacement.
    double avg_displacement = 0;
    for (int i = 0; i < NUM_PEDESTRIANS; i++) {
        avg_displacement += x_displacement(positions[i], initial_positions[i]);
    }
    avg_displacement /= NUM_PEDESTRIANS;

    return avg_displacement;
}

vec_2d generate_obstacle_positions(std::mt19937& gen, double rect_xmax, double rect_xmin, double rect_ymax, double rect_ymin, double radius) {
    std::uniform_real_distribution<double> x_uniform(rect_xmin + radius, rect_xmax - radius);
    std::uniform_real_distribution<double> y_uniform(rect_ymin + radius, rect_ymax - radius);

    // Initialize 2D vec to hold obstacle positions.
    vec_2d obstacle_positions (NUM_OBSTACLES, {0.0, 0.0});

    int accepted_samples = 0;
    while (accepted_samples < NUM_OBSTACLES) {
        // Sample a position inside the obstacle box.
        vec proposal_posn = {x_uniform(gen), y_uniform(gen)};
        bool accept_proposal = true;
        // Determine whether to accept this proposal for the seeding.
        for (int i = 0; i < accepted_samples; i++) {
            if ( sq_dist(proposal_posn, obstacle_positions[i]) < pow( (2 * radius), 2) ) {
                // If the proposal position overlaps with any of the previously created obstacles, try another sample.
                accept_proposal = false;
                break;
            }
        }
        // If we keep this obstacle position, record 2D vector in positions matrix.
        if (accept_proposal) {
            obstacle_positions[accepted_samples] = proposal_posn;
            accepted_samples += 1;
        }
    }

    return obstacle_positions;
}

vector<int> select_3_from_NP(const int NP, std::mt19937& gen, const int curr) {
    // Selects 3 unique indices from the set of all vectors.
    std::uniform_int_distribution<> rand_idx(0, NP - 1);
    std::vector<int> output_vec(3, 0);

    int curr_idx = 0;
    while (curr_idx < output_vec.size()) {
        int proposal_idx = rand_idx(gen);
        bool accept_idx = true;

        for (int i = 0; i < curr_idx; i++) {
            if (output_vec[i] == proposal_idx) {
                accept_idx = false;
                break;
            }
        }
        if (accept_idx and proposal_idx != curr) {
            output_vec[curr_idx] = proposal_idx;
            curr_idx += 1;
        }
    }
    return output_vec;
}

double compute_overlap_and_oob_penalty(const vec_2d& obstacle_positions, double rect_xmax, double rect_xmin, double rect_ymax, double rect_ymin, double radius) {
    bool apply_penalty = false;

    for (int i = 0; i < NUM_OBSTACLES; i++) {
        const vec& position = obstacle_positions[i];
        // Ensure vector is within bounds.
        if ((position[0] - radius < rect_xmin) or (position[0] + radius > rect_xmax) or (position[1] - radius < rect_ymin) or (position[1] + radius > rect_ymax)) {
            apply_penalty = true;
            break;
        }

        // Ensure does not clash with any other vectors.
        for (int j = i + 1; j < NUM_OBSTACLES; j++) {
            if ( sq_dist(position, obstacle_positions[j]) < pow( (radius * 2), 2) ) {
                apply_penalty = true;
                break;
            }
        }
        if (apply_penalty) {
            break;
        }
    }

    if (apply_penalty) {
        // Return very negative penalty for overlapping or out of bounds barriers since we want to maximize average distance.
        return -100000.0;
    } else {
        return 0.0;
    }
}

vec_2d run_differential_evolution() {
    // Obstacle Rectangle Constants (with 10, height 6)
    const double rect_xmax = 5.0;
    const double rect_xmin = -5.0;

    const double rect_ymax = 3.0;
    const double rect_ymin = -3.0;

    const double radius = 0.6;

    // Initialize RNGs.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> randb(0, 1);
    std::uniform_int_distribution rnbr_vec(0, NUM_OBSTACLES - 1);
    std::uniform_int_distribution rnbr_idx(0, 2);

    // Define DE constants.
    int dim = NUM_OBSTACLES * 2;
    int NP = 7 * dim;
    double F = 0.5;
    double CR = 0.1;
    int max_generations = 1000;

    // Initialize a matrix (all_vectors) to hold the [NP x 5 x 2] data
    vec_2d zeros(NUM_OBSTACLES, {0.0, 0.0});
    vector<vec_2d> all_vectors(NP, zeros);
    vec curr_displacements(NP, 0.0);
    for (int i = 0; i < NP; i++) {
        all_vectors[i] = generate_obstacle_positions(gen, rect_xmax, rect_xmin, rect_ymax, rect_ymin, radius);
        curr_displacements[i] = run_model(all_vectors[i]);
    }

    for (int epoch = 0; epoch < max_generations; epoch++) {
        cout << "epoch: " << epoch << ", best: " << *std::max_element(curr_displacements.begin(), curr_displacements.end()) << endl;
        for (int i = 0; i < NP; i++) {
            vec_2d original_vec = all_vectors[i];

            // Apply Mutation.
            // Select three non-redundant indices that aren't index i.
            vector<int> idx_vector = select_3_from_NP(NP, gen, i);
            // Compute X1 + F * (X2 - X3)
            vec_2d mut_vec = add_vecs(all_vectors[idx_vector[0]], scalar_mult_vecs(F, sub_vecs(all_vectors[idx_vector[1]], all_vectors[idx_vector[2]])));

            // Apply Cross-Over.
            vec_2d new_vec(NUM_OBSTACLES, {0.0, 0.0});
            int keep_new_vec = rnbr_vec(gen);
            int keep_new_idx = rnbr_idx(gen);

            for (int j = 0; j < NUM_OBSTACLES; j++) {
                for (int k = 0; k < 2; k++) {
                    if (randb(gen) <= CR or (keep_new_vec == j and keep_new_idx == k)) {
                        new_vec[j][k] = mut_vec[j][k];
                    } else {
                        new_vec[j][k] = original_vec[j][k];
                    }
                }
            }

            // Selection.
            double trial_avg_displacement = run_model(new_vec);
            trial_avg_displacement += compute_overlap_and_oob_penalty(new_vec, rect_xmax, rect_xmin, rect_ymax, rect_ymin, radius);
            if (curr_displacements[i] < trial_avg_displacement) {
                all_vectors[i] = new_vec;
                curr_displacements[i] = trial_avg_displacement;
            }
        }
    }

    double max = 0;
    vec_2d best_vec (5, {0.0, 0.0});
    int ctr = 0;
    for (auto disp: curr_displacements) {
        if (max < disp) {
            best_vec = all_vectors[ctr];
            max = disp;
        }
        ctr += 1;
    }
    return best_vec;
}

int main() {
    vec_2d best_vec = run_differential_evolution();
    print(best_vec);
    double best_displacement = run_model(best_vec, true);
    cout << best_displacement << endl;
    ofstream output_file;
    output_file.open("differential_evolution_1000.txt");
    output_file << best_displacement << endl;
    output_file << "[(" << best_vec[0][0] << ", " << best_vec[0][1] << "), "
        << "(" << best_vec[1][0] << ", " << best_vec[1][1] << "), "
        << "(" << best_vec[2][0] << ", " << best_vec[2][1] << "), "
        << "(" << best_vec[3][0] << ", " << best_vec[3][1] << "), "
        << "(" << best_vec[4][0] << ", " << best_vec[4][1] << ")]" << endl;

    return 0;
}
