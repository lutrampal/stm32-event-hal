# Convenience defines that can be used by individual tests in makefiles/
export UNITTEST_BUILD_DIR = build
export PROJECT_ROOT_DIR := $(abspath ../..)
export UNITTEST_ROOT := $(abspath .)
export PROJECT_SRC_DIR := $(PROJECT_ROOT_DIR)/src

export UNITTEST_SRC_DIR := $(UNITTEST_ROOT)/src
export UNITTEST_MAKEFILES_DIR := $(UNITTEST_ROOT)/makefiles
export CPPUTEST_MAKEFILE_INFRA := $(UNITTEST_ROOT)/MakefileWorkerOverrides.mk

# Collects all the Makefile_*.mk in this directory and then invokes them using
#  recursive make.
# Allows filtering by setting `UNITTEST_MAKEFILE_FILTER` to a wildcard filter. 
UNITTEST_MAKEFILE_FILTER ?= *
UNITTEST_MAKEFILES := $(wildcard $(UNITTEST_MAKEFILES_DIR)/Makefile_$(UNITTEST_MAKEFILE_FILTER))

export UNITTEST_EXTRA_INC_PATHS += \
  -I$(PROJECT_SRC_DIR)

# Run the test on all Makesfiles found
all: $(UNITTEST_MAKEFILES)

compile: $(UNITTEST_MAKEFILES)

$(UNITTEST_MAKEFILES):
	$(MAKE) -f $@ $(CPPUTEST_BUILD_RULE)

clean:
	rm -rf $(UNITTEST_BUILD_DIR)
	
.PHONY: all clean $(UNITTEST_MAKEFILES)
