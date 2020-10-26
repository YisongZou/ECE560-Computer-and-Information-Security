// Sample vulnerable program
//  By Dr. Tyler Bletsch (Tyler.Bletsch@duke.edu) for Computer and Information Security at Duke

#include <stdlib.h>
#include <stdio.h>

// We use a struct to ensure that the function pointer comes right after the buffer
// Also, we put it in global memory so the location is highly deterministic, 
// as even with ASLR turned off, the circumstances of launch can cause the stack to start at slightly different addresses.
//   (To learn more about how the stack can shift around even with ASLR off, see: http://stackoverflow.com/questions/17775186/buffer-overflow-works-in-gdb-but-not-without-it/17775966)
struct {
    char name[256];
    void (*func_ptr)();
} stuff;

// The intended destination of the function pointer, a simple function that just prints "Bye!"
void say_bye() {
    puts("Bye!");
}

int main() {
    stuff.func_ptr = say_bye; // set the function pointer to the say_bye function
    
    // Tell the world way too much info about yourself
    printf("Hi. I like to store your name at address %p, and I store the function pointer of what to do next at %p.\n", &stuff.name, &stuff.func_ptr);

    // Prompt for name
    printf("Anyway, what is your name? ");
    fgets(stuff.name,15,stdin); // << hey i'm a big dumb idiot who uses gets() in the year 2018.
    //Remove the new line got from fgets
    strtok(stuff.name, "\n");
    // Indicate that the user is cool.
    printf("%s is cool.\n", stuff.name);
    
    // Call function pointer
    stuff.func_ptr();
    
    // Exit with code 0
    return 0;
}
