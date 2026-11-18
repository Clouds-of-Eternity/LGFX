#pragma once
#include <lgfx/lgfx.h>

void LGFXLog(LGFXLogLevel logLevel, const char *format, ...);

#define LGFX_ERROR(...) LGFXLog(LGFXLogLevel_Error, __VA_ARGS__)
#define LGFX_WARN(...) LGFXLog(LGFXLogLevel_Warning, __VA_ARGS__)
#define LGFX_DEBUG(...) LGFXLog(LGFXLogLevel_Debug, __VA_ARGS__)
#define LGFX_INFO(...) LGFXLog(LGFXLogLevel_Info, __VA_ARGS__)