#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

// return 1d index from 2d index: 0<=i<h, 0<=j<w
int ind(i, j, h, w){
  return i*w + j;
}
// Index for a symetric mask: k(-i) = k(i)
int inds(int i){
    if(i<0) return -i;
    return i;
}

/*
Clasic convolution. Assumes *k to be a symetric 2n+1-sized 1d mask to be passed to lines
 and columns*/
float *conv1(float *input, int *r, int h, int w, float *k, int n){
  // Allocate memory for output
  float *out = malloc(h*w*sizeof(float));

  // lines pass
  for(int i = 0; i<h; i++){
    for (int j = 0; j < w; j++) {
      float sum = 0;
      for(int r = -1*n; r<=n; r++){
        if(j>=r)
          sum += input[ind(i,j-r)]*k[inds(r)];
      }
      out[ind(i,j,h,w)] = sum;
    }
  }
  // columns pass
  for(int j = 0; j<w; j++){
    for (int i = 0; i < h; i++) {
      float sum = 0;
      for(int r = -1*n; r<=n; r++){
        if(i>=r)
          sum += input[ind(i-r,j)]*k[inds(r)];
      }
      out[ind(i,j,h,w)] = sum;
    }
  }
  return out;
}

int main(int argc, char const *argv[]) {
    // If no valid file name -> error
    if( argc != 2 || !image.data )
    {
      printf( " No image data \n " );
      return -1;
    }

    char* imageName = argv[1];
    // Leer la imagen
    Mat image;
    image = image.convert(imread( imageName, 0), ;
    int w = image.rows;
    int h = image.cols;

    float *im = (float*)malloc(w*h*sizeof(float));

    // fill the array from the opencv image
    for(int i = 0; i<h; i++){
        for(int j = 0; j<w; j++){
            im(ind(i,j,h,w)) = (float)(image.at<uchar>(i,j))/255;
        }
    }

    return 0;

}


//BMP file to unsigned char array
/*unsigned char* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    // BGR image
    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);
    // BMP is allocated as BGR, so change to RGB
    for(i = 0; i < size; i += 3)
    {
      unsigned char tmp = data[i];
      data[i] = data[i+2];
      data[i+2] = tmp;
    }

    return data;
}*/
