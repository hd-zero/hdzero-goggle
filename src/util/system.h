#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int system_exec(const char *command);
int system_script(const char *script);

#ifdef __cplusplus
}
#endif
