DOLLAR = $

build.commands = echo "Building tests..."; \
               ./unit_tests.sh make

run.commands = echo "Running tests..."; \
               ./unit_tests.sh make run

clean.commands = echo "Cleaning tests..."; \
                 ./unit_tests.sh make clean

QMAKE_EXTRA_TARGETS += run clean build
