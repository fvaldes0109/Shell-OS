build:
	gcc -o main main.c src/**/*.c -I ./include

run:
	gcc -o main main.c src/*.c -I ./include && ./main
