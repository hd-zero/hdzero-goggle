/* *******************************************************************************
 * Copyright (c), 2001-2016, Allwinner Tech. All rights reserved.
 * *******************************************************************************/
/**
 * @file    lua_config_parser.h
 * @brief   lua 配置解析类
 * @author  id:826
 * @version v0.1
 * @date    2016-10-11
 */

#pragma once

#include <string>
#include <mutex>

struct lua_State;

class LuaConfig {
    public:
        LuaConfig();

        ~LuaConfig();

        lua_State *GetContext();

        int LoadFromFile(const std::string &filepath);

        int SyncConfigToFile(const std::string &filepath, const std::string &table);

        int SyncToFile(const std::string &filepath);

        std::string GetStringValue(const std::string &path);

        std::string GetStringValue(const std::string &path, const std::string &default_value);

        int GetIntegerValue(const std::string &path);

        int GetIntegerValue(const std::string &path, const int default_value);

        double GetDoubleValue(const std::string &path);

        double GetDoubleValue(const std::string &path, const double default_value);

        bool GetBoolValue(const std::string &path);

        bool GetBoolValue(const std::string &path, const bool default_value);

        int SetStringValue(const std::string &path, const std::string &value);

        int SetIntegerValue(const std::string &path, const int &value);

        int SetDoubleValue(const std::string &path, const double &value);

        int SetBoolValue(const std::string &path, const bool &value);

        void RegisterTableFiled(const std::string &table_name, const std::string &filed, const int &value);

    private:
        lua_State *lua_context_;
        std::mutex lock_;

        int LoadValueToStack(const std::string &path);

        int SetValueToLua(const std::string &path, const std::string &value);

}; // class LuaConfig
