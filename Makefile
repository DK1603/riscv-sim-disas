CXX=g++
CXXFLAGS=

SRCS=main.cc
TARGET=riscv-sim
OBJS := $(patsubst %.cc,%.o,$(SRCS))
all: $(TARGET)
%.o:%.cc
	$(CXX) $(CXXFLAGS) $< -c -o $@
$(TARGET): $(OBJS)
		   $(CXX) $(CXXFLAGS) $^ -o $@
.PHONY=clean
clean:
			rm -f $(OBJS) $(TARGET)