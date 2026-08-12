# Test harness for life. The graded files (../life.c, ../life.h) are always
# compiled with the exam's own flags; only the test code is allowed to need
# anything else.
#
#   make            build the program
#   make test       the full run: compile checks, subject, units, fuzz, mutants
#   make units      deterministic edge cases          (make units ARGS=-v for detail)
#   make fuzz       differential fuzzing              (make fuzz ARGS="--random")
#   make subject    replay the subject's five examples and diff them
#   make sanitize   the corpus under ASan/UBSan
#   make valgrind   the corpus under valgrind
#   make soak       fuzz over many seeds
#   make mutants    prove the suite catches injected bugs
#   make compile    warning matrix, allowed functions, process behaviour

CC       := cc
# Exactly what the moulinette runs: no -std, so the compiler's own default
# applies, the same way it will on the exam machine.
CFLAGS   := -Wall -Wextra -Werror
TESTFLAGS := -Wall -Wextra -g

DIR      := ..
SRC      := $(DIR)/life.c
HDR      := $(DIR)/life.h
BUILD    := build
BIN      := $(BUILD)/life
ARGS     ?=

GREEN := \033[32m
CYAN  := \033[36m
RED   := \033[1;31m
OFF   := \033[0m

.PHONY: all test units fuzz subject compile mutants sanitize valgrind soak clean fclean re

all: $(BIN)

$(BUILD):
	@mkdir -p $(BUILD)

# Built with the exam's exact flags, since this is the program the grader runs.
$(BIN): $(SRC) $(HDR) | $(BUILD)
	@$(CC) $(CFLAGS) -I$(DIR) $(SRC) -o $@

units: $(BIN)
	@python3 test_units.py $(BIN) $(ARGS)

fuzz: $(BIN)
	@python3 test_fuzz.py $(BIN) $(ARGS)

subject: $(BIN)
	@./run_subject.sh $(BIN) > $(BUILD)/subject.out 2> $(BUILD)/subject.err
	@if diff -u expected_subject.txt $(BUILD)/subject.out; then \
		printf "  $(GREEN)[ok]$(OFF) all five subject examples match expected_subject.txt\n"; \
	else \
		printf "  $(RED)[KO]$(OFF) a subject example differs\n"; exit 1; \
	fi
	@if [ -s $(BUILD)/subject.err ]; then \
		printf "  $(RED)[KO]$(OFF) something was written to stderr\n"; \
		cat $(BUILD)/subject.err; exit 1; \
	else \
		printf "  $(GREEN)[ok]$(OFF) nothing was written to stderr\n"; \
	fi

compile:
	@./compile_checks.sh

mutants:
	@./mutation.sh

# The board is indexed with a single flattened counter, so an off-by-one in
# the neighbour walk reads outside it without changing the answer -- ASan is
# the layer that sees that.
sanitize: | $(BUILD)
	@printf "$(CYAN)== ASan + UBSan ==$(OFF)\n"
	@$(CC) $(TESTFLAGS) -I$(DIR) -fsanitize=address,undefined -fno-omit-frame-pointer \
		$(SRC) -o $(BUILD)/life_san
	@UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
		python3 test_units.py $(BUILD)/life_san
	@UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
		python3 test_fuzz.py $(BUILD)/life_san --cases 1200 --long 15
	@printf "  $(GREEN)[ok]$(OFF) clean under ASan + UBSan\n"

valgrind: $(BIN)
	@printf "$(CYAN)== valgrind ==$(OFF)\n"
	@printf 'sdxddssaaww\n'                 | valgrind --error-exitcode=1 --leak-check=full \
		--errors-for-leak-kinds=all -q ./$(BIN) 5 5 0 > /dev/null
	@printf 'sdxssdswdxddddsxaadwxwdxwaa\n' | valgrind --error-exitcode=1 --leak-check=full \
		--errors-for-leak-kinds=all -q ./$(BIN) 10 6 4 > /dev/null
	@printf 'dxss\n'                        | valgrind --error-exitcode=1 --leak-check=full \
		--errors-for-leak-kinds=all -q ./$(BIN) 3 3 7 > /dev/null
	@printf 'x\n'                           | valgrind --error-exitcode=1 --leak-check=full \
		--errors-for-leak-kinds=all -q ./$(BIN) 1 1 1 > /dev/null
	@printf 'xddddssssaaaawwww\n'           | valgrind --error-exitcode=1 --leak-check=full \
		--errors-for-leak-kinds=all -q ./$(BIN) 20 20 3 > /dev/null
	@printf ''                              | valgrind --error-exitcode=1 --leak-check=full \
		--errors-for-leak-kinds=all -q ./$(BIN) 4 4 0 > /dev/null
	@printf "  $(GREEN)[ok]$(OFF) no leaks, no invalid accesses\n"

# Long fuzz campaigns over many seeds. Both knobs are overridable, so one
# target serves a quick regression and an overnight marathon:
#   make soak SEEDS=500
#   make soak SEEDS=200 FUZZARGS="--cases 30000 --long 200"
# The environment is honoured too, which is how run_all.sh passes them down.
SEEDS    ?= 30
FUZZARGS ?= --cases 3500 --long 30

soak: $(BIN)
	@printf "$(CYAN)== soak: $(SEEDS) seeds ==$(OFF)\n  "
	@i=1; while [ $$i -le $(SEEDS) ]; do \
		python3 test_fuzz.py $(BIN) --seed $$i $(FUZZARGS) > $(BUILD)/soak.log 2>&1 || \
			{ printf "\n  $(RED)[KO]$(OFF) seed $$i failed\n"; cat $(BUILD)/soak.log; exit 1; }; \
		[ $$(($$i % 25)) -eq 0 ] && printf "."; \
		i=$$((i + 1)); \
	done; printf "\n"
	@printf "  $(GREEN)[ok]$(OFF) $(SEEDS) seeds clean, $(FUZZARGS) each\n"

test: all
	@printf "\n$(CYAN)########## compile-time checks ##########$(OFF)\n"
	@$(MAKE) --no-print-directory compile
	@printf "\n$(CYAN)########## the subject's examples ##########$(OFF)\n"
	@$(MAKE) --no-print-directory subject
	@printf "\n$(CYAN)########## unit / edge cases ##########$(OFF)\n"
	@python3 test_units.py $(BIN)
	@printf "\n$(CYAN)########## fuzzing ##########$(OFF)\n"
	@python3 test_fuzz.py $(BIN)
	@printf "\n$(CYAN)########## mutation testing ##########$(OFF)\n"
	@$(MAKE) --no-print-directory mutants
	@printf "\n$(GREEN)########## everything passed ##########$(OFF)\n"

clean:
	@rm -rf $(BUILD) __pycache__

fclean: clean

re: fclean all
