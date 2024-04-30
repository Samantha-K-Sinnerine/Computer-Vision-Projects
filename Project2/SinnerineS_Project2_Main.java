import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;


public class enhancement {

    private int numRows;
    private int numCols;
    private int minVal;
    private int maxVal;

    public enhancement(int numRows, int numCols, int minVal, int maxVal) {
        this.numRows = numRows;
        this.numCols = numCols;
        this.minVal = minVal;
        this.maxVal = maxVal;
    }

    
     // Load the input image to the inside of frameAry begins at (2, 2)
    public void loadImage(String inFile, int[][] mirrorFramedAry) {
        try (BufferedReader reader = new BufferedReader(new FileReader(inFile))) {
            // Skip the first line (header)
            reader.readLine();
    
            // Read pixel values into mirrorFramedAry starting from (2, 2)
            for (int i = 2; i < numRows + 2; i++) {
                String[] values = reader.readLine().trim().split("\\s+");
                for (int j = 2; j < numCols + 2; j++) {
                    mirrorFramedAry[i][j] = Integer.parseInt(values[j - 2]);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Mirror framing method
    public void mirrorFraming(int[][] mirrorFramedAry) {
        // Copy row 2 to row 0 and row 1
        for (int j = 0; j < mirrorFramedAry[0].length; j++) {
            mirrorFramedAry[0][j] = mirrorFramedAry[2][j];
            mirrorFramedAry[1][j] = mirrorFramedAry[2][j];
        }

        // Copy row numRows + 1 to numRows + 2 and numRows + 3
        int numRows = mirrorFramedAry.length - 4;
        for (int j = 0; j < mirrorFramedAry[0].length; j++) {
            mirrorFramedAry[numRows + 2][j] = mirrorFramedAry[numRows + 1][j];
            mirrorFramedAry[numRows + 3][j] = mirrorFramedAry[numRows + 1][j];
        }

        // Copy column 2 to column 0 and column 1
        for (int i = 0; i < mirrorFramedAry.length; i++) {
            mirrorFramedAry[i][0] = mirrorFramedAry[i][2];
            mirrorFramedAry[i][1] = mirrorFramedAry[i][2];
        }

        // Copy column numCols + 1 to numCols + 2 and numCols + 3
        int numCols = mirrorFramedAry[0].length - 4;
        for (int i = 0; i < mirrorFramedAry.length; i++) {
            mirrorFramedAry[i][numCols + 2] = mirrorFramedAry[i][numCols + 1];
            mirrorFramedAry[i][numCols + 3] = mirrorFramedAry[i][numCols + 1];
        }
    }


    // Convolution 5x5 - Perform convolution on the 5x5 neighborhood of mirrorFramedAry (i,j) with one of the 8 masks, then returns the convolution result
    public int convolution5x5(int[][] mFAry, int i, int j, int[][] mask, String deBugFile) {
        int result = 0;
    
        try {
            // Open the debug file in append mode
            BufferedWriter writer = new BufferedWriter(new FileWriter(deBugFile, true));
            
            // Write entering message
            writer.write("Entering convolution5x5 method\n");
            
            // Perform convolution calculation
            for (int r = -2; r <= 2; r++) {
                for (int c = -2; c <= 2; c++) {
                    result += mask[r + 2][c + 2] * mFAry[i + r][j + c];
                }
            }
            
            // Write leaving message
            writer.write("Before leaving convolution5x5 method(): i=" + i + "; j=" + j + "; result=" + result + "\n");
            
            // Close the writer
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    
        return result;
    }

    // computeAvg5x5 based on algorithm
    public void computeAvg5x5(int[][] mirrorFramedAry, int[][] avgAry, String deBugFile) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(deBugFile, true));
            writer.write("Entering computeAvg5x5 method\n");
            int newMax = mirrorFramedAry[2][2];
            int newMin = mirrorFramedAry[2][2];

            for (int i = 2; i < numRows + 2; i++) {
                for (int j = 2; j < numCols + 2; j++) {
                    avgAry[i][j] = avg5x5(mirrorFramedAry, i, j);
                    if (newMax < avgAry[i][j]) {
                        newMax = avgAry[i][j];
                    }
                    if (newMin > avgAry[i][j]) {
                        newMin = avgAry[i][j];
                    }
                }
            }
            maxVal = newMax;
            minVal = newMin;
            writer.write("In computeAvg5x5: newMax=" + newMax + " newMin=" + newMin + "\n");
            writer.write("Leaving computeAvg5x5 method\n");
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Method to compute average of 5x5 neighborhood
    public int avg5x5(int[][] mirrorFramedAry, int row, int col) {
        int sum = 0;

        // Iterate over the 5x5 neighborhood
        for (int i = row - 2; i <= row + 2; i++) {
            for (int j = col - 2; j <= col + 2; j++) {
                sum += mirrorFramedAry[i][j];
            }
        }

        // Compute and return the average
        return sum / 25; // Since 5x5 = 25
    }

    // cornerPreserveAvg based on algorithm
    public void cornerPreserveAvg(int[][] frameAry, int[][][] mask, int[][] outAry, String deBugFile) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(deBugFile, true));
            writer.write("Entering cornerPreserveAvg() method\n");
            int newMax = frameAry[2][2];
            int newMin = frameAry[2][2];
            int numRows = frameAry.length - 4;
            int numCols = frameAry[0].length - 4;

            int maskIndex, minAvg, minDiff, diff, result;
            for (int i = 2; i < numRows + 2; i++) {
                for (int j = 2; j < numCols + 2; j++) {
                    maskIndex = 0;
                    minAvg = frameAry[i][j];
                    minDiff = 9999;
                    
                    for (maskIndex = 0; maskIndex < 8; maskIndex++) {
                        result = convolution5x5(frameAry, i, j, mask[maskIndex], deBugFile) / 9;
                        diff = Math.abs(result - frameAry[i][j]);
                        
                        if (diff < minDiff) {
                            minDiff = diff;
                            minAvg = result;
                        }
                    }
                    
                    outAry[i][j] = minAvg;
                    
                    if (newMax < minAvg) {
                        newMax = minAvg;
                    }
                    if (newMin > minAvg) {
                        newMin = minAvg;
                    }
                }
            }

            maxVal = newMax;
            minVal = newMin;
            writer.write("In cornerPreserveAvg(): newMax=" + newMax + " newMin=" + newMin + "\n");
            writer.write("Leaving cornerPreserveAvg() method\n");
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Compute Histogram using algorithm
    public void computeHist(int[][] inAry, int[] histAry, String deBugFile) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(deBugFile, true));
            writer.write("Entering computeHist() method\n");
            
            // Ensure histAry is initialized to 0
            for (int i = 0; i < histAry.length; i++) {
                histAry[i] = 0;
            }

            int numRows = inAry.length - 4;
            int numCols = inAry[0].length - 4;

            for (int i = 2; i < numRows + 2; i++) {
                for (int j = 2; j < numCols + 2; j++) {
                    histAry[inAry[i][j]]++;
                }
            }

            writer.write("Leaving computeHist() method\n");
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    // Display Histogram using method from project 1
    public void dispHist(int[] histAry, BufferedWriter writer) {
        try {
            writer.newLine();
            for (int i = 0; i <= maxVal; ++i) {
                writer.write(i + " (" + histAry[i] + "):");
                for (int j = 0; j < histAry[i]; ++j) {
                    writer.write("+");
                }
                writer.write("\n");
            }
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Output the histogram to outFile3
    public void printHist(int[] histAry, String histFile, String deBugFile) {
        try {
            // Write debug message
            BufferedWriter debugWriter = new BufferedWriter(new FileWriter(deBugFile, true));
            debugWriter.write("Entering printHist() method\n");

            // Write histogram to outFile3
            BufferedWriter writer = new BufferedWriter(new FileWriter(histFile, true));
            writer.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
            int index = 0;
            while (index <= maxVal) {
                writer.write(index + " " + histAry[index] + "\n");
                index++;
            }
            
            debugWriter.write("Leaving printHist() method\n");
            writer.close();
            debugWriter.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Image Reformatting using algorithm
    public void imgReformat(int[][] inAry, BufferedWriter writer) {
        try {
            // Writing numRows, numCols, minVal, maxVal to the file           
            writer.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");

            // Convert maxVal to string and calculate its width
            String strMaxVal = Integer.toString(inAry[0][1]);
            int width = strMaxVal.length();
    
            // Writing the image data to the file
            for (int i = 2; i < inAry.length - 2; i++) {
                for (int j = 2; j < inAry[i].length - 2; j++) {
                    writer.write(inAry[i][j] + " ");
                    String str = Integer.toString(inAry[i][j]);
                    int WW = str.length();
                    while (WW <= width) {
                        writer.write(" ");
                        WW++;
                    }
                }
                writer.newLine();
            }
    
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}


public class SinnerineS_Project2_Main {

    public static void main(String[] args) {
        // Read command line arguments
        String inFile = args[0];
        int choice = Integer.parseInt(args[1]);
        if (choice != 1 && choice != 2) {
            System.out.println("Invalid choice. Please enter 1 or 2");
            System.exit(1);
        }
        String outFile1 = args[2];
        String outFile2 = args[3]; // for printing the result of averaging or corner-preserve averaging.
        String outFile3 = args[4]; // for printing the histogram of the enhanced image
        String deBugFile = args[5];
        
        // Read image dimensions and pixel values from inFile
        int numRows = 0, numCols = 0, minVal = 0, maxVal = 0;
        try (BufferedReader reader = new BufferedReader(new FileReader(inFile))) {
            // Create a Scanner instance to read from the file
            Scanner scanner = new Scanner(reader);

            // Read values from the first line using Scanner's methods
            if (scanner.hasNextInt()) {
                numRows = scanner.nextInt();
            }
            if (scanner.hasNextInt()) {
                numCols = scanner.nextInt();
            }
            if (scanner.hasNextInt()) {
                minVal = scanner.nextInt();
            }
            if (scanner.hasNextInt()) {
                maxVal = scanner.nextInt();
            }
            scanner.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        // System.out.println("numRows: " + numRows + " numCols: " + numCols + " minVal: " + minVal + " maxVal: " + maxVal);
        enhancement imageEnhancement = new enhancement(numRows, numCols, minVal, maxVal);

        // Hardcoded 3D mask array
        int[][][] mask = {
            // Mask 1
            {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 1, 1, 1, 0},
                {1, 1, 1, 1, 1}
            },
            // Mask 2
            {
                {1, 0, 0, 0, 0},
                {1, 1, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 0, 0, 0},
                {1, 0, 0, 0, 0}
            },
            // Mask 3
            {
                {1, 1, 1, 1, 1},
                {0, 1, 1, 1, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0}
            },
            // Mask 4
            {
                {0, 0, 0, 0, 1},
                {0, 0, 0, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 0, 1, 1},
                {0, 0, 0, 0, 1}
            },
            // Mask 5
            {
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0}
            },
            // Mask 6
            {
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0}
            },
            // Mask 7
            {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1}
            },
            // Mask 8
            {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0}
            }
        };

        // Dynamically allocate and initialize arrays
        int[][] mirrorFramedAry = new int[numRows + 4][numCols + 4];
        for (int i = 0; i < mirrorFramedAry.length; i++) {
            for (int j = 0; j < mirrorFramedAry[0].length; j++) {
                mirrorFramedAry[i][j] = 0;
            }
        }

        int[][] avgAry = new int[numRows + 4][numCols + 4];
        for (int i = 0; i < avgAry.length; i++) {
            for (int j = 0; j < avgAry[0].length; j++) {
                avgAry[i][j] = 0;
            }
        }

        int[][] CPavgAry = new int[numRows + 4][numCols + 4];
        for (int i = 0; i < CPavgAry.length; i++) {
            for (int j = 0; j < CPavgAry[0].length; j++) {
                CPavgAry[i][j] = 0;
            }
        }

        int[] Avg_histAry = new int[maxVal + 1];
        for (int i = 0; i < Avg_histAry.length; i++) {
            Avg_histAry[i] = 0;
        }

        int[] CPavg_histAry = new int[maxVal + 1];
        for (int i = 0; i < CPavg_histAry.length; i++) {
            CPavg_histAry[i] = 0;
        }
        
        // Load image
        imageEnhancement.loadImage(inFile, mirrorFramedAry);

        // Mirror framing
        imageEnhancement.mirrorFraming(mirrorFramedAry);
        try{
            // Output to files
            BufferedWriter outFile1Writer = new BufferedWriter(new FileWriter(outFile1));
            BufferedWriter outFile2Writer = new BufferedWriter(new FileWriter(outFile2));
            BufferedWriter outFile3Writer = new BufferedWriter(new FileWriter(outFile3));

            outFile1Writer.write("Below is the image reformatted mirrorFramedAry\n");

            imageEnhancement.imgReformat(mirrorFramedAry, outFile1Writer);
            outFile1Writer.newLine();

            // choice == 1 - 5x5 averaging (AVG)
            if (choice == 1) {
                imageEnhancement.computeAvg5x5(mirrorFramedAry, avgAry, deBugFile);
                outFile1Writer.newLine();
                outFile1Writer.write("Below is the reformatted image of the result of 5x5 averaging on the input image\n");
                imageEnhancement.imgReformat(avgAry, outFile1Writer);  // print reformatted avgAry to outFile1
                imageEnhancement.imgReformat(avgAry, outFile2Writer);  // print reformatted avgAry to outFile2
                imageEnhancement.computeHist(avgAry, Avg_histAry, deBugFile);
                imageEnhancement.dispHist(Avg_histAry, outFile1Writer);
                outFile3Writer.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
                imageEnhancement.printHist(Avg_histAry, outFile3, deBugFile);
            }
            // choice == 2 - 5x5 corner-Preserve-averaging (CPavg)
            else if (choice == 2) {
                imageEnhancement.cornerPreserveAvg(mirrorFramedAry, mask, CPavgAry, deBugFile);
                outFile1Writer.write("Below is reformatted image of the result of corner-preserve averaging on the input image\n");              
                imageEnhancement.imgReformat(CPavgAry, outFile1Writer);  // print reformatted avgAry to outFile1
                imageEnhancement.imgReformat(CPavgAry, outFile2Writer);  // print reformatted avgAry to outFile2
                imageEnhancement.computeHist(CPavgAry, CPavg_histAry, deBugFile);
                imageEnhancement.dispHist(CPavg_histAry, outFile1Writer);
                outFile3Writer.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
                imageEnhancement.printHist(CPavg_histAry, outFile3, deBugFile);
            }
            outFile1Writer.close();
            outFile2Writer.close();
            outFile3Writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}