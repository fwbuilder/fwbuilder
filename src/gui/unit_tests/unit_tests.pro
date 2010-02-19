DOLLAR = $

run.commands = echo "Running tests..." && \
               display=:1; \
               Xvfb $${DOLLAR}$${DOLLAR}display & \
               pid=$${DOLLAR}$${DOLLAR}!; \
               for directory in `find . -maxdepth 1 -type d -regex \'\./[A-Za-z0-9\-\_]*\'`; \
               do \
                    home=`pwd`; \
                    cd $${DOLLAR}$${DOLLAR}directory; \
                    qmake -spec $${DOLLAR}$${DOLLAR}QMAKESPEC; \
                    DISPLAY=$${DOLLAR}$${DOLLAR}display make run || { kill $${DOLLAR}$${DOLLAR}pid; exit 1; }; \
                    cd $${DOLLAR}$${DOLLAR}home; \
              done; \
              kill $${DOLLAR}$${DOLLAR}pid;
			
QMAKE_EXTRA_TARGETS += run
