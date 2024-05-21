# Summary
**NOTE**: If it is difficult to read the files, copy-and-paste them into a normal text editor (some of the tabulation will be a little broken though due to REPL's weirdness)

**NOTE 2**: C generally appears to be dramatically faster because it uses a much more primitive algorithm. C++, Rust, and D all use high-level String operations while C uses manual byte manipulation.

**NOTE 3**: It's likely that a massive chunk of the processing time is due to memory writing. It's extremely difficult (if not impossible) to accurately externally benchmark these programs without the initial overhead. I suppose that's also a mark on the efficiency of program startup and pixel-writing as well.

**RULE-OF-THUMB**: -O1 is generally (almost always) 10x faster than -O0. Higher optimization levels are better but have diminishing returns.

Overall, the ranking (ranked in order of fastest to slowest) follows as such:
1. C
2. Rust
3. C++
4. D

Interestingly, Rust's `--release` option produced worst-possible code for all variants. Thus, we're not even going to consider it here.

## Comparison
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

Seems like D had a lot of trouble optimizing this one. Probably the shitty `color = Color(color.toInt() + 1);` statement.

Overall:
1. C
2. Rust
3. C++
4. D
