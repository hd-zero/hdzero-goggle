#include "emulator/motor_audio_dev.h"

#include <stdio.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
                "motor_audio_poc host tool\n"
                "  --motor-test\n"
                "  --motor-synthetic [--motor-wav out.wav]\n"
                "  --motor-replay <csv> --motor-poles <n> [--motor-wav out.wav]\n");
        return 1;
    }
    /* Always handled; exits internally on motor options. */
    if (!motor_audio_dev_main(argc, argv)) {
        fprintf(stderr, "no motor-audio option recognized\n");
        return 1;
    }
    return 0;
}
