DOLLAR = $

run.commands = echo "Running tests..." && \
               for directory in `find . -type d -maxdepth 1 -regex \'\./[A-Za-z0-9\-\_]*\'`; \
               do \
                    home=`pwd`; \
                    cd $${DOLLAR}$${DOLLAR}directory; \
                    qmake; \
                    make run; \
                    cd $${DOLLAR}$${DOLLAR}home; \
                done
			
QMAKE_EXTRA_TARGETS += run
