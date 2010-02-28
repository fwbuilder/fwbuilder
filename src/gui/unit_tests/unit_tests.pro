DOLLAR = $

build.commands = echo "Building tests..."; \
               ./unit_tests.sh make build_test

run.commands = echo "Running tests..."; \
               ./unit_tests.sh make run_test

clean.commands = echo "Cleaning tests..."; \
                 ./unit_tests.sh make clean_test

QMAKE_EXTRA_TARGETS += run clean build
