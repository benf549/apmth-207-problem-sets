#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

static void readCsv(const std::string& filename, std::vector<double>& x, std::vector<double>& y) {
    std::ifstream fin(filename);

    // skip header
    {
        std::string s;
        std::getline(fin, s);
    }

    double a, b;
    char delim;

    while(fin >> a >> delim >> b) {
        x.push_back(a);
        y.push_back(b);
    }
}

class PieceWiseLinearCDF {
    public:
        PieceWiseLinearCDF(std::vector<double> x, std::vector<double> y) : x_(std::move(x)) , y_(std::move(y)) {

        }

        PieceWiseLinearCDF(const std::string& filename) {
            readCsv(filename, x_, y_);
        }

        double generateSample(std::mt19937& gen) const {
            uint32_t rand_num = gen();
            uint32_t min = gen.min();
            uint32_t max = gen.max();

            // Generate a pseudorandom number between 0 and 1 using the mt19937 generator. Cast to double.
            double rand_01 = ((double) rand_num - min) / (max - min);

            // Compute a sample using CDF inversion given the pseudorandom number.
            return computeSample(rand_01);
        }

        double getMinX() const {return x_.front();}
        double getMaxX() const {return x_.back();}

        double calcAnalyticalMedian() {
            return computeSample(0.5);
        }

    private:
        double computeSample(double rand_01) const {
            double sample, x_i, x_ip1, y_i, y_ip1;

            // Find the x_i x_i+1 pair that is consistent with rule that x (the sample) falls between them.
            for (int i = 0; i < x_.size(); i++) {
                x_i = x_[i];
                x_ip1 = x_[i+1];
                y_i = y_[i];
                y_ip1 = y_[i+1];

                sample = invertedCDF(rand_01, x_i, y_i, x_ip1, y_ip1);
                if (x_i <= sample && sample < x_ip1) {
                    // Found the pair so break and return.
                    break;
                }
            }

            return sample;
        }

        double invertedCDF(double rand_01, double x_i, double y_i, double x_ip1, double y_ip1) const {
            // Define the invertedCDF over which the cdf is defined to be sampled from [0, 1]
            return ((rand_01 - y_i) * (x_ip1 - x_i) / (y_ip1 - y_i)) + x_i;
        }

        std::vector<double> x_;
        std::vector<double> y_;
};


int main(int argc, char **argv) {
    // Load the data
    PieceWiseLinearCDF cdf("cdf.csv");

    // histogram quantities
    const int nbins = 100;

    std::vector<double> histogramLoc(nbins); // center of each bin
    std::vector<int> histogramCounts(nbins); // number of samples per bin

    const double a = cdf.getMinX(); // lowest bound of the histogram
    const double b = cdf.getMaxX(); // highest bound of the histogram
    const double h = (b-a) / nbins; // bin size

    for (int i = 0; i < nbins; ++i)
        histogramLoc[i] = a + (i+0.5) * h;


    // Collect samples
    const int nsamples = 1'000'000;
    std::vector<double> samples(nsamples);
    std::mt19937 gen(3456789);

    // Ensure vector is initialized to 0 before filling with counts.
    fill(histogramCounts.begin(), histogramCounts.end(), 0);

    for (auto& x : samples) {
        x = cdf.generateSample(gen);
        for (int i = 0; i < histogramLoc.size(); i++) {
            double loc = histogramLoc[i];
            double radius = h * 0.5;
            if ((loc - radius) <= x && x < (loc + radius)) {
                histogramCounts[i]++;
            }
        }
    }

    // Sort samples to compute median.
    sort(samples.begin(), samples.end());

    double median;
    if (nsamples % 2 == 0) {
        int idx = nsamples / 2;
        median = (samples[idx - 1] + samples[idx]) / 2;
    } else {
        int idx = nsamples / 2;
        median = samples[idx];
    }

    // Compute and printout the empirical median and the analytical median
    std::cout << "Empirical Median: " << median << std::endl;
    std::cout << "Analytical Median: " << cdf.calcAnalyticalMedian() << std::endl;

    // Write the histogram to a csv file
    std::ofstream fout("histogram.csv");
    fout << "x,count" << std::endl;
    for (int i = 0; i < nbins; ++i)
        fout << histogramLoc[i] << ',' << histogramCounts[i] << std::endl;

    return 0;
}
