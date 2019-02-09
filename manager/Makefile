app    := manager
src    := $(wildcard src/*.cpp)
obj    := $(patsubst src/%.cpp,build/%.cpp.o,$(src))
inc    := ../lib/include include/
libdir := ../lib
lib    := global

CC       := g++
CPPFLAGS += $(addprefix -I ,$(inc))
LDFLAGS  := $(addprefix -L ,$(libdir)) $(addprefix -l ,$(lib))
vpath %.hpp $(inc)

all: $(app)

# Create binary from object files
$(app): $(obj)
	$(CC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

# Create build folder if it doesn't exist
build:
	mkdir $@

# Create object files
build/%.cpp.o: src/%.cpp build
	$(CC) $(CPPFLAGS) -c -o $@ $<

# Clean up
.PHONY: clean
clean:
	rm -rf $(app)
	rm -rf build
