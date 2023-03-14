all: main 

main: main.cpp lib.hpp  
	g++ -std=c++17 main.cpp && ./a.out 

test: test.c lib.hpp miniunit.h 
	g++ -std=c++17 -lc test.c && ./a.out
