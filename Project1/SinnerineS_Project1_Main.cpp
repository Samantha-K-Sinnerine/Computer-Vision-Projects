#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

using namespace std;

class ThresholdSelection {
public:
    int numRows, numCols, minVal, maxVal;
    int BiGaussThrVal;
    int histHeight;
    int maxHeight;
    int *histAry;
    int *GaussAry;
    int *bestFitGaussAry;
    char **Graph;

public:
    // Constructor - dynamically allocates all member arrays and initialization
    ThresholdSelection(int rows, int cols, int min, int max) {
        numRows = rows;
        numCols = cols;
        minVal = min;
        maxVal = max;
        BiGaussThrVal = 0;
        histHeight = 0;
        maxHeight = 0;
        bestFitGaussAry = new int[maxVal + 1]();
    }

    // Reads and loads the histAry from inFile and returns the max count.
    int loadHist(int *histAry, ifstream &inFile) {
        int maxCount = 0;
        for (int i = 0; i <= maxVal; ++i) {
            histAry[i] = 0; // Initialize all counts to zero
        }

        int index, count;
        while (inFile >> index >> count) {
            if (index >= minVal && index <= maxVal) {
                histAry[index] = count;
                if (count > maxCount) {
                    maxCount = count;
                }
            }
        }
        return maxCount;
    }

    // Outputs the histogram in the specified format
    void dispHist(int *histAry, ofstream &outfile) {
        outfile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
        for (int i = minVal; i <= maxVal; ++i) {
            outfile << i << " (" << histAry[i] << "): ";
            for (int j = 0; j < histAry[i]; ++j) {
                outfile << "+";
            }
            outfile << endl;
        }
    }

    // Copies ary1 to ary2
    void copyArys(int *ary1, int *ary2) {
        for (int i = 0; i <= maxVal; ++i) {
            ary2[i] = ary1[i];
        }
    }

    // Plots the histogram onto Graph with ‘+’
    void plotHist(int *histAry, char **Graph) {
        for (int i = 0; i <= maxVal; ++i) {
            for (int j = 0; j < histAry[i]; ++j)
                Graph[i][j] = '+';
        }
    }
    
    // Plots the two best fit Gaussian curves and the gaps between the two curves and histogram 
    // with symbols (* for points on the curve and ^ for the gaps) onto graph.
    void plotGaussCurves(int *histAry, int *bestFitGaussAry, int maxVal, char **Graph, ofstream &deBugFile) {
        deBugFile << "Entering plotGaussCurves() method" << endl;

        for (int index = 0; index <= maxVal; ++index) {
            int end1, end2;
            if (bestFitGaussAry[index] <= histAry[index]) {
                end1 = bestFitGaussAry[index];
                end2 = histAry[index];
            } else {
                end1 = histAry[index];
                end2 = bestFitGaussAry[index];
            }

            int i = end1;
            while (i <= end2) {
                Graph[index][i] = '#';
                i++;
            }

            Graph[index][bestFitGaussAry[index]] = '*';
        }

        deBugFile << "Leaving plotGaussCurves()" << endl;
    }


    // Sets 1D Ary to zero;
    void setZero(int *Ary) {
        for (int i = 0; i <= maxVal; ++i)
            Ary[i] = 0;
    }   

    // The method determines the best threshold selection (via fitGauss method)
    // where the two Gaussian curves fit the histogram the best
    int biGaussian(int *histAry, int *GaussAry, int &maxHeight, int minVal, int maxVal, char **Graph, ofstream &deBugFile) {
        deBugFile << "Entering biGaussian method" << endl;

        double sum1, sum2, total, minSumDiff = 99999.0;
        int offSet = (maxVal - minVal) / 10;
        int dividePt = offSet;
        int bestThr = dividePt;

        while (dividePt < (maxVal - offSet)) {
            setZero(GaussAry);
            sum1 = fitGauss(0, dividePt, histAry, GaussAry, maxHeight, Graph, deBugFile);
            sum2 = fitGauss(dividePt, maxVal, histAry, GaussAry, maxHeight, Graph, deBugFile);
            total = sum1 + sum2;

            if (total < minSumDiff) {
                minSumDiff = total;
                bestThr = dividePt;
                copyArys(GaussAry, bestFitGaussAry);
            }

            deBugFile << "In biGaussian(): dividePt = " << dividePt << ", sum1 = " << sum1 << ", sum2 = " << sum2 << ", total = " << total << ", minSumDiff = " << minSumDiff << " and bestThr = " << bestThr << endl;

            dividePt++;
        }

        deBugFile << "Leaving biGaussian method, minSumDiff = " << minSumDiff << ", bestThr is " << bestThr << endl;

        return bestThr;
    }

    // Computes the Gaussian curve fitting to the histogram
    double fitGauss(int leftIndex, int rightIndex, int *histAry, int *GaussAry, int &maxHeight, char **Graph, ofstream &deBugFile) {
        deBugFile << "Entering fitGauss method" << endl;

        double mean, var, sum = 0.0, Gval;
        mean = computeMean(leftIndex, rightIndex, maxHeight, histAry, deBugFile);
        var = computeVar(leftIndex, rightIndex, mean, histAry, deBugFile);

        for (int index = leftIndex; index <= rightIndex; ++index) {
            Gval = modifiedGauss(index, mean, var, maxHeight);
            sum += abs(Gval - static_cast<double>(histAry[index]));
            GaussAry[index] = static_cast<int>(Gval);
        }

        deBugFile << "Leaving fitGauss method, sum is: " << sum << endl;

        return sum;
    }

    // Computes the mean from leftIndex to rightIndex of the histogram
    // and returns the *weighted* average of the histogram; i.e., i * hist[i]
    double computeMean(int leftIndex, int rightIndex, int &maxHeight, int *histAry, ofstream &deBugFile) {
        deBugFile << "Entering computeMean method" << endl;

        double sum = 0.0;
        int numPixels = 0;

        for (int index = leftIndex; index <= rightIndex; ++index) {
            sum += histAry[index] * index;
            numPixels += histAry[index];
            if (histAry[index] > maxHeight) {
                maxHeight = histAry[index];
            }
        }
        double result = sum / numPixels;
        deBugFile << "Leaving computeMean() method, maxHeight is: " << maxHeight << ", result is: " << result << endl;
        return result;
    }

    // Computes the *weighted* variance from the given leftIndex to rightIndex of the histogram 
    // and returns the *weighted* variance
    double computeVar(int leftIndex, int rightIndex, double mean, int *histAry, ofstream &deBugFile) {
        deBugFile << "Entering computeVar() method" << endl;

        double sum = 0.0;
        int numPixels = 0;

        for (int index = leftIndex; index <= rightIndex; ++index) {
            sum += histAry[index] * pow(index - mean, 2);
            numPixels += histAry[index];
        }
        double result = sum / numPixels;
        deBugFile << "Leaving computeVar method, returning result: " << result << endl;
        return result;
    }

    // Uses the given mean and variance to compute the Gaussian value based on the modified Gaussian function
    double modifiedGauss(int x, double mean, double var, int &maxHeight) {
        double Gval = maxHeight * exp(-pow(x - mean, 2) / (2 * var));
        return Gval;
    }
};

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Arguments must include: <inFile1> <outFile1> <outFile2> <deBugFile>" << endl;
        return 1;
    }

    ifstream inFile(argv[1]);
    ofstream outFile1(argv[2]);
    ofstream outFile2(argv[3]);
    ofstream deBugFile(argv[4]);

    if (!inFile.is_open() || !outFile1.is_open() || !outFile2.is_open() || !deBugFile.is_open()) {
        cerr << "Error: Unable to open files." << endl;
        return 1;
    }

    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;


    // Create an instance of the ThresholdSelection class
    ThresholdSelection thresholdSelection(numRows, numCols, minVal, maxVal);

    int *histAry = new int[maxVal + 1]();
    int histHeight = thresholdSelection.loadHist(histAry, inFile);

    // Dynamically allocate GaussAry and Graph 
    int *GaussAry = new int[maxVal + 1]();
    char **Graph = new char*[maxVal + 1];
    for (int i = 0; i <= maxVal; ++i) {
        Graph[i] = new char[histHeight + 1](); 
    }

    // output input histogram
    outFile1 << "In main (), below is the input histogram:" << endl;
    thresholdSelection.dispHist(histAry, outFile1);

    // plot histogram
    thresholdSelection.plotHist(histAry, Graph);

    deBugFile << "In main (), below is the Graph after plotting the histogram onto Graph" << endl;
    // Print graph to debug file
    for (int i = 0; i <= maxVal; ++i) {
        for (int j = 0; j <= histHeight; ++j) {
            deBugFile << Graph[i][j];
        }
        deBugFile << endl;
    }

    // compute BiGaussThrVal
     int BiGaussThrVal = thresholdSelection.biGaussian(histAry, GaussAry,
                                                      thresholdSelection.maxHeight, thresholdSelection.minVal,
                                                      thresholdSelection.maxVal,Graph,
                                                      deBugFile);
    // output BiGaussThrVal
    outFile2 << "The BiGaussThrVal is " << BiGaussThrVal << endl;

    // plot Gaussian curves
    thresholdSelection.plotGaussCurves(histAry, thresholdSelection.bestFitGaussAry,
                                        thresholdSelection.maxVal, Graph, deBugFile);


    // output graph showing histogram, best fitted Gaussian curves, and gap
    outFile2 << "In main(). Below is the graph showing the histogram, the best fitted Gaussian curves, and the gap:" << endl;
    // Print graph to outFile2
    for (int i = 0; i <= maxVal; ++i) {
        for (int j = 0; j <= histHeight; ++j) {
            outFile2 << Graph[i][j];
        }
        outFile2 << endl;
    }

    // close files
    inFile.close();
    outFile1.close();
    outFile2.close();
    deBugFile.close();

    return 0;
}