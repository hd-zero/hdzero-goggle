#pragma once

// Append a line to the diagnostic log on the SD card (/mnt/extsd/hwlog.txt),
// stamped with seconds since boot and synced immediately. Silently does
// nothing when the card is absent.
void hwlog(const char *fmt, ...);
