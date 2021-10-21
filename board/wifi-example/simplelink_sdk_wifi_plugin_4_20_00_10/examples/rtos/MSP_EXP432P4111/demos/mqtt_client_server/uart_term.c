/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *
 *  Terminal
 */

// Standard includes
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "uart_term.h"

extern int vsnprintf(char * s,
                     size_t n,
                     const char * format,
                     va_list arg);

//*****************************************************************************
//                          LOCAL DEFINES
//*****************************************************************************
#define IS_SPACE(x)       (x == 32 ? 1 : 0)

//*****************************************************************************
//                 GLOBAL VARIABLES
//*****************************************************************************
static UART_Handle uartHandle;

//*****************************************************************************
//
//! Initialization
//!
//! This function
//!        1. Configures the UART to be used.
//!
//! \param  none
//!
//! \return none
//
//*****************************************************************************
UART_Handle InitTerm(void)
{
    UART_Params uartParams;

    UART_init();
    UART_Params_init(&uartParams);

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uartHandle = UART_open(Board_UART0, &uartParams);
    if(uartHandle == NULL)
    {
        // UART_open failed
        while(1)
        {
            ;
        }
    }

    /* remove uart receive from LPDS dependency */
    UART_control(uartHandle, UART_CMD_RXDISABLE, NULL);

    return(uartHandle);
}

//*****************************************************************************
//
//! prints the formatted string on to the console
//!
//! \param[in]  format  - is a pointer to the character string specifying the
//!                       format in the following arguments need to be
//!                       interpreted.
//! \param[in]  [variable number of] arguments according to the format in the
//!             first parameters
//!
//! \return count of characters printed
//
//*****************************************************************************
int Report(const char *pcFormat,
           ...)
{
    int iRet = 0;
    char        *pcBuff;
    char        *pcTemp;
    int iSize = 256;
    va_list list;

    pcBuff = (char*)malloc(iSize);
    if(pcBuff == NULL)
    {
        return(-1);
    }
    while(1)
    {
        va_start(list,pcFormat);
        iRet = vsnprintf(pcBuff, iSize, pcFormat, list);
        va_end(list);
        if((iRet > -1) && (iRet < iSize))
        {
            break;
        }
        else
        {
            iSize *= 2;
            if((pcTemp = realloc(pcBuff, iSize)) == NULL)
            {
                Message("Could not reallocate memory\n\r");
                iRet = -1;
                break;
            }
            else
            {
                pcBuff = pcTemp;
            }
        }
    }
    Message(pcBuff);
    free(pcBuff);

    return(iRet);
}

//*****************************************************************************
//
//! Trim the spaces from left and right end of given string
//!
//! \param  pcInput - string on which trimming happens
//!
//! \return length of trimmed string
//
//*****************************************************************************
int TrimSpace(char * pcInput)
{
    size_t size;
    char        *endStr;
    char        *strData = pcInput;
    char index = 0;

    size = strlen(strData);

    if(!size)
    {
        return(0);
    }

    endStr = strData + size - 1;
    while((endStr >= strData) && (IS_SPACE(*endStr)))
    {
        endStr--;
    }
    *(endStr + 1) = '\0';

    while(*strData && IS_SPACE(*strData))
    {
        strData++;
        index++;
    }
    memmove(pcInput, strData, strlen(strData) + 1);

    return(strlen(pcInput));
}

//*****************************************************************************
//
//! Get the Command string from UART
//!
//! \param[in]  pucBuffer   - is the command store to which command will be
//!                           populated
//! \param[in]  ucBufLen    - is the length of buffer store available
//!
//! \return Length of the bytes received. -1 if buffer length exceeded.
//!
//*****************************************************************************
int GetCmd(char *pcBuffer,
           unsigned int uiBufLen)
{
    char cChar;
    int iLen = 0;

#ifdef UART_NONPOLLING
    UART_read(uartHandle, &cChar, 1);
#else
    UART_readPolling(uartHandle, &cChar, 1);
#endif

    iLen = 0;

    //
    // Checking the end of Command
    //
    while(1)
    {
        //
        // Handling overflow of buffer
        //
        if(iLen >= uiBufLen)
        {
            return(-1);
        }

        //
        // Copying Data from UART into a buffer
        //
        if((cChar == '\r') || (cChar == '\n'))
        {
#ifdef UART_NONPOLLING
            UART_write(uartHandle, &cChar, 1);
#else
            UART_writePolling(uartHandle, &cChar, 1);
#endif
            break;
        }
        else if(cChar == '\b')
        {
            //
            // Deleting last character when you hit backspace
            //
            char ch;

#ifdef UART_NONPOLLING
            UART_write(uartHandle, &cChar, 1);
#else
            UART_writePolling(uartHandle, &cChar, 1);
#endif
            ch = ' ';
#ifdef UART_NONPOLLING
            UART_write(uartHandle, &ch, 1);
#else
            UART_writePolling(uartHandle, &ch, 1);
#endif
            if(iLen)
            {
#ifdef UART_NONPOLLING
                UART_write(uartHandle, &cChar, 1);
#else
                UART_writePolling(uartHandle, &cChar, 1);
#endif
                iLen--;
            }
            else
            {
                ch = '\a';
#ifdef UART_NONPOLLING
                UART_write(uartHandle, &ch, 1);
#else
                UART_writePolling(uartHandle, &ch, 1);
#endif
            }
        }
        else
        {
            //
            // Echo the received character
            //
#ifdef UART_NONPOLLING
            UART_write(uartHandle, &cChar, 1);
#else
            UART_writePolling(uartHandle, &cChar, 1);
#endif

            *(pcBuffer + iLen) = cChar;
            iLen++;
        }
#ifdef UART_NONPOLLING
        UART_read(uartHandle, &cChar, 1);
#else
        UART_readPolling(uartHandle, &cChar, 1);
#endif
    }

    *(pcBuffer + iLen) = '\0';

    return(iLen);
}

//*****************************************************************************
//
//! Outputs a character string to the console
//!
//! This function
//!        1. prints the input string character by character on to the console.
//!
//! \param[in]  str - is the pointer to the string to be printed
//!
//! \return none
//!
//! \note If UART_NONPOLLING defined in than Message or UART write should be
//!       called in task/thread context only.
//
//*****************************************************************************
void Message(const char *str)
{
#ifdef UART_NONPOLLING
    UART_write(uartHandle, str, strlen(str));
#else
    UART_writePolling(uartHandle, str, strlen(str));
#endif
}

//*****************************************************************************
//
//! Clear the console window
//!
//! This function
//!        1. clears the console window.
//!
//! \param  none
//!
//! \return none
//
//*****************************************************************************
void ClearTerm()
{
    Message("\33[2J\r");
}

//*****************************************************************************
//
//! Read a character from the console
//!
//! \param none
//!
//! \return Character
//
//*****************************************************************************
char getch(void)
{
    char ch;

#ifdef UART_NONPOLLING
    UART_read(uartHandle, &ch, 1);
#else
    UART_readPolling(uartHandle, &ch, 1);
#endif
    return(ch);
}

//*****************************************************************************
//
//! Outputs a character to the console
//!
//! \param[in]  char    - A character to be printed
//!
//! \return none
//
//*****************************************************************************
void putch(char ch)
{
#ifdef UART_NONPOLLING
    UART_write(uartHandle, &ch, 1);
#else
    UART_writePolling(uartHandle, &ch, 1);
#endif
}
