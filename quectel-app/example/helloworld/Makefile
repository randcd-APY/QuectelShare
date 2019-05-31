TARGET=helloworld  
LDFLAGS= -lpthread   
 
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all: $(TARGET)

$(TARGET): $(CUR_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(CUR_OBJS) $(TARGET)