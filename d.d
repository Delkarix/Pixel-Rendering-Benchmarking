/*
CMDLINE OPTIONS:
	r: makes random colors (to test average speed)
	s: uses a singular color (to test fastest speed)
	v: every pixel uses a different color (to test slowest speed)
*/

/*

BENCHMARK RESULTS (measured in seconds/milliseconds):
    NOTE: for SOLID -O2 and -O3, there were imprecisions in the benchmarking so CYCLES was set to 10 and the overall time was then divided by 10.

					-O0			-O1				-O2			        -O3
	RANDOM			1.761s		1.088s			974.9ms             958.5ms

	SOLID			18.0ms		6.7ms			3.7ms               6.9ms 
                                (4.1 actual)	(1.98ms actual)     (2.21ms actual)

	VARIABLE		1.623s		995.9ms			989.5ms		        895.8ms

*/

import std.stdio;
import std.random;
import std.datetime;
import std.format;
static import core.stdc.time;

enum WIDTH = 1000;
enum HEIGHT = 1000;
enum CYCLES = 10;

// Represents an ARGB color value.
struct Color {
	ubyte alpha; // The value of the Alpha channel.
	ubyte red; // The value of the Red channel.
	ubyte green; // The value of the Green channel.
	ubyte blue; // The value of the Blue channel.

	this(ubyte a, ubyte r, ubyte g, ubyte b) {
		alpha = a;
		red = r;
		green = g;
		blue = b;
	}
	
	// Initializes a Color struct with a given numerical value. NOTE: ENDIANNESS WILL FUCK US OVER
	this(uint argb) {
		alpha = cast(ubyte)((argb & 0xFF000000) >> 24);
		red = cast(ubyte)((argb & 0x00FF0000) >> 16);
		green = cast(ubyte)((argb & 0x0000FF00) >> 8);
		blue = cast(ubyte)(argb & 0x000000FF);
	}

	uint toInt() {
		return (alpha << 24) + (red << 16) + (green << 8) + blue;
	}
}

// Returns a string that's been repeated n-times.
string repeat(string str, uint n) {
	string new_str = "";
	
	for (; n > 0; n--) {
		new_str ~= str;
	}

	return new_str;
}

string RenderToString(Color* pixels, ushort width, ushort height) {
    string buffer = "\x1b[H\x1b[48;2;;;m"; // Change the background to black
    uint stored_color;
    uint current_color;
    ushort count;

    // Loop through y
    for (ushort y = 0; y < height; y++) {
        // Loop through x
        for (ushort x = 0; x < width; x++) {
            current_color = pixels.toInt();
            
            // Test if the stored color matches the currently selected pixel.
            if (stored_color != current_color) {
                // If not, set the new stored color and update the color data.
                stored_color = current_color;
                buffer ~= " ".repeat(count); // Fill in the missing spaces

                // Send the virtual terminal sequence to change the color.
                buffer ~= format("\x1b[48;2;%u;%u;%um", pixels.red, pixels.green, pixels.blue);

                count = 0; // Reset the count.
            }

            count++;
            pixels++; // Increments pixels to next position in memory. This should be more efficient than doing pixels[x][y] every loop.
        }
    }

    buffer ~= " ".repeat(count); // Fill in any remaining missing spaces
    buffer ~= "\x1b[m"; // Virtual terminal sequence to reset everything.

    return buffer;
}

void main(string[] args) {
    if (args.length > 1) {
        Color[] _pixels = new Color[WIDTH*HEIGHT];
        Color* pixels = &_pixels[0];
		Color color = Color(0);
        Random r = Random();

		switch (args[1][0]) {
			case 'r':
				for (int i = 0; i < WIDTH*HEIGHT; i++) {
					pixels[i] = Color(uniform!"[]"(0, uint.max, r));
				}

				break;
			case 's':
				for (int i = 0; i < WIDTH*HEIGHT; i++) {
					pixels[i] = Color(0);
				}

				break;
			case 'v':
				for (int i = 0; i < WIDTH*HEIGHT; i++) {
					pixels[i] = color;
                    color = Color(color.toInt() + 1);
				}
				break;
            default:
                break;
		}


		for (int i = 0; i < CYCLES; i++) {
            write(RenderToString(pixels, WIDTH, HEIGHT));
		}
    }
}