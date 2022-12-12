#ifndef _MCP_3021_H
#define _MCP_3021_H

typedef struct {
	int type;
	int voltage;
}sys_battery_t;

extern sys_battery_t g_battery;

int mcp_detect_type(void);
int mcp_read_vatage(void);

#endif
