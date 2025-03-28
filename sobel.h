#define BLACK 0
#define WHITE 255
#define MIN_ARRAY_SIZE 9

// These globals are declared here, but defined externally in another .c file
extern unsigned char **input_image;
extern unsigned char **output_image;
extern unsigned char threshold;
extern int width, height;
extern int num_threads;
extern int Kx[3][3];
extern int Ky[3][3];

int convolute(unsigned char** array, int size, int yPoint, int xPoint);
unsigned char** sobelFilter(unsigned char** input_image, int height, int width);