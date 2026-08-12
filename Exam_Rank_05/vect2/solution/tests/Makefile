# Test harness for vect2. The graded files (../vect2.cpp, ../vect2.hpp) are
# always compiled with the exam's own flags; only the test code is allowed to
# need anything else.
#
#   make            build everything
#   make test       the full run: compile checks, units, fuzz, subject, mutants
#   make units      deterministic edge cases          (make units ARGS=-v for detail)
#   make fuzz       differential + expression fuzzing (make fuzz ARGS="--random")
#   make subject    run the subject's main and diff it against the expected output
#   make sanitize   units + fuzz under ASan/UBSan
#   make valgrind   units + fuzz under valgrind
#   make soak       fuzz over many seeds
#   make mutants    prove the suite catches injected bugs
#   make compile    warning matrix, const-correctness, ODR

CXX      := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
TESTFLAGS := -Wall -Wextra -std=c++98 -g

SRC      := ../vect2.cpp
HDR      := ../vect2.hpp
SUBJECT  := ../../subject_main.cpp
BUILD    := build
ARGS     ?=

GREEN := \033[32m
CYAN  := \033[36m
RED   := \033[1;31m
OFF   := \033[0m

.PHONY: all test units fuzz subject compile mutants sanitize valgrind soak clean fclean re

all: $(BUILD)/units $(BUILD)/fuzz $(BUILD)/subject

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/units: $(SRC) $(HDR) test_units.cpp vtest.hpp | $(BUILD)
	@$(CXX) $(TESTFLAGS) $(SRC) test_units.cpp -o $@

$(BUILD)/fuzz: $(SRC) $(HDR) test_fuzz.cpp vtest.hpp | $(BUILD)
	@$(CXX) $(TESTFLAGS) $(SRC) test_fuzz.cpp -o $@

# Built with the exam's exact flags, since this is the program the grader runs.
$(BUILD)/subject: $(SRC) $(HDR) $(SUBJECT) | $(BUILD)
	@$(CXX) $(CXXFLAGS) -I.. $(SRC) $(SUBJECT) -o $@

units: $(BUILD)/units
	@./$(BUILD)/units $(ARGS)

fuzz: $(BUILD)/fuzz
	@./$(BUILD)/fuzz $(ARGS)

subject: $(BUILD)/subject
	@./$(BUILD)/subject > $(BUILD)/subject.out
	@if diff -u expected_subject.txt $(BUILD)/subject.out; then \
		printf "  $(GREEN)[ok]$(OFF) subject main output matches expected_subject.txt\n"; \
	else \
		printf "  $(RED)[KO]$(OFF) subject main output differs\n"; exit 1; \
	fi

compile:
	@./compile_checks.sh

mutants:
	@./mutation.sh

# Signed overflow, bad reads/writes and leaks. The fuzz model already refuses
# to trigger overflow on purpose, so anything UBSan reports here is real.
sanitize: | $(BUILD)
	@printf "$(CYAN)== ASan + UBSan ==$(OFF)\n"
	@$(CXX) $(TESTFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer \
		$(SRC) test_units.cpp -o $(BUILD)/units_san
	@$(CXX) $(TESTFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer \
		$(SRC) test_fuzz.cpp -o $(BUILD)/fuzz_san
	@UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 ./$(BUILD)/units_san
	@UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 ./$(BUILD)/fuzz_san --steps 60000 --trees 8000
	@printf "  $(GREEN)[ok]$(OFF) clean under ASan + UBSan\n"

valgrind: $(BUILD)/units $(BUILD)/fuzz
	@printf "$(CYAN)== valgrind ==$(OFF)\n"
	@valgrind --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all \
		-q ./$(BUILD)/units
	@valgrind --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all \
		-q ./$(BUILD)/fuzz --steps 20000 --trees 2000
	@printf "  $(GREEN)[ok]$(OFF) no leaks, no invalid accesses\n"

# Long fuzz campaigns over many seeds. Both knobs are overridable, so one
# target serves a quick regression and an overnight marathon:
#   make soak SEEDS=500
#   make soak SEEDS=2000 FUZZARGS="--steps 2000000 --trees 200000"
# The environment is honoured too, which is how run_all.sh passes them down.
SEEDS    ?= 120
FUZZARGS ?= --steps 120000 --trees 12000

soak: $(BUILD)/fuzz
	@printf "$(CYAN)== soak: $(SEEDS) seeds ==$(OFF)\n  "
	@i=1; while [ $$i -le $(SEEDS) ]; do \
		./$(BUILD)/fuzz --seed $$i $(FUZZARGS) > $(BUILD)/soak.log 2>&1 || \
			{ printf "\n  $(RED)[KO]$(OFF) seed $$i failed\n"; cat $(BUILD)/soak.log; exit 1; }; \
		[ $$(($$i % 25)) -eq 0 ] && printf "."; \
		i=$$((i + 1)); \
	done; printf "\n"
	@printf "  $(GREEN)[ok]$(OFF) $(SEEDS) seeds clean, $(FUZZARGS) each\n"

test: all
	@printf "\n$(CYAN)########## compile-time checks ##########$(OFF)\n"
	@$(MAKE) --no-print-directory compile
	@printf "\n$(CYAN)########## subject main ##########$(OFF)\n"
	@$(MAKE) --no-print-directory subject
	@printf "\n$(CYAN)########## unit / edge cases ##########$(OFF)\n"
	@./$(BUILD)/units
	@printf "\n$(CYAN)########## fuzzing ##########$(OFF)\n"
	@./$(BUILD)/fuzz
	@printf "\n$(CYAN)########## mutation testing ##########$(OFF)\n"
	@$(MAKE) --no-print-directory mutants
	@printf "\n$(GREEN)########## everything passed ##########$(OFF)\n"

clean:
	@rm -rf $(BUILD)

fclean: clean

re: fclean all
