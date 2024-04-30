#include <iostream>
#include <fstream>

using namespace std;

class Morphology {
private:
    int numImgRows;
    int numImgCols;
    int imgMin;
    int imgMax;
    int numStructRows;
    int numStructCols;
    int structMin;
    int structMax;
    int rowOrigin;
    int colOrigin;
    int** structAry;
    int rowFrameSize = numStructRows / 2;
    int colFrameSize = numStructCols / 2;

    int extraRows = rowFrameSize * 2;
    int extraCols = colFrameSize * 2;

    int rowSize = numImgRows + extraRows;
    int colSize = numImgCols + extraCols;

public:
// Constructor
    Morphology(int numRows, int numCols, int min, int max, int structRows, int structCols, int sMin, int sMax, int rOrigin, int cOrigin, int** structArray)
        : numImgRows(numRows), numImgCols(numCols), imgMin(min), imgMax(max),
          numStructRows(structRows), numStructCols(structCols), structMin(sMin), structMax(sMax),
          rowOrigin(rOrigin), colOrigin(cOrigin), structAry(structArray) {}

    // Set the entire Ary (nRows by nCols) to zero
    void zero2DAry(int** Ary, int nRows, int nCols) {
        for (int i = 0; i < nRows; ++i) {
            for (int j = 0; j < nCols; ++j) {
                Ary[i][j] = 0;
            }
        }
    }

    // load imgFile to zeroFramedAry inside of frame, begins at (rowOrigin, colOrigin).
    void loadImg(ifstream& imgFile, int** zeroFramedAry) {
        int value;
        for (int i = rowOrigin; i < rowSize - rowFrameSize; ++i) {
            for (int j = colOrigin; j < colSize - colFrameSize; ++j) {
                imgFile >> value;
                zeroFramedAry[i][j] = value;
            }
        }
    }

    // load structFile to structAry
    void loadstruct(ifstream& structFile, int** structAry) {
        int value;                            
        for (int i = 0; i < numStructRows; ++i) {
            for (int j = 0; j < numStructCols; ++j) {
                structFile >> value;
                structAry[i][j] = value;
            }
        }
    }

    void ComputeDilation(int** inAry, int** outAry) {
    // process dilation on each pixel inside of inAry
        for (int i = rowFrameSize; i < rowSize - rowFrameSize; ++i) {
            for (int j = colFrameSize; j < colSize - colFrameSize; ++j) {
                if (inAry[i][j] > 0) {
                    onePixelDilation(i, j, inAry, outAry);
                }
            }
        }
    } 

    void ComputeErosion(int** inAry, int** outAry) {
        // process erosion on each pixel inside of inAry
        for (int i = rowFrameSize; i < rowSize - rowFrameSize; ++i) {
            for (int j = colFrameSize; j < colSize - colFrameSize; ++j) {
                if (inAry[i][j] > 0) {
                    onePixelErosion(i, j, inAry, outAry);
                }
            }
        }
    }

    void ComputeOpening(int** inAry, int** outAry, int** tmpAry) {
        ComputeErosion(inAry, tmpAry);
        ComputeDilation(tmpAry, outAry);
    }

    void ComputeClosing(int** inAry, int** outAry, int** tmpAry) {
        ComputeDilation(inAry, tmpAry);
        ComputeErosion(tmpAry, outAry);
    }

    void onePixelDilation(int i, int j, int** inAry, int** outAry) {
        int iOffset = i - rowOrigin;
        int jOffset = j - colOrigin;
         // translation of image’s coordinate (i, j) with respected of the origin of the structuring element
        for (int r = 0; r < numStructRows; ++r) {
            for (int c = 0; c < numStructCols; ++c) {
                if (structAry[r][c] > 0) {
                    outAry[iOffset + r][jOffset + c] = 1;
                }
            }
        }
    }

    void onePixelErosion(int i, int j, int** inAry, int** outAry) {
        int iOffset = i - rowOrigin;
        int jOffset = j - colOrigin;
        // translation of image’s coordinate (i, j) with respected of the origin of the structuring element
        bool matchFlag = true;

        for (int r = 0; r < numStructRows && matchFlag; ++r) {
            for (int c = 0; c < numStructCols && matchFlag; ++c) {
                if (structAry[r][c] > 0 && inAry[iOffset + r][jOffset + c] <= 0) {
                    matchFlag = false;
                }
            }
        }

        if (matchFlag) {
            outAry[i][j] = 1;
        } else {
            outAry[i][j] = 0;
        }
    }

    void AryToFile(int** Ary, ofstream& outFile) {
        // Output image header (assuming it's already been read from input)
        outFile << numImgRows << " " << numImgCols << " " << imgMin << " " << imgMax << endl;
        // Output rows and columns of Ary to outFile, excluding the framed borders
        for (int i = rowFrameSize; i < rowSize - rowFrameSize; ++i) {
            for (int j = colFrameSize; j < colSize - colFrameSize; ++j) {
                outFile << Ary[i][j] << " ";
            }
            outFile << endl;
        }
    }

    void prettyPrint(int** Ary, ofstream& outFile) {
        
        // Output for Structured Element (no frame)
        if (Ary == structAry) {
            outFile << numStructRows << " " << numStructCols << " " << structMin << " " << structMax << endl;
            for (int i = 0; i < numStructRows; ++i) {
                for (int j = 0; j < numStructCols; ++j) {
                    if (Ary[i][j] == 0)
                        outFile << ". ";
                    else
                        outFile << "1 ";
                }
                outFile << endl;
            }
            return;
        }
        else {
            outFile << numImgRows << " " << numImgCols << " " << imgMin << " " << imgMax << endl;
            // Output rows and columns of Ary to outFile, excluding the framed borders
            for (int i = rowFrameSize; i < rowSize - rowFrameSize; ++i) {
                for (int j = colFrameSize; j < colSize - colFrameSize; ++j) {
                    if (Ary[i][j] == 0)
                        outFile << ". ";
                    else
                        outFile << "1 ";
                }
                outFile << endl;
            }
        }     
    }
};

int main(int argc, char* argv[]) {
    if (argc != 8) {
        cout << "Usage: " << argv[0] << " imgFile structFile dilateOutFile erodeOutFile closingOutFile openingOutFile prettyPrintFile" << endl;
        return 1;
    }

    // open files from the command line
    ifstream imgFile(argv[1]);
    ifstream structFile(argv[2]);
    ofstream dilateOutFile(argv[3]);
    ofstream erodeOutFile(argv[4]);
    ofstream closingOutFile(argv[5]);
    ofstream openingOutFile(argv[6]);
    ofstream prettyPrintFile(argv[7]);

    // read in headers and origin
    int numImgRows, numImgCols, imgMin, imgMax;
    int numStructRows, numStructCols, structMin, structMax;
    int rowOrigin, colOrigin;

    imgFile >> numImgRows >> numImgCols >> imgMin >> imgMax;
    structFile >> numStructRows >> numStructCols >> structMin >> structMax >> rowOrigin >> colOrigin;


    // dynamically allocate 2D arrays- zeroFramedAry, structAry, morphAry, tempAry
    int rowFrameSize = numStructRows / 2;
    int colFrameSize = numStructCols / 2;

    int extraRows = rowFrameSize * 2;
    int extraCols = colFrameSize * 2;

    int rowSize = numImgRows + extraRows;
    int colSize = numImgCols + extraCols;

    int** zeroFramedAry = new int*[rowSize];
    int** morphAry = new int*[rowSize];
    int** tempAry = new int*[rowSize];
    for (int i = 0; i < rowSize; ++i) {
        zeroFramedAry[i] = new int[colSize]();
        morphAry[i] = new int[colSize]();
        tempAry[i] = new int[colSize]();
    }

    int** structAry = new int*[numStructRows];
    for (int i = 0; i < numStructRows; ++i) {
        structAry[i] = new int[numStructCols]();
    }   


    Morphology morphology(numImgRows, numImgCols, imgMin, imgMax, numStructRows, numStructCols, structMin, structMax, rowOrigin, colOrigin, structAry);

    morphology.zero2DAry(zeroFramedAry, rowSize, colSize); 
    morphology.loadImg(imgFile, zeroFramedAry);
    prettyPrintFile << "Zero Framed Image" << endl;
    morphology.prettyPrint(zeroFramedAry, prettyPrintFile);

    morphology.zero2DAry(structAry, numStructRows, numStructCols);
    morphology.loadstruct(structFile, structAry);
    prettyPrintFile << "\nStructuring Element" << endl;
    morphology.prettyPrint (structAry, prettyPrintFile);

    morphology.zero2DAry(morphAry, rowSize, colSize);
    morphology.ComputeDilation(zeroFramedAry, morphAry);
    morphology.AryToFile(morphAry, dilateOutFile); 
    prettyPrintFile << "\nDilation" << endl;
    morphology.prettyPrint(morphAry, prettyPrintFile);

    morphology.zero2DAry(morphAry, rowSize, colSize);
    morphology.ComputeErosion(zeroFramedAry, morphAry);
    morphology.AryToFile(morphAry, erodeOutFile);
    prettyPrintFile << "\nErosion" << endl;
    morphology.prettyPrint(morphAry, prettyPrintFile);

    morphology.zero2DAry(morphAry, rowSize, colSize);
    morphology.ComputeOpening(zeroFramedAry, morphAry, tempAry);
    morphology.AryToFile(morphAry, openingOutFile);
    prettyPrintFile << "\nOpening" << endl;
    morphology.prettyPrint(morphAry, prettyPrintFile);
    
    morphology.zero2DAry(morphAry, rowSize, colSize);
    morphology.ComputeClosing(zeroFramedAry, morphAry, tempAry); 
    morphology.AryToFile(morphAry, closingOutFile);
    prettyPrintFile << "\nClosing" << endl;
    morphology.prettyPrint(morphAry, prettyPrintFile);

    imgFile.close();
    structFile.close();
    dilateOutFile.close();
    erodeOutFile.close();
    closingOutFile.close();
    openingOutFile.close();
    prettyPrintFile.close();

    return 0;
}
