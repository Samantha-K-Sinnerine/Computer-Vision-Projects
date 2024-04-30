# Computer-Vision-Projects

Welcome to the Computer Vision Projects repository! This collection comprises eight projects covering a diverse range of topics in computer vision. Each project aims to explore fundamental concepts and techniques in image processing and analysis. Below is a brief overview of the projects and their purpose included:

1. **Binary and Non-Binary Thresholding**:
   - Given a grey-scale image and a threshold value, the binary/non-binary threshold operation is to transform pixels in the input image (imgIn) from grey-scale to binary/non-binary values in the output image (imgOut) where imgIn (i, j) >= threshold value. This allows for the removal of object noise (salt) and background noise (pepper).

2. **Bi-Means Automatic Threshold Selection**:
   - Given the histogram of a gray-scale image, this project implements the Bi-Means algorithm to automatically determine the optimal thresholds for image segmentation, simplifying the process of separating foreground and background regions.

3. **5x5 Averaging and Corner-Preserve Averaging**:
   - This project applies two averaging filters to a grayscale image and utilizes convolution operations to smooth out pixel values. By replacing each pixel value with the average value of its neighboring pixels, noise in the image is reduced, enhancing image quality. The process involves convolving the image matrix with a kernel matrix (mask), where each element of the kernel represents a weight. 

4. **Morphology Operations**:
   - By using a binary image and a structuring element, this project employs operations such as erosion, dilation, opening, and closing for object decomposition and image enhancement. These operations involve altering the pixel values of the image based on the interactions between the image and the structuring element. Erosion and dilation modify the shape and size of objects, while opening and closing combinations smooth contours, fill gaps, and remove noise. 

5. **Connected Components in Binary Images**:
   - This project assigns unique labels to connected sets of object pixels and computes a list of attributes in the Property Record File for each component. This allows for object shape analysis and image segementation.

6. **Gradient and Zero-Crossing Edge Detectors**:
   - This project focuses on detecting edges within images using gradient-based techniques and zero-crossing methods. Gradient-based edge detection algorithms compute the magnitude and direction of intensity gradients, highlighting abrupt changes in pixel values. Zero-crossing methods identify locations where the gradient changes sign, pinpointing potential edge locations. 

7. **Hough Transform**:
   -  This project explores the Hough Transform algorithm for detecting lines and other shapes within images. It converts pixel coordinates into parameter space and identifies patterns that correspond to geometric shapes.

8. **Chain Code**:
      - This project implements chain codes to represent boundaries and shapes within images. Chain codes trace the outline of objects and provide a simplified representation that facilitates object shape analysis and image compression.

---
