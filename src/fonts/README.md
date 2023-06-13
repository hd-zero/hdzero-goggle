# LVGL's online converter
Location: https://lvgl.io/tools/fontconverter

Requires a True Type Font in order to convert to source code

# Example
QuickSilver OSD is based on Roboto Mono font family:

Symbols are included below, copy and paste them into the convertor.
```
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890`-=[]\;',./~!@#$%^&*()_+{}|:"<> ?
```
![image](https://github.com/SumolX/hdzero-goggle/assets/1988793/3231174e-97b9-4a92-9948-f1d4d1ea3f84)

# Compiling

Simply click convert then add the newly downloaded source file to the font directory and regenerate cmake via setup.sh

In order to include the font in the HDZero Goggle Source code, you must forward declare the font source.
```
extern const lv_font_t conthrax_26;
extern const lv_font_t robotomono_26;
```
