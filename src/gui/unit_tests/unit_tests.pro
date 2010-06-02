DOLLAR = $

guilib.target = guilib/libguilib.a
guilib.commands = cd guilib && qmake -spec $$QMAKESPEC && make && cd ..

build_tests.commands = echo "Building tests..."; \
               ./unit_tests.sh make build_tests

run_tests.commands = echo "Running tests..."; \
               ./unit_tests.sh make run_tests

clean_tests.commands = echo "Cleaning tests..."; \
                 ./unit_tests.sh make clean_tests

build_tests.depends = guilib
run_tests.depends = guilib

QMAKE_EXTRA_TARGETS += guilib run_tests clean_tests build_tests
