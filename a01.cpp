#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <vector>
#include <chrono>
#include <iostream>
#include <complex>
#include <cmath>

/* Recursive filter implementation */

// Recursive filter on 1d arrays.
// input = c[i0], c[i0+s], ...., c[i0+m*s], i.e., c(i) = c(i0+s*i)
// kernel = k[0], k[1], ...., k[n-1]

// left-right pass
static void rightRFilter(double *in, int i0, int s, int m, double *k, int n, double *out){
    // First element (i = 0) doesnt change
    out[0] = in[0];
    for(int i = 1; i < m+1; i++){
        double sum = 0;
        // Suma en los elementos del k x los de c no negativos
        for(int r = 0; r<i && r<n; r++){
            sum += k[r]*out[s*(i-1-r)+i0];
        }
        //Actualizar c(i) = c(i) - sum
        out[i0+s*i] = in[i0+s*i]-sum;
    }
}
// right-left pass
static void leftRFilter(double *in, int i0, int s, int m, double *k, int n, double *out){
    // Last element (i = m) doesnt change
    out[i0+s*m] = in[i0+s*m];
    for(int i = 1; i < m+1; i++){
        double sum = 0;
        // Suma en los elementos del k x los de c no negativos
        for(int j = 0; j<n && j<i; j++){
            sum += k[j]*out[s*(m-i+1+j)+i0];
        }
        //Update c(m-i) = in(m-i) - sum
        out[i0+s*(m-i)] = in[i0+s*(m-i)]-sum;
    }
}
// Pass on matrix lines
static void hrec(double *in, int w, int h, int p, double *k, int n, double *out) {
    for(int i = 0; i<h; i++){
        // left-right pass
        rightRFilter(in, i*p, 1, w-1, k, n, out);
        // right-left pass
        leftRFilter(in, i*p, 1, w-1, k, n, out);
    }
}
// Pass on matrix columns
static void vrec(double *in, int w, int h, int p, double *k, int n, double *out) {
    for(int j = 0; j<w; j++){
        // top-down pass
        rightRFilter(in, j, p, h-1, k, n, out);
        // down-top pass
        leftRFilter(in, j, p, h-1, k, n, out);
    }
}

// Debug function
static void showMatrix(double *a, int h, int w){
    for(int i = 0; i<h; i++){
        for(int j = 0; j<w; j++){
            printf("%.2f\t", a[i*w+j]);
        }
        printf("\n");
    }
}

/* Convolution implementation */

// convolution on lines: line l = in[l*p+0], ... , in[l*p+w-1]
static void hconv(double *in, int w, int h, int p, double *k, int n, double *out) {
    // For every line
    for(int l=0; l<h; l++){
        // for every element in line l
        for(int i = 0; i<w; i++){
            double sum = 0;
            for(int j = -n+1; j<=n-1 && i-j<w && i-j>=0; j++){
                // in[i,j] = in[p*i+j]
                sum += k[abs(j)]*in[i-j];
            }
            out[i] = sum;
        }
    }
}
// Convolution on columns: column l = in[0*p+l], ... , in[(h-1)*p+l]
static void vconv(double *in, int w, int h, int p, double *k, int n, double *out) {
    // For every column
    for(int l=0; l<w; l++){
        // for every element in column c
        for(int i = 0; i<h; i++){
            double sum = 0;
            for(int j = -n+1; j<=n-1 && i-j<h && i-j>=0; j++){
                // in[i,j] = in[p*i+j]
                sum += k[abs(j)]*in[p*(i-j)+l];
            }
            out[i*p+l] = sum;
        }
    }
}

// Test program
int main(int argc, char const *argv[]) {
    FILE *fout;
    if(argc>1)
        fout = fopen(argv[1], "w");
    else
        fout = fopen("out.txt", "w");

    fprintf(fout, "n\tw\th\tp\threc\tvrec\thconv\tvconv\n" );
    int reps = 100;
    // somehow define n, h, w, and p
    int max_n = 20, max_h = 2048, max_w = 2048, max_p = 2048;
    int n, h, w, p;
    // allocate and somehow fill it
    std::vector<double> kernel(max_n, 0.f);
    // allocate and somehow fill them, pitch the same for all
    std::vector<double> input(max_h*max_p, 0.f), output(max_h*max_p, 0.f), temp(max_h*max_p, 0.f);

    // Random kernel
    printf("Kernel:\t");
    for (int i = 0; i < max_n; i++) {
        kernel[i] = (double)(rand()%10)/10;
    }
    showMatrix(&kernel[0], 1,5);

    // Random image
    for(int i = 0; i<max_h; i++){
        for(int j = 0; j<max_w ;j++){
            input[i*max_p+j] = (double)rand()/RAND_MAX;
        }
    }
    printf("%i x %i random image generated\n", max_h, max_w );

	for(n = 1; n<max_n; n++){
    printf("n = %i\n", n);
		for(w = 32; w<= max_w; w*=2){
        //printf("holaaa\n");
        h = w; p = w;
        printf("%i\t%i\t%i\t%i\t", n, w, h, p);
        fprintf(fout, "%i\t%i\t%i\t%i\t", n, w, h, p);

        // Test recursive
        auto begin = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < reps; i++) {
            hrec(&input[0], w, h, p, &kernel[0], n, &output[0]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
        fprintf(fout, "%ld\t", (long int)(duration/reps) );
        printf("%ld\t", (long int)(duration/reps) );
        //std::cout << "hrec time was " << duration/reps << "ns\n";

        begin = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < reps; i++) {
                vrec(&input[0], w, h, p, &kernel[0], n, &output[0]);
            }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
        fprintf(fout, "%ld\t", (long int)(duration/reps) );
        printf("%ld\t", (long int)(duration/reps) );
        //std::cout << "vrec time was " << duration/reps << "ns\n";

        // Test convolution
        // Horizontal
        for (int i = 0; i < reps; i++) {
            hconv(&input[0], w, h, p, &kernel[0], n, &temp[0]);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
        fprintf(fout, "%ld\t", (long int)(duration/reps) );
        printf("%ld\t", (long int)(duration/reps) );
        //std::cout << "hconv time was " << duration/reps << "ns\n";
        //Vertical
        begin = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < reps; i++) {
                vconv(&temp[0], w, h, p, &kernel[0], n, &output[0]);
            }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
        fprintf(fout, "%ld\n", (long int)(duration/reps) );
        printf("%ld\n", ( long int)(duration/reps) );
        //std::cout << "vconv time was " << duration/reps << "ns\n";
      }
	  }
    fclose(fout);
    return 0;

    /*
    srand (time(NULL));

    // Test to different image resolution
    for(int cont = 1; cont <=10; cont ++){

        int start = clock();
        // horizontal filtering
        hrec(&input[0], w, h, p, k, n, &output[0])
        int stop = clock();
        printf("%i %.6f\n", cont,1000*(double)(stop-start)/double(CLOCKS_PER_SEC));

        int start = clock();
        // vertical filtering
        vrec(&input[0], w, h, p, k, n, &output[0]);
        int stop = clock();
        printf("%i %.6f\n", cont,1000*(double)(stop-start)/double(CLOCKS_PER_SEC));

        //printf("Despues del filtro:\n");
        //showMatrix(im, h, w);
        printf("%i %.6f\n", cont,1000*(double)(stop-start)/double(CLOCKS_PER_SEC));
    }

    return 0;*/

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
