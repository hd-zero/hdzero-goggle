#pragma once
/* <termios.h> stub for the Windows (mingw) emulator build. The serial drivers
   that configure a tty (uart, dm5680, esp32, ...) never open a real port in the
   emulator, so these are inert stubs that let that device code compile. */

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

#define NCCS 32
struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_line;
    cc_t c_cc[NCCS];
    speed_t c_ispeed;
    speed_t c_ospeed;
};

/* c_cc subscripts */
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VTIME 5
#define VMIN 6

/* c_iflag */
#define IGNBRK 0000001
#define BRKINT 0000002
#define IGNPAR 0000004
#define PARMRK 0000010
#define INPCK 0000020
#define ISTRIP 0000040
#define INLCR 0000100
#define IGNCR 0000200
#define ICRNL 0000400
#define IXON 0002000
#define IXOFF 0010000

/* c_oflag */
#define OPOST 0000001

/* c_cflag */
#define CSIZE 0000060
#define CS5 0000000
#define CS6 0000020
#define CS7 0000040
#define CS8 0000060
#define CSTOPB 0000100
#define CREAD 0000200
#define PARENB 0000400
#define PARODD 0001000
#define HUPCL 0002000
#define CLOCAL 0004000
#define CRTSCTS 020000000000

/* c_lflag */
#define ISIG 0000001
#define ICANON 0000002
#define ECHO 0000010

/* baud rates (opaque tags here; values mirror Linux <termios.h>) */
#define B0 0000000
#define B2400 0000013
#define B4800 0000014
#define B9600 0000015
#define B19200 0000016
#define B38400 0000017
#define B57600 0010001
#define B115200 0010002
#define B230400 0010003
#define B460800 0010004
#define B921600 0010007

/* tcsetattr optional_actions */
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

/* tcflush queue_selector */
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

static inline int tcgetattr(int fd, struct termios *t) { (void)fd; (void)t; return -1; }
static inline int tcsetattr(int fd, int act, const struct termios *t) { (void)fd; (void)act; (void)t; return -1; }
static inline int tcflush(int fd, int q) { (void)fd; (void)q; return -1; }
static inline int tcdrain(int fd) { (void)fd; return -1; }
static inline int tcsendbreak(int fd, int d) { (void)fd; (void)d; return -1; }
static inline int cfsetispeed(struct termios *t, speed_t s) { (void)t; (void)s; return 0; }
static inline int cfsetospeed(struct termios *t, speed_t s) { (void)t; (void)s; return 0; }
static inline speed_t cfgetispeed(const struct termios *t) { (void)t; return 0; }
static inline speed_t cfgetospeed(const struct termios *t) { (void)t; return 0; }
static inline void cfmakeraw(struct termios *t) { (void)t; }
