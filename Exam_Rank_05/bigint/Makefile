CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all: subject

subject: bigint.cpp main.cpp bigint.hpp
	$(CXX) $(CXXFLAGS) bigint.cpp main.cpp -o subject

tests: bigint.cpp tests.cpp bigint.hpp
	$(CXX) $(CXXFLAGS) bigint.cpp tests.cpp -o tests

test: tests subject
	./subject
	./tests

fuzz: bigint.cpp training/fuzz.cpp bigint.hpp
	$(CXX) $(CXXFLAGS) bigint.cpp training/fuzz.cpp -o training/fuzz
	cd training && python3 fuzz.py

clean:
	rm -f subject tests training/fuzz training/torture

.PHONY: all test clean fuzz
