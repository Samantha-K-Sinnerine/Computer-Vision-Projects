#include <iostream>
#include <fstream>

using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Property {
    int label;
    int numPixels;
    int minR;
    int minC;
    int maxR;
    int maxC;
};

class ccLabel {
public:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int newLabel;
    int trueNumCC;
    int newMin;
    int newMax;
    int **zeroFramedAry;
    int *EQAry;
    char option;
    Property *CCproperty;
    int *NonZeroNeighborAry;

public:

    // constructor - need to dynamically allocate all arrays; and assign values to numRows, etc
    ccLabel(int rows, int cols, int min, int max, char opt) {
        numRows = rows;
        numCols = cols;
        minVal = min;
        maxVal = max;
        option = opt;
        newLabel = 0;
        trueNumCC = 0;
        newMin = 0;
        newMax = 0;

        CCproperty = new Property[trueNumCC + 1];

        int *EQAry = new int[(numRows * numCols) / 4];
        for (int i = 0; i < (numRows * numCols) / 4; i++) {
            EQAry[i] = i;
        }

    }

    // ** Initialized a 2-D array to zero
    void zero2D(int** arr) {
        for (int i = 0; i < numRows + 2; i++) {
            for (int j = 0; j < numCols + 2; j++) {
                arr[i][j] = 0;
            }
        }
    }

    // ** Initialized a 1-D array to -1
   void negative1D(int* arr) {
        int size = sizeof(arr) / sizeof(arr[0]); // Calculate the size of the array
        for (int i = 0; i < size; i++) {
            arr[i] = -1;
        }
    }

    // read from input file and write to zeroFramedAry begin at (1,1)
   void loadImage(ifstream& inFile, int** zeroFramedAry) {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                inFile >> zeroFramedAry[i][j];
            }
        }
    }

     // converts every pixel inside the zeroFramedAry begin at (1,1) from 0 to 1 and 1 to zero.
     // leave the frame boarder to 0.
    void conversion(int **zeroFramedAry) {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                zeroFramedAry[i][j] = (zeroFramedAry[i][j] == 0) ? 1 : 0;  
            }
        }
    }



    void imgReformat(int** zeroFramedAry, ofstream& RFprettyPrintFile) {
        RFprettyPrintFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
        // Output rows and columns of zeroFramedAry to RFprettyPrintFile, excluding the frame
        for (int i = 1; i < numRows; ++i) {
            for (int j = 1; j < numCols; ++j) {
                if (zeroFramedAry[i][j] == 0)
                    RFprettyPrintFile << ". ";
                else
                    RFprettyPrintFile << "1 ";
            }
            RFprettyPrintFile << endl;
        }
    }

    void connected4(int** zeroFramedAry, int& newLabel, ofstream& RFprettyPrintFile, ofstream& deBugFile) {
    deBugFile << "Entering connected4 method" << endl;
    
    // Step 1: Perform Pass 1 for 4-connectivity
    connect4Pass1(zeroFramedAry, newLabel, EQAry);
    deBugFile << "After connected4 pass1, newLabel = " << newLabel << endl;
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, RFprettyPrintFile);

    // Step 2: Perform Pass 2 for 4-connectivity
    connect4Pass2(zeroFramedAry, EQAry);
    deBugFile << "After connected4 pass2, newLabel = " << newLabel << endl;
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, RFprettyPrintFile);

    // Step 3: Manage EQAry to determine trueNumCC
    int trueNumCC = manageEQAry(EQAry, newLabel);
    printEQAry(newLabel, RFprettyPrintFile);
    deBugFile << "In connected4, after manage EQAry, trueNumCC = " << trueNumCC << endl;

    // Step 4: Perform Pass 3 and update CCproperty
    connectPass3(zeroFramedAry, EQAry, CCproperty, trueNumCC, deBugFile);

    // Additional steps (if any)
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, RFprettyPrintFile);
    
    deBugFile << "Leaving connected4 method" << endl;
}

void connected8(int** zeroFramedAry, int& newLabel, ofstream& RFprettyPrintFile, ofstream& deBugFile) {
    deBugFile << "Entering connected8 method" << endl;
    
    // Step 1: Perform Pass 1 for 8-connectivity
    connect8Pass1(zeroFramedAry, newLabel, EQAry);
    deBugFile << "After connected8 pass1, newLabel = " << newLabel << endl;
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, RFprettyPrintFile);

    // Step 2: Perform Pass 2 for 8-connectivity
    connect8Pass2(zeroFramedAry, EQAry);
    deBugFile << "After connected8 pass2, newLabel = " << newLabel << endl;
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, RFprettyPrintFile);

    // Step 3: Manage EQAry to determine trueNumCC
    int trueNumCC = manageEQAry(EQAry, newLabel);
    printEQAry(newLabel, RFprettyPrintFile);
    deBugFile << "In connected8, after manage EQAry, trueNumCC = " << trueNumCC << endl;

    // Step 4: Perform Pass 3 and update CCproperty
    connectPass3(zeroFramedAry, EQAry, CCproperty, trueNumCC, deBugFile);

    // Additional steps (if any)
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, RFprettyPrintFile);
    
    deBugFile << "Leaving connected8 method" << endl;
}




    void connect4Pass1(int** zeroFramedAry, int& newLabel, int* EQAry) {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (zeroFramedAry[i][j] > 0) {
                    int a = zeroFramedAry[i - 1][j];
                    int b = zeroFramedAry[i][j - 1];
                    if (a == 0 && b == 0) {
                        newLabel++;
                        zeroFramedAry[i][j] = newLabel;
                    } else {
                        int minLabel = min(a, b);
                        if (a != 0 && b != 0 && a != b)
                            minLabel = min(minLabel, min(a, b));
                        zeroFramedAry[i][j] = minLabel;
                    }
                }
            }
        }
    }

    void connect4Pass2(int** zeroFramedAry, int* EQAry) {
        for (int i = numRows; i >= 1; i--) {
            for (int j = numCols; j >= 1; j--) {
                if (zeroFramedAry[i][j] > 0) {
                    int e = zeroFramedAry[i][j + 1];
                    int f = zeroFramedAry[i + 1][j];
                    int minLabel = min(e, f);
                    if (e != 0 && f != 0 && e != f)
                        minLabel = min(minLabel, min(e, f));
                    if (zeroFramedAry[i][j] > minLabel) {
                        EQAry[zeroFramedAry[i][j]] = minLabel;
                        zeroFramedAry[i][j] = minLabel;
                    }
                }
            }
        }
    }


    void connect8Pass1(int** zeroFramedAry, int& newLabel, int* EQAry) {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (zeroFramedAry[i][j] > 0) {
                    int a = zeroFramedAry[i - 1][j - 1];
                    int b = zeroFramedAry[i - 1][j];
                    int c = zeroFramedAry[i - 1][j + 1];
                    int d = zeroFramedAry[i][j - 1];
                    // case 1
                    if (a == b && b == c && c == d) {
                        newLabel++;
                        zeroFramedAry[i][j] = newLabel;
                    // case 2
                    } else {
                        int minLabel = min(min(min(a, b), c), d);
                        // case 3
                        if (a != 0 && b != 0 && c != 0 && d != 0 && a != b && b != c && c != d)
                            minLabel = min(minLabel, min(a, min(b, min(c, d))));
                        zeroFramedAry[i][j] = minLabel;
                    }
                }
            }
        }
    }

    void connect8Pass2(int** zeroFramedAry, int* EQAry) {
        for (int i = numRows; i >= 1; i--) {
            for (int j = numCols; j >= 1; j--) {
                if (zeroFramedAry[i][j] > 0) {
                    int e = zeroFramedAry[i][j + 1];
                    int f = zeroFramedAry[i + 1][j + 1];
                    int g = zeroFramedAry[i + 1][j];
                    int h = zeroFramedAry[i + 1][j - 1];
                    // case 1
                    int minLabel = min(min(min(min(e, f), g), h), zeroFramedAry[i][j]);
                    // case 2
                    if (e != 0 && f != 0 && g != 0 && h != 0 && e != f && f != g && g != h)
                        minLabel = min(minLabel, min(e, min(f, min(g, h))));
                    // case 3
                    if (zeroFramedAry[i][j] > minLabel) {
                        EQAry[zeroFramedAry[i][j]] = minLabel;
                        zeroFramedAry[i][j] = minLabel;
                    }
                }
            }
        }
    }


    void connectPass3(int** zeroFramedAry, int* EQAry, Property* CCproperty, int trueNumCC, ofstream& deBugFile) {
        deBugFile << "entering connectPas3 method" << endl;
        
        // Initialize CCproperty array
        for (int i = 1; i <= trueNumCC; i++) {
            CCproperty[i].label = i;
            CCproperty[i].numPixels = 0;
            CCproperty[i].minR = numRows;
            CCproperty[i].maxR = 0;
            CCproperty[i].minC = numCols;
            CCproperty[i].maxC = 0;
        }

        // Scan the zeroFramedAry to update properties and EQAry
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                int label = EQAry[zeroFramedAry[i][j]];
                if (zeroFramedAry[i][j] > 0) {
                    zeroFramedAry[i][j] = label;
                    CCproperty[label].numPixels++;
                    CCproperty[label].minR = min(CCproperty[label].minR, i);
                    CCproperty[label].maxR = max(CCproperty[label].maxR, i);
                    CCproperty[label].minC = min(CCproperty[label].minC, j);
                    CCproperty[label].maxC = max(CCproperty[label].maxC, j);

                    // Update EQAry for non-minimum label neighbors
                    updateEQ(zeroFramedAry, EQAry, i, j, label);
                }
            }
        }

        deBugFile << "leaving connectPas3 method" << endl;
    }


    // Update EQAry for all non-zero neighbors to minLabel.
    void updateEQ(int** zeroFramedAry, int* EQAry, int i, int j, int minLabel) {
        int nonZeroNeighbors[4] = { zeroFramedAry[i - 1][j], zeroFramedAry[i][j - 1], zeroFramedAry[i][j + 1], zeroFramedAry[i + 1][j] };
        for (int k = 0; k < 4; ++k) {
            int neighborLabel = nonZeroNeighbors[k];
            if (neighborLabel != 0 && neighborLabel != minLabel)
                EQAry[neighborLabel] = minLabel;
        }
    }

    int manageEQAry(int* EQAry, int& newLabel) {
        for (int i = 1; i <= newLabel; i++) {
            if (EQAry[i] == i)
                trueNumCC++;
            else
                EQAry[i] = EQAry[EQAry[i]]; // Path compression
        }
        return trueNumCC;
    }


    void printCCproperty(ofstream& propertyFile) {
        // Print header of the input image
        propertyFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

        // Print total number of connected components
        propertyFile << trueNumCC << endl;

        // Loop through each connected component and print its properties
        for (int i = 1; i <= trueNumCC; i++) {
            propertyFile << CCproperty[i].label << endl;
            propertyFile << CCproperty[i].numPixels << endl;
            propertyFile << CCproperty[i].minR << " " << CCproperty[i].minC << endl;
            propertyFile << CCproperty[i].maxR << " " << CCproperty[i].maxC << endl;
        }
    }


    void printEQAry(int& newLabel, ofstream& RFprettyPrintFile) {
        RFprettyPrintFile << "EQAry up to newLabel = " << newLabel << ":\n";
        for (int i = 1; i <= newLabel; ++i) {
            RFprettyPrintFile << "EQAry[" << i << "] = " << EQAry[i] << endl;
        }
    }



    void drawBoxes(int** zeroFramedAry, ofstream& deBugFile) {
        deBugFile << "entering drawBoxes method" << endl;

        // Iterate through each connected component
        for (int index = 1; index <= trueNumCC; index++) {
            int minRow = CCproperty[index].minR + 1;
            int minCol = CCproperty[index].minC + 1;
            int maxRow = CCproperty[index].maxR + 1;
            int maxCol = CCproperty[index].maxC + 1;
            int label = CCproperty[index].label;

            // Draw bounding box along minRow
            for (int j = minCol; j <= maxCol; j++) {
                zeroFramedAry[minRow][j] = label;
            }

            // Draw bounding box along maxRow
            for (int j = minCol; j <= maxCol; j++) {
                zeroFramedAry[maxRow][j] = label;
            }

            // Draw bounding box along minCol
            for (int i = minRow; i <= maxRow; i++) {
                zeroFramedAry[i][minCol] = label;
            }

            // Draw bounding box along maxCol
            for (int i = minRow; i <= maxRow; i++) {
                zeroFramedAry[i][maxCol] = label;
            }
        }

        deBugFile << "leaving drawBoxes method" << endl;
    }

    void printImg(int** zeroFramedAry, ofstream& labelFile) {
        // Output image header to labelFile
        labelFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

        // Output zeroFramedAry (inside of framing) to labelFile
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                labelFile << zeroFramedAry[i][j] << " ";
            }
            labelFile << endl;
        }
    }


};

int main(int argc, char *argv[]) {

    // Open input file
    ifstream inFile(argv[1]);
    char connectness = argv[2][0];
    char option = argv[3][0];
    ofstream RFprettyPrintFile(argv[4]);
    ofstream labelFile(argv[5]);
    ofstream propertyFile(argv[6]);
    ofstream deBugFile(argv[7]);

    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;

    ccLabel connectedComponent(numRows, numCols, minVal, maxVal, option);

    // Dynamically allocate zeroFramedAry and EQAry
    int **zeroFramedAry = new int*[numRows + 2];
    for (int i = 0; i < numRows + 2; ++i) {
        zeroFramedAry[i] = new int[numCols + 2];
    }


  

    int newLabel = 0;

    connectedComponent.zero2D(zeroFramedAry);

    connectedComponent.loadImage(inFile, zeroFramedAry);

    if (option == 'y' || option == 'Y') {
        connectedComponent.conversion(zeroFramedAry);
    }

    // Initialize EQAry

    if (connectness == '4') {
        connectedComponent.connected4(zeroFramedAry, newLabel,RFprettyPrintFile, deBugFile);
    } else if (connectness == '8') {
        connectedComponent.connected8(zeroFramedAry, newLabel, RFprettyPrintFile, deBugFile);
    }


    labelFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
    connectedComponent.printImg(zeroFramedAry, labelFile);

    connectedComponent.printCCproperty(propertyFile);
    connectedComponent.drawBoxes (zeroFramedAry, deBugFile);
    connectedComponent.imgReformat(zeroFramedAry, RFprettyPrintFile);

    RFprettyPrintFile << "trueNumCC: " << connectedComponent.trueNumCC << endl;

    // Close all files
    inFile.close();
    RFprettyPrintFile.close();
    labelFile.close();
    propertyFile.close();
    deBugFile.close();

    return 0;
}
