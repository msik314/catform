#include "util/globalDefs.h"
#include "core/input.h"

#include <GLFW/glfw3.h>
#include "containers/vector.h"

void inputPlayerCreate(InputPlayer* player)
{
    vectorCreate(InputButton)(&player->buttons);
    vectorCreate(InputAxis)(&player->axes);
}

void inputPlayerDestroy(InputPlayer* player)
{
    vectorDestroy(InputAxis)(&player->axes);
    vectorDestroy(InputButton)(&player->buttons);
}

static void pollKeyboard(InputPlayer* player, GLFWwindow* window)
{
    register int32_t buttonVal;
    register float axisVal;
    for(uint32_t i = 0; i < player->buttons.size; ++i)
    {
        if(player->buttons.data[i].src < 0)
        {
            buttonVal = glfwGetKey(window, -player->buttons.data[i].src); 
        }
        else
        {
            buttonVal = glfwGetMouseButton(window, player->buttons.data[i].src);
        }
        
        if(player->buttons.data[i].state <= CAT_BUTTON_RELEASED)
        {
            player->buttons.data[i].state = buttonVal ? CAT_BUTTON_DOWN : CAT_BUTTON_RELEASED;
        }
        else
        {
            player->buttons.data[i].state = buttonVal ? CAT_BUTTON_PRESSED : CAT_BUTTON_UP;
        }
    }

    for(uint32_t i = 0; i < player->axes.size; ++i)
    {
        axisVal = 0.0f;
        if(player->axes.data[i].positive == CAT_INPUT_ACTION_NONE){}
        else if(player->axes.data[i].positive < 0)
        {
            axisVal += glfwGetKey(window, -player->axes.data[i].positive) ? 1.0f : 0.0f; 
        }
        else
        {
            axisVal += glfwGetMouseButton(window, player->axes.data[i].positive) ? 1.0f : 0.0f; 
        }
        
        if(player->axes.data[i].negative == CAT_INPUT_ACTION_NONE){}
        else if(player->axes.data[i].negative < 0)
        {
            axisVal -= glfwGetKey(window, -player->axes.data[i].negative) ? 1.0f : 0.0f; 
        }
        else
        {
            axisVal -= glfwGetMouseButton(window, player->axes.data[i].negative) ? 1.0f : 0.0f; 
        }
        
        player->axes.data[i].value = axisVal;
    }
}

static void pollGamepad(InputPlayer* player, GLFWwindow* window)
{
    GLFWgamepadstate state = {};
    register float axisVal;
    
    glfwGetGamepadState(player->inputSrc, &state);
    
    for(uint32_t i = 0; i < player->buttons.size; ++i)
    {
        if(player->buttons.data[i].state <= CAT_BUTTON_RELEASED)
        {
            player->buttons.data[i].state = state.buttons[player->buttons.data[i].src] ? CAT_BUTTON_DOWN : CAT_BUTTON_RELEASED;
        }
        else
        {
            player->buttons.data[i].state = state.buttons[player->buttons.data[i].src] ? CAT_BUTTON_PRESSED : CAT_BUTTON_UP;
        }
    }
    
    for(uint32_t i = 0; i < player->axes.size; ++i)
    {
        axisVal = 0.0f;
        if(player->axes.data[i].positive != CAT_INPUT_ACTION_NONE)
        {
            axisVal += state.axes[player->axes.data[i].positive]; 
        }
        
        if(player->axes.data[i].negative != CAT_INPUT_ACTION_NONE)
        {
            axisVal -= state.axes[player->axes.data[i].negative]; 
        }
        
        player->axes.data[i].value = axisVal;
    }
}

void inputPlayerPoll(InputPlayer* player, void* window)
{
    if(player->inputSrc == CAT_INPUT_KEYBOARD)
    {
        pollKeyboard(player, (GLFWwindow*)window);
    }
    else
    {
        pollGamepad(player, (GLFWwindow*)window);
    }
}

Input* inputGetInstance()
{
    static Input input;
    return &input;
}

void inputCreate(Input* input)
{
    vectorCreate(InputPlayer)(&input->players);
}

void inputDestroy(Input* input)
{
    for(uint32_t i = 0; i < input->players.size; ++i)
    {
        inputPlayerDestroy(&input->players.data[i]);
    }
    
    vectorDestroy(InputPlayer)(&input->players);
}

void inputPoll(Input* input, void* window)
{
    glfwPollEvents();
    for(uint32_t i = 0; i < input->players.size; ++i)
    {
        inputPlayerPoll(&input->players.data[i], window);
    }
    
    input->mouseX[1] = input->mouseX[0];
    input->mouseY[1] = input->mouseY[0];
    glfwGetCursorPos((GLFWwindow*)window, input->mouseX, input->mouseY);
}
