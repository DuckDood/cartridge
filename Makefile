all: obj/ build/ obj/main.o build/cartridge 
clean:
	rm -r obj/ build/
.PHONY: clean

obj/:
	mkdir -p obj/

build/:
	mkdir -p build/

obj/main.o: src/main.cpp
	${CXX} src/main.cpp -c -o obj/main.o

build/cartridge: obj/main.o 
	g++ obj/main.o  -o build/cartridge 

