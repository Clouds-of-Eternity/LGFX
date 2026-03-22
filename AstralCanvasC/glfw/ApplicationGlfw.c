#include "Application_c.h"
#include "GLFW/glfw3.h"

double AstralCanvas_GetElapsedTime()
{
    return glfwGetTime();
}
void AstralCanvas_SetClipboardText(const char *value)
{
    glfwSetClipboardString(NULL, value);
}
const char *AstralCanvas_GetClipboardText()
{
    return glfwGetClipboardString(NULL);
}

void AstralCanvas_ResetDeltaTimer()
{
    glfwSetTime(0.0);
    instance.shouldResetDeltaTimer = true;
}