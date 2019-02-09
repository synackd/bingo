app_bingo   := bingo/
app_manager := manager/
lib_global  := lib/

.PHONY: all clean $(app_manager) $(app_bingo) $(lib_global)

# Toplevel target
all: $(app_manager) $(app_bingo)

# Invoke make on subdirectories
$(app_manager) $(app_bingo) $(lib_global):
	$(MAKE) --directory=$@

# Build library before apps
$(app_manager): $(lib_global)
$(app_bingo): $(lib_global)

# Clean up
clean:
	$(MAKE) --directory=$(app_manager) clean
	$(MAKE) --directory=$(app_bingo) clean
	$(MAKE) --directory=$(lib_global) clean
