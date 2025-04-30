all: obj/ build/ obj/main.o build/cartridge 

clean:
	rm -r obj/ build/ 

obj/:
	mkdir -p obj/

build/:
	mkdir -p build/

obj/main.o: src/main.cpp
	${CC} src/main.cpp -c -o obj/main.o 

build/cartridge: obj/main.o 
	${CXX} obj/main.o  -o build/cartridge 

