CC = g++
CFLAGS = -std=c++11

nori: Nori_1.0

clean:
	rm *.o *.hlt

Nori_1.2: 
	$(CC) $(CFLAGS) Nori_1.2.cpp -o Nori_1.2.o

Nori_1.1: 
	$(CC) $(CFLAGS) Nori_1.1.cpp -o Nori_1.1.o

Nori_1.0: 
	$(CC) $(CFLAGS) Nori_1.0.cpp -o Nori_1.0.o