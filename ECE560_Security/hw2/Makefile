  
TARGETS=duke-crypter
all: $(TARGETS)
clean:
	rm -f $(TARGETS) *~

duke-crypter: duke-crypter.cpp
	g++  -g -o $@ $< -L/usr/local/lib/ -lcryptopp