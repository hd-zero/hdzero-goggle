/*
 * LuaConfigHelper.h
 *
 *  Created on: 2017年5月3日
 *      Author: liuyangcheng
 */

#ifndef SYSTEM_PUBLIC_LUACONFIG_LUACONFIGHELPER_H_
#define SYSTEM_PUBLIC_LUACONFIG_LUACONFIGHELPER_H_

#include <sstream>
#include <string>

class LuaConfig;

class LuaConfigHelper {
public:
    LuaConfigHelper(LuaConfig &luaConfig);
    ~LuaConfigHelper();

    LuaConfigHelper& operator[] (char* fieldStr);
    LuaConfigHelper& operator[] (int index);

    void Clear();
    int IntValue();
    bool BoolValue();
    std::string StringValue();

    void SetValue(int   value);
    void SetValue(const std::string& value);
    void SetValue(bool  value);

private:
    LuaConfig &luaConfig_;
    std::stringstream exprSStream_;
};

#endif /* SYSTEM_PUBLIC_LUACONFIG_LUACONFIGHELPER_H_ */
