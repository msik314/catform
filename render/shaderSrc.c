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
"layout(location = 2)uniform sampler2DArray textureBank;"
"layout(location = 3)uniform int texIn;"

"vec4 texIndex(in sampler2DArray tex, in vec2 xy, in int index)"
"{"
    "ivec3 texSize = textureSize(tex, 0);"
    "return texture(tex, vec3(xy, float(index)/float(texSize.z - 1)));"
"}"

"void main()"
"{"
    "fragColor = texIndex(textureBank, texCoord, texIn & 0xff);"
"}";
