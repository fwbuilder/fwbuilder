DOLLAR = $

run.commands = echo "Running tests..." && \
               for directory in `find . -maxdepth 1 -type d -regex \'\./[A-Za-z0-9\-\_]*\'`; \
               do \
                    home=`pwd`; \
                    cd $${DOLLAR}$${DOLLAR}directory; \
                    qmake -spec $${DOLLAR}$${DOLLAR}QMAKESPEC; \
                    make run || exit 1; \
                    cd $${DOLLAR}$${DOLLAR}home; \
                done
			
QMAKE_EXTRA_TARGETS += run
