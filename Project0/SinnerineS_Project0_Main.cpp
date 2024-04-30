#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

// Class to represent image properties
class Image {
public:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int thrValue;

    // Constructor for the Image class to initialize class member variables
    Image() {
        numRows = 0;
        numCols = 0;
        minVal = 0;
        maxVal = 0;
        thrValue = 0;
    }

  // Method to read image header from input file
    void readHeader(ifstream& inFile) {
        inFile >> numRows >> numCols >> minVal >> maxVal;
    }

    // Method to write image header to output file with updated minVal and maxVal
    void writeHeader(ofstream& outFile, int newMinVal, int newMaxVal) {
        outFile << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;
    }
};

// Function to perform binary and non-binary threshold operations
void processing(ifstream& inFile, ofstream& outFile1, ofstream& outFile2, int thrValue) {

    Image image;

    // Read the image header from the input file
    image.readHeader(inFile);

    // Output header to both output files with updated minVal and maxVal
    image.writeHeader(outFile1, 0, 1);  // Updated for binary threshold operation
    image.writeHeader(outFile2, 0, image.maxVal);  // Updated for non-binary threshold operation


    int pixelVal, count = 0;

    // Read pixel values from the input file
    while (inFile >> pixelVal) {
        if (pixelVal >= thrValue) {
            outFile1 << "1 "; // binary threshold
            outFile2 << pixelVal << " "; // non-binary threshold
        } else {
            outFile1 << "0 ";
            outFile2 << "0  ";
        }

        count++;

        // Check if the end of the row is reached
        if (count >= image.numCols) {
            // Write end of line to both output files
            outFile1 << endl;
            outFile2 << endl;
            
            // Reset the count for the next row
            count = 0;
        }
    }
}


int main(int argc, char* argv[]) {
    // Check for correct number of command-line arguments
    if (argc != 5) {
        cerr << "Invalid Arguments: inFile thrValue outFile1 outFile2" << endl;
        return 1;
    }

    // Open input file
    ifstream inFile(argv[1]);
    if (!inFile) {
        cerr << "Error: Unable to open input file" << endl;
        return 1;
    }

    // Convert threshold value from string (containing digits) to integer
    int thrValue = atoi(argv[2]);

    // Open output files
    ofstream outFile1(argv[3]);
    ofstream outFile2(argv[4]);

    if (!outFile1 || !outFile2) {
        cerr << "Error: Unable to open output files" << endl;
        return 1;
    }

    // Perform processing and write results to output files
    processing(inFile, outFile1, outFile2, thrValue);

    // Close all files
    inFile.close();
    outFile1.close();
    outFile2.close();

    return 0;
}
