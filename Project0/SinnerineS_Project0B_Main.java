
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

class Image {
    // instance variables
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int thrValue;

    Image() {
        numRows = 0;
        numCols = 0;
        minVal = 0;
        maxVal = 0;
        thrValue = 0;
    }

    void readHeader(BufferedReader inFile) throws IOException {
        String line = inFile.readLine();
        String[] header = line.split(" ");
        numRows = Integer.parseInt(header[0]);
        numCols = Integer.parseInt(header[1]);
        minVal = Integer.parseInt(header[2]);
        maxVal = Integer.parseInt(header[3]);
    }

    void writeHeader(BufferedWriter outFile, int newMinVal, int newMaxVal) throws IOException {
        outFile.write(numRows + " " + numCols + " " + newMinVal + " " + newMaxVal + "\n");
    }
}

public class SinnerineS_Project0_Main {
    static void processing(BufferedReader inFile, BufferedWriter outFile1, BufferedWriter outFile2, int thrValue)
            throws IOException {

        Image image = new Image();
        image.readHeader(inFile);

        // Output header to both output files with updated minVal and maxVal
        image.writeHeader(outFile1, 0, 1); // Updated for binary threshold operation
        image.writeHeader(outFile2, 0, image.maxVal); // Updated for non-binary threshold operation

        int pixelVal, count = 0;

        // Read pixel values from the input file
        String line;
        while ((line = inFile.readLine()) != null) {
            String[] pixels = line.split(" ");
            for (String pixelStr : pixels) {
                pixelVal = Integer.parseInt(pixelStr);
                if (pixelVal >= thrValue) {
                    outFile1.write("1 "); // binary threshold
                    outFile2.write(pixelVal + " "); // non-binary threshold
                } else {
                    outFile1.write("0 ");
                    outFile2.write("0  ");
                }

                count++;

                // Check if the end of the row is reached
                if (count >= image.numCols) {
                    // Write end of line to both output files
                    outFile1.write("\n");
                    outFile2.write("\n");

                    // Reset the count for the next row
                    count = 0;
                }
            }
        }
    }

    public static void main(String[] args) {
        if (args.length != 4) {
            System.err.println("Invalid Arguments: inFile thrValue outFile1 outFile2");
            System.exit(1);
        }

        try (BufferedReader inFile = new BufferedReader(new FileReader(args[0]));
                BufferedWriter outFile1 = new BufferedWriter(new FileWriter(args[2]));
                BufferedWriter outFile2 = new BufferedWriter(new FileWriter(args[3]))) {

            int thrValue = Integer.parseInt(args[1]);
            processing(inFile, outFile1, outFile2, thrValue);

        } catch (IOException e) {
            System.err.println("Error: Unable to open or process files");
            e.printStackTrace();
            System.exit(1);
        }
    }
}
