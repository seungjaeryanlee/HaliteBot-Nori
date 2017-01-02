CC = g++
CFLAGS = -std=c++11

nori: Nori_1.0

clean:
	rm *.o *.hlt 1-*.log

Nori_2.1: 
	$(CC) $(CFLAGS) Nori_2.1.cpp -o Nori_2.1.o

Nori_2.0: 
	$(CC) $(CFLAGS) Nori_2.0.cpp -o Nori_2.0.o

Nori_1.2: 
	$(CC) $(CFLAGS) Nori_1.2.cpp -o Nori_1.2.o

Nori_1.1: 
	$(CC) $(CFLAGS) Nori_1.1.cpp -o Nori_1.1.o

Nori_1.0: 
	$(CC) $(CFLAGS) Nori_1.0.cpp -o Nori_1.0.o