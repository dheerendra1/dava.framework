/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky
=====================================================================================*/
#include <stdarg.h>
#include <stdio.h>
#include "Utils/StringFormat.h"
#include "Debug/DVAssert.h"

namespace DAVA
{


static const int32 FORMAT_STRING_SIZE = 2048;
static const int32 FORMAT_STRING_MAX_LEN = 256;

static char8 formatString8[FORMAT_STRING_SIZE];
static char16 formatString16[FORMAT_STRING_SIZE];
static int32 formatString8Position = 0;
static int32 formatString16Position = 0;

//! formatting function (use printf syntax)
const char8 * Format(const char8 * text, ...)
{
	if (formatString8Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString8Position = 0;
	}
	va_list ll;

	va_start(ll, text);
	int32 len = vsnprintf(&formatString8[formatString8Position],  FORMAT_STRING_MAX_LEN, text, ll);
    DVASSERT(len < FORMAT_STRING_MAX_LEN);
    
	formatString8Position += (len + 1);

	va_end(ll);

	return &formatString8[formatString8Position  - (len + 1)];
}
	
const char8 * GetIndentString(char8 indentChar, int32 level)
{
	if (formatString8Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString8Position = 0;
	}
	if (level > FORMAT_STRING_MAX_LEN)level = FORMAT_STRING_MAX_LEN - 1;
	
	for (int k = 0; k < level; k++)
	{
		formatString8[formatString8Position + k] = indentChar;
	}
	formatString8[formatString8Position + level] = 0;
	formatString8Position += (level + 1);
	return &formatString8[formatString8Position  - (level + 1)];
}
	
	
#if defined(__DAVAENGINE_ANDROID__)
	
#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */
    
    
#define do_div(n,base) ({ \
int32 __res; \
__res = ((unsigned long long) n) % (unsigned) base; \
n = ((unsigned long long) n) / (unsigned) base; \
__res; })
    
    
static int32 skip_atoi_android(const char16 **s)
{
    int32 i=0;
    
    while (iswdigit(**s))
    {
        i = i*10 + *((*s)++) - L'0';
    }
    
    return i;
}


static char16 * number_android (char16 *str, int64 num, int32 base, int32 size, int32 precision, int32 type)
{
    const char16 *digits = L"0123456789abcdefghijklmnopqrstuvwxyz";
    if (type & LARGE)
        digits = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    
    char16 c = (type & ZEROPAD) ? L'0' : L' ';
    char16 sign = 0;
    
    if (type & SIGN) 
    {
        if (num < 0) 
        {
            sign = L'-';
            num = -num;
            --size;
        } 
        else if (type & PLUS) 
        {
            sign = L'+';
            --size;
        } 
        else if (type & SPACE) 
        {
            sign = L' ';
            --size;
        }
    }
    
    if (type & SPECIAL) 
    {
        if (base == 16)
        {
            size -= 2;
        }
        else if (base == 8)
        {
            --size;
        }
    }
    
    char16 tmp[66];
    int32 i = 0;
    if (num == 0)
    {
        tmp[i++]='0';
    }
    else 
    {
        while (num != 0)
        {
            tmp[i++] = digits[do_div(num,base)];
        }
    }
    
    if (i > precision)
    {
        precision = i;
    }
    
    size -= precision;
    if (!(type & (ZEROPAD+LEFT)))
    {
        while(size-- > 0)
        {
            *str++ = L' ';
        }
    }
    
    if (sign)
    {
        *str++ = sign;
    }
    
    if (type & SPECIAL)
    {
        if (base==8)
        {
            *str++ = L'0';
        }
        else if (base==16) 
        {
            *str++ = L'0';
            *str++ = digits[33];
        }
    }
    
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            *str++ = c;
        }
    }
    
    while (i < precision--)
    {
        *str++ = '0';
    }
    
    while (i-- > 0)
    {
        *str++ = tmp[i];
    }
    
    while (size-- > 0)
    {
        *str++ = L' ';
    }
    
    return str;
}

static char16 * numberf_android (char16 *str, float64 num, int32 base, int32 size, int32 precision, int32 type)
{
    int32 whole = (int32)num;
    
    num -= whole;
    
    for(int32 i = 0; i < precision; ++i)
        {
        num *= 10;
    }
    
    int32 tail = (int32)num;
    
    type = SIGN | LEFT;
    char16 *firstStr = number_android(str, whole, 10, -1, -1, type);
    if(tail)
    {
        *firstStr++ = '.';
    
        type = LEFT;
        firstStr = number_android(firstStr, tail, 10, -1, -1, type);
    }
    
    return firstStr;
}



int32 _vsnwprintf_android(char16 *buf, size_t cnt, const char16 *fmt, va_list args)
{
    int32 len;
    uint64 num;
    int32 i, base;
    const char8 *s;
    const char16 *sw;
    
    int32 flags = 0;		/* flags to number() */
    
    int32 field_width = 0;	/* width of output field */
    int32 precision = 0;		/* min. # of digits for integers; max
                         number of chars for from string */
    int32 qualifier = 0;		/* 'h', 'l', 'L', 'w' or 'I' for integer fields */
    
    char16 * str = NULL;
    for (str=buf ; *fmt ; ++fmt) 
    {
        if (*fmt != L'%') 
        {
            *str++ = *fmt;
            continue;
        }
        
        /* process flags */
        const char16 *floatFormatPointer = fmt;
        ++floatFormatPointer;

        flags = 0;
    repeat:
        ++fmt;		/* this also skips first '%' */
        
        switch (*fmt) 
        {
            case L'-': flags |= LEFT; goto repeat;
            case L'+': flags |= PLUS; goto repeat;
            case L' ': flags |= SPACE; goto repeat;
            case L'#': flags |= SPECIAL; goto repeat;
            case L'0': flags |= ZEROPAD; goto repeat;
        }
        
        /* get field width */
        field_width = -1;
        if (iswdigit(*fmt))
        {
            field_width = skip_atoi_android(&fmt);
        }
        else if (*fmt == L'*') 
        {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int32);
            if (field_width < 0) 
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }
        
        /* get the precision */
        precision = -1;
        if (*fmt == L'.') 
        {
            ++fmt;	
            if (iswdigit(*fmt))
            {
                precision = skip_atoi_android(&fmt);
            }
            else if (*fmt == L'*') 
            {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, int32);
            }
            
            if (precision < 0)
            {
                precision = 0;
        }
        }
        
        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'w') 
        {
            qualifier = *fmt;
            ++fmt;
        } 
        else if (*fmt == 'I' && *(fmt+1) == '6' && *(fmt+2) == '4') 
        {
            qualifier = *fmt;
            fmt += 3;
        }
        
        /* default base */
        base = 10;
        
        switch (*fmt) 
        {
            case L'c':
                if (!(flags & LEFT))
                {
                    while (--field_width > 0)
                    {
                        *str++ = L' ';
                    }
                }
                
                if (qualifier == 'h')
                {
                    *str++ = (char16) va_arg(args, int32);
                }
                else
                {
                    *str++ = (char16) va_arg(args, int32);
                }
                
                while (--field_width > 0)
                {
                    *str++ = L' ';
                }
                
                continue;
                
            case L'C':
                if (!(flags & LEFT))
                {
                    while (--field_width > 0)
                    {
                        *str++ = L' ';
                    }
                }
                
                if (qualifier == 'l' || qualifier == 'w')
                {
                    *str++ = (char16) va_arg(args, int32);
                }
                else
                {
                    *str++ = (char16) va_arg(args, int32);
                }
                
                while (--field_width > 0)
                {
                    *str++ = L' ';
                }
                
                continue;
                
            case L's':
                if (qualifier == 'h') 
                {
                    /* print ascii string */
                    s = va_arg(args, char8 *);
                    if (s == NULL)
                    {
                        s = "<NULL>";
                    }
                    
                    len = strlen (s);
                    if ((uint32)len > (uint32)precision)
                    {
                        len = precision;
                    }
                    
                    if (!(flags & LEFT))
                    {
                        while (len < field_width--)
                        {
                            *str++ = L' ';
                        }
                    }
                    
                    for (i = 0; i < len; ++i)
                    {
                        *str++ = (char16)(*s++);
                    }
                    
                    while (len < field_width--)
                    {
                        *str++ = L' ';
                    }
                } 
                else 
                {
                    /* print unicode string */
                    sw = va_arg(args, char16 *);
                    if (sw == NULL)
                    {
                        sw = L"<NULL>";
                    }
                    
                    len = wcslen (sw);
                    if ((uint32)len > (uint32)precision)
                    {
                        len = precision;
                    }
                    
                    if (!(flags & LEFT))
                    {
                        while (len < field_width--)
                        {
                            *str++ = L' ';
                        }
                    }
                    
                    for (i = 0; i < len; ++i)
                    {
                        *str++ = *sw++;
                    }
                    
                    while (len < field_width--)
                    {
                        *str++ = L' ';
                }
                }
                continue;
                
            case L'S':
                if (qualifier == 'l' || qualifier == 'w') 
                {
                    /* print unicode string */
                    sw = va_arg(args, char16 *);
                    if (sw == NULL)
                    {
                        sw = L"<NULL>";
                    }
                    
                    len = wcslen (sw);
                    if ((uint32)len > (uint32)precision)
                    {
                        len = precision;
                    }
                    
                    if (!(flags & LEFT))
                    {
                        while (len < field_width--)
                        {
                            *str++ = L' ';
                        }
                    }
                    
                    for (i = 0; i < len; ++i)
                    {
                        *str++ = *sw++;
                    }
                    
                    while (len < field_width--)
                    {
                        *str++ = L' ';
                    }
                } 
                else 
                {
                    /* print ascii string */
                    s = va_arg(args, char8 *);
                    if (s == NULL)
                    {
                        s = "<NULL>";
                    }
                    
                    len = strlen (s);
                    if ((uint32)len > (uint32)precision)
                    {
                        len = precision;
                    }
                    
                    if (!(flags & LEFT))
                    {
                        while (len < field_width--)
                        {
                            *str++ = L' ';
                        }
                    }
                    
                    for (i = 0; i < len; ++i)
                    {
                        *str++ = (char16)(*s++);
                    }
                    
                    while (len < field_width--)
                    {
                        *str++ = L' ';
                }
                }
                continue;
                
            case L'Z':
                if (qualifier == 'h') 
                {
                    /* print counted ascii string */
                    char8 * pus = va_arg(args, char8 *);
                    if ((pus == NULL)) 
                    {
                        sw = L"<NULL>";
                        while ((*sw) != 0)
                        {
                            *str++ = *sw++;
                        }
                    }
                    else 
                    {
                        for (i = 0; pus[i] && i < strlen(pus); i++)
                        {
                            *str++ = (char16)(pus[i]);
                        }
                    }
                } 
                else 
                {
                    /* print counted unicode string */
                    char16* pus = va_arg(args, char16*);
                    if ((pus == NULL)) 
                    {
                        sw = L"<NULL>";
                        while ((*sw) != 0)
                        {
                            *str++ = *sw++;
                        }
                    } 
                    else 
                    {
                        for (i = 0; pus[i] && i < wcslen(pus); ++i) // / sizeof(WCHAR); i++)
                        {
                            *str++ = pus[i];
                    }
                }
                }
                continue;
                
            case L'p':
                if (field_width == -1) 
                {
                    field_width = 2*sizeof(void *);
                    flags |= ZEROPAD;
                }
                
                str = number_android(str,
                             (unsigned long) va_arg(args, void *), 16,
                             field_width, precision, flags);
                continue;
                
            case L'n':
                if (qualifier == 'l') 
                {
                    long * ip = va_arg(args, long *);
                    *ip = (str - buf);
                } 
                else 
                {
                    int32 * ip = va_arg(args, int32 *);
                    *ip = (str - buf);
                }
                continue;
                
                /* integer number formats - set up the flags and "break" */
            case L'o':
                base = 8;
                break;
                
            case L'b':
                base = 2;
                break;
                
            case L'X':
                flags |= LARGE;
            case L'x':
                base = 16;
                break;
                
            case L'd':
            case L'i':
                flags |= SIGN;
            case L'u':
                break;
                
            case L'f':
                qualifier = 'f';
                flags |= SIGN;
                break;
                
            default:
                if (*fmt != L'%')
                {
                    *str++ = L'%';
                }
                if (*fmt)
                {
                    *str++ = *fmt;
                }
                else
                {
                    --fmt;
                }
                continue;
        }
        
        
        if(qualifier == 'f')
        {
            float64 floatValue = va_arg(args, float64);

            precision = 0;
            if(*floatFormatPointer != 'f')
            {
                bool wasPoint = false;
                while(*floatFormatPointer != 'f')
                {
                    if('.' == *floatFormatPointer)
                    {
                        wasPoint = true;
                    }
                    else
                    {
                        if(wasPoint)
                        {
                            precision = (precision * 10) + (*floatFormatPointer) - '0';
                        }
                    }
                    ++floatFormatPointer;
                }
                
                if(!wasPoint)
                {
                    precision = 6;
                }
            }
            else
            {
                precision = 6;
            }
            
            str = numberf_android(str, floatValue, base, field_width, precision, flags);
        }
        else
        {
        if (qualifier == 'I')
            {
                num = va_arg(args, uint64);   
            }
        else if (qualifier == 'l')
            {
            num = va_arg(args, unsigned long);
            }
            else if (qualifier == 'h') 
            {
            if (flags & SIGN)
                {
                    num = va_arg(args, int32);
                }
            else
                {
                    num = va_arg(args, uint32);
                }
        }
            else 
            {
            if (flags & SIGN)
                {
                    num = va_arg(args, int32);
                }
            else
                {
                    num = va_arg(args, uint32);
                }
        }
        str = number_android(str, num, base, field_width, precision, flags);
    }
    }
    
    *str = L'\0';
    return str-buf;
}

#endif //#if defined(__DAVAENDGINE_ANDROID__)


//! formatting function (use printf syntax)
const char16 * Format(const char16 * text, ...)
{
	if (formatString16Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString16Position = 0;
	}
	va_list ll;

	va_start(ll, text);

#if defined(_WIN32)
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], (wchar_t *)text, ll);
#elif defined (__DAVAENGINE_ANDROID__)
    int32 len = _vsnwprintf_android((char16 *)&formatString16[formatString16Position], FORMAT_STRING_MAX_LEN, (char16 *)text, ll);
#else // MAC_OS & other nix systems
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], FORMAT_STRING_MAX_LEN, (wchar_t *)text, ll);
#endif
    DVASSERT(len < FORMAT_STRING_MAX_LEN);
	
	formatString16Position += (len + 1);
	va_end(ll);
	return &formatString16[formatString16Position - (len + 1)];
}

const char8 * FormatVL(const char8 * text, va_list ll)
{
	if (formatString8Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString8Position = 0;
	}
	int32 len = vsprintf(&formatString8[formatString8Position],  text, ll);
    DVASSERT(len < FORMAT_STRING_MAX_LEN);

	formatString8Position += (len + 1);

	return &formatString8[formatString8Position  - (len + 1)];
}

const char16 * FormatVL(const char16 * text, va_list ll)
{
	if (formatString16Position + FORMAT_STRING_MAX_LEN >= FORMAT_STRING_SIZE)
	{
		formatString16Position = 0;
	}
#if defined(_WIN32)
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], (wchar_t *)text, ll);
#else // MAC_OS & other nix systems
	int32 len = vswprintf((wchar_t *)&formatString16[formatString16Position], FORMAT_STRING_MAX_LEN, (wchar_t *)text, ll);
#endif
	DVASSERT(len < FORMAT_STRING_MAX_LEN);

	formatString16Position += (len + 1);
	return &formatString16[formatString16Position - (len + 1)];
}
	
	

}; // end of namespace Log
