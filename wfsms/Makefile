SHELL = /bin/sh
.SUFFIXES:

OBJECT_DIR := objects
TARGET_DIR := targets

COMPRESS_TARGETS := $(addsuffix _compress,boolfa logfa ratfa)
COMPRESS_OBJECTS := $(addprefix $(OBJECT_DIR)/,$(addsuffix .o,$(COMPRESS_TARGETS)))
WA_LIB_NAMES := termiter sat wfsms rational_weight safe_integer
WA_TARGET_NAMES := $(COMPRESS_TARGETS) ratfa_possel ratfa_learn ratfa_classify algorithm2
OTHER_TARGET_NAMES := 1+1ea
DIMACS_TARGETS := $(addprefix $(TARGET_DIR)/,1+1ea algorithm2)

TARGET_NAMES := $(WA_TARGET_NAMES) $(OTHER_TARGET_NAMES)
OBJECT_NAMES := $(TARGET_NAMES) $(SHARED_NAMES)

OBJECTS := $(addprefix $(OBJECT_DIR)/,$(addsuffix .o,$(OBJECT_NAMES)))
TARGETS := $(addprefix $(TARGET_DIR)/,$(TARGET_NAMES))

uc = $(shell echo $(1) | tr 'a-z' 'A-Z')

# All targets get associated $(target)_NAMES and $(target)_OBJECTS
#  variables containing their dependencies.  We prepopulate the
#  variables for which the targets depend on the local weighted automata
#  library with objects for that library.
$(foreach wa_target,$(WA_TARGET_NAMES),\
	$(eval $(call uc,$(wa_target))_NAMES := $(wa_target) $(WA_LIB_NAMES)))

$(foreach target,$(TARGET_NAMES),\
	$(eval $(call uc,$(target))_OBJECTS := $(addprefix $(OBJECT_DIR)/,$(addsuffix .o,$(target) $($(call uc,$(target))_NAMES)))))


# Let $(CXX) determine which headers each source file depends on.
DEPS := $(OBJECTS:%.o=%.d)

all: $(TARGETS)
	

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(TARGETS)

.PHONY: clean distclean

$(OBJECT_DIR) $(TARGET_DIR):
	mkdir -p $@

-include $(DEPS)

$(COMPRESS_OBJECTS): fatype_compress.cpp
	$(CXX) -c -DFA_TYPE=$(subst _compress.o,,$(@F)) -std=c++1z -MMD -O3 -o $@ $<

$(TARGET_DIR)/1+1ea: 1+1ea.cpp $(OBJECT_DIR)/dimacs.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^

$(TARGET_DIR)/algorithm2: $(ALGORITHM2_OBJECTS) $(OBJECT_DIR)/dimacs.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

.SECONDEXPANSION:

$(filter-out $(COMPRESS_OBJECTS),$(OBJECTS)): $$(subst .o,.cpp,$$(@F)) | $(OBJECT_DIR)
	$(CXX) -c -std=c++1z -MMD -O3 -o $@ $<

$(filter-out $(DIMACS_TARGETS),$(TARGETS)): $$($$(call uc,$$(@F))_OBJECTS) | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst