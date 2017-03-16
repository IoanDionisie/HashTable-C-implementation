
build: tema

tema: tema1.o libhash.so
	gcc tema1.o  -lhash -L. -o tema1

tema1.o: tema1.c
	gcc -g -c -o tema1.o tema1.c

clean:
	rm -f *.o tema1	

