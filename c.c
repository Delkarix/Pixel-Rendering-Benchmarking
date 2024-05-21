/*
CMDLINE OPTIONS:
	r: makes random colors (to test average speed)
	s: uses a singular color (to test fastest speed)
	v: every pixel uses a different color (to test slowest speed)
*/

/*

BENCHMARK RESULTS (measured in milliseconds):
    NOTE: for SOLID, there were imprecisions in the benchmarking so CYCLES was set to 10 and the overall time was then divided by 10.

					-O0			        -O1			        -O2			        -O3
	RANDOM			168.4ms		        161.5ms		        165.0ms		        160.8ms

	SOLID			10.1ms		        6.0ms		        5.1ms		        5.2ms
                    (4.8ms actual)      (2.71ms actual)     (2.56ms actual)     (3.01ms actual)

	VARIABLE		141.5ms		        133.4ms		        134.3ms		        130.8ms

*/

// Godbolt decomp (-O3): https://godbolt.org/z/b4TnG9sne

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

static const char* black_color = "\x1b[48;2;;;m";
static const char* reset_color = "\x1b[m";
static const size_t black_color_size = 10;  // Note that these will need to be reset if black_color or reset_color are changed
static const size_t reset_color_size = 3;
#define COLOR_STRING_LENGTH 20      // Maximum possible length of an individual colorizing string:      "\x1b[48;2;XXX;XXX;XXXm" + " "

#define WIDTH 1000
#define HEIGHT 1000
#define CYCLES 1

typedef union {
  struct {
		unsigned char A;
		unsigned char R;
		unsigned char G;
		unsigned char B;
	};

	unsigned int argb;
} Color;

void RenderImage(Color* pixels, unsigned char* buffer, unsigned short width, unsigned short height) {
	unsigned int current_color;
	//size_t black_color_size = strlen(black_color);
	size_t offset = black_color_size;
	size_t count = 0;

	memset(buffer, 0, width*height*COLOR_STRING_LENGTH + black_color_size + reset_color_size);
	memcpy(buffer, black_color, black_color_size);

	for (unsigned short y = 0; y < height; y++) {
		for (unsigned short x = 0; x < width; x++) {
			if (current_color != pixels->argb) {
				current_color = pixels->argb;
				memset(buffer + offset, ' ', count); // Fill the space up to the current spot with spaces.
				offset += count;
				offset += snprintf(buffer + offset, COLOR_STRING_LENGTH, "\x1b[48;2;%u;%u;%um", pixels->R, pixels->G, pixels->B);

				count = 0;
			}

			count++;
			pixels++;
		}
	}

	// Once we hit the end, fill out any remaining spaces to complete the image.
	memset(buffer + offset, ' ', count);
	offset += count;

	// Copy the "reset color" instruction to the end of the buffer to ensure that the image does not "color flood" the output after the image.
	memcpy(buffer + offset, reset_color, reset_color_size);
}

int main(int argc, char** argv) {
    if (argc > 1) {
		Color* pixels = (Color*)malloc(WIDTH*HEIGHT*sizeof(Color));
        unsigned char* buffer = malloc(WIDTH*HEIGHT*COLOR_STRING_LENGTH + black_color_size + reset_color_size);
		Color color = {0, 0, 0, 0};

		switch (argv[1][0]) {
			case 'r':
				srand(time(NULL));

				for (int i = 0; i < WIDTH*HEIGHT; i++) {
					pixels[i].argb = rand();
				}

				break;
			case 's':
				for (int i = 0; i < WIDTH*HEIGHT; i++) {
					pixels[i].argb = 0;
				}

				break;
			case 'v':
				for (int i = 0; i < WIDTH*HEIGHT; i++) {
					pixels[i].argb = color.argb++;
				}
				break;
		}


		for (int i = 0; i < CYCLES; i++) {
            RenderImage(pixels, buffer, WIDTH, HEIGHT);
			printf("\x1b[H%s", buffer);
		}
		
		free(pixels);
        free(buffer);
	}
}
