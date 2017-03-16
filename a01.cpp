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

using namespace std;

// Horizontal recursive
static void hrec(double *in, int w, int h, int p, double *k, int n, double *out) {
    double *iline = &in[0];
    double *oline = &out[0];
    for(int i = 0; i<h; i++){
        // left-right pass
        for(int j = 0; j<w; j++){
            double sum = 0;
            int minn = std::min(j,n);
            for(int r = 0; r<minn; r++){
                sum += k[r]*oline[j-1-r];
            }
            oline[j] = iline[j] - sum;
        }
        // right-left pass
        for(int j = 0; j<w; j++){
            double sum = 0;
            int minn = std::min(w-j-1,n);
            for(int r = 0; r<minn; r++){
                sum += k[r]*oline[j+1+r];
            }
            oline[j] = iline[j] - sum;
        }
        iline +=p;
        oline +=p;
    }
}
// Vertical recursive
static void vrec(double *in, int w, int h, int p, double *k, int n, double *out) {
    // top-bottom pass
    for(int i = 0; i<h; i++){
        for(int j = 0; j<w; j++){
            int minn = std::min(n, i);
            double sum = 0;
            for(int r = 0; r<minn; r++){
                sum += k[r]*out[p*(i-1-r)+j];
            }
            out[p*i+j] = in[p*i+j]-sum;
        }
    }
    // bottom-top pass
    for(int i = 0; i<h; i++){
        for(int j = 0; j<w; j++){
            int minn = std::min(n, h-i-1);
            double sum = 0;
            for(int r = 0; r<minn; r++){
                sum += k[r]*out[p*(i+1+r)+j];
            }
            out[p*i+j] = in[p*i+j]-sum;
        }
    }
}

/* Convolution implementation */

// convolution on ROWS:  l = in[l*p+0], ... , in[l*p+w-1]
static void hconv(double *in, int _w, int _h, int p, double *k, int _n, double *out) {
    int n = _n-1;
    int w = _w-1;
    int h = _h-1;

    double *iline = &in[0];
    double *oline = &out[0];
    int lcont = 0;
    for(int i = 0; i<h; i++){
        int kl = 0, kr = 0, kn = 0;
        for(int j = 0; j<=n; j++){
            kr = min(w-j, n);
            kl = min(n,j);
            kn = min(kl, kr);
            double sum = k[0]*iline[j];
            for(int s = kn+1; s<= kl; s++){
                sum += k[s]*iline[j-s];
            }
            for(int s =0 ; s<= kn; s++){
                sum += k[s]*(iline[j+s]+iline[j-s]);
            }
            for(int s = kn+1; s<=kr; s++){
                sum += k[s]*(iline[j+s]);
            }
            oline[j] = sum;
        }
        iline+=p;
        oline+=p;
    }

}

// convolution on COLUMNS: line l = in[l*p+0], ... , in[l*p+w-1]
static void vconv(double *in, int _w, int _h, int p, double *k, int _n, double *out) {
    int n = _n-1;
    int w = _w-1;
    int h = _h-1;

    for(int i = 0; i<=h; i++){
        int kl = 0, kr = 0, kn = 0;
        for(int j = 0; j<=n; j++){
            kr = min(w-i, n);
            kl = min(n,i);
            kn = min(kl, kr);
            double sum = k[0]*in[i*p+j];
            for(int s = kn+1; s<= kl; s++){
                sum += k[s]*in[(i-s)*p+j];
            }
            for(int s =0 ; s<= kn; s++){
                sum += k[s]*(in[(i+s)*p+j]+in[(i-s)*p+j]);
            }
            for(int s = kn+1; s<=kr; s++){
                sum += k[s]*(in[(i+s)*p+j]);
            }
            out[i*p+j] = sum;
        }
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

// Test program
int main(int argc, char const *argv[]) {
    FILE *fout;
    if(argc>1)
        fout = fopen(argv[1], "w");
    else
        fout = fopen("out.txt", "w");

    fprintf(fout, "n\tw\th\tp\threc\tvrec\thconv\tvconv\n" );
    int reps = 10;
    // somehow define n, h, w, and p
    int max_n = 15, max_h = 1024, max_w = 1024, max_p = 1024;
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
    printf("%i-sized random kernel generated\n", max_n);
    //showMatrix(&kernel[0], 1,5);

    // Random image
    for(int i = 0; i<max_h; i++){
        for(int j = 0; j<max_w ;j++){
            input[i*max_p+j] = (double)rand()/RAND_MAX;
        }
    }
    printf("%i x %i random image generated\n", max_h, max_w );

	for(n = 5; n<=max_n; n+=5){
    printf("n = %i\n", n);
		for(w = 256; w<= max_w; w+=128){
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
        begin = std::chrono::high_resolution_clock::now();
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
