# Test harness for bsq. The graded file (../solution/bsq.c) is always compiled
# with the exam's own flags; only the test code is allowed to need anything
# else.
#
#   make            build the program
#   make test       the full run: compile checks, subject, units, fuzz, mutants
#   make units      deterministic edge cases          (make units ARGS=-v for detail)
#   make fuzz       differential fuzzing              (make fuzz ARGS="--random")
#   make subject    run the subject's own example and diff it
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

SRC      := ../solution/bsq.c
BUILD    := build
BIN      := $(BUILD)/bsq
ARGS     ?=

# The subject's one genuine ambiguity -- see README.md. SEP=blank switches the
# whole behavioural suite over to expecting a blank line between maps, which
# is the other reading of "each solution ... must be followed by a line break".
SEP      ?= none

GREEN := \033[32m
CYAN  := \033[36m
RED   := \033[1;31m
OFF   := \033[0m

.PHONY: all test units fuzz subject compile mutants sanitize valgrind soak clean fclean re

all: $(BIN)

$(BUILD):
	@mkdir -p $(BUILD)

# Built with the exam's exact flags, since this is the program the grader runs.
$(BIN): $(SRC) | $(BUILD)
	@$(CC) $(CFLAGS) $(SRC) -o $@

units: $(BIN)
	@python3 test_units.py $(BIN) --sep $(SEP) $(ARGS)

fuzz: $(BIN)
	@python3 test_fuzz.py $(BIN) --sep $(SEP) $(ARGS)

subject: $(BIN)
	@./$(BIN) maps/subject.map > $(BUILD)/subject.out 2> $(BUILD)/subject.err
	@if diff -u expected_subject.txt $(BUILD)/subject.out; then \
		printf "  $(GREEN)[ok]$(OFF) the subject's example matches expected_subject.txt\n"; \
	else \
		printf "  $(RED)[KO]$(OFF) the subject's example differs\n"; exit 1; \
	fi
	@if [ -s $(BUILD)/subject.err ]; then \
		printf "  $(RED)[KO]$(OFF) something was written to stderr\n"; \
		cat $(BUILD)/subject.err; exit 1; \
	else \
		printf "  $(GREEN)[ok]$(OFF) nothing was written to stderr\n"; \
	fi
	@./$(BIN) < maps/subject.map | diff -q - expected_subject.txt > /dev/null && \
		printf "  $(GREEN)[ok]$(OFF) the same example read from stdin\n" || \
		{ printf "  $(RED)[KO]$(OFF) the stdin path gives a different answer\n"; exit 1; }

compile:
	@./compile_checks.sh

mutants:
	@./mutation.sh

# The DP row and the row array are the two places an off-by-one would write
# out of bounds, and neither shows up as a wrong answer -- only ASan sees it.
sanitize: | $(BUILD)
	@printf "$(CYAN)== ASan + UBSan ==$(OFF)\n"
	@$(CC) $(TESTFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer \
		$(SRC) -o $(BUILD)/bsq_san
	@UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
		python3 test_units.py $(BUILD)/bsq_san --sep $(SEP)
	@UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
		python3 test_fuzz.py $(BUILD)/bsq_san --sep $(SEP) --maps 1500 --big 10
	@printf "  $(GREEN)[ok]$(OFF) clean under ASan + UBSan\n"

valgrind: $(BIN) | $(BUILD)
	@printf "$(CYAN)== valgrind ==$(OFF)\n"
	@printf '4 . o x\n....\n.o..\n....\n..o.\n' > $(BUILD)/ok.map
	@printf '2 . o x\n..\n.\n'                  > $(BUILD)/bad.map
	@printf 'not a map\n'                       > $(BUILD)/head.map
	@printf '2 . o x\noo\noo\n'                 > $(BUILD)/full.map
	@valgrind --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all -q \
		./$(BIN) $(BUILD)/ok.map $(BUILD)/bad.map $(BUILD)/head.map \
		$(BUILD)/full.map /nonexistent_zz > /dev/null
	@valgrind --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all -q \
		./$(BIN) < $(BUILD)/ok.map > /dev/null
	@valgrind --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all -q \
		./$(BIN) maps/subject.map > /dev/null
	@printf "  $(GREEN)[ok]$(OFF) no leaks, no invalid accesses\n"

# Long fuzz campaigns over many seeds. Both knobs are overridable, so one
# target serves a quick regression and an overnight marathon:
#   make soak SEEDS=500
#   make soak SEEDS=300 FUZZARGS="--maps 40000 --big 200"
# The environment is honoured too, which is how run_all.sh passes them down.
SEEDS    ?= 40
FUZZARGS ?= --maps 5000 --big 30

soak: $(BIN)
	@printf "$(CYAN)== soak: $(SEEDS) seeds ==$(OFF)\n  "
	@i=1; while [ $$i -le $(SEEDS) ]; do \
		python3 test_fuzz.py $(BIN) --sep $(SEP) --seed $$i $(FUZZARGS) > $(BUILD)/soak.log 2>&1 || \
			{ printf "\n  $(RED)[KO]$(OFF) seed $$i failed\n"; cat $(BUILD)/soak.log; exit 1; }; \
		[ $$(($$i % 25)) -eq 0 ] && printf "."; \
		i=$$((i + 1)); \
	done; printf "\n"
	@printf "  $(GREEN)[ok]$(OFF) $(SEEDS) seeds clean, $(FUZZARGS) each\n"

test: all
	@printf "\n$(CYAN)########## compile-time checks ##########$(OFF)\n"
	@$(MAKE) --no-print-directory compile
	@printf "\n$(CYAN)########## the subject's example ##########$(OFF)\n"
	@$(MAKE) --no-print-directory subject
	@printf "\n$(CYAN)########## unit / edge cases ##########$(OFF)\n"
	@python3 test_units.py $(BIN) --sep $(SEP)
	@printf "\n$(CYAN)########## fuzzing ##########$(OFF)\n"
	@python3 test_fuzz.py $(BIN) --sep $(SEP)
	@printf "\n$(CYAN)########## mutation testing ##########$(OFF)\n"
	@$(MAKE) --no-print-directory mutants
	@printf "\n$(GREEN)########## everything passed ##########$(OFF)\n"

clean:
	@rm -rf $(BUILD) __pycache__

fclean: clean

re: fclean all
