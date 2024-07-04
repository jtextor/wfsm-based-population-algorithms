SHELL = /bin/sh
.SUFFIXES:

OBJECT_DIR := objects
TARGET_DIR := targets

WA_LIB_NAMES := termiter sat wfsms rational_weight safe_integer
WA_TARGET_NAMES := $(addsuffix _compress,boolfa logfa ratfa) boolfa_possel ratfa_possel ratfa_learn ratfa_classify algorithm2 solve
OTHER_TARGET_NAMES := 1+1ea

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
	$(eval $(call uc,$(target))_OBJECTS := $(addprefix $(OBJECT_DIR)/,$(addsuffix .o,$($(call uc,$(target))_NAMES)))))


# Let $(CXX) determine which headers each source file depends on.
DEPS := $(OBJECTS:%.o=%.d)


all: $(TARGETS)
	

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(TARGETS)

.PHONY: clean distclean

$(TARGET_DIR)/boolfa_compress: $(BOOLFA_COMPRESS_OBJECTS) $(OBJECT_DIR)/boolfa_compress.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/logfa_compress: $(LOGFA_COMPRESS_OBJECTS) $(OBJECT_DIR)/logfa_compress.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/ratfa_compress: $(RATFA_COMPRESS_OBJECTS) $(OBJECT_DIR)/ratfa_compress.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/boolfa_possel: $(BOOLFA_POSSEL_OBJECTS) $(OBJECT_DIR)/boolfa_possel.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/ratfa_possel: $(RATFA_POSSEL_OBJECTS) $(OBJECT_DIR)/ratfa_possel.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/ratfa_classify: $(RATFA_CLASSIFY_OBJECTS) $(OBJECT_DIR)/ratfa_classify.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/ratfa_learn: $(RATFA_LEARN_OBJECTS) $(OBJECT_DIR)/ratfa_learn.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/algorithm2: $(ALGORITHM2_OBJECTS) $(OBJECT_DIR)/dimacs.o $(OBJECT_DIR)/algorithm2.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/solve: $(SOLVE_OBJECTS) $(OBJECT_DIR)/dimacs.o $(OBJECT_DIR)/solve.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/1+1ea: 1+1ea.cpp $(OBJECT_DIR)/dimacs.o | trim.hpp $(TARGET_DIR)
	$(CXX) -g -std=c++1z -o $@ $^

-include $(DEPS)

$(OBJECT_DIR)/%.o: %.cpp | $(OBJECT_DIR)
	$(CXX) -std=c++1z -c -MMD -O3 -o $@ $<

$(OBJECT_DIR) $(TARGET_DIR):
	mkdir -p $@
