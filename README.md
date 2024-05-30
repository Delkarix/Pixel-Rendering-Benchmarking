# Summary

### CPU (according to `neofetch`): Intel i7-8565U (8) @ 4.600GHz

**NOTE**: If it is difficult to read the files, copy-and-paste them into a normal text editor (some of the tabulation will be a little broken though due to REPL's/VS Code's weirdness)

**NOTE 2**: It's likely that a massive chunk of the processing time is due to the initial pixel writing (memory writing). It's extremely difficult (if not impossible) to accurately externally benchmark these programs without the initial overhead. I suppose that's also a mark on the efficiency of program startup, stdout writing, and pixel-writing as well.

**NOTE 3**: Consider D's outputs (particularly the *Variable* part) to be invalid. Some weird voodoo shit is going on with the compiler and now it's not even compiling the original properly. I suspend it's something to do with `stdout` buffering or the `write` function.

**NOTE 4**: For the first benchmarking comparisons, the GCC branch of compilers was used for C, C++, and D (gcc, g++, and gdc respectively.). For the cycle counts, Clang was used instead (clang, clang++, and ldc respectively).

**NOTE 5**: For the **Cycle Counts** field, the C++ file has been changed to accomodate the Clang compiler. It no longer uses `std::format()` and instead uses the C standard library `std::snprintf()` with buffering and shows a dramatic improvement in performance, which should be grounds for retesting. However, the original intent of this project was to illustrate the speedup (or rather, slowdown) comparison in "naive" programming across different languages (to show the potential dangers of overreliance on high-level constructs): using `snprintf()` in C is perfectly logical because that is what the language (and any beginner) is restricted to. However, using `std::snprintf()` in C++ relies on knowledge of C, which cuts C++ down to the level of C. This is illogical, because the intent was to use the target language's native constructs which a beginner would likely use (i.e. `std::format()`). In other words, we wrote C code inside of a C++ file. It doesn't count.

**TODO**: REDO ALL TESTS (they were done on separate days and appear dramatically faster/slower in some cases)

**RULE-OF-THUMB**: -O1 is generally (almost always) 10x faster than -O0. Higher optimization levels are better but have diminishing returns.

Overall, the ranking (ranked in order of fastest to slowest) follows as such:
1. C
2. Rust
3. C++
4. D

C generally appears to be dramatically faster because it uses a much more primitive algorithm. C++, Rust, and D all use high-level String operations (incur runtime allocation costs due to repeated string reallocations) while C uses manual byte manipulation and one-time AOT allocations.
Due to this, C starts out 10x faster than the other algorithms but has a much flatter optimization curve.

Interestingly, Rust's `--release` option produced worst-possible code for all variants. Thus, we're not even going to consider it here. (RETEST!!!)

## Basic Benchmarking Comparison
### Random
|language|   -O0   |   -O1   |   -02   |   -O3   | Release |
|--------|---------|---------|---------|---------|---------|
|   C    | 168.4ms | 161.5ms | 165.0ms | 160.8ms |    -    |
|  C++   | 2.696s  | 294.6ms | 275.4ms | 247.8ms |    -    |
|  Rust  | 1.921s  | 185.4ms | 181.2ms | 178.2ms | 1.641s  |
|   D    | 1.761s  | 1.088s  | 974.9ms | 958.5ms |    -    |

Ranking:
|   -O0   |   -O1   |   -02   |   -O3   |
|---------|---------|---------|---------|
|   C     |    C    |    C    |    C    |
|   D     |   Rust  |   Rust  |   Rust  |
|  Rust   |   C++   |   C++   |   C++   |
|  C++    |    D    |    D    |    D    |

Overall:
1. C
2. Rust
3. C++
4. D

Maybe we should calculate (multiplicatively) how much slower each language is than its predecessor?

### Solid
Using the "actual" values calculated with the multi-pass loop division.

|language|   -O0   |   -O1   |   -02   |   -O3   | Release |
|--------|---------|---------|---------|---------|---------|
|   C    |  4.8ms  |  2.71ms | 2.56ms  | 3.01ms  |    -    |
|  C++   |  4.27ms |  2.09ms |  2.0ms  | 3.99ms  |    -    |
|  Rust  |  22.3ms |  3.17ms | 3.77ms  | 3.71ms  | 37.4ms  |
|   D    |  18.0ms |  4.1ms  | 1.98ms  | 2.21ms  |    -    |

|   -O0   |   -O1   |   -02   |   -O3   |
|---------|---------|---------|---------|
|   C     |   C++   |    D    |    D    |
|  C++    |    C    |   C++   |    C    |
|   D     |    D    |    C    |   Rust  |
|  Rust   |   Rust  |   Rust  |   C++   |

Overall:
1. D
2. C++
3. C
4. Rust

Generally, higher optimization levels tend to cause these values to converge so take them with a grain of salt: the difference isn't as big as you'd think.
Interesting how -O3 tends to take as long or slightly longer than -O2. Might want to take this into consideration for future projects.

### Variable
Note that each of these use a slightly different algorithm so take these with a grain of salt.

**NOTE**: I NEED TO RETEST THIS BECAUSE D WAS TESTED INCORRECTLY (was testing it on 10 loops rather than just 1 lol). Also should retest on Rust release.

|language|   -O0   |   -O1   |   -02   |   -O3   | Release |
|--------|---------|---------|---------|---------|---------|
|   C    | 141.5ms | 133.4ms | 134.3ms | 130.8ms |    -    |
|  C++   | 2.531s  | 226.1ms | 219.5ms | 194.4ms |    -    |
|  Rust  | 522.7ms | 145.1ms | 152.4ms | 145.0ms | 1.391s  |
|   D    | 1.623s  | 995.9ms | 989.5ms | 895.8ms |    -    |

|   -O0   |   -O1   |   -02   |   -O3   |
|---------|---------|---------|---------|
|    C    |    C    |    C    |    C    |
|  Rust   |   Rust  |   Rust  |   Rust  |
|    D    |   C++   |   C++   |   C++   |
|   C++   |    D    |    D    |    D    |

Seems like D had a lot of trouble optimizing this one. Probably the shitty `color = Color(color.toInt() + 1);` statement. However, after going back and changing
```d
color = Color(color.toInt() + 1);
```
to
```d
uint val;
...
val = *cast(uint*)(&color) + 1;
color = *cast(Color*)(&val);
```
the execution time became **dramatically** slower. It went from ~900ms to *8 seconds* on -O3, which is ridiculous and I'm not sure why it's doing that.
Unless my logic sucks, it seems like something is making the compiler shit the bed.


Overall:
1. C
2. Rust
3. C++
4. D

### Instruction Count
|language|   -O0   |   -O1   |   -02   |   -O3   |
|--------|---------|---------|---------|---------|
|   C    |   248   |   192   |   209   |   209   |
|  C++   |   3401  |  3752   |  4232   |  5459   |
|  Rust  |  51973  |  31011  |  33642  |  33771  |
|   D    |  27352  |  22224  |  21191  |  22224  |

Rust may have even more code due to how I generated the ASM files (`cargo rustc -- -C opt-level=3 --emit asm`)

Overall:
1. C
2. C++
3. D
4. Rust

## Cycle Counts
**NOTE**: We are using the LLVM family of compilers (clang, clang++, cling, and ldc).

Note that the first benchmarking tests may be erroneous due to overhead, as we are measuring both the startup costs and the actual algorithm. Measuring the actual time taken isn't ideal nor practical, so instead we measure the rough arbitrary amount of time taken by using the `clock()` function from the C standard library:
```
clock_t begin = clock();
    <...CODE...>
clock_t diff = clock() - begin;
```

We will run each 10 times and take the average.
Additionally, we are including the C++ tool Cling which is a Just-In-Time (JIT) compiler for C++ due to some conditional performance improvements and conveniences (primarily on the unoptimized side).

|language|   -O0   |   -O1   |   -02   |   -O3   |
|--------|---------|---------|---------|---------|
|   C    |  263171 |  236177 |  246000 |  241425 |
|  C++   |  224388 |  214929 |  216624 |  215521 |
| Cling  |  214885 |  244283 |  235951 |  230707 |
|  Rust  |  387515 |  208011 |  206614 |  222697 |
|   D    | 1345003 |  778337 |  767568 |  781692 |

|   -O0   |   -O1   |   -02   |   -O3   |
|---------|---------|---------|---------|
|  Cling  |  Rust   |   Rust  |   C++   |
|   C++   |   C++   |   C++   |   Rust  |
|    C    |    C    |  Cling  |  Cling  |
|  Rust   |  Cling  |    C    |    C    |
|    D    |    D    |    D    |    D    |

Overall:
1. Rust
2. C++
3. Cling
4. C
5. D

Note that C, C++, Cling, and Rust all lie closely together (so true comparison is unnecessary) while D is a heavy outlier.
