app := libglobal.a
src := $(wildcard src/*.cpp)
obj := $(patsubst src/%.cpp,build/%.cpp.o,$(src))
inc := include/

CC       := g++
CPPFLAGS += $(addprefix -I ,$(inc))
vpath %.hpp $(inc)

all: $(app)

# Create binary from object files
$(app): $(obj)
	@echo ' [AR]   $@'
	@$(AR) $(ARFLAGS) $@ $^

# Create build folder if it doesn't exist
build:
	@mkdir $@

# Create object files
build/%.cpp.o: src/%.cpp build
	@echo ' [CC]   $@'
	@$(CC) $(CPPFLAGS) -c -o $@ $<

# Build socket base before subclasses
build/server.cpp.o: build/socket.cpp.o build/colors.cpp.o
build/client.cpp.o: build/socket.cpp.o build/colors.cpp.o

# Clean up
.PHONY: clean
clean:
	@echo ' [CLEAN]   $(app)'
	@rm -rf $(app)
	@rm -rf build
