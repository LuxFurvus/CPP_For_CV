# === Compiler settings ===
CXX := g++

# === Static analyzers ===

WARNINGS_BASE := -g -O0 -fno-omit-frame-pointer
WARNINGS_CLASSIC := -Wall -Wextra -pedantic
USE_PIE := -fPIE -pie
WARNINGS_AS_ERRORS := -Werror
WARNINGS_SANITAZER := \
	# -fsanitize=undefined -fsanitize=address -fsanitize=leak
WARNINGS_EFFC := -Weffc++
# WARNINGS_FANALYZER := -fanalyzer

# === Compiler flags ===

CXXFLAGS := -std=c++26 $(WARNINGS_BASE) $(WARNINGS_CLASSIC) $(WARNINGS_SANITAZER) $(WARNINGS_EFFC) $(WARNINGS_AS_ERRORS)

CXXFLAGS_NO_ERRORS := -std=c++26 $(WARNINGS_BASE) $(WARNINGS_CLASSIC) $(WARNINGS_SANITAZER) $(WARNINGS_EFFC)

# === Library Specific Flags ===

LDFLAGS := -lgtest -lgtest_main -pthread
SQLITE3FLAG := -lsqlite3



# ===# ===# ===# ===# ===# ===
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


# ===# ===# ===# ===# ===# ===#
# === Run Cppcheck Analysis ===

CPPCHECK_FILTER := \
	-e "Active checkers:"
	
analyze-cppcheck:
	@echo "\033[1;35m\n [*] Running cppcheck...\n\033[0m"
	@cppcheck \
		--enable=all \
		--inconclusive --std=c++23 --language=c++ --quiet --force \
		--suppress=missingIncludeSystem \
		$(CPP_FOR_MAIN) \
		2>&1 | grep -v $(CPPCHECK_FILTER) \
		|| (echo "[!] Cppcheck found issues." && exit 1)


# ===# ===# ===# ===# ===# ===#
# === Run Clang Static Analyzer (scan-build) ===

CLANG_FILTER := \
	-e " in namespace 'std'" \
	-e " |     std::print("

analyze-clang:
	@echo "\033[1;35m\n [*] Running clang...\n\033[0m"
	@scan-build -o scan-report $(CXX) $(CXXFLAGS) \
		$(CPP_FOR_MAIN) $(SQLITE3FLAG) -o main-clang-analysis \
		2>&1 | grep -v $(CLANG_FILTER)
	@rm -rf scan-report/
	@rm -f main-clang-analysis


# ===# ===# ===# ===# ===# ===#
# === Run specific module (Unity Build & Non-Unity Build) ===
run_nonunity:
	@clear
	$(call resolve_module,$(MODULE))
	@echo "\033[1;35m\n [+] Compiling $(MODULE).test with sources:\n\033[0m"
	@for f in $(sort $(ALL_SRC_FILES)); do echo "    $$f"; done

	@echo "\033[1;35m\n [*] Running compilation...\n\033[0m"
	@{ \
		START=$$(date +%s); \
		$(CXX) $(CXXFLAGS) $(sort $(ALL_SRC_FILES)) $(LDFLAGS) $(ALL_EXTRA_LDFLAGS) -o $(MODULE).test; \
		END=$$(date +%s); \
		echo "\033[1;36m\t [*] Compilation completed in $$((END - START)) seconds.\033[0m"; \
	}

	@echo "\033[1;35m\n [*] Running test binary...\n\033[0m"
	@./$(MODULE).test

	@echo "\033[1;35m\n [*] Cleaning up...\n\033[0m"
	@rm -f $(MODULE).test

run:
	@clear
	$(call resolve_module,$(MODULE))

	@echo "\033[1;35m\n [*] Creating Unity Build for module: $(MODULE)...\n\033[0m"
	@echo "// === Unity Build for $(MODULE) ===" > UnityBuild.cpp
	@for f in $(sort $(ALL_SRC_FILES)); do \
		echo "#include \"$$f\"" >> UnityBuild.cpp; \
	done

	@echo "\033[1;35m\n [*] Unity sources:\n\033[0m"
	@cat UnityBuild.cpp | grep '#include' | sed 's/^/    /'

	@echo "\033[1;35m\n [*] Running compilation...\n\033[0m"
	@{ \
		START=$$(date +%s); \
		$(CXX) $(CXXFLAGS) UnityBuild.cpp $(LDFLAGS) $(ALL_EXTRA_LDFLAGS) -o $(MODULE).test; \
		END=$$(date +%s); \
		echo "\033[1;36m\t [*]Compilation completed in $$((END - START)) seconds.\033[0m"; \
	}

	@echo "\033[1;35m\n [*] Running test binary...\n\033[0m"
	@./$(MODULE).test

	@echo "\033[1;35m\n [*] Cleaning up...\n\033[0m"
	@rm -f UnityBuild.cpp $(MODULE).test

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
	@echo "\033[1;35m\n [*] Running compilation...\n\033[0m"
	$(CXX) $(CXXFLAGS_NO_ERRORS) studytester.cpp -o stud
	@echo "\033[1;35m\n [*] Compiled binary output...\n\033[0m"
	@./stud; rm -f stud
	@echo "\033[1;35m\n [*] fin...\n\033[0m"

# === Run main program ===

CPP_FOR_MAIN := main.cpp \
				ConditionChecker/ConditionChecker.cpp \
				SQLiteConnector/SQLiteConnector.cpp

main:
	@clear
	$(MAKE) --no-print-directory anal
	@echo "\033[1;35m\n [*] Running compilation...\n\033[0m"
	$(CXX) $(CXXFLAGS) $(CPP_FOR_MAIN) $(SQLITE3FLAG) -o main
	@echo "\033[1;35m\n [*] Compiled binary output...\n\033[0m"
	@./main; rm -f main
	@echo "\033[1;35m\n [*] fin...\n\033[0m"

# === Run static analysis ===

anal:
	$(MAKE) --no-print-directory analyze-cppcheck
	$(MAKE) --no-print-directory analyze-clang

# === fin ===