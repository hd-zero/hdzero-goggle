# Font generation for non-ascii characters

To display non-ascii characters in the goggles, you have to generate the font library for lvgl. This is done by doing
three things:

1. Describe your language in the `language_info.json` file.
1. Run the `generate_font_lib.py` script.
1. Copy the generated font library to the `lib/lvgl/lvgl/src/font` directory.

## Describing the font

The `language_info.json` file is a json array of objects, where each object has the following properties:

- `name`: The name of the language.
- `ini`: The name of the ini file that contains the font.
- `font`: The name of the font file.
- `range`: The range of characters that are supported by the font. You can look those up on the [unicode website](https://https://home.unicode.org/).

## Running the script

You can run the script on either a Linux machine or inside the WSL by following these steps. The only prerequesite is
that you have a fairly recent version of Python installed.

### Install npm

```
sudo apt-get install npm
```

### Install lv_font_conv

```
sudo npm install -g lv_font_conv
```

### Make font

```
python3 generate_font_lib.py
```

## Copying the font library

Copy the generated font library (i.e. all lv_font_*.c files) from the `out` directory to the `lib/lvgl/lvgl/src/font`
directory. Don't forget to commit them to the repository or others won't be able to see all characters necessary by the
localized strings.

This can also be achieved by calling the script with the `--copy` flag:

```
python3 generate_font_lib.py --copy
```

Note that this operation might need privileges to copy the files to the target directory.
