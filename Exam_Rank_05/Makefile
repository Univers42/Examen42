# Exam Rank 05 -- one entry point for every exercise's harness.
#
#   make            compile checks, subject output, units, fuzz, mutants  (~3 min)
#   make full       ... and also sanitize, valgrind and soak              (~10 min)
#   make marathon   five long fuzz campaigns, run in parallel             (~12 min)
#   make sanitize   ASan + UBSan everywhere
#   make valgrind   leaks and invalid accesses everywhere
#   make soak       long fuzz runs across many seeds everywhere
#   make clean      remove every build directory
#
# The fuzz volume is a knob, not a constant. Every harness reads SEEDS and
# FUZZARGS from the environment, so a heavier pass is:
#
#   SEEDS=500 make soak
#   SEEDS=2000 make -C bigint/tests soak FUZZARGS="--steps 2000000 --exprs 200000"
#
# Each exercise can also be driven on its own:  make -C bigint/tests test

SUITES := vect2/solution/tests bigint/tests polyset/tests bsq/tests life/tests

.PHONY: all test full marathon sanitize valgrind soak mutants compile units fuzz subject clean

all: test

test:
	@./run_all.sh

full:
	@./run_all.sh full

marathon:
	@./run_all.sh marathon

sanitize valgrind soak mutants compile units fuzz subject:
	@./run_all.sh $@

clean:
	@for s in $(SUITES); do $(MAKE) --no-print-directory -C $$s fclean; done
	@rm -rf .run_all_logs
	@echo "cleaned"
