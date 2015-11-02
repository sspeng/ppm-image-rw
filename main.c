#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void loadImage(
  const char *filename,
  uint32_t **dest,
  uint32_t *width,
  uint32_t *height)
{
  char buffer[80];
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file %s.", filename);
    exit(1);
  }

  fgets(buffer, 80, file); // Type
  fgets(buffer, 80, file); // Comment
  fscanf(file, "%d %d\n", width, height);
  fgets(buffer, 80, file); // Max intensity

  *dest = malloc((*width) * (*height) * sizeof(uint32_t) * 3);

  for (uint32_t i = 0; i < (*width) * (*height) * 3; i++) {
    int v;
    fscanf(file, "%u", &v);
    (*dest)[i] = v;
  }

  fclose(file);
}

void writeImage(char *filename, uint32_t *data, uint32_t width, uint32_t height)
{
  FILE *file = fopen(filename, "w");

  fprintf(file, "P3\n");
  fprintf(file, "#generated\n");
  fprintf(file, "%d %d\n", width, height);
  fprintf(file, "255\n");

  for (uint32_t i = 0; i < width * height * 3; i++)
    fprintf(file, "%u\n", data[i]);

  fclose(file);
}


uint32_t *getChannel(uint32_t *data, uint32_t width, uint32_t height, int idx)
{
  uint32_t *red = malloc(sizeof(*red) * width * height);

  for (uint32_t i = 0; i < width * height; i++) {
    red[i] = data[3*i + idx];
  }

  return red;
}

uint32_t *getRedChannel(uint32_t *data, uint32_t width, uint32_t height)
{
  return getChannel(data, width, height, 0);
}

uint32_t *getGreenChannel(uint32_t *data, uint32_t width, uint32_t height)
{
  return getChannel(data, width, height, 1);
}

uint32_t *getBlueChannel(uint32_t *data, uint32_t width, uint32_t height)
{
  return getChannel(data, width, height, 2);
}

uint32_t *createZeros(uint32_t n) {
  uint32_t *ret = malloc(sizeof(ret) * n);
  memset(ret, 0, sizeof(ret) * n);

  return ret;
}

uint32_t *combineChannels(uint32_t *red, uint32_t *green, uint32_t *blue,
                          uint32_t width, uint32_t height)
{
  uint32_t *ret = malloc(sizeof(ret) * width * height * 3);

  for (uint32_t i = 0; i < width * height; i++) {
    ret[i*3+0] = red[i];
    ret[i*3+1] = green[i];
    ret[i*3+2] = blue[i];
  }

  return ret;
}

int main() {
  const char *filename = "lena256.ppm";
  uint32_t *data = NULL;
  uint32_t width = 0;
  uint32_t height = 0;

  loadImage(filename, &data, &width, &height);

  printf("image width = %d, height = %d\n", width, height);

  uint32_t *zeros = createZeros(width * height);
  uint32_t *red = getRedChannel(data, width, height);
  uint32_t *green = getGreenChannel(data, width, height);
  uint32_t *blue = getBlueChannel(data, width, height);

  uint32_t *redImage = combineChannels(red, zeros, zeros, width, height);
  uint32_t *greenImage = combineChannels(zeros, green, zeros, width, height);
  uint32_t *blueImage = combineChannels(zeros, zeros, blue, width, height);

  writeImage("lena256_copy.ppm", data, width, height);
  writeImage("red.ppm", redImage, width, height);
  writeImage("green.ppm", greenImage, width, height);
  writeImage("blue.ppm", blueImage, width, height);

  free(data);
  free(zeros);
  free(red);
  free(green);
  free(blue);
  free(redImage);
  free(greenImage);
  free(blueImage);

  return 0;
}
