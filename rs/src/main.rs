/*
CMDLINE OPTIONS:
	r: makes random colors (to test average speed)
	<*>: uses a singular color (to test fastest speed)
	v: every pixel uses a different color (to test slowest speed)
*/

/*
BENCHMARK RESULTS (measured in seconds/milliseconds):
    --NOTE: for SOLID, there were imprecisions in the benchmarking so CYCLES was set to 10 and the overall time was then divided by 10.

					DEBUG		        -O1			        -O2			        -O3                     RELEASE
	RANDOM			1.921s		        185.4ms		        181.2ms		        178.2ms                 1.641s

	SOLID			22.3ms		        5.0ms		        5.1ms		        3.6ms                   37.4ms
                                        (3.17ms actual)     (3.77ms actual)     (3.71ms actual)

	VARIABLE		522.7ms		        145.1ms		        152.4ms		        145.0ms                 1.391s

*/

use std::env;
use rand::Rng;
extern crate libc;

pub type Color = (u8, u8, u8, u8);

pub type clock_t = ::libc::clock_t;

union Converter {
    color: Color,
    argb: u32
}

extern {
    pub fn clock() -> clock_t;
}

const WIDTH: u32 = 1000;
const HEIGHT: u32 = 1000;
const CYCLES: u8 = 1;

fn renderToString(pixels: &[Color], width: u32, height: u32) -> String {
    let mut current_color_index: usize = 0;
    let mut str_buffer: String = String::from("\x1b[48;2;;;m");

    let mut stored_color: Color = (0, 0, 0, 0);
    let mut current_color: Color;

    let mut count: usize = 0;

    for y in 0..height {
        for x in 0..width {
            current_color = pixels[current_color_index];

            if stored_color != current_color {
                stored_color = current_color;
                
                str_buffer.push_str(&(" ".repeat(count)));
                str_buffer.push_str(&format!("\x1b[48;2;{};{};{}m", current_color.1, current_color.2, current_color.3));

                count = 0;
            }

            count += 1;
            current_color_index += 1;
        }
    }
    
    str_buffer.push_str(&(" ".repeat(count)));
    str_buffer.push_str("\x1b[m");
    str_buffer
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut pixels: Vec<Color> = vec![(0, 0, 0, 0); (WIDTH*HEIGHT) as usize];
    let mut color_box: Box<Color> = Box::new((0, 0, 0, 0));
    let color_ptr: *mut Color = pixels.as_mut_ptr();
    //let mut conv: u32;
    let mut conv_ptr: *mut u32;

    if args.len() > 1 {
        match args[1].as_bytes()[0] {
            b'r' => {
                for i in 0..WIDTH*HEIGHT {
                    pixels[i as usize].1 = rand::thread_rng().gen_range(0..=255);
                    pixels[i as usize].2 = rand::thread_rng().gen_range(0..=255);
                    pixels[i as usize].3 = rand::thread_rng().gen_range(0..=255);
                }
            },
            // No s needed (handled by default)
            b'v' => {
                for i in 0..WIDTH*HEIGHT {
                    unsafe {
                        conv_ptr = color_ptr.add(i as usize) as *mut u32;
                        *conv_ptr += 1;
                        //pixels[i as usize] = *(conv_ptr as *mut Color);
                    }
                }
            },
            _ => ()
        }
    }

    let begin: clock_t = unsafe {clock()};
    for i in 0..CYCLES {
        print!("\x1b[H{}", renderToString(&pixels, WIDTH, HEIGHT));
    }
    let diff: clock_t = unsafe {clock() - begin};

    println!("{}", diff);
}