# Test harness for polyset. The graded files (searchable_array_bag.{cpp,hpp},
# searchable_tree_bag.{cpp,hpp}, set.{cpp,hpp}) are always compiled with the
# exam's own flags; only the test code is allowed to need anything else.
#
#   make            build everything
#   make test       the full run: compile checks, subject, units, fuzz, mutants
#   make units      deterministic edge cases          (make units ARGS=-v for detail)
#   make fuzz       differential fuzzing              (make fuzz ARGS="--random")
#   make subject    run the subject's main and diff it against the expected output
#   make sanitize   units + fuzz under ASan/UBSan
#   make valgrind   units + fuzz under valgrind
#   make soak       fuzz over many seeds
#   make mutants    prove the suite catches injected bugs
#   make compile    warning matrix, canonical form, const-correctness, ODR

CXX      := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
TESTFLAGS := -Wall -Wextra -std=c++98 -g

DIR      := ..
# The provided implementations. The grader supplies its own copies.
PROVIDED := $(DIR)/array_bag.cpp $(DIR)/tree_bag.cpp
# The six files the exam actually collects.
GRADED   := $(DIR)/searchable_array_bag.cpp $(DIR)/searchable_tree_bag.cpp $(DIR)/set.cpp
HDR      := $(DIR)/searchable_array_bag.hpp $(DIR)/searchable_tree_bag.hpp $(DIR)/set.hpp \
            $(DIR)/bag.hpp $(DIR)/searchable_bag.hpp $(DIR)/array_bag.hpp $(DIR)/tree_bag.hpp
SRC      := $(PROVIDED) $(GRADED)
SUBJECT  := $(DIR)/main.cpp
SUBJARGS := 5 3 5 1
BUILD    := build
ARGS     ?=

# array_bag.cpp (provided, not graded) frees a new[] array with plain delete.
# Both tools are told to ignore exactly that, and nothing else -- see
# provided.supp and the note in README.md.
ASANOPTS := ASAN_OPTIONS=alloc_dealloc_mismatch=0
VGFLAGS  := --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all \
            --suppressions=provided.supp -q

GREEN := \033[32m
CYAN  := \033[36m
RED   := \033[1;31m
OFF   := \033[0m

.PHONY: all test units fuzz subject compile mutants sanitize valgrind soak clean fclean re

all: $(BUILD)/units $(BUILD)/fuzz $(BUILD)/subject

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/units: $(SRC) $(HDR) test_units.cpp vtest.hpp | $(BUILD)
	@$(CXX) $(TESTFLAGS) -I$(DIR) $(SRC) test_units.cpp -o $@

$(BUILD)/fuzz: $(SRC) $(HDR) test_fuzz.cpp vtest.hpp | $(BUILD)
	@$(CXX) $(TESTFLAGS) -I$(DIR) $(SRC) test_fuzz.cpp -o $@

# Built with the exam's exact flags, since this is the program the grader runs.
$(BUILD)/subject: $(SRC) $(HDR) $(SUBJECT) | $(BUILD)
	@$(CXX) $(CXXFLAGS) -I$(DIR) $(SRC) $(SUBJECT) -o $@

units: $(BUILD)/units
	@./$(BUILD)/units $(ARGS)

fuzz: $(BUILD)/fuzz
	@./$(BUILD)/fuzz $(ARGS)

subject: $(BUILD)/subject
	@./$(BUILD)/subject $(SUBJARGS) > $(BUILD)/subject.out
	@if diff -u expected_subject.txt $(BUILD)/subject.out; then \
		printf "  $(GREEN)[ok]$(OFF) subject main output matches expected_subject.txt\n"; \
	else \
		printf "  $(RED)[KO]$(OFF) subject main output differs\n"; exit 1; \
	fi
	@if ./$(BUILD)/subject > /dev/null 2>&1; then \
		printf "  $(RED)[KO]$(OFF) subject main returned 0 with no arguments\n"; exit 1; \
	else \
		printf "  $(GREEN)[ok]$(OFF) subject main exits non-zero with no arguments\n"; \
	fi

compile:
	@./compile_checks.sh

mutants:
	@./mutation.sh

sanitize: | $(BUILD)
	@printf "$(CYAN)== ASan + UBSan ==$(OFF)\n"
	@$(CXX) $(TESTFLAGS) -I$(DIR) -fsanitize=address,undefined -fno-omit-frame-pointer \
		$(SRC) test_units.cpp -o $(BUILD)/units_san
	@$(CXX) $(TESTFLAGS) -I$(DIR) -fsanitize=address,undefined -fno-omit-frame-pointer \
		$(SRC) test_fuzz.cpp -o $(BUILD)/fuzz_san
	@$(ASANOPTS) UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 ./$(BUILD)/units_san
	@$(ASANOPTS) UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
		./$(BUILD)/fuzz_san --bags 1500 --sets 1500 --views 1000
	@printf "  $(GREEN)[ok]$(OFF) clean under ASan + UBSan\n"

valgrind: $(BUILD)/units $(BUILD)/fuzz
	@printf "$(CYAN)== valgrind ==$(OFF)\n"
	@valgrind $(VGFLAGS) ./$(BUILD)/units
	@valgrind $(VGFLAGS) ./$(BUILD)/fuzz --bags 400 --sets 400 --views 300
	@printf "  $(GREEN)[ok]$(OFF) no leaks, no invalid accesses in the graded code\n"

# Long fuzz campaigns over many seeds. Both knobs are overridable, so one
# target serves a quick regression and an overnight marathon:
#   make soak SEEDS=500
#   make soak SEEDS=500 FUZZARGS="--bags 40000 --sets 40000 --views 30000"
# The environment is honoured too, which is how run_all.sh passes them down.
SEEDS    ?= 60
FUZZARGS ?= --bags 6000 --sets 6000 --views 4500

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
