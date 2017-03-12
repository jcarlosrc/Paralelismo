#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>

using namespace std;

// Recursive filter on 1d arrays.
// input = c[i0], c[i0+s], ...., c[i0+m*s], i.e., c(i) = c(i0+s*i)
// kernel = k[0], k[1], ...., k[n-1]
static void rightRFilter(double *c, int i0, int s, int m, double *k, int n, double *out){
    // First element (i = 0) doesnt change
    out[0] = c[0];
    for(int i = 1; i < m+1; i++){
        float sum = 0;
        // Suma en los elementos del k x los de c no negativos
        for(int r = 0; r<i && r<n; r++){
            sum += k[r]*out[s*(i-1-r)+i0];
        }
        //Actualizar c(i) = c(i) - sum
        out[i0+s*i] = c[i0+s*i]-sum;
    }
}
static void leftRFilter(double *c, int i0, int s, int m, double *k, int n, double *out){
    // Last element (i = m) doesnt change
    out[i0+s*m] = c[i0+s*m];
    for(int i = 1; i < m+1; i++){
        float sum = 0;
        // Suma en los elementos del k x los de c no negativos
        for(int j = 0; j<n && j<i; j++){
            sum += k[j]*out[s*(m-i+1+j)+i0];
        }
        //Update c(m-i) = c(m-i) - sum
        out[i0+s*(m-i)] = c[i0+s*(m-i)]-sum;
    }
}

static void hrec(float *in, int w, int h, int p, float *k, int n, float *out) {
    // Pass on lines
    for(int i = 0; i<h; i++){
        //printf("\nFila %i\n",i);
        // left-right pass
        rightRFilter(in, i*w, 1, w-1, k, n);
        //printf("Pasada a derecha:\n" );
        //showMatrix(im, h, w);
        // right-left pass
        leftRFilter(in, i*w, 1, w-1, k, n);
        //printf("Pasada a izquierda:\n");
        //showMatrix(im, h, w);
    }
}
static void vrec(float *in, int w, int h, int p, float *k, int n, float *out) {
    // Pass on columns
    for(int j = 0; j<w; j++){
        //printf("\nColumna %i\n\n", j);
        // top-down pass
        rightRFilter(in, j, w, h-1, k, n);
        //printf("Pasada top-down:\n" );
        //showMatrix(im, h, w);
        // down-top pass
        leftRFilter(in, j, w, h-1, k, n);
        //printf("Pasada down-top:\n" );
        //showMatrix(im, h, w);
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

int main(int argc, char const *argv[]) {

    int reps = 100;
    // somehow define n, h, w, and p
    int n = 3, h = 1024, w = 1024, p = 1024;
    // allocate and somehow fill it
    std::vector<float> kernel(n, 0.f);
    // allocate and somehow fill them
    std::vector<float> input(h*p, 0.f), output(h*p, 0.f), temp(h*p, 0.f);

    //if(argc>0)
    //int nro_it = (int)atoi(argv[1]);
    // Si no hay nombre error
    srand (time(NULL));
    // Filtro
    int n = 5;
    double k[n];
    // fill the array from the opencv image
    for (int i = 0; i < n; i++) {
        k[i] = (double)(rand()%10)/10;
    }
    //printf("Filtro:\n");
    //showMatrix(k, 1, n);
    int h = 1; int w = 1;
    // Test to different image resolution
    for(int cont = 1; cont <=10; cont ++){

        // For each 2^cont generate a random image and apply the filter
        int h = 100*cont; int w = h;
        // Imagen
        double im[w*h];
        for(int i = 0; i<h; i++){
            for(int j = 0; j<w ;j++){
                im[i*w+j] = (double)rand()/RAND_MAX;
            }
        }
        //printf("%i x %i random image generated\n", h, w );
        //showMatrix(im, h, w);

        int start = clock();
        // horizontal filtering
        hrec(im, w, h, p, k, n, out);
        int stop = clock();
        printf("%i %.6f\n", cont,1000*(double)(stop-start)/double(CLOCKS_PER_SEC));

        int start = clock();
        // vertical filtering
        vrec(im, w, h, p, k, n, out);
        int stop = clock();
        printf("%i %.6f\n", cont,1000*(double)(stop-start)/double(CLOCKS_PER_SEC));

        //printf("Despues del filtro:\n");
        //showMatrix(im, h, w);
        printf("%i %.6f\n", cont,1000*(double)(stop-start)/double(CLOCKS_PER_SEC));
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
