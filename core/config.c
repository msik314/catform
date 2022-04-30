#include "util/globalDefs.h"
#include "core/config.h"

#include "core/error.h"
#include "core/input.h"
#include "json/jsonData.h"

static int32_t configLoadPlayer(const JsonData* config, const JsonObject* playerConfig)
{
    const JsonObject* array;
    const JsonObject* axisArray;
    Input* input = inputGetInstance();
    InputPlayer player;
    InputButton button = {};
    InputAxis axis = {};
    uint32_t objectIndex;
    int32_t intValue;
    
    inputPlayerCreate(&player);
    player.inputSrc = jsonObjectGetKey(playerConfig, "source", &intValue) == JSON_TYPE_INT ? intValue : CAT_INPUT_ACTION_NONE;
    
    if(jsonObjectGetKey(playerConfig, "buttons", &objectIndex) != JSON_TYPE_ARRAY)
    {
        printErr(CAT_ERROR_CONFIG, "config invalid: \"buttons\" should be an array of integers\n");
        inputPlayerDestroy(&player);
        return CAT_ERROR_CONFIG;
    }
    
    array = jsonDataGetChildConst(config, objectIndex);
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        button.src = jsonArrayGetIndex(array, i, &intValue) == JSON_TYPE_INT ? (int16_t)intValue : CAT_INPUT_ACTION_NONE;
        vectorAdd(InputButton)(&player.buttons, &button);
    }
    
    if(jsonObjectGetKey(playerConfig, "axes", &objectIndex) != JSON_TYPE_ARRAY)
    {
        printErr(CAT_ERROR_CONFIG, "config invalid: \"axes\" should be an array of pairs of integers\n");
        inputPlayerDestroy(&player);
        return CAT_ERROR_CONFIG;
    }
    
    array = jsonDataGetChildConst(config, objectIndex);
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        if(jsonArrayGetIndex(array, i, &objectIndex) != JSON_TYPE_ARRAY)
        {
            printErr(CAT_ERROR_CONFIG, "config invalid: \"axes\" should be an array of pairs of integers\n");
            inputPlayerDestroy(&player);
            return CAT_ERROR_CONFIG;
        }
        axisArray = jsonDataGetChildConst(config, objectIndex);
        
        axis.positive = jsonArrayGetIndex(axisArray, 0, &intValue) == JSON_TYPE_INT ? (int16_t)intValue : CAT_INPUT_ACTION_NONE;
        axis.negative = jsonArrayGetIndex(axisArray, 1, &intValue) == JSON_TYPE_INT ? (int16_t)intValue : CAT_INPUT_ACTION_NONE;
        vectorAdd(InputAxis)(&player.axes, &axis);
    }
    
    vectorAdd(InputPlayer)(&input->players, &player);
    return CAT_SUCCESS;
}

int32_t configLoadInput(const JsonData* config)
{
    uint32_t objectIndex;
    const JsonObject* players;
    const JsonObject* playerConfig;
    register int32_t res;
    
    if(jsonObjectGetKey(&config->root, "input", &objectIndex) != JSON_TYPE_ARRAY)
    {
        printErr(CAT_ERROR_CONFIG, "config invalid: \"input\" should be an array of player configurations");
        return CAT_ERROR_CONFIG;
    }
    
    players = jsonDataGetChildConst(config, objectIndex);
    for(uint32_t i = 0; i < jsonArraySize(players); ++i)
    {
        if(jsonArrayGetIndex(players, i, &objectIndex) != JSON_TYPE_OBJECT)
        {
            printErr
            (
                CAT_ERROR_CONFIG,
                "config invalid: player configurations should be an object of the form\n"
                "{\n    \"source\":int,\n    \"buttons\":[int],\n    \"axes\":[[int, int],[int, int]]\n}"
            );
            return CAT_ERROR_CONFIG;
        }
        
        playerConfig = jsonDataGetChildConst(config, objectIndex);
        res = configLoadPlayer(config, playerConfig);
    }
    
    return CAT_SUCCESS;
}

static int32_t configSavePlayer(const JsonData* data, const JsonObject* parent)
{
    
}

int32_t configSaveInput(JsonData* config)
{
    return CAT_SUCCESS;
}
