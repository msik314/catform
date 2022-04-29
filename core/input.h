#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include "containers/vector.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define CAT_INPUT_KEYBOARD -1
#define CAT_INPUT_GAMEPAD(X) X

#define CAT_INPUT_ACTION_NONE -32767
#define CAT_INPUT_ACTION_KEY(X) (-X)
#define CAT_INPUT_ACTION_MOUSE(X) (X)

#define CAT_BUTTON_UP -1
#define CAT_BUTTON_RELEASED 0
#define CAT_BUTTON_PRESSED 1
#define CAT_BUTTON_DOWN 2

typedef struct
{
    int16_t src;
    int16_t state;
}
InputButton;

VECTOR_DECL(InputButton)

typedef struct
{
    int16_t positive;
    int16_t negative;
    float value;
}
InputAxis;

VECTOR_DECL(InputAxis)

typedef struct
{
    int32_t inputSrc;
    Vector(InputButton) buttons;
    Vector(InputAxis) axes;
}
InputPlayer;

void inputPlayerCreate(InputPlayer* input);
void inputPlayerDestroy(InputPlayer* input);
void inputPlayerPoll(InputPlayer* input, void* window);

static inline bool inputPlayerGetButton(const InputPlayer* player, uint32_t button){return player->buttons.data[button].state >= CAT_BUTTON_PRESSED;}
static inline bool inputPlayerGetButtonDown(const InputPlayer* player, uint32_t button){return player->buttons.data[button].state == CAT_BUTTON_DOWN;}
static inline bool inputPlayerGetButtonUp(const InputPlayer* player, uint32_t button){return player->buttons.data[button].state == CAT_BUTTON_UP;}
static inline float inputPlayerGetAxis(const InputPlayer* player, uint32_t axis){return player->axes.data[axis].value;}

VECTOR_DECL(InputPlayer)

typedef struct
{
    Vector(InputPlayer) players;
    double mouseX[2];
    double mouseY[2];
}
Input;

Input* inputGetInstance();

void inputCreate(Input* input);
void inputDestroy(Input* input);
void inputPoll(Input* input, void* window);

static inline bool inputGetButton(const Input* input, uint32_t player, uint32_t button){return inputPlayerGetButton(&input->players.data[player], button);}
static inline bool inputGetButtonDown(const Input* input, uint32_t player, uint32_t button){return inputPlayerGetButtonDown(&input->players.data[player], button);}
static inline bool inputGetButtonUp(const Input* input, uint32_t player, uint32_t button){return inputPlayerGetButtonUp(&input->players.data[player], button);}
static inline float inputGetAxis(const Input* input, uint32_t player, uint32_t axis){return inputPlayerGetAxis(&input->players.data[player], axis);}
static inline float inputGetMouseX(const Input* input){return (float)(input->mouseX[0]);}
static inline float inputGetMouseDX(const Input* input){return (float)(input->mouseX[0] - input->mouseX[1]);}
static inline float inputGetMouseY(const Input* input){return (float)(input->mouseY[0]);}
static inline float inputGetMouseDY(const Input* input){return (float)(input->mouseY[0] - input->mouseY[1]);}

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //INPUT_H
