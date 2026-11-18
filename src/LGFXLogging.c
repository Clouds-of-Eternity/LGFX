#include "LGFXLogging.h"
#include <stdio.h>
#include <stdarg.h>
#include "LGFXMemory.h"

LGFXLoggerInfo currLogger;

static inline void DefaultLogMessage(void *userData, LGFXLogLevel logLevel, const char *message)
{
    const char *prefix = "Unknown";
    switch (logLevel)
    {
        default:
            prefix = "LGFX Info";
            break;
        case LGFXLogLevel_Debug:
            prefix = "LGFX Debug";
            break;
        case LGFXLogLevel_Warning:
            prefix = "LGFX Warning";
            break;
        case LGFXLogLevel_Error:
            prefix = "LGFX Error";
            break;
    }
    if (logLevel == LGFXLogLevel_Error)
    {
        fprintf(stderr, "%s: %s", prefix, message);
    }
    else
    {
        printf("%s: %s", prefix, message);
    }
}

void LGFXSetLogger(const LGFXLoggerInfo *loggerInfo)
{
    currLogger = *loggerInfo;
}
void LGFXUseConsoleLogger()
{
    currLogger.userData = NULL;
    currLogger.logCallback = &DefaultLogMessage;
}
void LGFXLog(LGFXLogLevel logLevel, const char *format, ...)
{
    if (currLogger.logCallback == NULL)
    {
        return;
    }

    va_list args;
    va_list args2;
    va_start(args, format);
    va_copy(args2, args);
    int32_t len = vsnprintf(NULL, 0, format, args);
    va_end(args);

    char *buffer = Allocate(char, len + 1);
    vsnprintf(buffer, len + 1, format, args2);
    buffer[len] = '\0';

    va_end(args2);

    currLogger.logCallback(currLogger.userData, logLevel, buffer);

    free(buffer);
}