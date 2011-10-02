#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/* struct definitions */

typedef struct __mode
{
    int hr, hss, hse, hfl;
    int vr, vss, vse, vfl;
    float pclk, h_freq, v_freq;
    float real_v_rate;
    int rb, in;
} mode;


typedef struct __options
{
    int x, y;
    int reduced_blank, interlaced;
    int xf86mode, fbmode;
    float v_freq;
} options;




/* prototypes */

void print_value(int n, char *name, float val);
void print_xf86_mode (mode *m);
char* print_sax_mode (mode *m);
void print_fb_mode (mode *m);
options *parse_command_line (int argc, char *argv[]);

mode *vert_refresh (
	int h_pixels, int v_lines, float freq,
    int interlaced, int reduced_blank, int margins
);

