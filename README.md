# LBP-Image-Filter-Parallelization-With-CUDA-and-OpenMP
This project was part of a semester evaluation on code parallelization using CUDA and OpenMP.

## Project Goals
This project focused on learning basic parallelization techniques using the OpenMP and CUDA API separately to explore both methods and their respective results.

## The filter in question
LBP image filtering disects the local binary paterns of an image, effectively isolating the "texture". LBP can and has been used in classification problems such as face or fringerprint recognition.

In this project, the filter was applied in grayscale images of 8-bit values, as it was a good base for understanding image filtering.

## The CUDA part
Two versions of the filter were produced, using CUDA, one version uses shared memory and one does not. For this particular project, the code which doesn't use shared memory appeared to be the fastest.

## The OpenMP part
Only one version of the filter was chosen as part of the final project, after carefull optimization and simplification of the algorithm.

## Results
In small images, there is little benefit to using CUDA and there is actually adverse effects to using OpenMP, but the bigger the image, the better the results, with CUDA reaching a 153 times faster computation regarding the serial code and OpenMP reaching as much as 29 times faster computation!


