# terrain
Experimenting with destructible terrain in SFML

# Requirements

1. A C++14-enabled compiler (makefile assumes g++)  
2. SFML (best if >= 2.4)

# Howto

1. Compile with `make`  
2. You can replace `fg.png` with any image you like,. Non-transparent pixels are considered solid.  
3. `./main`  
4. Clicking anywhere will dig a 30px-hole in the image and recalculate normals.
