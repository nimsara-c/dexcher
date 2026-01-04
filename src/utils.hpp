#ifndef DEXCHER_UTILS_H
#define DEXCHER_UTILS_H

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

// From External Sources
#include <nlohmann/json.hpp>

#include "types.hpp"

namespace Dexcher
{
    void loadConfig(ConfigStruct &_config, std::string jsonFileName = "settings.json")
    {
        using json = nlohmann::json;

        std::ifstream jsonFile(jsonFileName);
        json data = json::parse(jsonFile);
        jsonFile.close();

        _config.activeAppList = data["activeAppList"].get<std::vector<std::string>>();
        _config.activeForAllApps = data["activeForAllApps"].get<bool>();
        _config.totalDesktopCount = data["totalDesktopCount"].get<unsigned int>();
        _config.offsetPixels = data["offsetPixels"].get<unsigned int>();
        _config.isKeyboardSwitchingOn = data["turnOnKeyboardSwitching"].get<bool>();
        _config.isCursorSwitchingOn = data["turnOnMouseSwitching"].get<bool>();
        _config.isMouseSwitchingFollowsActiveAppListRule = data["doesMouseSwitchingFollowsActiveAppListRule"].get<bool>();
    }

    int writeConfig(ConfigStruct &_config, std::string jsonFileName = "settings.json")
    {
        using json = nlohmann::json;

        json data;
        data["activeAppList"] = _config.activeAppList;
        data["activeForAllApps"] = _config.activeForAllApps;
        data["totalDesktopCount"] = _config.totalDesktopCount;
        data["offsetPixels"] = _config.offsetPixels;
        data["turnOnKeyboardSwitching"] = _config.isKeyboardSwitchingOn;
        data["turnOnMouseSwitching"] = _config.isCursorSwitchingOn;
        data["doesMouseSwitchingFollowsActiveAppListRule"] = _config.isMouseSwitchingFollowsActiveAppListRule;

        std::ofstream output_file(jsonFileName);
        if (!output_file.is_open())
        {
            return ERRORS::FILE_NOT_FOUND;
        }
        else
        {
            output_file << data;
            output_file.close();
            return ERRORS::FILE_OK;
        }
    }

    const char *strVecToChar(std::vector<std::string> _strVec)
    {
        std::string output = "";
        for (auto &str : _strVec)
        {
            output += str;
            output += '\n';
        }
        return output.c_str();
    }

    std::vector<std::string> charArrayToStrVec(char *_charArray)
    {
        std::vector<std::string> strVec;
        std::stringstream ss(_charArray);
        std::string token;

        while (std::getline(ss, token, '\n'))
        {
            // Optional: add a check to skip empty strVec if multiple delimiters appear consecutively
            if (!token.empty())
            {
                strVec.push_back(token);
            }
        }
        return strVec;
    }
}

#endif