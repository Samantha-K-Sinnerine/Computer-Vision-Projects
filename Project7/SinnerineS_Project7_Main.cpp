#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// Define point structure
struct Point {
    int row;
    int col;
};

// Define CCProperty struct
struct CCProperty {
    int label;
    int numPixels;
    int minRow, minCol, maxRow, maxCol; // bounding box
};

// Define ChainCode class
class ChainCode {
public:
    int numRows, numCols, minVal, maxVal;
    int **imgAry;
    int **boundaryAry;
    int **CCAry;
    // The index is the chain directions from currentP to its eight neighbors
    Point coordOffset[8] = {{0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}};
    // The look-up table to update the lastZero chain direction from currentP to nextP.
    int zeroTable[8] = {6, 0, 0, 2, 2, 4, 4, 6}; 
    // the row and col position of the first none zero pixel.
    Point startP;
    // current none zero border pixel
    Point currentP;
    // next none-zero border pixel
    Point nextP;
    // Range from 0 to 7; it is the chain direction of the last zero scanned by currentP
    int lastQ;
    // the next scanning direction of currentP to find nextP, range from 0 to 7, need to mod 8.
    int nextDir; 
    // chain code direction from currentP to nextP
    int PchainDir;

public:
    ChainCode(int rows, int cols, int min, int max) {
        numRows = rows;
        numCols = cols;
        minVal = min;
        maxVal = max;
    }

    void zeroFramed(int **array) {
        for (int i = 0; i <= numRows + 1; i++) {
            for (int j = 0; j <= numCols + 1; j++) {
                array[i][j] = 0;
            }
        }
    }

    // Read from the label file onto inside frame of imgAry begin at (1,1)
   void loadImage(ifstream& inFile, int** imgAry) {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                inFile >> imgAry[i][j];
            }
        }
   }

    // zero out CCAry, for next connected component
    void clearCCAry(int**CCAry) {
        for (int i=0; i<numRows+2; i++) {
            for (int j=0; j<numCols+2; j++) {
                CCAry[i][j] = 0;
            }
        }
    }

    // Extract the pixels with ccLabel from imgAry to CCAry.
    void loadCCAry(int ccLabel, int **CCAry) {
        for (int i = 0; i < numRows + 2; i++) {
            for (int j = 0; j < numCols + 2; j++) {
                if (imgAry[i][j] == ccLabel) {
                    CCAry[i][j] = ccLabel;
                } else {
                    CCAry[i][j] = 0; // Set to zero if not part of the connected component
                }
            }
        }
    }
    

  
    void getChainCode(CCProperty CC, int **CCAry, ofstream& chainCodeFile, ofstream& deBugFile) {
        deBugFile << "Entering getChainCode method" << endl;
        int label = CC.label;
        startP.row = -1;
        startP.col = -1;
        currentP.row = -1;
        currentP.col = -1;   
        bool terminateLoops = false;

        for (int i = 1; i <= numRows && !terminateLoops; i++) {
            for (int j = 1; j <= numCols && !terminateLoops; j++) {
                if (CCAry[i][j] == label) {  
                    if (startP.row < 0  && startP.col < 0) { // Found the first none-zero pixel
                        chainCodeFile << i << " " << j << " " << label << endl; 
                        startP.row = i;
                        startP.col = j;
                        currentP.row = i;
                        currentP.col = j;
                        lastQ = 4;                    
                    }
                    else { // Found the next none-zero pixel    
                        int nextQ = (lastQ + 1) % 8; 
                        int PchainDir = findNextP(currentP, nextQ, deBugFile, label); 
                        chainCodeFile << PchainDir << " "; 

                        nextP.row = currentP.row + coordOffset[PchainDir].row; 
                        nextP.col = currentP.col + coordOffset[PchainDir].col; 
                        currentP = nextP; 

                        if (PchainDir == 0)
                            lastQ = zeroTable[7]; 
                        else
                            lastQ = zeroTable[PchainDir-1]; 

                        deBugFile << "lastQ = " << lastQ << "; nextQ = " << nextQ << "; currentP.row = " << currentP.row 
                        << "; currentP.col = " << currentP.col << ": nextP.row = " << nextP.row << ": nextP.col = " << nextP.col << endl;
                        if (currentP.row == startP.row && currentP.col == startP.col) {
                             terminateLoops = true; // Set flag to terminate both loops
                             break; // Terminate inner loop
                        } 
                    }
                }
            }
        }
        //reset startP and currentP
        startP.row = -1;
        startP.col = -1;
        currentP.row = -1;
        currentP.col = -1;   
        deBugFile << "Leaving getChainCode method" << endl;
    }

    int findNextP(Point currentP, int lastQ, ofstream& deBugFile, int label) {
        deBugFile << "Entering findNextP method" << endl;

        int index = lastQ;
        bool found = false;
        int chainDir;

        while (!found) {
            int iRow = currentP.row + coordOffset[index].row;
            int jCol = currentP.col + coordOffset[index].col;

            if (imgAry[iRow][jCol] == label) {
                chainDir = index;
                found = true;
            } else {
                index = (index + 1) % 8;
            }
        }

        deBugFile << "Leaving findNextP method" << endl;
        deBugFile << "chainDir = " << chainDir << endl;

        return chainDir;
    }


    void constructBoundary(int** boundaryAry, ifstream& chainCodeInputFile) {
        string line;
        
        getline(chainCodeInputFile, line); // Skip the header line
        int label, startRow, startCol;
        chainCodeInputFile >> startRow >> startCol >> label; // Read the starting point and label
    
        int currentRow = startRow;
        int currentCol = startCol;
        boundaryAry[currentRow][currentCol] = 1; // Mark the starting point

        int chainDir;
        while (chainCodeInputFile >> chainDir) {
            currentRow += coordOffset[chainDir].row;
            currentCol += coordOffset[chainDir].col;
            boundaryAry[currentRow][currentCol] = 1; // Mark the boundary pixel
        }
    }



    void reformatPrettyPrint(int **imgAry, ofstream& outFile1) {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (imgAry[i][j] == 0)
                    outFile1 << ".  ";
                else
                    outFile1 << "1  ";
            }
            outFile1 << endl;
        }
        outFile1 << endl;
    }

    void imgReformat(int **inAry, ofstream& outFile1) {
        outFile1 << numRows << " " << numCols << " " << minVal << " " << maxVal << "\n";
        int width = to_string(maxVal).length();
        
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                outFile1 << inAry[i][j] << " ";
                int WW = to_string(inAry[i][j]).length();
                while (WW < width) {
                    outFile1 << " ";
                    WW++;
                }
            }
            outFile1 << "\n";
        }
    }

};

int main(int argc, char *argv[]) {

    ifstream labelFile(argv[1]);
    ifstream propFile(argv[2]);
    ofstream outFile1(argv[3]);
    ofstream deBugFile(argv[4]);
    ofstream chainCodeFile(argv[5]);
    ofstream boundaryFile(argv[6]);

    // Read numRows, numCols, minVal, maxVal and numCC from labelFile and propFile
    int numRows, numCols, minVal, maxVal, numCC; 
    labelFile >> numRows >> numCols >> minVal >> maxVal;
    propFile >> numRows >> numCols >> minVal >> maxVal;
    propFile >> numCC;

    // Initialize ChainCode object
    ChainCode chainCode(numRows, numCols, minVal, maxVal);

    int** imgAry = new int*[numRows+2];
    for (int i=0; i<numRows+2; i++) {
        imgAry[i] = new int[numCols+2];
    }

    int** CCAry = new int*[numRows+2];  
    for (int i=0; i<numRows+2; i++) {
        CCAry[i] = new int[numCols+2];
    }

    int **boundaryAry = new int*[numRows+2];
    for (int i=0; i<numRows+2; i++) {
        boundaryAry[i] = new int[numCols+2];
    }

    // Zero frame, load image, reformatPrettyPrint
    chainCode.zeroFramed(imgAry);
    chainCode.loadImage(labelFile, imgAry);
    chainCode.reformatPrettyPrint(imgAry, outFile1);

    chainCode.imgAry = imgAry;
    chainCode.boundaryAry = boundaryAry;
    chainCode.CCAry = CCAry;

    // Output header and numCC to chainCodeFile
    chainCodeFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
    chainCodeFile << numCC;

    // Loop through each connected component
    for (int i = 0; i < numCC; i++) {
        CCProperty CC;
        // Read CC properties from propFile
        propFile >> CC.label >> CC.numPixels >> CC.minRow >> CC.minCol >> CC.maxRow >> CC.maxCol;

        // zero out the old CCAry for next CC
        chainCode.clearCCAry(CCAry);
        chainCode.loadCCAry(CC.label, CCAry);
        chainCode.reformatPrettyPrint(CCAry, deBugFile);
        chainCode.getChainCode(CC, CCAry, chainCodeFile, deBugFile);
    }


    chainCodeFile.close(); // Close the output stream

    // Reopen chainCodeFile for reading
    ifstream chainCodeInputFile(argv[5]);

    chainCode.zeroFramed(boundaryAry);
    chainCode.constructBoundary(boundaryAry, chainCodeInputFile);
    chainCode.reformatPrettyPrint(boundaryAry, outFile1);
    chainCode.imgReformat(boundaryAry, boundaryFile);

    // Close all files
    labelFile.close();
    propFile.close();
    outFile1.close();
    deBugFile.close();
    chainCodeFile.close();
    boundaryFile.close();

    return 0;
}
