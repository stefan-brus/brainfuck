/**
 * Brainfuck
 *
 * TODO:
 *      - Make loop stack an actual stack?
 *      - Add line and column to error messages
 *      - Handle files
 */

#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Input buffer size
 */

static const int INPUT_BUF_SIZE = 0xFF;

/**
 * Interpreter memory size
 */

static const int BRAINFUCK_CELLS = 0xFFFF;

/**
 * Loop stack size
 */

static const int LOOP_STACK_SIZE = 0xFF;

/**
 * Find the next ']' in the program, starting with the given index
 *
 * ARGS:
 *      program = The program string
 *      idx = The index
 *
 * RETURNS:
 *      the index of the matched ']', or -1 on error
 */

static int bf_find_loop_end(const char* program, int idx)
{
    int i = 0;

    check(idx >= 0 && idx < INPUT_BUF_SIZE, "Index out of bounds");

    for(i = idx; program[i] != '\0'; i++)
    {
        check(i < INPUT_BUF_SIZE, "Index out of bounds");
        if(program[i] == ']')
        {
            return i;
        }
    }

    return -1;

error:
    return -1;
}

/**
 * Execute a brainfuck program on the given memory block
 *
 * ARGS:
 *      program = The program string
 *      mem = The memory block
 *      ptr = Pointer to the current brainfuck pointer
 *
 * RETURNS:
 *      the number of instructions processed, or -1 on error
 */

static int bf_run(const char* program, char* mem, int* ptr)
{
    int i = 0;
    int rc = 0;
    int loop_stack[LOOP_STACK_SIZE];
    int loop_ind = -1;

    check(program, "No program to run");
    check(mem, "No memory block to run on");

    memset(loop_stack, -1, LOOP_STACK_SIZE);

    for(i = 0; program[i] != '\0'; i++)
    {
        check(i < INPUT_BUF_SIZE, "Index out of bounds");
        switch(program[i])
        {
            case '>':
                check(*ptr < BRAINFUCK_CELLS, "Out of brain cells");
                (*ptr)++;
                break;

            case '<':
                check(*ptr >= 0, "Can't go into negative brain cells");
                (*ptr)--;
                break;

            case '+':
                mem[*ptr]++;
                break;

            case '-':
                mem[*ptr]--;
                break;

            case '.':
                rc = fputc(mem[*ptr], stdout);
                check(rc != EOF, "Error printing value at %d", *ptr);
                break;

            case ',':
                rc = fgetc(stdin);
                check(rc != EOF, "Error getting user input");
                mem[*ptr] = rc;
                break;

            case '[':
                check(loop_ind < LOOP_STACK_SIZE, "Loop stack overflow");
                loop_stack[++loop_ind] = i;
                if(mem[*ptr] == 0)
                {
                    i = bf_find_loop_end(program, loop_ind >= 0 ?
                                                  loop_stack[loop_ind] : i);
                    check(i != -1, "Unmatched '['");
                }
                break;

            case ']':
                check(loop_ind >= 0, "Unmatched ']'");
                if(mem[*ptr] != 0)
                {
                    check(loop_stack[loop_ind] >= 0 &&
                          loop_stack[loop_ind] < INPUT_BUF_SIZE,
                          "Invalid loop index");
                    i = loop_stack[loop_ind];
                }
                else
                {
                    loop_stack[loop_ind--] = -1;
                }
                break;

            // whitespace
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;

            default:
                sentinel("Invalid character");
        }
    }

    return i;

error:
    return -1;
}

int main(int argc, char* argv[])
{
    char* buf = calloc(INPUT_BUF_SIZE, sizeof(char));
    char* mem = calloc(BRAINFUCK_CELLS, sizeof(char));
    int ptr = 0;
    int rc = 0;

    while((buf = fgets(buf, INPUT_BUF_SIZE, stdin)))
    {
        check(buf, "Input error");
        rc = bf_run(buf, mem, &ptr);
        check(rc >= 0, "Brainfuck error");
    }

    free(buf);
    free(mem);

    return 0;

error:
    free(buf);
    free(mem);

    return 1;
}
