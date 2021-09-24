//
// Created by tumbar on 9/23/21.
//

#include "usr.h"
#include <ctype.h>
#include <uart.h>
#include <string.h>


static char*
read_into_buffer(char buf[], U32 n)
{
    if (USART2->ISR & USART_ISR_RXNE)
    {
        // Character found from user input
        U32 l = strlen(buf);

        if (l + 1 >= n)
        {
            // No more space available
            buf[l] = 0;
            return buf;
        }

        char c = (char)(USART2->RDR & 0xFF);

        // Allow the user to view what they are typing
        USART_Write(USART2, (U8*) &c, 1);

        if (c == '\r')
        {
            char lf = '\n';
            USART_Write(USART2, (U8*) &lf, 1);

            // End of line
            return buf;
        }
        else if (c == 0x7F)
        {
            if (l > 0)
            {
                // Delete a character
                buf[l - 1] = 0;

                // Delete a character on the screen
                const char* s = "\b \b";
                USART_Write(USART2, (const U8*)s, 3);
            }
        }
        else
        {
            buf[l] = c;
            buf[l + 1] = 0; // null terminator
        }

        // String not finished
        return NULL;
    }
    else
    {
        // No new character available
        return NULL;
    }
}

void user_task(Sequence* engines[2])
{
    static char user_input_buf[32] = {0};
    char* user_input = read_into_buffer(user_input_buf, sizeof(user_input_buf));

    // Check if user input available
    if (user_input)
    {
        // Check if the command has 2 characters
        if (strlen(user_input) == 2)
        {
            // Handle each command for either motor
            for (U8 i = 0; i < 2; i++)
            {
                // Case-insensitive commanding
                user_input[i] = tolower(user_input[i]);

                switch(user_input[i])
                {
                    case 'p':
                        // Pause not available during these states
                        if (!(engines[i]->status == SEQ_STATUS_FINISHED ||
                              engines[i]->status == SEQ_STATUS_COMMAND_ERR ||
                              engines[i]->status == SEQ_STATUS_NESTED_LOOP_ERR))
                        {
                            // Pause the sequence engine execution
                            engines[i]->status = SEQ_STATUS_PAUSED;
                        }
                        break;
                    case 'c':
                        if (!(engines[i]->status == SEQ_STATUS_FINISHED ||
                              engines[i]->status == SEQ_STATUS_COMMAND_ERR ||
                              engines[i]->status == SEQ_STATUS_NESTED_LOOP_ERR))
                        {
                            // Pause the sequence engine execution
                            engines[i]->status = SEQ_STATUS_RUNNING;
                        }
                        break;
                    case 'r':
                        if (!(engines[i]->status == SEQ_STATUS_COMMAND_ERR ||
                              engines[i]->status == SEQ_STATUS_NESTED_LOOP_ERR))
                        {
                            // Make sure we can move to the right
                            if (mot_get_position(i) - 1 >= 0)
                            {
                                mot_set_position(i, mot_get_position(i) - 1);
                            }
                            else
                            {
                                uprintf("At motor %d right limit\r\n", i + 1);
                            }
                        }
                        break;
                    case 'l':
                        if (!(engines[i]->status == SEQ_STATUS_COMMAND_ERR ||
                              engines[i]->status == SEQ_STATUS_NESTED_LOOP_ERR))
                        {
                            // Make sure we can move to the left
                            if (mot_get_position(i) + 1 <= 5)
                            {
                                mot_set_position(i, mot_get_position(i) + 1);
                            }
                            else
                            {
                                uprintf("At motor %d left limit\r\n", i + 1);
                            }
                        }
                        break;
                    case 'n':
                        // nop
                        break;
                    case 's':
                        uprintf("Motor %d: %d\r\n", i + 1, mot_get_position(i));
                        break;
                    case 'b':
                        // Starts/restarts the recipe immediately
                        engines[i]->pc = 0;
                        engines[i]->status = SEQ_STATUS_RUNNING;
                        engines[i]->wait_flag = 0; // clear any pending waits
                        engines[i]->ls_idx = 0; // clear any pending loops
                        break;
                    default:
                        uprintf("Invalid user input command '%c' on motor %d\r\n",
                                user_input[i], i + 1);
                        break;
                }
            }
        }
        else
        {
            uprintf("\r\nExpected 2 characters\r\n");
        }

        // Clear the input
        // Re-print the prompt
        user_input[0] = 0;
        uprintf("Enter command: ");
    }
}
