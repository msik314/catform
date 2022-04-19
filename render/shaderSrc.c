#include "util/globalDefs.h"
#include "render/shaderSrc.h"

const char* VERT_SRC =
"#version 430\n"

"layout(location = 0)in vec4 vertex;"
"layout(location = 0)out vec2 texCoord;"

"layout(location = 0)uniform mat4 mvp;"

"void main()"
"{"
    "gl_Position = mvp * vec4(vertex.xy, 0, 1);"
    "texCoord = vertex.zw;"
"}";

const char* FRAG_SRC =
"#version 430\n"

"layout(location = 0)in vec2 texCoord;"

"layout(location = 0)out vec4 fragColor;"

"layout(location = 1)uniform vec4 tint;"
"layout(location = 2)uniform int texIn;"
"layout(location = 4)uniform sampler2DArray textureBanks[8];" 

"vec4 texIndex(in sampler2DArray tex, in vec2 xy, in int index)"
"{"
    "ivec3 texSize = textureSize(tex, 0);"
    "return texture(tex, vec3(xy, float(index)/float(max(texSize.z - 1, 1))));"
"}"

"vec4 texBank(int tex, in vec2 xy)"
"{"
    "int index = tex & 0xff;"
    "int texBank = (tex >> 16) & 0xff;"
    "switch(texBank)\n"
    "{"
    "case 0:"
        "return texIndex(textureBanks[0], xy, index);\n"
    "case 1:"
        "return texIndex(textureBanks[1], xy, index);\n"
    "case 2:"
        "return texIndex(textureBanks[2], xy, index);\n"
    "case 3:"
        "return texIndex(textureBanks[3], xy, index);\n"
    "case 4:"
        "return texIndex(textureBanks[4], xy, index);\n"
    "case 5:"
        "return texIndex(textureBanks[5], xy, index);\n"
    "case 6:"
        "return texIndex(textureBanks[6], xy, index);\n"
    "case 7:"
        "return texIndex(textureBanks[7], xy, index);\n"
    "default:"
        "return texIndex(textureBanks[7], xy, index);\n"
    "}"
"}"

"void main()"
"{"
    "fragColor = texBank(texIn, texCoord);"
"}";
