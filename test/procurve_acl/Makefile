
FW_OBJECTS := $(shell fwbedit list -f objects-for-regression-tests.fwb -o /User/Firewalls -c -F%name% | sort)
CL_OBJECTS := $(shell fwbedit list -f cluster-tests.fwb -o /User/Clusters -c -F%name% | sort)


$(FW_OBJECTS):
	fwb_procurve_acl -f objects-for-regression-tests.fwb -xt $@

$(CL_OBJECTS):
	fwb_procurve_acl -f cluster-tests.fwb -xt -xc $@

.PHONY: all firewalls clusters $(FW_OBJECTS) $(CL_OBJECTS)
all: firewalls clusters

firewalls: $(FW_OBJECTS)

clusters: $(CL_OBJECTS)
