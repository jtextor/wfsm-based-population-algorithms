OBJECT_DIR := objects
TARGET_DIR := targets

WA_LIB_NAMES := termiter sat ratfa rational_weight safe_integer
WA_TARGET_NAMES := algorithm2
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
	

$(TARGET_DIR)/algorithm2: $(ALGORITHM2_OBJECTS) $(OBJECT_DIR)/algorithm2.o | $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^ -lfst

$(TARGET_DIR)/1+1ea: 1+1ea.cpp | trim.hpp $(TARGET_DIR)
	$(CXX) -std=c++1z -o $@ $^

-include $(DEPS)

$(OBJECT_DIR)/%.o: %.cpp | $(OBJECT_DIR)
	$(CXX) -std=c++1z -c -MMD -O3 -o $@ $<

$(OBJECT_DIR) $(TARGET_DIR):
	mkdir -p $@
