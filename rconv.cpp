#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

// return 1d index from 2d index: 0<=i<h, 0<=j<w
int ind(int i, int j, int h, int w){
  return i*w + j;
}

// Recursive filter on 1d arrays. Sobreescribe el input c
// input = c[i0], c[i0+s], ...., c[i0+m*s], i.e., c(i) = c(i0+s*i)
// kernel = k[0], k[1], ...., k[n-1]
void rightRFilter(double *c, int i0, int s, int m, double *k, int n){
    // Primer elemento (i = 0) no cambia
    for(int i = 1; i < m+1; i++){
        float sum = 0;
        // Suma en los elementos del k x los de c no negativos
        for(int r = 0; r<i && r<n; r++){
            sum += k[r]*c[s*(i-1-r)+i0];
        }
        //Actualizar c(i) = c(i) - sum
        c[i0+s*i] = c[i0+s*i]-sum;
    }
}
void leftRFilter(double *c, int i0, int s, int m, double *k, int n){
    // Primer elemento (i = 0) no cambia
    for(int i = 1; i < n; i++){
        float sum = 0;
        // Suma en los elementos del k x los de c no negativos
        for(int r = 0; r<n && r<i; r++){
            sum += k[r]*c[s*(m-i+1+r)+i0];
        }
        //Actualizar c(m-i) = c(m-i) - sum
        c[i0+s*(m-i)] = c[i0+s*(m-i)]-sum;
    }
}

// Filter to 2d image
void applyRFilter(double *input, int *r, int w, double *k, int n){
  // line pass
  for(int i = 0; i<h; i++){
      // left pass
      leftRFilter(input, i*w, 1, w-1, k, n);
      // Right pass
      rightRFilter(input, (i+1)*w-1, 1, w-1, k, n);
  }
  // Column pass
  for(int j = 0; j<w; j++){
      // top-down pass
      leftRFilter(input, j, w, h-1, k, n);
      // down-top pass
      rightRFilter(input, (h-1)*w+1+j, w, h-1, k, n);
  }
}



int main(int argc, char const *argv[]) {
    // Si no hay nombre error
    if( argc != 2 || !image.data )
    {
      printf( " No image data \n " );
      return -1;
    }
    // Kernel definition
    double k[] = {1/3, 1/3, 1/3};
    // Leer la imagen
    Mat image;
    image = imread( imageName, 0);
    // fill the array from the opencv image
    for(int i = 0; i<h; i++){
        for(int j = 0; j<w; j++){
            im(ind(i,j,h,w)) = (float)(image.at<uchar>(i,j))/255;
        }
    }
    // aplicar el filtro
    applyRFilter(input, h, w, k, n);

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
