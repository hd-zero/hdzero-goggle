#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Emulator/dev entry for synthetic motor audio PoC.
 * Returns 1 if argv was handled (caller should exit), 0 otherwise.
 */
int motor_audio_dev_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif
