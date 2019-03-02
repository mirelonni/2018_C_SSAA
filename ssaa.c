#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "ssaa.h"

int num_threads;
int resize_factor;

unsigned char ** grey_i;
unsigned char ** rgb_i;
int maxWidth_i;
int maxHeight_i;

unsigned char ** grey_o;
unsigned char ** rgb_o;

int maxWidth_o;
int maxHeight_o;

unsigned char gaussKernel[3][3] = {{1, 2, 1},
                                   {2, 4, 2},
                                   {1, 2, 1}};

void readInput(const char *fileName, image *img) {
    FILE * pFile;
    int i;
    pFile = fopen(fileName, "rb");

    fscanf(pFile, "P%hhu\n%d %d\n%d\n", &img->color, &img->width, &img->height, &img->maxVal);

    if (img->color == 5) {

        grey_i = malloc(sizeof(unsigned char *) * img->height);

        for (i = 0; i < img->height; i++) {
            grey_i[i] = malloc(sizeof(unsigned char) * img->width);
            fread(grey_i[i], 1, img->width, pFile);
        }

    } else {

        rgb_i = malloc(sizeof(unsigned char *) * img->height);

        for (i = 0; i < img->height; i++) {

            rgb_i[i] = malloc(sizeof(unsigned char) * img->width * 3);
            fread(rgb_i[i], 1, img->width * 3, pFile);
        }

    }

    fclose(pFile);

    maxWidth_i = (img->width / resize_factor) * resize_factor;
    maxHeight_i = (img->height / resize_factor) * resize_factor;

    maxWidth_o = img->width / resize_factor;
    maxHeight_o = img->height / resize_factor;

    if (img->color == 5) {

        grey_o = malloc(sizeof(unsigned char *) * maxHeight_o);

        for (i = 0; i < maxHeight_o; i++) {
            grey_o[i] = malloc(sizeof(unsigned char) * maxWidth_o);
        }

    } else {

        rgb_o = malloc(sizeof(unsigned char *) * maxHeight_o);

        for (i = 0; i < maxHeight_o; i++) {

            rgb_o[i] = malloc(sizeof(unsigned char) * maxWidth_o * 3);
        }

    }
}
void writeData(const char *fileName, image *img) {
    FILE * pFile;
    int i;
    pFile = fopen(fileName, "wb");

    fprintf(pFile, "P%hhu\n%d %d\n%d\n", img->color, img->width, img->height, img->maxVal);
    // fprintf(pFile, "%d ", );
    // fprintf(pFile, "%d\n", );
    // fprintf(pFile, "%d\n", img->maxVal);

    if (img->color == 5) {
        for (i = 0; i < img->height; i++) {
            fwrite(grey_o[i], 1, img->width, pFile);
        }

    } else {

        for (i = 0; i < img->height; i++) {
            fwrite(rgb_o[i], 1, img->width * 3, pFile);
        }

    }

    fclose(pFile);
}

void* resize_even_color(void *var) {

    int thread_id = *(int*)var;
    int i, j, sum_r, sum_g, sum_b, i_mini, j_mini;

    int start = thread_id * floor(maxHeight_o / num_threads);
    int end = start + floor(maxHeight_o / num_threads);

    if (thread_id == num_threads - 1) {
        end = maxHeight_o;
    }

    for (i = start; i < end; i++) {
        for (j = 0; j < maxWidth_o * 3; j += 3) {
            sum_r = 0;
            sum_g = 0;
            sum_b = 0;

            for (i_mini = 0; i_mini < resize_factor; i_mini++) {
                for (j_mini = 0; j_mini < resize_factor; j_mini++) {
                    sum_r += rgb_i[i * resize_factor + i_mini][j * resize_factor + (j_mini * 3) + 0];
                    sum_g += rgb_i[i * resize_factor + i_mini][j * resize_factor + (j_mini * 3) + 1];
                    sum_b += rgb_i[i * resize_factor + i_mini][j * resize_factor + (j_mini * 3) + 2];
                }
            }

            rgb_o[i][j + 0] = (sum_r) / (resize_factor * resize_factor);
            rgb_o[i][j + 1] = (sum_g) / (resize_factor * resize_factor);
            rgb_o[i][j + 2] = (sum_b) / (resize_factor * resize_factor);
        }
    }

    return 0;

}

void* resize_odd_color(void *var) {

    int thread_id = *(int*)var;
    int i, j, sum_r, sum_g, sum_b, i_mini, j_mini;

    int start = thread_id * floor(maxHeight_o / num_threads);
    int end = start + floor(maxHeight_o / num_threads);

    if (thread_id == num_threads - 1) {
        end = maxHeight_o;
    }

    for (i = start; i < end; i++) {
        for (j = 0; j < maxWidth_o * 3; j += 3) {
            sum_r = 0;
            sum_g = 0;
            sum_b = 0;

            for (i_mini = 0; i_mini < resize_factor; i_mini++) {
                for (j_mini = 0; j_mini < resize_factor; j_mini++) {
                    sum_r += rgb_i[i * resize_factor + i_mini][j * resize_factor + (j_mini * 3) + 0] * gaussKernel[i_mini][j_mini];
                    sum_g += rgb_i[i * resize_factor + i_mini][j * resize_factor + (j_mini * 3) + 1] * gaussKernel[i_mini][j_mini];
                    sum_b += rgb_i[i * resize_factor + i_mini][j * resize_factor + (j_mini * 3) + 2] * gaussKernel[i_mini][j_mini];

                }
            }

            rgb_o[i][j + 0] = (sum_r) / (16);
            rgb_o[i][j + 1] = (sum_g) / (16);
            rgb_o[i][j + 2] = (sum_b) / (16);
        }
    }

    return 0;

}

void* resize_even_grey(void *var) {

    int thread_id = *(int*)var;
    int i, j, sum_grey, i_mini, j_mini;

    int start = thread_id * floor(maxHeight_o / num_threads);
    int end = start + floor(maxHeight_o / num_threads);

    if (thread_id == num_threads - 1) {
        end = maxHeight_o;
    }

    for (i = start; i < end; i++) {
        for (j = 0; j < maxWidth_o; j++) {
            sum_grey = 0;

            for (i_mini = 0; i_mini < resize_factor; i_mini++) {
                for (j_mini = 0; j_mini < resize_factor; j_mini++) {
                    sum_grey += grey_i[i * resize_factor + i_mini][j * resize_factor + j_mini];
                }
            }

            grey_o[i][j] = (sum_grey) / (resize_factor * resize_factor);
        }
    }

    return 0;

}

void* resize_odd_grey(void *var) {

    int thread_id = *(int*)var;
    int i, j, sum_grey, i_mini, j_mini;

    int start = thread_id * floor(maxHeight_o / num_threads);
    int end = start + floor(maxHeight_o / num_threads);

    if (thread_id == num_threads - 1) {
        end = maxHeight_o;
    }

    for (i = start; i < end; i++) {
        for (j = 0; j < maxWidth_o; j++) {
            sum_grey = 0;

            for (i_mini = 0; i_mini < resize_factor; i_mini++) {
                for (j_mini = 0; j_mini < resize_factor; j_mini++) {
                    sum_grey += grey_i[i * resize_factor + i_mini][j * resize_factor + j_mini] * gaussKernel[i_mini][j_mini];
                }
            }

            grey_o[i][j] = (sum_grey) / (16);
        }
    }

    return 0;

}

void resize(image *in, image *out) {

    int t_i;

    out->color = in->color;
    out->width = maxWidth_o;
    out->height = maxHeight_o;
    out->maxVal = in->maxVal;

    pthread_t tid[num_threads];
    int thread_id[num_threads];

    for (t_i = 0; t_i < num_threads; t_i++)
        thread_id[t_i] = t_i;

    if (in->color == 5) {

        if (resize_factor % 2 == 0) {

            for (t_i = 0; t_i < num_threads; t_i++) {
                pthread_create(&(tid[t_i]), NULL, resize_even_grey, &(thread_id[t_i]));
            }

        } else {

            for (t_i = 0; t_i < num_threads; t_i++) {
                pthread_create(&(tid[t_i]), NULL, resize_odd_grey, &(thread_id[t_i]));
            }

        }
    } else {
        if (resize_factor % 2 == 0) {

            for (t_i = 0; t_i < num_threads; t_i++) {
                pthread_create(&(tid[t_i]), NULL, resize_even_color, &(thread_id[t_i]));
            }

        } else {

            for (t_i = 0; t_i < num_threads; t_i++) {
                pthread_create(&(tid[t_i]), NULL, resize_odd_color, &(thread_id[t_i]));
            }

        }
    }

    for (t_i = 0; t_i < num_threads; t_i++) {
        pthread_join(tid[t_i], NULL);
    }

}
