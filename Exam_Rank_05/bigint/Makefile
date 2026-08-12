CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all: subject

subject: bigint.cpp main.cpp bigint.hpp
	$(CXX) $(CXXFLAGS) bigint.cpp main.cpp -o subject

# The real harness lives in tests/: units, differential fuzzing, mutation
# testing, the warning matrix and the subject-output diff.
test:
	$(MAKE) -C tests test

# The ad-hoc checker that predates tests/. Kept because it is quick to run.
legacy_tests: bigint.cpp tests.cpp bigint.hpp
	$(CXX) $(CXXFLAGS) bigint.cpp tests.cpp -o legacy_tests
	./legacy_tests

# Cross-check driver + python oracle in assess/.
assess: bigint.cpp assess/fuzz.cpp bigint.hpp
	$(CXX) $(CXXFLAGS) bigint.cpp assess/fuzz.cpp -o assess/fuzz
	cd assess && python3 fuzz.py

clean:
	rm -f subject legacy_tests assess/fuzz assess/torture
	$(MAKE) -C tests clean

.PHONY: all test legacy_tests assess clean
