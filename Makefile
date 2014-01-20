.PHONY: clean make

PROG = prog5Client
PROG2 = prog5Server
FLAGS = -lpthread


make: $(PROG) $(PROG2) 

$(PROG): $(PROG).o library.o
	g++ $(PROG).o library.o $(FLAGS) -o $(PROG)

$(PROG).o: $(PROG).cc
	g++ -c $(PROG).cc

$(PROG2): $(PROG2).o library.o
	g++ $(PROG2).o library.o $(FLAGS) -o $(PROG2)

$(PROG2).o: $(PROG2).cc

$library.o: library.cc
	g++ -c library.cc

clean:
	rm -f $(PROG) $(PROG).o
	rm -f $(PROG2) $(PROG2).o
	rm -f library.o
