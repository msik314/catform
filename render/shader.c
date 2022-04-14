#include "util/globalDefs.h"
#include "render/shader.h"

#include <string.h>
#include "core/error.h"
#include "cmath/cVec.h"
#include "util/assert.h"

#ifndef NDEBUG
static void checkShaderError(GLuint shader, GLuint flag, bool isProgram, const char* errorMessage);
#endif //NDEBUG

static inline GLuint createShader(const char* text, GLenum shaderType)
{
    //Create shader
    GLuint shader = glCreateShader(shaderType);

    ASSERT(shader, "Error: shader creation failed\n");

    //Compile shader
    GLint len = strlen(text);

    glShaderSource(shader, 1, &text, &len);
    glCompileShader(shader);

    #ifndef NDEBUG
    //Error checking
    if(shaderType == GL_VERTEX_SHADER)
        checkShaderError(shader, GL_COMPILE_STATUS, false, "vertex shader compilation failed");
    else if(shaderType == GL_FRAGMENT_SHADER)
        checkShaderError(shader, GL_COMPILE_STATUS, false, "fragment shader compilation failed");
    else
        checkShaderError(shader, GL_COMPILE_STATUS, false, "shader compilation failed");
    #endif //NDEBUG

    return shader;
}


int32_t shaderCreate(Shader* shader, const char* vertSrc, const char* fragSrc)
{
    *shader = glCreateProgram();
    GLuint vertShader = createShader(vertSrc, GL_VERTEX_SHADER);
    GLuint fragShader = createShader(fragSrc, GL_FRAGMENT_SHADER);

    glAttachShader(*shader, vertShader);
    glAttachShader(*shader, fragShader);

    glLinkProgram(*shader);
    #ifndef NDEBUG
    checkShaderError(*shader, GL_LINK_STATUS, true, "program linking failed");
    #endif //NDEBUG

    glValidateProgram(*shader);
    #ifndef NDEBUG
    checkShaderError(*shader, GL_VALIDATE_STATUS, true, "program is invalid");
    #endif //NDEBUG

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void shaderDestroy(Shader* shader)
{
    glDeleteProgram(*shader);
}

#ifndef NDEBUG
static void checkShaderError(GLuint shader, GLuint flag, bool isProgram, const char* errorMessage)
{
    GLint success = 0;
    GLchar error[2048] = {0};

    //Check for errors
    if(isProgram)
    {
        glGetProgramiv(shader, flag, &success);
    }
    else
    {
        glGetShaderiv(shader, flag, &success);
    }

    //Get and print error messages if ther is an errror
    if(success == GL_FALSE)
    {
        if (isProgram)
        {
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        }
        else
        {
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);
        }

        printErr(CAT_ERROR_SHADER, error);
    }
}
#endif //NDEBUG
