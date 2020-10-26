VFLAGS=-fno-stack-protector -z execstack
# ^ turn of W^X protections

BIN=vuln1 attack.bin
# ^ binaries to build. note:
#   because this Makefile can convert *any* asm file to a corresponding bin file,
#   you can just add additional bin entries here and build them from asm

all: $(BIN)
clean:
	rm $(BIN)

vuln1: vuln1.c
	gcc -g $(VFLAGS) -o $@ $<

# convert *any* asm file to a corresponding bin file
#   (info here if curious: https://www.gnu.org/software/make/manual/html_node/Suffix-Rules.html )
.SUFFIXES: .asm .bin
.asm.bin: 
	nasm -o $@ $<

