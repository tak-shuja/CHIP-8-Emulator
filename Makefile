main:
	mkdir -p out/
	gcc src/main.c src/cpu.c src/screen.c src/io.c -o out/main -lSDL2

all: main

clean:
	rm out/*
