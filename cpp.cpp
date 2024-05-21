/*
CMDLINE OPTIONS:
	r: makes random colors (to test average speed)
	s: uses a singular color (to test fastest speed)
	v: every pixel uses a different color (to test slowest speed)
*/

/*

BENCHMARK RESULTS (measured in seconds/milliseconds):
	NOTE: for SOLID, there were imprecisions in the benchmarking so CYCLES was set to 10 and the overall time was then divided by 10.

					-O0					-O1					-O2					-O3
	RANDOM			2.696s				294.6ms				275.4ms				247.8ms

	SOLID			8.6ms 				4.7ms 				4.6ms 				6.9ms 
					(4.27ms actual)		(2.09ms actual)		(2.0ms actual)		(3.99ms actual)

	VARIABLE		2.531s				226.1ms				219.5ms				194.4ms

*/

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <format>

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
} RGB;

std::string renderToString(RGB* pixels, unsigned short width, unsigned short height) {
	RGB current_color = RGB();
	RGB color = RGB();

	std::string string_buffer = "";
	size_t count = 0;
	
	// Loop through the image's pixels
	for (unsigned int i = 0; i < width*height; i++, count++) {
		color = pixels[i];

		// If the next color does not equal the current color, attach a color-change string to the buffer string.
		if (current_color.argb != color.argb) {
			current_color = color;
			string_buffer.append(std::string(count, ' '));

			string_buffer.append(std::format("\x1b[48;2;{};{};{}m", current_color.R, current_color.G, current_color.B));
			count = 0;
		}
	}

	string_buffer.append(std::string(count, ' '));
	string_buffer.append("\x1b[m");

	return string_buffer;
}

int main(int argc, char** argv) {
	if (argc > 1) {
		RGB* pixels = new RGB[WIDTH*HEIGHT];
		RGB color = {0, 0, 0, 0};

		switch (argv[1][0]) {
			case 'r':
				std::srand(std::time(nullptr));

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
			std::cout << "\x1b[H" << renderToString(pixels, WIDTH, HEIGHT);
		}
		
		delete[] pixels;
	}
}