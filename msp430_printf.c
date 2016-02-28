#include "msp430_printf.h"
#include "msp430_uart.h"


int msp430_fputc(unsigned char character, FILE * stream )
{
    if (character == '\r'){

             unsigned char tmp = '\n';
            UARTSendArray (&tmp, 1);  
            tmp = '\r' ;
            UARTSendArray (&tmp, 1);  
             return 0 ;
    }
    
     UARTSendArray (&character,1);   
    return 0;
}
#include "stdbool.h"

void reverse(char* str , int len)
{
    char* i = str ;
    char* j = str +len- 1;
    while( i < j ){
    char tmp = * i;
    *i = * j;
    *j = tmp;
        i ++;
        j --;
    }
}
char* itoa(int num, char* str , int base)
{
    int i = 0;
    bool isNegative = false;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0 )
    {
        str [i++] = '0';
        str [i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = - num;
    }
 
    // Process individual digits
    while (num != 0 )
    {
        int rem = num % base ;
        str [i++] = (rem > 9 )? ( rem-10 ) + 'a' : rem + '0';
        num = num/ base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str [i++] = '-';
 
    str[i ] = '\0' ; // Append string terminator
 
    // Reverse the string
    reverse(str , i);
 
    return str ;
}

int msp430_vprintf(FILE *stream ,const char* format, va_list vlist )
{
    int done = 0 ;
    while(*format != '\0')
    {
         if (* format == '%'){
            format ++;
             switch (* format)
             {
                 case 'd' :
                 case 'x' :
                 {
                  int base =   (*format == 'd' ) ? 10 :16;
                  format ++;
                  int iTemp = 0;
                  iTemp = va_arg(vlist, int);
                  char acTemp[256] = "" ;
                 
                  itoa (iTemp, acTemp, base );
                  int i;
                  for(i = 0 ; i < 256 ; i++)
                  {
                     if(acTemp [i] == '\0') break;
                    msp430_fputc (acTemp[ i],0);
                    done ++;
                  }
                 }
                 break;
                 case 'c' :
                 {
                    format ++;
                     int iTemp = 0;
                    iTemp = va_arg(vlist, int);
                    msp430_fputc (iTemp, 0);
                    done ++;
                 }
                 break;
                 case 's' :
                 {
                    format ++;
                     char* sTemp ;
                    sTemp = va_arg(vlist, char*);
                     while(*sTemp != '\0' )
                     {
                        msp430_fputc (*sTemp, 0 );
                        done ++;
                        sTemp ++;
                     }
                 }
                 break;
                 default:
                  msp430_fputc ('%', 0);
                  done ++;
                 break;
             }       
         }
        msp430_fputc (*format, 0);
        format ++;
        done ++;
    }
    return done ;
}

int printf(const char * format, ...)
{
    int done;
    va_list arg ;
    va_start(arg , format);
        done = msp430_vprintf(0,format ,arg);
    va_end(arg );
    return done ;
}
