#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const double PI = 3.14159265358979323846;

class HoughTransform {
private:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int HoughDist;
    int HoughAngle;
    int angleInDegree;
    double angleInRadians;
    int** imgAry;
    int** CartesianAry;
    int** PolarAry;
    int offSet;

public:
    HoughTransform(int rows, int cols, int min, int max, int dist, int angle, int** img, int** cartesian, int** polar)  {
        numRows = rows;
        numCols = cols;
        minVal = min;
        maxVal = max;
        imgAry = img;
        CartesianAry = cartesian;
        PolarAry = polar;
        HoughAngle = 180;
        HoughDist = 2 * sqrt(numRows * numRows + numCols * numCols);
        offSet = sqrt(numRows * numRows + numCols * numCols);
    }

    void loadImage(ifstream& imgFile, int** imgAry) {
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                imgFile >> imgAry[i][j];
            }
        }
    }

    void buildHoughSpace(ofstream& outFile1, ofstream& deBugFile) {
        deBugFile << "Entering buildHoughSpace" << endl;
        for (int x = 0; x < numRows; ++x) {
            for (int y = 0; y < numCols; ++y) {
                if (imgAry[x][y] > 0) {
                    computeSinusoid(x , y, deBugFile);
                }
            }
        }
        deBugFile << "Leaving buildHoughSpace" << endl;
    }

 

    void computeSinusoid(int x, int y, ofstream& deBugFile) {
        deBugFile << "Entering computeSinusoid()" << endl;
        angleInDegree = 0;
        while (angleInDegree <= 179) {
            angleInRadians = (double)angleInDegree / 180.0 * PI;
            double dist = CartesianDist(x, y, angleInRadians);
            int distInt = (int)dist;
            CartesianAry[distInt][angleInDegree]++;
            dist = PolarDist(x, y, angleInRadians);
            distInt = (int)dist;
            PolarAry[distInt][angleInDegree]++;
            angleInDegree++;
        }
        deBugFile << "Leaving computeSinusoid()" << endl;
    }


    double CartesianDist(int x, int y, double angleInRadians) {
        double newX = x * cos(angleInRadians) + y * sin(angleInRadians) + offSet;
        return newX;
    }

    double PolarDist(int x, int y, double angleInRadians) {
        double newX = x * cos(angleInRadians) + y * sin(angleInRadians) + offSet;
        return newX;
    }


    void reformatPrettyPrint(int** inAry, ofstream& outFile1) {       
        if (inAry == imgAry){
            outFile1 << numRows << " " << numCols << " " << minVal << " " << maxVal << "\n";
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    if (inAry[i][j] == 0)
                        outFile1 << ".  ";
                    else
                        outFile1 << "1  ";
                }
                outFile1 << endl;
            }
        }
        else 
        {   outFile1 << HoughDist << " " << HoughAngle << " " << minVal << " " << maxVal << "\n";
            for (int i = 0; i < HoughDist; i++) {
                for (int j = 0; j < HoughAngle; j++) {
                    if (inAry[i][j] == 0)
                        outFile1 << ".  ";
                    else
                        outFile1 << "1  ";
                }
                outFile1 << endl;
            }
        }
    }

    void imgReformat(int** inAry, int numRows, int numCols, int minVal, int maxVal, ofstream& outFile1) {
        // Writing numRows, numCols, minVal, maxVal to the file           
        outFile1 << numRows << " " << numCols << " " << minVal << " " << maxVal << "\n";

        // Convert maxVal to string and calculate its width
        string strMaxVal = to_string(maxVal);
        int width = strMaxVal.length();

        // Writing the image data to the file
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                outFile1 << inAry[i][j] << " ";
                string str = to_string(inAry[i][j]);
                int WW = str.length();
                while (WW < width) {
                    outFile1 << " ";
                    WW++;
                }
            }
            outFile1 << endl;
        }
    }
};

int main(int argc, char* argv[]) {


    ifstream inFile(argv[1]);
    if (!inFile) {
        cerr << "Error: Unable to open input file." << endl;
        return 1;
    }

    ofstream outFile1(argv[2]);
    if (!outFile1) {
        cerr << "Error: Unable to open output file 1." << endl;
        return 1;
    }

    ofstream deBugFile(argv[3]);
    if (!deBugFile) {
        cerr << "Error: Unable to open debug file." << endl;
        return 1;
    }

    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;

    int HoughAngle = 180;
    int HoughDist = 2 * sqrt(numRows * numRows + numCols * numCols);
    int ** imgAry = new int*[numRows];
    for (int i = 0; i < numRows; ++i) {
        imgAry[i] = new int[numCols](); // Initialize to zero
    }
    int ** CartesianAry = new int*[HoughDist];
    int ** PolarAry = new int*[HoughDist];
    for (int i = 0; i < HoughDist; ++i) {
        CartesianAry[i] = new int[HoughAngle]();
        PolarAry[i] = new int[HoughAngle]();
    }  
    for (int i = 0; i < HoughDist; ++i) {
        for (int j = 0; j < HoughAngle; ++j) {
            CartesianAry[i][j] = 0;
            PolarAry[i][j] = 0;
        }
    }

    int offSet = sqrt(numRows * numRows + numCols * numCols);


    HoughTransform hough(numRows, numCols, minVal, maxVal, HoughDist, HoughAngle, imgAry, CartesianAry, PolarAry);

    hough.loadImage(inFile, imgAry);
    outFile1 << "Original Image" << endl;
    hough.reformatPrettyPrint(imgAry, outFile1);

    hough.buildHoughSpace(outFile1, deBugFile);
    
    outFile1 << "Hough Space Cartesian" << endl;
    hough.reformatPrettyPrint(CartesianAry, outFile1);

    outFile1 << "Hough Space Polar" << endl;
    hough.reformatPrettyPrint(PolarAry, outFile1);

    inFile.close();
    outFile1.close();
    deBugFile.close();

    return 0;
}
