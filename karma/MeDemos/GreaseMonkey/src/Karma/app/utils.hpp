/***********************************************************************************************
*
*	$Id: utils.hpp,v 1.1.2.1 2002/03/01 19:55:03 richardm Exp $
*
************************************************************************************************/
#ifndef UTILS_H
#define UTILS_H

/* MACROS	*/
#ifndef MAX
#define MAX(a, b) (a>b?a:b)
#endif
#ifndef MIN
#define MIN(a, b) (a<b?a:b)
#endif
#ifndef LIMITS
#define LIMITS(a, val, b) (val<a?a:val>b?b:val)
#endif
#ifndef INTERPOLATE
#define INTERPOLATE(a, val, b) ((a) + ((b)-(a))*val)
#endif
#ifndef FRACTION
#define FRACTION(a, val, b) (((val)-(a))/((b)-(a)))
#endif
#ifndef DegsToRads
#define DegsToRads(x) ((x)*3.14159f/180.0f)
#endif
#ifndef RadsToDegs
#define RadsToDegs(x) ((x)*180.0f/3.14159f)
#endif
#ifndef SIGN
#define SIGN(x) ((x)<0?-1:1)
#endif

typedef struct meter_bar_s {
	RwRGBA low_end, high_end;
	RwInt32 max_len, width;
	RwInt32 fixed_end[2];
	RwChar label[10];
	RWIM2DVERTEX corners[4];
} METER_BAR;

#define NUM_ARC_SEGMENTS (20)

typedef struct meter_arc_s {
	RwRGBA low_end, high_end;
	RwInt32 width, inner_rad;
	RwReal max_ang, max_val, val;
	RwInt32 centre[2];
	RwChar label[10];
	RWIM2DVERTEX corners[2*NUM_ARC_SEGMENTS+2];
	RWIMVERTEXINDEX pt_index[2*NUM_ARC_SEGMENTS*3];
} METER_ARC;

#define MAX_POINTS (200)
typedef struct _3d_pline_s {
	RwInt32 num_points;
	RWIM3DVERTEX points[MAX_POINTS];
	RWIMVERTEXINDEX pt_index[MAX_POINTS];
} _3D_PLINE;

#ifdef _DEBUG_WINDOW
#define MAX_DEBUG_SAMPLES (200)

typedef struct _dbg_sample {
	RwReal y_val, delta_x;
	struct _dbg_sample *next, *prev;
} DEBUG_SAMPLE;

typedef struct _dbg_channel {
	RwReal y_scale, y_offset;
	struct _dbg_sample data[MAX_DEBUG_SAMPLES];
	struct _dbg_sample *newest, *oldest;
	RwRGBA col;
	RwInt32 initialised;
	char label[25];
} DEBUG_CHANNEL;

typedef struct _dbg_stream {
	RwReal x_scale;
	RwInt32 num_channels;
	struct _dbg_channel *channel;
} DEBUG_STREAM;

extern DEBUG_STREAM debug_stream;
void InitDebugStream(RwInt32 num_ch, RwReal x_range);
void InitDebugChannel(RwInt32 ch, RwReal y_min, RwReal y_max, RwRGBA *col, char *label);
extern void DebugChannel(RwInt32 ch, RwReal delta_x, RwReal y_val);
#endif

extern float render_time, dynamics_time, collision_time;
extern void InitialiseFrameRate(void);
extern void SetSpeedoGrapics(MeReal spd);
extern void DebugLine(MeReal *pt1, MeReal *pt2, int red, int green, int blue);
extern void ResetDebugLines(void);
extern MeReal GetFrameRate(void);
extern RwReal SmallestAngleRads(RwReal curr, RwReal req);
extern RwReal SmallestAngleDegs(RwReal curr, RwReal req);
extern void InitialiseMeterBar(METER_BAR *bar, RwInt32 screen_x, RwInt32 screen_y, RwInt32 max_len, RwInt32 width);
extern void InitialiseMeterArc(METER_ARC *arc, RwInt32 screen_x, RwInt32 screen_y, RwInt32 inner_rad, RwInt32 max_ang, RwInt32 width, RwReal max_val);
extern void SetMeterBar(METER_BAR *bar, RwReal val);
extern void SetMeterArc(METER_ARC *arc, RwReal val);
extern void ResetTimer(void);
extern int ReadTimer(void);
extern RwReal GetGroundHeight(RwReal x, RwReal z);
extern void MEAPI OutputMessage(const int level,const char *const string,va_list ap);
extern void LogMessage(const int level,const char *const string,va_list ap);


#endif //UTILS_H
