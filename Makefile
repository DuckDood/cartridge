all: obj/ build/ obj/main.o build/cartridge 

install:
	install build/cartridge /usr/local/bin/
.PHONY: install

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
	${CXX} obj/main.o  -o build/cartridge 

