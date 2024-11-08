
#include "simplified_chinese.h"

translate_t translate_simplified_chinese[TRANSLATE_STRING_NUM] = {
    // status bar
    // number: 14
    {"SD Card", "SD卡"},
    {"Off", "关闭"},
    {"On", "开启"},
    {"In", "输入"},
    {"Expansion Module", "扩展模块"},
    {"Integrity check", "完整性检查"},
    {"#FF0000 SD Card full#", "#FF0000 SD卡满#"},
    {"Unsupported", "不支持"},
    {"No SD Card", "无SD卡"},
    {"Searching", "WiFi: 搜索中"},
    {"RF", "信道"},
    {"clip(s)", "个文件"},
    {"full", "满"},
    {"available", "有效"},

    // scan now
    // number:6
    {"Scan Now", "扫频"},
    {"Scan Ready", "扫频就绪"},
    {"When scanning is complete, use the\n dial to select a channel and press\n the Enter button to choose", "扫频完成后, 使用滚轮选择信道并按下确认键进入"},
    {"Scanning...", "扫频中..."},
    {"Scanning done", "扫频完成"},
    {"Scanning ready", "扫频就绪"},

    // source
    // number: 12
    {"Source", "信号源"},
    {"Expansion Module", "扩展模块"},
    {"Analog Video", "模拟制式"},
    {"HDZero Band", "HDZero频段"},
    {"Raceband", "竞速频段"},
    {"Lowband", "低频段"},
    {"HDZero BW", "HDZero带宽"},
    {"Wide", "宽带"},
    {"Narrow", "窄带"},
    {"< Back", "< 返回"},
    {"#00FF00 Detected#", "#00FF00 已连接#"},
    {"#C0C0C0 Disconnected", "#C0C0C0 未连接"},

    // image setting
    // number: 8
    {"Image Setting", "图像设置"},
    {"Brightness", "亮度"},
    {"Saturation", "饱和度"},
    {"Contrast", "对比度"},
    {"Auto", "自动"},
    {"To change image settings, click the Enter button to enter video mode. \nMake sure a HDZero VTX or analog VTX is powered on for live video.", "点击进入确认键进入视频模式以修改图像参数. \n确保HDZero VTX或模拟VTX已经运行"},
    {"Never", "从不"},
    {"min", "分钟"},

    // osd
    // number：11
    {"OSD", "OSD"},
    {"Adjust OSD Elements", "调整OSD元素"},
    {"Orbit", "轨迹"},
    {"Mode", "模式"},
    {"Min", "最小"},
    {"Max", "最大"},
    {"At Startup", "启动时"},
    {"Show", "显示"},
    {"Hide", "隐藏"},
    {"Last", "上一次"},
    {"Note: The positioning preview will display all OSD elements. Some elements might\nnot show during normal operation, depending on input source and conditions.\nOSD Element positioning is based on a 1280x720 canvas.\nPositions can be set for 4x3 and 16x9 modes separately,\nthe Show Element toggle is shared between both modes.", "注意: 位置预览会显示所有OSD.\n某些元素在正常运行可能不会显示,这取决于信号源和其它条件.\nOSD元素基于1280x720画布进行定位.\n可以分别在4x3和16x9模式下设置位置.\n元素的显示与隐藏在两种模式下一样."},

    // power
    // number: 15
    {"Power", "电源"},
    {"Battery", "电池"},
    {"Cell Mode", "电芯计数模式"},
    {"Manual", "手动"},
    {"Cell Count", "电芯数"},
    {"Warning Cell Voltage", "单芯警告电压"},
    {"Voltage Calibration", "电压校准"},
    {"Display Mode", "显示模式"},
    {"Total", "总和"},
    {"Cell Avg.", "单芯"},
    {"Warning Type", "警报方式"},
    {"Beep", "蜂鸣器"},
    {"Visual", "屏显"},
    {"Both", "同时"},
    {"AnalogRx", "外挂模块"},

    {"Fans", "风扇"},
    {"Record Option", "录像设置"},
    {"Auto Scan", "自动扫频"},
    {"ELRS", "ELRS"},
    {"WiFi Module", "WiFi模块"},
    {"Head Tracker", "头部追踪"},
    {"Playback", "录像回放"},
    {"Storage", "存储"},
    {"Firmware   ", "固件"},
    {"Focus Chart", "焦点图"},
    {"Clock", "时间"},
    {"Input", "按键"},
    {"Go Sleep!", "休眠!"},
};