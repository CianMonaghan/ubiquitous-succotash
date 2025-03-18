#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "sobel.h"

/**
 * Does the convolution on the array at the point specificed
 * @param array the specified array
 * @param size the size of the input array (taken as height x width)
 * @param yPoint the y coordinate of the point
 * @param xPoint the x coordinate of the point
 */
int convolute(unsigned char** array, int size, int yPoint, int xPoint){
    if(size > MIN_ARRAY_SIZE){
        printf("ERROR: Array is too small.");
        exit(EXIT_FAILURE);
    }
    //edge case detection
    if(yPoint <= 0 || yPoint >= height){//out of bounds or on the edge
        return 0;
    }
    if(xPoint <= 0 || xPoint >= width){//out of bounds or on the edge
        return 0;
    }
    
    int upperYLimit = yPoint + 1; //row above point
    int lowerYLimit = yPoint - 1; //row below point

    int leftXLimit = xPoint - 1; //col to left of pt 
    int rightXLimit = xPoint + 1; //col to right of pt

    //calculate Gx, Gy, and G
    int Gx=0;
    int Gy=0;
    int G=0;
    Gx = (array[upperYLimit][leftXLimit]*Kx[0][0])+(array[upperYLimit][xPoint]*Kx[0][1])+(array[upperYLimit][rightXLimit]*Kx[0][2])
        +(array[yPoint][leftXLimit]*Kx[1][0])+(array[yPoint][xPoint]*Kx[1][1])+(array[yPoint][rightXLimit]*Kx[1][2])
        +(array[lowerYLimit][leftXLimit]*Kx[2][0])+(array[lowerYLimit][xPoint]*Kx[2][1])+(array[lowerYLimit][rightXLimit]*Kx[2][2]);

    Gy = (array[upperYLimit][leftXLimit]*Ky[0][0])+(array[upperYLimit][xPoint]*Ky[0][1])+(array[upperYLimit][rightXLimit]*Ky[0][2])
        +(array[yPoint][leftXLimit]*Ky[1][0])+(array[yPoint][xPoint]*Ky[1][1])+(array[yPoint][rightXLimit]*Ky[1][2])
        +(array[lowerYLimit][leftXLimit]*Ky[2][0])+(array[lowerYLimit][xPoint]*Ky[2][1])+(array[lowerYLimit][rightXLimit]*Ky[2][2]);

    G = sqrt(pow(Gx, 2)+pow(Gy,2));
    //clamp G
    if(G>255){
        G = WHITE;
    } else if (G<threshold){
        G = BLACK;
    }
    return G;
}
/**
 * Computes the Sobel filter of the inputted image
 * @param input_image image you want to Sobel filter
 * @param height height of the array (how many rows the array has)
 * @param width width of the array (how many cols the array has)
 */
unsigned char** sobelFilter(unsigned char** input_image, int height, int width){

    unsigned char** copy_image = (unsigned char**) malloc(sizeof(unsigned char*) * height);
    for(int i=0; i<height; i++){//copying all the data
        for(int j=0;j<width; j++){
            copy_image[i][j]=input_image[i][j];
        }
    }

    //preparing output array
    // malloc a size 'height' array of pointers (these are the rows)
    unsigned char** output = (unsigned char**) malloc(sizeof(unsigned char*) * height);
    // iterate through each row and malloc an array of size 'width'
    for (int i = 0; i < height; i++) {
        output[i] = (unsigned char*) malloc(sizeof(int) * width);
    }
    // Now we have a 2D array, accessible via output_image[x][y]!!

    for(int i=0; i<height; i++){//convolution process
        for(int j=0;j<width;j++){
            output[i][j] = convolute(copy_image, height*width, i, j); //i is y, j is x
        }
    }
    free(copy_image);
    free(output);
    return output;
}