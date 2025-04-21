#!/bin/sh

# CONFIGURATION
ACTUAL_DIR="/mnt/extsd/DCIM/100HDZRO"
LINK_NAME="media"
PORT=8080
SERVE_DIR="."
OUTPUT="$SERVE_DIR/index.html"
INTERVAL=10

create_symlink() {
  [ ! -L "$SERVE_DIR/$LINK_NAME" ] && ln -sf "$ACTUAL_DIR" "$SERVE_DIR/$LINK_NAME"
  [ ! -e "$SERVE_DIR/HDZero.png" ] && ln -sf /mnt/app/media/HDZero.png "$SERVE_DIR/HDZero.png"
}

generate_index() {
  # Create HTML header and styles
  cat > "$OUTPUT" << 'EOF'
<!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <title>DVR web gallery</title>
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: #1a1a1a;
      color: #e0e0e0;
    }
    .container {
      max-width: 1200px;
      margin: 0 auto;
      padding: 24px;
    }
    .header {
      display: flex;
      align-items: flex-start;
      gap: 24px;
      margin-bottom: 32px;
    }
    .logo-wrapper {
      background: #f0f0f0;
      border-radius: 10px;
      padding: 6px 10px;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    .logo {
      height: auto;
      max-height: 64px;
    }
    .title-block {
      display: flex;
      flex-direction: column;
    }
    .title {
      font-size: 2.5em;
      font-weight: 600;
      margin: 0 0 12px 0;
      color: #ffffff;
    }
    .buttons {
      display: flex;
      gap: 12px;
    }
    .download-link {
      background: #2563eb;
      color: white;
      padding: 10px 16px;
      border-radius: 6px;
      text-decoration: none;
      font-size: 0.95em;
      transition: background 0.2s ease;
    }
    .download-link:hover {
      background: #1d4ed8;
    }
    .gallery {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
      gap: 20px;
    }
    .item {
      background: #2a2a2a;
      padding: 10px;
      border-radius: 10px;
      box-shadow: 0 0 4px rgba(0, 0, 0, 0.2);
      text-align: center;
    }
    .item img {
      width: 100%;
      border-radius: 8px;
    }
    .filename {
      margin-top: 8px;
      word-wrap: break-word;
    }
    .filename a {
      color: #8ab4f8;
      text-decoration: none;
    }
    .filename a:hover {
      text-decoration: underline;
    }
    .no-media {
      grid-column: 1 / -1;
      text-align: center;
      padding: 40px;
      background: #2a2a2a;
      border-radius: 10px;
      font-size: 1.2em;
      color: #888;
    }
  </style>
</head>
<body>
  <div class='container'>
    <div class='header'>
      <div class='logo-wrapper'>
        <img src='/HDZero.png' class='logo'>
      </div>
      <div class='title-block'>
        <div class='title'>DVR web gallery</div>
        <div class='buttons'>
          <a class='download-link' href='javascript:alert("Download functionality coming soon")'>Download all DVR (with preview)</a>
          <a class='download-link' href='javascript:alert("Download functionality coming soon")'>Download all DVR (no preview)</a>
        </div>
      </div>
    </div>
    <div class='gallery'>
EOF

  # Generate gallery content
  if [ -d "$ACTUAL_DIR" ]; then
    media_count=$(find "$ACTUAL_DIR" -maxdepth 1 -type f \( -name "*.mp4" -o -name "*.ts" \) | wc -l)
    
    if [ "$media_count" -gt 0 ]; then
      find "$ACTUAL_DIR" -maxdepth 1 -type f \( -name "*.mp4" -o -name "*.ts" \) | sort | while read -r file; do
        base=$(basename "$file")
        name="${base%.*}"
        video_link="$LINK_NAME/$base"

        jpeg_link=""
        for ext in jpeg jpg JPG JPEG; do
          [ -f "$ACTUAL_DIR/$name.$ext" ] && jpeg_link="$LINK_NAME/$name.$ext" && break
        done

        {
          echo "<div class='item'>"
          if [ -n "$jpeg_link" ]; then
            echo "<a href='$video_link' download><img src='$jpeg_link' alt='$base'></a>"
          else
            echo "<a href='$video_link' download><div style='width:100%;height:120px;background:#444;display:flex;align-items:center;justify-content:center;color:#aaa;'>No preview</div></a>"
          fi
          echo "<div class='filename'><a href='$video_link' download>$base</a></div>"
          echo "</div>"
        } >> "$OUTPUT"
      done
    else
      echo "<div class='no-media'>No media found</div>" >> "$OUTPUT"
    fi
  else
    echo "<div class='no-media'>No media found</div>" >> "$OUTPUT"
  fi

  # Close HTML
  echo "</div></div></body></html>" >> "$OUTPUT"
}

start_httpd() {
  IP=$(ip addr show wlan0 | awk '/inet / {print $2}' | cut -d/ -f1)
  httpd -p "$PORT" -h "$SERVE_DIR" 2>/dev/null
  echo "Gallery available at: http://$IP:$PORT/index.html"
}

# MAIN
create_symlink
(while true; do generate_index 2>/dev/null; sleep "$INTERVAL"; done) &
start_httpd
