TARGET := ctrl
INCLUDES := -I. -Iinclude -I../common/include
SRCS = ../common/exchange.c ../common/tools.c ../common/helper.c kernel.c main.c
CC := gcc
OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^
	rm -rf $(SRCS:.c=.o)
%.o:%.c
	$(CC) $(INCLUDES) -o $@ -c $<

install:
	cp $(TARGET) ..

clean:
	rm -rf $(basename $(TARGET)) $(SRCS:.c=.o)