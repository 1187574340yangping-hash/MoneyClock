CC      = gcc
CFLAGS  = -O2
TARGET  = MoneyClock.exe
SRC     = moneyclock.c

all: $(TARGET)

$(TARGET): $(SRC) index_html.h
	$(CC) $(CFLAGS) -o $@ $(SRC) -lshlwapi

index_html.h: index.html build_gen.py
	python build_gen.py index.html index_html.h

clean:
	rm -f $(TARGET) index_html.h *.o
