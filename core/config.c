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

static int32_t configSavePlayer(const InputPlayer* player, JsonData* config, uint32_t parent)
{
    uint32_t playerObject = jsonDataArrayAddObject(config, parent);
    uint32_t arrayObject;
    uint32_t axisObject;
    
    jsonDataAddInt(config, playerObject, jsonKey("source"), player->inputSrc);
    arrayObject = jsonDataAddArray(config, playerObject, jsonKey("buttons"));
    for(uint32_t i = 0; i < player->buttons.size; ++i)
    {
        jsonDataArrayAddInt(config, arrayObject, player->buttons.data[i].src);
    }
    
    arrayObject = jsonDataAddArray(config, playerObject, jsonKey("axes"));
    for(uint32_t i = 0; i < player->axes.size; ++i)
    {
        axisObject = jsonDataArrayAddArray(config, arrayObject);
        jsonDataArrayAddInt(config, axisObject, player->axes.data[i].positive);
        jsonDataArrayAddInt(config, axisObject, player->axes.data[i].negative);
    }
    
    return CAT_SUCCESS;
}

int32_t configSaveInput(JsonData* config)
{
    Input* input = inputGetInstance();
    uint32_t array = jsonDataAddArray(config, JSON_DATA_ROOT_INDEX, jsonKey("input"));
    register int32_t res;
    
    for(uint32_t i = 0; i < input->players.size; ++i)
    {
        res = configSavePlayer(&input->players.data[i], config, array);
        if(res < CAT_SUCCESS) return res;
    }
    
    return CAT_SUCCESS;
}
