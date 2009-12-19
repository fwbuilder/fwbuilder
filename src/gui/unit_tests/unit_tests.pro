run.commands = echo "Running tests..." && \
			   cd FWBTree && qmake && make run; cd ..; \
			   cd ImporterTest && qmake && make run; cd ..; \
			   cd RCS && qmake && make run; cd ..; \
			   cd UsageResolverTest && qmake && make run; cd ..;
			
QMAKE_EXTRA_TARGETS += run
