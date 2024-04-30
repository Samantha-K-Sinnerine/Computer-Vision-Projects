# Computer-Vision-Projects

Welcome to the Computer Vision Projects repository! This collection comprises eight projects covering a diverse range of topics in computer vision. Each project aims to explore fundamental concepts and techniques in image processing and analysis. Below is a brief overview of the projects and their purpose included:

1. **Binary and Non-Binary Thresholding**:
   - Given a grey-scale image and a threshold value, the binary/non-binary threshold operation is to transform pixels in the input image (imgIn) from grey-scale to binary/non-binary values in the output image (imgOut) where imgIn (i, j) >= threshold value. This allows for the removal of object noise (salt) and background noise (pepper).

2. **Bi-Means Automatic Threshold Selection**:
   - Given the histogram of a gray-scale image, this project implements the Bi-Means algorithm to automatically determine the optimal thresholds for image segmentation, simplifying the process of separating foreground and background regions.

3. **5x5 Averaging and Corner-Preserve Averaging**:
   - This project applies two averaging filters to a grayscale image and utilizes convolution operations to smooth out pixel values. By replacing each pixel value with the average value of its neighboring pixels, noise in the image is reduced, enhancing image quality. The process involves convolving the image matrix with a kernel matrix (mask), where each element of the kernel represents a weight. 

4. **Morphology Operations**:
   - This project uses a binary image and a structuring element employs the morphology operations erosion, dilation, opening, and closing for object decomposition and image enhancement.

5. **Connected Components in Binary Images**:
   - Identifies and labels connected components in binary images for object shape analysis and segementation.

6. **Gradient and Zero-Crossing Edge Detectors**:
   - Detects edges in images.

7. **Hough Transform**:
   - Detects straight lines and shapes in images.

8. **Chain Code**:
   - Used for image compression and object shape analysis.

---
