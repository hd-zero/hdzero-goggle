import json
import re
import subprocess
import os
from pathlib import Path


def extract_unicode_points(input_file: Path, char_pattern: re.Pattern = re.compile('.')) -> list[int]:
    codes: list[int] = []

    with open(input_file, 'r', encoding='utf-8') as file:
        for line in file:
            if line.count('"') == 0:
                continue
            text = line[line.find('"') + 1:line.rfind('"')]
            for char in text:
                if char_pattern.match(char):
                    code_point = ord(char)
                    if not code_point in codes:
                        codes.append(code_point)

    return sorted(codes)


def list_to_plain_string(list: list[int]) -> str:
    return ",".join(map(str, list))


def unicode_points_range(input_file: Path, char_pattern: re.Pattern = re.compile('.')) -> str:
    return list_to_plain_string(extract_unicode_points(input_file, char_pattern))


def patch():
    folder_path = "out"

    for filename in os.listdir(folder_path):
        if filename.endswith(".c"):
            file_path = os.path.join(folder_path, filename)

            with open(file_path, "r", encoding="utf-8") as file:
                lines = file.readlines()

            if len(lines) >= 11:
                lines_to_keep = lines[: -10] + lines[-6:]

                with open(file_path, "w", encoding="utf-8") as file:
                    file.writelines(lines_to_keep)


def generate_fonts():
    cmd_app = "lv_font_conv"
    cmd_bpp = " --bpp 4"
    cmd_size = " --size "
    cmd_compress = " --no-compress"
    cmd_format = " --format lvgl"
    cmd_output = " -o out/lv_font_montserrat_"

    cmd_font_default = " --font Montserrat-Medium.ttf"
    cmd_range_default = " --range 32-127,176,8226"

    cmd_font_lvgl_privite = " --font FontAwesome5-Solid+Brands+Regular.woff"
    cmd_range_lvgl_privite = " --range 61441,61448,61451,61452,61452,61453,61457,61459,61461,61465,61468,61473,61478,61479,61480,61502,61507,61512,61515,61516,61517,61521,61522,61523,61524,61543,61544,61550,61552,61553,61556,61559,61560,61561,61563,61587,61589,61636,61637,61639,61641,61664,61671,61674,61683,61724,61732,61787,61931,62016,62017,62018,62019,62020,62087,62099,62212,62189,62810,63426,63650"

    cmd_languages: str = ""

    with open(Path(__file__).parent / "language_info.json", 'r') as f:
        language_info = json.load(f)

    BASE_LANGUAGE_PATH: Path = Path(__file__).parent / "../../mkapp/app/language"
    for language in language_info:
        cmd_languages += " --font " + language["font"] + " --range " + unicode_points_range(BASE_LANGUAGE_PATH / language["ini"], re.compile(language["range"]))

    font_size = [8, 10, 12, 14, 16, 18, 20, 22, 24,
                 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48]

    for s in font_size:
        command = ""
        command += cmd_app
        command += cmd_bpp
        command += cmd_size + str(s)
        command += cmd_compress

        command += cmd_font_default
        command += cmd_range_default
        command += cmd_font_lvgl_privite
        command += cmd_range_lvgl_privite
        command += cmd_languages

        command += cmd_format
        command += cmd_output + str(s) + ".c"
        print(command)
        subprocess.run(command, text=True, shell=True, capture_output=True)

    print("Patch")
    patch()
    print("Done")


if __name__ == '__main__':
    generate_fonts()
