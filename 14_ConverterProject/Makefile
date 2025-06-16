# === Compiler settings ===
CXX := g++
CXXFLAGS := -std=c++26 -g -O0 -fno-omit-frame-pointer -Wall -Wextra -pedantic -fsanitize=address -fsanitize=undefined
LDFLAGS := -lgtest -lgtest_main -pthread
SQLITE3FLAG := -lsqlite3

# === Module discovery ===
MODULE_DIRS := $(wildcard */)
MODULES := $(patsubst %/,%,$(MODULE_DIRS))

# === Collected files and flags ===
ALL_SRC_FILES :=
ALL_EXTRA_LDFLAGS :=
VISITED_MODULES :=

# === Module loader with variable prefixing ===
define resolve_module
$(info Resolving module: $(1))
$(eval include $(1)/Module.mk)

# Prefix variables with module name
$(eval SRC_FILES_$(1) := $(SRC_FILES))
$(eval DEP_MODULES_$(1) := $(DEP_MODULES))
$(eval EXTRA_LDFLAGS_$(1) := $(EXTRA_LDFLAGS))

$(if $(findstring $(1),$(VISITED_MODULES)),,\
$(info Adding sources from $(1): $(SRC_FILES_$(1))) \
$(eval VISITED_MODULES += $(1)) \
$(foreach dep,$(DEP_MODULES_$(1)),$(call resolve_module,$(dep))) \
$(eval ALL_SRC_FILES += $(SRC_FILES_$(1))) \
$(eval ALL_EXTRA_LDFLAGS += $(EXTRA_LDFLAGS_$(1))) )
endef

# === Run specific module ===
run:
	@clear
	$(call resolve_module,$(MODULE))
	@echo "[+] Compiling $(MODULE).test with sources:"
	@for f in $(sort $(ALL_SRC_FILES)); do echo "    $$f"; done
	$(CXX) $(CXXFLAGS) $(sort $(ALL_SRC_FILES)) $(LDFLAGS) $(ALL_EXTRA_LDFLAGS) -o $(MODULE).test
	@echo "[*] Running test binary:"
	./$(MODULE).test
	@echo "[*] Removing binary: $(MODULE).test"
	rm -f $(MODULE).test

# === Run all testable modules ===
all:
	@for m in $(MODULES); do \
		if [ -f $$m/Module.mk ]; then \
			$(MAKE) --no-print-directory -s MODULE=$$m || exit 1; \
		fi; \
	done

# === Run education examples ===
stud:
	@clear
	$(CXX) $(CXXFLAGS) studytester.cpp $(SQLITE3FLAG) -o stud && ./stud; rm -f stud

# === Run main program ===
main:
	@clear
	$(CXX) $(CXXFLAGS) main.cpp $(SQLITE3FLAG) -o main && ./main; rm -f main

# === fin ===