/*
 * ANSI C89 fallback for platforms whose runtime lacks vsnprintf.
 * Trio is embedded in this translation unit in its snprintf-only mode.
 */
#ifndef OPENDOORS_HAVE_VSNPRINTF

#define TRIO_SNPRINTF_ONLY
#define TRIO_MINIMAL
#ifdef __TURBOC__
#include "trio/trio.c"
#else
#include "third_party/trio/trio.c"
#endif

int ODFallbackVsnprintf(char *buffer, size_t size, const char *format,
   va_list args)
{
   return(trio_vsnprintf(buffer, size, format, args));
}

#endif /* !OPENDOORS_HAVE_VSNPRINTF */
