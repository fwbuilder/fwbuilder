DOLLAR = $

build_tests.commands = echo "Building tests..."; \
               ./unit_tests.sh make build_tests

run_tests.commands = echo "Running tests..."; \
               ./unit_tests.sh make run_tests

clean_tests.commands = echo "Cleaning tests..."; \
                 ./unit_tests.sh make clean_tests

QMAKE_EXTRA_TARGETS += run_tests clean_tests build_tests
