emcc main.c Engine/*.c -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s SDL2_MIXER_FORMATS='["wav"]' -s INITIAL_MEMORY=64MB -s ALLOW_MEMORY_GROWTH=1 --preload-file Art --preload-file Engine --preload-file Sounds --preload-file Fonts -o index.html
