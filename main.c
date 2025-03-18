#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sobel.h"
#include "rtclock.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Sobel kernels
int Kx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

int Ky[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

// Globals: Image and threading data
unsigned char **input_image;
unsigned char **output_image;
unsigned char threshold = 127;
int width, height;
int num_threads;

//struct for thread information
typedef struct threadInfo
{
    int startingRow;
    int endingRow;
} threadInfo;


/**
 * Worker thread for pthreads
 */
void* worker(void *param){
    int *ID = (int*) param; //which thread it is (0-num_threads-1)
    threadInfo IDInfo;
    IDInfo.startingRow = (*ID % num_threads == 0) ? 0 : width/num_threads;
    IDInfo.endingRow = (*ID % num_threads == 0) ? width/num_threads : width;

    for(int i = IDInfo.startingRow; i< IDInfo.endingRow; i++){
        sobelFilter(input_image, i, width);
    }
    free(param);
    return NULL;
}


/**
 * Main method
 */
int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Usage: ./sobel <input-file> <num-threads (>= 1)> <threshold (0-255)> \n");
        return -1;
    }

    char *filename = argv[1];
    num_threads = (int)argv[2];
    threshold = (unsigned char)argv[3];

    // TODO - Read image file into array a 1D array (see assignment write-up)
    unsigned char *data = stbi_load(filename, &width, &height, NULL, 1);
    input_image = (unsigned char**) malloc(sizeof(unsigned char*) * height);
    for (int i = 0; i < height; i++) {
        // assign each row the proper pixel offset
        input_image[i] = &data[i * width];
    }

    
    printf("Loaded %s. Height=%d, Width=%d\n", filename, height, width);

    // malloc a size 'height' array of pointers (these are the rows)
    output_image = (unsigned char**) malloc(sizeof(unsigned char*) * height);
    // iterate through each row and malloc an array of size 'width'
    for (int i = 0; i < height; i++) {
        output_image[i] = (unsigned char*) malloc(sizeof(int) * width);
    }
    // Now we have a 2D array, accessible via output_image[x][y]!!


    // Start clocking!
    double startTime, endTime;
    startTime = rtclock();

    // TODO - Prepare and create threads
    pthread_t tid[num_threads];
    for(int i=0;i<num_threads;i++){
        int *threadID = (int*)malloc(sizeof(int));
        *threadID=i;
        pthread_create(&tid[i], NULL, worker, threadID);
    }

    // TODO - Wait for threads to finish
    
    // End clocking!
    endTime = rtclock();
    printf("Time taken (thread count = %d): %.6f sec\n", num_threads, (endTime - startTime));

    char *outfilename;
    //preparing outfilename;
    char *filenamecopy = filename;
    outfilename = strtok(filenamecopy, ".");
    strcat(outfilename, "-sobel.jpg");
    

    // This code re-maps output_image[][] down to a 1D array
    unsigned char *array1D = (unsigned char*) malloc(width * height * sizeof(unsigned char));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            array1D[i * width + j] = output_image[i][j];
        }
    }
    stbi_write_jpg(outfilename, width, height, 1, array1D, 80);
    //freeing data?
    free(data);

    //freeing input_image
    //free each row
    for (int i = 0; i < height; i++) {
        free(input_image[i]);
        input_image[i] = NULL;  // dangling pointer
    }
    // free original array
    free(input_image);
    input_image = NULL;  // dangling pointer
        return 0;

    //freeing output_image
    // free each row
    for (int i = 0; i < height; i++) {
    free(output_image[i]);
    output_image[i] = NULL;  // dangling pointer
    }
    // free original array
    free(output_image);
    output_image = NULL;  // dangling pointer
        return 0;
}
