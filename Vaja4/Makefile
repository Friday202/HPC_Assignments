# Compiler
CC = nvcc

# Compiler flags
INCLUDES = -I/d/hpc/software/openmpi-4.1.3/build/include 
LDFLAGS = -L/d/hpc/software/openmpi-4.1.3/build/lib/
LDLIBS = -lmpi -lm
CFLAGS = -O3 -Xcompiler --openmp,-Wall

SRC=src

# Source files
SRCS=$(wildcard $(SRC)/*.cu)
SRCS+=$(wildcard $(SRC)/*.c)

# Header files
HDRS = $(wildcard $(SRC)/*.h)

# Object files
OBJS_1 = $(SRCS:$(SRC)/%.c=%.o)
OBJS = $(OBJS_1:$(SRC)/%.cu=%.o)

# Executable name
TARGET = lenia.out

# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

# Compile source files to object files
%.o: $(SRC)/%.cu $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: $(SRC)/%.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
# Clean
clean:
	rm -f $(OBJS) $(TARGET)