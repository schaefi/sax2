/***************
FILE          : xmode.c
***************
PROJECT       : X11 Modeline algorithm
              :
BELONGS TO    : X11 Inc.
              :  
              :
DESCRIPTION   : xmode provides the libXmode to 
              : calculate modelines with a minimum on
              : requirements
              : 
              :
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "xmode.h"

//================================================
// calculate the fbmode from the X11 timings
// -----------------------------------------------
int XmodeFbTiming (struct xtiming *t) {
	return (0);
}

//================================================
// calculate horizontal/vertical timing values
//------------------------------------------------
int XmodeTiming (struct xmode *data,struct xtiming *ht) {
	float h_pixels_rnd;
	float v_lines_rnd;
	float v_field_rate_rqd;
	float top_margin;
	float bottom_margin;
	float interlace;
	float h_period_est;
	float vsync_plus_bp;
	float v_back_porch;
	float total_v_lines;
	float v_field_rate_est;
	float h_period;
	float v_field_rate;
	float v_frame_rate;
	float left_margin;
	float right_margin;
	float total_active_pixels;
	float ideal_duty_cycle;
	float h_blank;
	float total_pixels;
	float pixel_freq;
	float h_freq;

	float h_sync;
	float h_front_porch;
	float v_odd_front_porch_lines;

	// .../
	// 1. In order to give correct results, the number of horizontal
    // pixels requested is first processed to ensure that it is divisible
    // by the character size, by rounding it to the nearest character
    // cell boundary:
    // ---
    // [H PIXELS RND] = ((ROUND([H PIXELS]/[CELL GRAN RND],0))*[CELLGRAN RND])
	// ---
	h_pixels_rnd = rint((float) data->x / CELL_GRAN) * CELL_GRAN;

	// .../
	// 2. If interlace is requested, the number of vertical lines assumed
	// by the calculation must be halved, as the computation calculates
	// the number of vertical lines per field. In either case, the
	// number of lines is rounded to the nearest integer.
	// ---
	// [V LINES RND]=IF([INT RQD?]="y",ROUND([V LINES]/2,0),ROUND([V LINES],0))
	// ---
	if (data->interlaced) {
		v_lines_rnd = rint((float) data->y) / 2.0;
	} else {
		v_lines_rnd = rint((float) data->y);
	}

	// .../
	// 3. Find the frame rate required:
	// ---
	// [V FIELD RATE RQD]=IF([INT RQD?]="y", [I/P FREQ RQD]*2,*[I/P FREQ RQD])
	// ---
	if (data->interlaced) {
		v_field_rate_rqd = data->vsync * 2.0;
	} else {
		v_field_rate_rqd = data->vsync;
	}

	// .../
	// 4. Find number of lines in Top margin:
	// ---
	// [TOP MARGIN (LINES)] = IF([MARGINS RQD?]="Y",
	//         ROUND(([MARGIN%]/100*[V LINES RND]),0),0)
	// ---
	if (data->margins) {
		top_margin = rint(MARGIN_PERCENT / 100.0 * v_lines_rnd);
	} else {
		top_margin = 0.0;
	}

	// .../
	// 5. Find number of lines in Bottom margin:
	// ---
	// [BOT MARGIN (LINES)] = IF([MARGINS RQD?]="Y",
	//         ROUND(([MARGIN%]/100*[V LINES RND]),0),*0)
	// ---
	if (data->margins) {
	    bottom_margin = rint(MARGIN_PERCENT/100.0 * v_lines_rnd);
	} else {
		bottom_margin = 0.0;
	}

	// .../
	// 6. If interlace is required, then set variable [INTERLACE]=0.5:
	// ---  
	// [INTERLACE]=(IF([INT RQD?]="y",0.5,0))
	// ---
	if (data->interlaced) {
		interlace = 0.5;
	} else {
		interlace = 0.0;
	}

	// .../
	// 7. Estimate the Horizontal period
	// ---
	// [H PERIOD EST] = ((1/[V FIELD RATE RQD]) - [MIN VSYNC+BP]/1000000) /
	//                   ([V LINES RND] + (2*[TOP MARGIN (LINES)]) +
	//                    [MIN PORCH RND]+[INTERLACE]) * 1000000
	// ---
	h_period_est = (
		((1.0/v_field_rate_rqd) - (MIN_VSYNC_PLUS_BP/1000000.0)) / 
		(v_lines_rnd + (2*top_margin) + MIN_PORCH + interlace) * 1000000.0
	);

	// .../
	// 8. Find the number of lines in V sync + back porch:
	// ---
	// [V SYNC+BP] = ROUND(([MIN VSYNC+BP]/[H PERIOD EST]),0)
	// ---
	vsync_plus_bp = rint(MIN_VSYNC_PLUS_BP/h_period_est);

	// .../
	// 9. Find the number of lines in V back porch alone:
	// ---
	// [V BACK PORCH] = [V SYNC+BP] - [V SYNC RQD]
	// ---
	v_back_porch = vsync_plus_bp - V_SYNC_RQD;

	// .../
	// 10. Find the total number of lines in Vertical field period:
	// ---
	// [TOTAL V LINES] = [V LINES RND] + [TOP MARGIN (LINES)] +
	//                    [BOT MARGIN (LINES)] + [V SYNC+BP] + [INTERLACE] +
	//                    [MIN PORCH RND]
	// ---
	total_v_lines = v_lines_rnd + top_margin + bottom_margin +
					vsync_plus_bp + interlace + MIN_PORCH;

	// .../
	// 11. Estimate the Vertical field frequency:
	// ---
	// [V FIELD RATE EST] = 1 / [H PERIOD EST] / [TOTAL V LINES] * 1000000
	// ---
	v_field_rate_est = 1.0 / h_period_est / total_v_lines * 1000000.0;

	// .../
	// 12. Find the actual horizontal period:
	// ---
	// [H PERIOD] = [H PERIOD EST] / ([V FIELD RATE RQD] / [V FIELD RATE EST])
	// ---
	h_period = h_period_est / (v_field_rate_rqd / v_field_rate_est);

	// .../
	// 13. Find the actual Vertical field frequency:
	// ---
	// [V FIELD RATE] = 1 / [H PERIOD] / [TOTAL V LINES] * 1000000
	// ---
	v_field_rate = 1.0 / h_period / total_v_lines * 1000000.0;

	// .../
	// 14. Find the Vertical frame frequency:
	// ---
	// [V FRAME RATE] = (IF([INT RQD?]="y", [V FIELD RATE]/2, [V FIELD RATE]))
	// ---
	if (data->interlaced) {
		v_frame_rate = v_field_rate / 2.0;
	} else {
		v_frame_rate = v_field_rate;
	}

	// .../
	// 15. Find number of pixels in left margin:
	// ---
	// [LEFT MARGIN (PIXELS)] = (IF( [MARGINS RQD?]="Y",
	//         (ROUND( ([H PIXELS RND] * [MARGIN%] / 100 /
	//                  [CELL GRAN RND]),0)) * [CELL GRAN RND],
	//         0))
	// ---
	if (data->margins) {
	    left_margin = rint(
			h_pixels_rnd * MARGIN_PERCENT / 100.0 / CELL_GRAN
		) * CELL_GRAN;
	} else {
		left_margin = 0.0;
	}

	// .../
	// 16. Find number of pixels in right margin:
	// ---
	// [RIGHT MARGIN (PIXELS)] = (IF( [MARGINS RQD?]="Y",
	//          (ROUND( ([H PIXELS RND] * [MARGIN%] / 100 /
	//                   [CELL GRAN RND]),0)) * [CELL GRAN RND],
	//          0))
	// ---
	if (data->margins) {
		right_margin = rint(
			h_pixels_rnd * MARGIN_PERCENT / 100.0 / CELL_GRAN
		) * CELL_GRAN;
	} else {
		right_margin = 0.0;
	}

	// .../
	// 17. Find total number of active pixels in image and left and right
	//  margins:
	// ---
	// [TOTAL ACTIVE PIXELS] = [H PIXELS RND] + [LEFT MARGIN (PIXELS)] +
	//                          [RIGHT MARGIN (PIXELS)]
	// ---
	total_active_pixels = h_pixels_rnd + left_margin + right_margin;

	// .../
	// 18. Find the ideal blanking duty cycle from the blanking duty cycle
	// equation:
	// ---
	// [IDEAL DUTY CYCLE] = [C'] - ([M']*[H PERIOD]/1000)
	// ---
	ideal_duty_cycle = C_PRIME - (M_PRIME * h_period / 1000.0);

	// .../
	// 19. Find the number of pixels in the blanking time to the nearest
	// double character cell:
	// ---
	// [H BLANK (PIXELS)] = (ROUND(([TOTAL ACTIVE PIXELS] *
	//                               [IDEAL DUTY CYCLE] /
	//                               (100-[IDEAL DUTY CYCLE]) /
	//                               (2*[CELL GRAN RND])), 0))
	//                       * (2*[CELL GRAN RND])
	// ---
	h_blank = rint (
		total_active_pixels * ideal_duty_cycle /
		(100.0 - ideal_duty_cycle) /
		(2.0 * CELL_GRAN)
	) * (2.0 * CELL_GRAN);

	// .../
	// 20. Find total number of pixels:
	// ---
	// [TOTAL PIXELS] = [TOTAL ACTIVE PIXELS] + [H BLANK (PIXELS)]
	// ---
	total_pixels = total_active_pixels + h_blank;

	// .../
	// 21. Find pixel clock frequency:
	// ---
	// [PIXEL FREQ] = [TOTAL PIXELS] / [H PERIOD]
	// ---
	pixel_freq = total_pixels / h_period;

	// .../
	// 22. Find horizontal frequency:
	// ---
	// [H FREQ] = 1000 / [H PERIOD]
	// ---
	h_freq = 1000.0 / h_period;
	
	//==============================================
	// Stage 1 done now... Stage 2 follows
	//----------------------------------------------
	// 17. Find the number of pixels in the horizontal sync period:
	// ---
	// [H SYNC (PIXELS)] =(ROUND(([H SYNC%] / 100 * [TOTAL PIXELS] /
	//                             [CELL GRAN RND]),0))*[CELL GRAN RND]
	// ---
	h_sync = rint(H_SYNC_PERCENT/100.0 * total_pixels / CELL_GRAN) * CELL_GRAN;

	// .../
	// 18. Find the number of pixels in the horizontal front porch period:
	// ---
	// [H FRONT PORCH (PIXELS)] = ([H BLANK (PIXELS)]/2)-[H SYNC (PIXELS)]
	// ---
	h_front_porch = (h_blank / 2.0) - h_sync;

	// .../
	// 36. Find the number of lines in the odd front porch period:
	// ---
	// [V ODD FRONT PORCH(LINES)]=([MIN PORCH RND]+[INTERLACE])
	// ---
	v_odd_front_porch_lines = MIN_PORCH + interlace;

	//==============================================
	// Save results to timing struct
	//----------------------------------------------
	ht->hdisp  = (int) (h_pixels_rnd);
	ht->hsyncstart = (int) (h_pixels_rnd + h_front_porch);
	ht->hsyncend = (int) (h_pixels_rnd + h_front_porch + h_sync);
	ht->htotal = (int) (total_pixels);

	ht->vdisp  = (int) (v_lines_rnd);
	ht->vsyncstart = (int) (v_lines_rnd + v_odd_front_porch_lines);
	ht->vsyncend = (int) (int) (v_lines_rnd+v_odd_front_porch_lines+V_SYNC_RQD);
	ht->vtotal = (int) (total_v_lines);

	ht->dac   = pixel_freq;
	ht->hsync = h_freq;
	ht->vsync = data->vsync;

	return (0); 
}

//================================================
// init data and look at the options
//------------------------------------------------
int XmodeInit (int argc,char *argv[],struct xmode *data) {
	int  dacdefined     = FALSE;
	int  hsyncdef       = FALSE;
	int  vsyncdef       = FALSE; 
	int  nocheckdefined = FALSE;
	int  c;

	data->x     = 640;
	data->y     = 480;
	data->vsync = 140;
	data->hsync = 0;
	data->dac   = 0;
	data->fbdev = 0;
	data->flag  = 0;

	data->interlaced = 0;
	data->margins    = 0;

	while (1) {
	int option_index = 0;
	static struct option long_options[] =
	{
		{"dacspeed"   , 1 , 0 , 'd'},
		{"xdim"       , 1 , 0 , 'x'},
		{"ydim"       , 1 , 0 , 'y'},
		{"refresh"    , 1 , 0 , 'r'},
		{"fbdev"      , 1 , 0 , 'f'},
		{"sync"       , 1 , 0 , 's'},
		{"nocheck"    , 0 , 0 , 'n'},
		{"help"       , 0 , 0 , 'h'},
		{0            , 0 , 0 , 0  }
	};
	c = getopt_long (
		argc, argv, "hd:x:y:r:s:nf",long_options, &option_index
	);
	if (c == -1) {
		break;
	}

	switch (c) {
	case 0:
		fprintf (stderr,"option %s", long_options[option_index].name);
		if (optarg) {
			fprintf (stderr," with arg %s", optarg);
		}
		fprintf (stderr,"\n");
	break;

	case 'h':
		return(-1);

	case 'r':
		vsyncdef = TRUE;
		data->vsync = atof(optarg);
	break;

	case 's':
		hsyncdef = TRUE; 
		data->hsync = atof(optarg);
	break;

	case 'x':
		data->x = atoi(optarg);
	break;

	case 'y':
		data->y = atoi(optarg);
	break;
   
	case 'd':
		dacdefined = TRUE;
		data->dac = atof(optarg);
	break;

	case 'f':
		data->fbdev = 1;
	break;

	case 'n':
		nocheckdefined = TRUE;
	break;

	default:
		return(-1);
	}
	}
	if (vsyncdef && hsyncdef) {
		#if 0
		int khz = (int)((data->y * data->hsync) / 1000);
		int hz  = (int)((data->hsync * 1000) / data->y);
		int delta_khz = (data->hsync - khz) * -1;
		int delta_hz  = (data->vsync - hz)  * -1;
		if (delta_hz < delta_khz) {
			data->vsync = hz;
		}
		#endif
	}
	return(0);
}

//================================================
// how to use this program
//------------------------------------------------
void XmodeUsage(void) {
	printf("usage: xmode [ options ]\n");
	printf("options:\n");
	printf("  [ -d | --dacspeed ]\n");
	printf("    ramdac speed in Mhz\n");
	printf("  [ -r | --refresh ]\n"); 
	printf("    refresh rate in Hz\n");
	printf("  [ -s | --sync ]\n");
	printf("    horizontal sync rate in Khz\n");
	printf("  [ -x | --xdim ]\n");
	printf("    X - dimension in pixels\n");
	printf("  [ -y | --ydim ]\n");
	printf("    Y - dimension in pixels\n");
	printf("  [ -f | --fbdev ]\n");
	printf("    calculate framebuffer mode\n");
	exit(0);
}

