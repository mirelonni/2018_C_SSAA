#ifndef SSAA_H
#define SSAA_H

typedef struct {
    unsigned char color;
    int width;
    int height;
    int maxVal;
}image;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* SSAA_H */
