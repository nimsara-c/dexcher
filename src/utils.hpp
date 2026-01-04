#include <vector>
#include <string>

// From External Sources
#include <nlohmann/json.hpp>

#include "types.hpp"

namespace Dexcher
{
    static void loadConfig(ConfigStruct &_config, std::string jsonFileName = "settings.json")
    {
        using json = nlohmann::json;

        std::ifstream jsonFile(jsonFileName);
        json data = json::parse(jsonFile);
        jsonFile.close();

        _config.activeAppList = data["activeAppList"].get<std::vector<std::string>>();
        _config.totalDesktopCount = data["totalDesktopCount"].get<unsigned int>();
        _config.offsetPixels = data["offsetPixels"].get<unsigned int>();
        _config.isKeyboardSwitchingOn = data["turnOnKeyboardSwitching"].get<bool>();
        _config.isCursorSwitchingOn = data["turnOnMouseSwitching"].get<bool>();
        _config.isMouseSwitchingFollowsActiveAppListRule = data["doesMouseSwitchingFollowsActiveAppListRule"].get<bool>();
    }

    static const char *strVecToChar(std::vector<std::string> _strVec)
    {
        std::string output = "";
        for (auto &str : _strVec)
        {
            output += str;
            output += '\n';
        }
        return output.c_str();
    }
}