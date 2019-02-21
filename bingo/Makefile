app    := bingo
src    := $(wildcard src/*.cpp)
obj    := $(patsubst src/%.cpp,build/%.cpp.o,$(src))
inc    := ../lib/include include/
libdir := ../lib/
lib    := global

CC       := g++
CPPFLAGS += $(addprefix -I ,$(inc)) -std=c++11
LDFLAGS  := $(addprefix -L ,$(libdir)) $(addprefix -l ,$(lib))
vpath %.a $(lib)
vpath %.hpp $(inc)

all: $(app)

# Create binary from object files
$(app): $(obj)
	@echo ' [LD]   $@'
	@$(CC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

# Create build folder if it doesn't exist
build:
	@mkdir $@

# Create object files
build/%.cpp.o: src/%.cpp build
	@echo ' [CC]   $@'
	@$(CC) $(CPPFLAGS) -c -o $@ $<

# Clean up
.PHONY: clean
clean:
	@echo ' [CLEAN]   $(app)'
	@rm -rf $(app)
	@rm -rf build
