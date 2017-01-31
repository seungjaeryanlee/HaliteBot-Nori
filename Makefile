CC = g++
CFLAGS = -std=c++11

nori: Nori_1.0

clean:
	rm *.o *.hlt 1-*.log

Nori_4: 
	$(CC) $(CFLAGS) Nori_4.cpp -o Nori_4.o

Nori_3: 
	$(CC) $(CFLAGS) Nori_3.cpp -o Nori_3.o

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