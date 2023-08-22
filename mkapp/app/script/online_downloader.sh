#!/bin/sh

version_gt() { test "$(echo "$@" | tr " " "\n" | sort -g | head -n 1)" != "$1"; }
version_le() { test "$(echo "$@" | tr " " "\n" | sort -g | head -n 1)" == "$1"; }
version_lt() { test "$(echo "$@" | tr " " "\n" | sort -rg | head -n 1)" != "$1"; }
version_ge() { test "$(echo "$@" | tr " " "\n" | sort -rg | head -n 1)" == "$1"; }

download() {
    dst_path="$1"
    fw_links="$2"
    fw_sizes="$3"
    fw_notes="$4"
    fw_total=1
    fw_counter=1

    # Do we have a SD Card mounted?
    if [ ! -z "$(mount | grep /mnt/extsd)" ]; then
        dst_path="/mnt/extsd/FIRMWARE/$dst_path"
    else
        dst_path="/tmp/FIRMWARE/$dst_path"
    fi

    # Check if we need to download?
    if test -f $dst_path/release.notes; then
        echo "Local storage contains latest release!"
        return 0
    fi

    # Blindly create directory, errors checking handled below.
    mkdir -p $dst_path 2> /dev/null

    # The real download work is performed here!
    for link in $fw_links; do
        file="$(basename "$link")"
        target="$dst_path/$file"
        if test -f "$target"; then
            current_size=$(wc -c < "$target")
            archive_size=$(echo $fw_sizes | cut -d" " -f$fw_counter)
            if [ $current_size -ne $archive_size ]; then
                echo "Resuming download: $target ...."
                curl -s -k -C $current_size -L $link -o "$target" && \
                fw_total=$((fw_total + 1)) && \
                echo "Resuming download: $target DONE!"
            else
                fw_total=$((fw_total + 1))
            fi
        else
            echo "Downloading firmware: $target ...."
            curl -s -k -L $link -o "$target" && \
            fw_total=$((fw_total + 1)) && \
            echo "Downloading firmware: $target DONE!"
        fi
        fw_counter=$((fw_counter + 1))
    done

    # Having release notes identify a successful download.
    if [ $fw_total -eq $fw_counter ]; then
        printf "$fw_notes" | tr -d '\r' > $dst_path/release.notes && \
        echo "Completed downloading latest release!"
    fi
}

app_version_check() {
    if version_gt $1 $2; then
        echo "Goggle running older firmware!"
        return 0
    else
        echo "Goggle running latest firmware!"
        return 1
    fi
}

online_goggle_fw_check() {
    fw_info="/tmp/hdz_goggle_fw.latest"
    echo "Checking Goggle Releases" && \
    curl -ks -o $fw_info https://api.github.com/repos/hd-zero/hdzero-goggle/releases/latest && \
    fw_link=$(cat $fw_info | grep 'browser_' | cut -d\" -f4) && \
    fw_note=$(cat $fw_info | grep 'body' | cut -d\" -f4) && \
    fw_size=$(cat $fw_info | grep 'size' | cut -d\" -f4) && \
    fw_file=$(basename $fw_link) && \
    rversion="$(echo ${fw_file%%.bin} | cut -d "-" -f4)" && \
    lversion="$(cat /mnt/app/version | cut -d "-" -f1)" && \
    app_version_check $rversion $lversion && \
    echo "Searching local storage for latest release..." && \
    download "GOGGLE/$rversion" "$fw_link" "$fw_size" "$fw_note"
}

online_vtx_fw_check() {
    fw_info="/tmp/hdz_vtx_fw.latest"
    echo "Checking VTX Releases" && \
    curl -ks -o $fw_info https://api.github.com/repos/hd-zero/hdzero-vtx/releases/latest && \
    fw_links=$(cat $fw_info | grep 'browser_' | cut -d\" -f4) && \
    fw_notes=$(cat $fw_info | grep 'body' | cut -d\" -f4) && \
    fw_sizes=$(cat $fw_info | grep 'size' | cut -d: -f2  | cut -d, -f1) && \
    rversion=$(cat $fw_info | grep 'tag_name' | cut -d\" -f4) && \
    echo "Searching local storage for latest release..." && \
    download "VTX/$rversion" "$fw_links" "$fw_sizes" "$fw_notes"
}

online_goggle_fw_check
online_vtx_fw_check
