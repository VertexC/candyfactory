# Make file
# final targer
TARGET = candykids
# .o file to link in (for all .c files)
OBJS = candykids.o bbuff.o stats.o

# Add support for pthread and real-time clock
CFLAGS = -pthread -lrt -Wall -Werror -g -std=c99 -D_POSIX_C_SOURCE=199309L

CC_C = gcc

all:clean $(TARGET)

%.o : %.c
	$(CC) -c $(CFLAGS) $<

$(TARGET):$(OBJS)
	$(CC_C) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)


