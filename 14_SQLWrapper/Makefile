# === Compiler settings ===
CXX := g++ -std=c++23

# === Static analyzers ===

ADAPTOR_FLAGS := -g -O0 -fno-omit-frame-pointer -fPIE -pie

COMPILE_LIMITS := \
	-fmax-errors=5 \
	-fno-rtti \

WARNINGS_CLASSIC := -Wall -Wextra -pedantic

WARNINGS_SANITAIZER := \
	-fsanitize=undefined,address,leak \
	-fsanitize=bounds,alignment,vptr,object-size,pointer-compare,pointer-subtract \
	# -fsanitize=thread

WARNINGS_BONUS := \
	-Weffc++ \
	-Wuseless-cast -Wcast-align=strict -Wcast-qual \
	-Wshadow -Warith-conversion -Wconversion -Wdouble-promotion \
	-Wundef -Wstrict-overflow=5 -Wuninitialized -Wuse-after-free=3 \
	-Wduplicated-cond -Wduplicated-branches -Wlogical-op \
	-Wmissing-include-dirs -Wredundant-decls \
	-Wmissing-format-attribute\
	-Wmissing-declarations\
	
WARNINGS_GCC := \
	-Wctor-dtor-privacy -Woverloaded-virtual \
	-Wzero-as-null-pointer-constant \
	-Wformat=2

WARNINGS_AS_ERRORS := \
	-Werror -Wfatal-errors -pedantic-errors

WARNINGS_NOTUSED := \
	-Wsign-conversion \
	-Wframe-larger-than=4096 \
	
WARNINGS_SUPER_SLOW := -fanalyzer

# === Compiler flags ===

CXXFLAGS := \
	$(ADAPTOR_FLAGS) \
	$(COMPILE_LIMITS) \
	$(WARNINGS_CLASSIC) \
	$(WARNINGS_SANITAIZER) \
	$(WARNINGS_BONUS) \
	$(WARNINGS_GCC) \
	$(ALL_WARNINGS) \
	$(WARNINGS_AS_ERRORS)

EXAMPLE_CODE_FLAGS := \
	$(CXX) $(ADAPTOR_FLAGS) \
	$(WARNINGS_CLASSIC) \
	$(WARNINGS_SANITAIZER) \

# === Library Specific Flags ===

LDFLAGS := -lgtest -lgtest_main -pthread

SQLITE3FLAG := -lsqlite3




# ===# ===# ===# ===# ===# ===
# === Module discovery ===
MODULE_DIRS := $(wildcard src/*/)
MODULES := $(patsubst src/%/,%,$(MODULE_DIRS))

# === Collected files and flags ===
ALL_SRC_FILES :=
ALL_TEST_FILES :=
ALL_EXTRA_LDFLAGS :=
VISITED_MODULES :=

# === Module resolution macro ===
define resolve_module
$(eval include src/$(1)/Module.mk)

$(eval SRC_FILES_$(1) := $(addprefix src/$(1)/,$(SRC_FILES)))
$(eval TEST_FILES_$(1) := $(addprefix src/$(1)/,$(TEST_FILES)))
$(eval DEP_MODULES_$(1) := $(DEP_MODULES))
$(eval EXTRA_LDFLAGS_$(1) := $(EXTRA_LDFLAGS))

$(if $(findstring $(1),$(VISITED_MODULES)),,\
$(eval VISITED_MODULES += $(1)) \
$(foreach dep,$(DEP_MODULES_$(1)),$(call resolve_module,$(dep))) \
$(eval ALL_SRC_FILES += $(SRC_FILES_$(1))) \
$(eval ALL_TEST_FILES += $(TEST_FILES_$(1))) \
$(eval ALL_EXTRA_LDFLAGS += $(EXTRA_LDFLAGS_$(1))) )
endef






# ===# ===# ===# ===# ===# ===#
# === Static analysis

# === Static analysis target configuration ===
ANALYZE_MODULES :=\
	SQLite_Wrapper\

ANALYZE_SOURCES :=\

# === Run Cppcheck Analysis ===
analyze-cppcheck:
	@echo "\033[1;35m\n [*] Running cppcheck...\n\033[0m"
	$(eval ALL_SRC_FILES :=)
	$(eval ALL_TEST_FILES :=)
	$(eval ALL_EXTRA_LDFLAGS :=)
	$(eval VISITED_MODULES :=)
	$(foreach m,$(ANALYZE_MODULES),$(call resolve_module,$(m)))
	@sh -c '\
		cppcheck \
			--enable=all \
			--inconclusive --std=c++23 --language=c++ --quiet --force \
			--suppress=missingIncludeSystem \
			$(ANALYZE_SOURCES) $(ALL_SRC_FILES) \
		|| { echo "\033[1;31m[!] Cppcheck found issues.\033[0m"; exit 1; }'

# === Run Clang Static Analyzer (scan-build) ===
analyze-clang:
	@echo "\033[1;35m\n [*] Running clang (compile-only analysis)...\n\033[0m"
	$(eval ALL_SRC_FILES :=)
	$(eval ALL_TEST_FILES :=)
	$(eval ALL_EXTRA_LDFLAGS :=)
	$(eval VISITED_MODULES :=)
	$(foreach m,$(ANALYZE_MODULES),$(call resolve_module,$(m)))
	@mkdir -p scan-report
	@sh -c '\
		for f in $(ANALYZE_SOURCES) $(ALL_SRC_FILES); do \
			scan-build -o scan-report --status-bugs $(CXX) $(CXXFLAGS) -c $$f -o /dev/null || exit 1; \
		done; \
		rm -rf scan-report'

# === Run all static analysis targets ===
analyze-all:
	@clear
	@$(MAKE) --no-print-directory analyze-cppcheck
	@$(MAKE) --no-print-directory analyze-clang






# ===# ===# ===# ===# ===# ===#
# === Run tests of specific modules (Unity Build & Non-Unity Build) ===

run_old:
	$(eval ALL_SRC_FILES :=)
	$(eval ALL_TEST_FILES :=)
	$(eval ALL_EXTRA_LDFLAGS :=)
	$(eval VISITED_MODULES :=)

	@clear
	$(call resolve_module,$(MODULE))

	@echo "\033[1;35m\n [*] Compiling $(MODULE).test with individual source files:\n\033[0m"
	@for f in $(sort $(ALL_SRC_FILES) $(ALL_TEST_FILES)); do echo "    $$f"; done

	@echo "\033[1;35m\n [*] Running compilation and test...\n\033[0m"; \
	START=$$(date +%s); \
	trap 'echo "\033[1;35m\n [*] Cleaning up...\n\033[0m"; rm -f $(MODULE).test' EXIT; \
	$(CXX) $(CXXFLAGS) $(sort $(ALL_SRC_FILES) $(ALL_TEST_FILES)) $(LDFLAGS) $(ALL_EXTRA_LDFLAGS) -o $(MODULE).test && \
	END=$$(date +%s) && \
	echo "\033[1;36m\t [*] Compilation completed in $$((END - START)) seconds.\033[0m" && \
	echo "\033[1;35m\n [*] Running test binary...\n\033[0m" && \
	./$(MODULE).test

run:
	$(eval ALL_SRC_FILES :=)
	$(eval ALL_TEST_FILES :=)
	$(eval ALL_EXTRA_LDFLAGS :=)
	$(eval VISITED_MODULES :=)

	@clear
	$(call resolve_module,$(MODULE))

	@echo "\033[1;35m\n [*] Running cppcheck on resolved sources before compilation...\n\033[0m"
	@cppcheck \
		--enable=all \
		--inconclusive --std=c++23 --language=c++ --quiet --force \
		--suppress=missingIncludeSystem \
		$(foreach f,$(ALL_TEST_FILES),--suppress=*:$f) \
		$(ALL_SRC_FILES) $(ALL_TEST_FILES) \
		|| { echo "\033[1;31m[!] Cppcheck found issues.\033[0m"; exit 1; }

	@echo "\033[1;35m\n [*] Creating Unity Build for module: $(MODULE)...\n\033[0m"
	@echo "// === Unity Build for $(MODULE) ===" > UnityBuild.cpp
	@for f in $(sort $(ALL_SRC_FILES) $(ALL_TEST_FILES)); do \
		echo "#include \"$$f\"" >> UnityBuild.cpp; \
	done

	@NUM_INCLUDES=$$(grep -c '^#include' UnityBuild.cpp); \
	echo "\033[1;35m\n [*] Unity sources ($$NUM_INCLUDES):\n\033[0m"; \
	grep '^#include' UnityBuild.cpp | sed 's/^/    /'

	@echo "\033[1;35m\n [*] Running compilation and test...\n\033[0m"; \
	START=$$(date +%s); \
	trap 'echo "\033[1;35m\n [*] Cleaning up...\n\033[0m"; rm -f UnityBuild.cpp $(MODULE).test' EXIT; \
	$(CXX) $(CXXFLAGS) UnityBuild.cpp $(LDFLAGS) $(ALL_EXTRA_LDFLAGS) -o $(MODULE).test && \
	END=$$(date +%s) && \
	echo "\033[1;36m\t [*] Compilation completed in $$((END - START)) seconds.\033[0m" && \
	echo "\033[1;35m\n [*] Running test binary...\n\033[0m" && \
	./$(MODULE).test

# === Run all testable modules ===
all:
	@for m in $(MODULES); do \
		if [ -f $$m/Module.mk ]; then \
			$(MAKE) --no-print-directory -s MODULE=$$m || exit 1; \
		fi; \
	done




# ===# ===# ===# ===# ===# ===#
# === Main program configuration ===
MAIN_MODULES :=\
	

MAIN_SOURCES :=\
	main.cpp

# === Main program specific flags ===
MAIN_CXXFLAGS :=\
	$(ADAPTOR_FLAGS)\
	$(WARNINGS_CLASSIC)\
	$(WARNINGS_SANITAIZER)\

# === Reset caches ===
$(eval ALL_SRC_FILES :=)
$(eval ALL_TEST_FILES :=)
$(eval ALL_EXTRA_LDFLAGS :=)
$(eval VISITED_MODULES :=)

# === Only expand module resolution if MAIN_MODULES is not empty ===
ifeq (false,$(strip $(MAIN_MODULES) ==))
  $(foreach m,$(MAIN_MODULES),$(eval $(call resolve_module,$(m))))
endif

main:
	@clear

	@if [ "$(strip $(MAIN_MODULES))" != "" ]; then \
		printf "\033[1;35m\n [*] Resolving modules: $(MAIN_MODULES)\n\033[0m"; \
	fi

	@if [ "$(strip $(MAIN_SOURCES))" != "" ]; then \
		printf "\033[1;35m\n [*] Compiling standalone sources:\n\033[0m"; \
		for f in $(MAIN_SOURCES); do echo "    $$f"; done; \
	fi

	@if [ "$(strip $(ALL_SRC_FILES))" != "" ]; then \
		printf "\033[1;35m\n [*] Module sources:\n\033[0m"; \
		for f in $(sort $(ALL_SRC_FILES)); do echo "    $$f"; done; \
	fi

	@printf "\033[1;35m\n [*] Compiling and running combined sources...\n\033[0m"; \
	CompileStart=$$(date +%s.%N); \
	if $(CXX) $(MAIN_CXXFLAGS) $(MAIN_SOURCES) $(ALL_SRC_FILES) $(ALL_EXTRA_LDFLAGS) -o main; then \
		CompileEnd=$$(date +%s.%N); \
		CompileTime=$$(echo "$$CompileEnd - $$CompileStart" | bc); \
		printf "\033[1;36m [✓] Compilation completed in %0.4fs\033[0m\n" "$$CompileTime"; \
		printf "\033[1;35m\n [*] Running compiled binary...\n\033[0m"; \
		RunStart=$$(date +%s.%N); \
		./main; \
		RunEnd=$$(date +%s.%N); \
		RunTime=$$(echo "$$RunEnd - $$RunStart" | bc); \
		rm -f main; \
		printf "\033[1;36m [✓] Execution completed in %0.4fs\033[0m\n" "$$RunTime"; \
		printf "\033[1;35m [*] fin...\n\033[0m"; \
	else \
		CompileEnd=$$(date +%s.%N); \
		CompileTime=$$(echo "$$CompileEnd - $$CompileStart" | bc); \
		printf "\033[1;31m\n [!] Compilation failed after %0.4fs. Skipping run.\n\033[0m" "$$CompileTime"; \
	fi

# === fin ===