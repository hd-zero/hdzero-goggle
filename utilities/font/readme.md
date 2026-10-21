## Install npm

```
sudo apt-get install npm
```

## Install lv_font_conv

```
sudo npm install -g lv_font_conv
```

## Make font

```
python3 generate_font_lib.py
```

## Add Spanish, German, and Polish characters

Run these commands from `utilities/font` after generating the fonts in `out`:

```
python3 analyze_and_add_spanish_chars.py
bash copy-fonts.bash
```

The script regenerates every font in `out` with the Spanish, German, and Polish characters listed in the script. No command-line options are needed.
