import java.io.*;
import java.util.Scanner;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class Edge {
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int[][] mask;

    // Constructor
    public Edge(int numRows, int numCols, int minVal, int maxVal) {
        this.numRows = numRows;
        this.numCols = numCols;
        this.minVal = minVal;
        this.maxVal = maxVal;

    }

    // load inFile into inside frame of MFAry
     public void loadImage(String inFile, int[][] MFAry) {
        try (BufferedReader reader = new BufferedReader(new FileReader(inFile))) {
            // Skip the first line (header)
            reader.readLine();
    
            // Read pixel values into MFAry
            for (int i = 1; i < numRows + 1; i++) {
                String[] values = reader.readLine().trim().split("\\s+");
                for (int j = 1; j < numCols + 1; j++) {
                    MFAry[i][j] = Integer.parseInt(values[j - 1]);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void zeroCrossingEdgeDetector(int[][] MFAry, int[][] edgeAry, int[][] mask, int[][] zeroCrossAry, String zeroCrossFile, String binEdgeFile, String deBugFile) {
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(deBugFile));
            computeEdge(MFAry, edgeAry, mask, bw);
            crossZero(edgeAry, zeroCrossAry, bw);
            BufferedWriter writer1 = new BufferedWriter(new FileWriter(zeroCrossFile));
            BufferedWriter writer2 = new BufferedWriter(new FileWriter(binEdgeFile));         

            writer1.write(Integer.toString(numRows) + " " + Integer.toString(numCols) + " 0" + " 1" + '\n');

            imgReformatPrettyPrint(zeroCrossAry, writer1);

            writer2.write(Integer.toString(numRows) + " " + Integer.toString(numCols) + " 0" + " 1" + '\n');

            writer1.close();
            
            bw.close();
            binaryEdgeDetector(zeroCrossAry, writer2);   
            writer2.close();  
                     
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    public void computeEdge(int[][] MFAry, int[][] edgeAry, int[][] mask, BufferedWriter bw) {
        try {
            
            bw.write("entering computeEdge() method\n");
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    edgeAry[i][j] = convolution3x3(MFAry, i, j, mask);
                }
            }
            bw.write("leaving computeEdge() method\n");           
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    // returns the result of the convolution of pixel MFAry (i, j) with the 3x3 mask
    public int convolution3x3(int[][] MFAry, int i, int j, int[][] mask) {
        int sum = 0;
        for (int r = -1; r <= 1; r++) {
            for (int c = -1; c <= 1; c++) {
                sum += MFAry[i + r][j + c] * mask[r + 1][c + 1];
            }
        }
        return sum;
    }

    private void crossZero(int[][] edgeAry, int[][] zeroCrossAry, BufferedWriter bw) {
        try {
            bw.write("entering crossZero() method\n");
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    zeroCrossAry[i][j] = edgeAry[i][j] <= 0 ? 0 : 1;
                }
            }
            bw.write("leaving crossZero() method\n");        
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // a pixel is an binary edge, if pixel(i,j) >= 1 and it has a least one 0 neighbor in its 3x3 neighborhood.
    public void binaryEdgeDetector(int[][] zeroCrossAry, BufferedWriter writer) {
        try {  
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (zeroCrossAry[i][j] >= 1 && hasZeroNeighbor(i, j, zeroCrossAry)) {
                        writer.write("1  ");
                    } else {
                        writer.write("0  ");
                    }
                }
                writer.write("\n");
            }
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    

    private boolean hasZeroNeighbor(int i, int j, int[][] zeroCrossAry) {
        // Check only the 4 neighbors (top, bottom, left, right)
        if (zeroCrossAry[i - 1][j] == 0 ||  // Top neighbor
            zeroCrossAry[i + 1][j] == 0 ||  // Bottom neighbor
            zeroCrossAry[i][j - 1] == 0 ||  // Left neighbor
            zeroCrossAry[i][j + 1] == 0) {  // Right neighbor
            return true;
        }
        return false;
    }
    

    void imgReformatPrettyPrint(int[][] MFAry, BufferedWriter writer) {
        try {
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (MFAry[i][j] == 0)
                        writer.write(".  ");
                    else
                        writer.write("1  ");
                }
                writer.write("\n");
            }
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public void imgReformat(int[][] inAry, String outFile1) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(outFile1));
            // Writing numRows, numCols, minVal, maxVal to the file           
            writer.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
    
            // Convert maxVal to string and calculate its width
            String strMaxVal = Integer.toString(maxVal);
            int width = strMaxVal.length();
    
            // Writing the image data to the file
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    writer.write(inAry[i][j] + " ");
                    String str = Integer.toString(inAry[i][j]);
                    int WW = str.length();
                    while (WW < width) {
                        writer.write(" ");
                        WW++;
                    }
                }
                writer.newLine();
            }
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }   
}


public class SinnerineS_Project5_Main {
    public static void main(String[] args) throws IOException {
        String inFile = args[0];
        String outFile1 = args[1];
        String zeroCrossFile = args[2];
        String binEdgeFile = args[3];
        String deBugFile = args[4];
        
        // Zero-crossing edge detection mask
        int[][] mask = {
            {0, -2, 0},
            {-2, 8, -2},
            {0, -2, 0}
        };

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

        // dynamically allocate 2D arrays
        int[][] MFAry = new int[numRows + 2][numCols + 2];
        int[][] edgeAry = new int[numRows + 2][numCols + 2];
        int[][] zeroCrossAry = new int[numRows + 2][numCols + 2];

        Edge edge = new Edge(numRows, numCols, minVal, maxVal);
        edge.loadImage(inFile, MFAry);
        // I made a slight adjustment to the specs here since using the imgReformat method 
        // instead of the imgReformatPrettyPrint method since there are no zeros in the input image 
        edge.imgReformat(MFAry, outFile1); 
        edge.zeroCrossingEdgeDetector (MFAry, edgeAry, mask, zeroCrossAry, zeroCrossFile, binEdgeFile, deBugFile);
    }
}
