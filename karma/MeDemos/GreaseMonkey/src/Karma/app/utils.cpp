/***********************************************************************************************
*
*   $Id: utils.cpp,v 1.1.2.3 2002/03/13 13:17:33 richardm Exp $
*
************************************************************************************************/
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#ifndef _XBOX
    #ifdef WIN32
        #include <windows.h>
    #endif
#else
    #include <xtl.h>
#endif

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "skeleton.h"
#include "RwFuncs.hpp"
#include "platxtra.h"
//#include "carAPI.hpp"
#include "McdFrame.h"

#include"MdtCar.hpp"
#include "utils.hpp"
#include "car.hpp"

#include "MeMemory.h"


_3D_PLINE debug_lines;
static RwUInt32 FrameCountTime, lastFrameTime;

#ifdef _DEBUG_WINDOW
DEBUG_STREAM debug_stream;
#endif

/***********************************************************************************************
*
*
*
************************************************************************************************/
void InitialiseFrameRate(void)
{
    FrameCountTime = lastFrameTime = RsTimer();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void SetSpeedoGrapics(MeReal spd)
{
    SetMeterArc(&speedo, (RwReal)(spd));
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void DebugLine(MeReal *pt1, MeReal *pt2, int red, int green, int blue)
{
#ifdef _MECHECK
    if(debug_lines.num_points < MAX_POINTS-1) {

        /* Points expected in Kea axis system and translated to RW  */
        RWIM3DVERTEXSetPos (&debug_lines.points[debug_lines.num_points],(RwReal)(Worigin.x+pt1[0]),
                                                                        (RwReal)(Worigin.y+pt1[2]),
                                                                        (RwReal)(Worigin.z-pt1[1]));
        RWIM3DVERTEXSetRGBA(&debug_lines.points[debug_lines.num_points], red, green, blue, 255);

        RWIM3DVERTEXSetPos (&debug_lines.points[debug_lines.num_points+1],(RwReal)(Worigin.x+pt2[0]),
                                                                        (RwReal)(Worigin.y+pt2[2]),
                                                                        (RwReal)(Worigin.z-pt2[1]));
        RWIM3DVERTEXSetRGBA(&debug_lines.points[debug_lines.num_points+1], red, green, blue, 255);

        debug_lines.num_points += 2;
    }
#endif
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void ResetDebugLines(void)
{
    debug_lines.num_points = 0;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal GetFrameRate(void)
{
    MeReal delta = 0.0f;
    RwUInt32 thisTime;

    thisTime = RsTimer();

    delta = (thisTime - lastFrameTime) * 0.001f;
    if(delta < 0.0f) delta = 0.0f;

    if(delta < MIN_FRAME_TIME)
    {
        delta = 0.0f; //May want to limit frame rate
    }
    else if(delta > MAX_FRAME_TIME) {
        delta = MAX_FRAME_TIME; //limits timestep but won't be running in realtime
        lastFrameTime = thisTime;
    } else
    {
        lastFrameTime = thisTime;
    }

    if( thisTime > (FrameCountTime + 1000) )
    {
        FramesPerSecond = FrameCounter;

        FrameCounter = 0;

        FrameCountTime = thisTime;
    }


    return(delta);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
RwReal SmallestAngleRads(RwReal curr, RwReal req)
{
    RwReal diff = req - curr;

    if(diff > ME_PI)
    {
        diff = diff - 2.0f*(RwReal)ME_PI;
    }
    else if(diff < (RwReal)-ME_PI)
    {
        diff = diff + 2.0f*(RwReal)ME_PI;
    }
    return(diff);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
RwReal SmallestAngleDegs(RwReal curr, RwReal req)
{
    RwReal diff = req - curr;

    if(diff > 180.0f)
    {
        diff = diff - 360.0f;
    }
    else if(diff < -180.0f)
    {
        diff = diff + 360.0f;
    }
    return(diff);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void InitialiseMeterBar(METER_BAR *bar, RwInt32 screen_x, RwInt32 screen_y, RwInt32 max_len, RwInt32 width)
{
    /* Set colour at each end   */
    bar->low_end.red = 0;   bar->low_end.green = 255;   bar->low_end.blue = 0; bar->low_end.alpha = 255;
    bar->high_end.red = 255;bar->high_end.green = 0;    bar->high_end.blue = 0; bar->high_end.alpha = 255;

    /* Set fixed end of bar in screen coords    */
    bar->fixed_end[0] = screen_x;
    bar->fixed_end[1] = screen_y;

    bar->max_len = max_len;
    bar->width = width;
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void InitialiseMeterArc(METER_ARC *arc, RwInt32 screen_x, RwInt32 screen_y, RwInt32 inner_rad, RwInt32 max_ang, RwInt32 width, RwReal max_val)
{
    int i,j;

    /* Set colour at each end   */
    arc->low_end.red = 0;   arc->low_end.green = 255;   arc->low_end.blue = 0; arc->low_end.alpha = 255;
    arc->high_end.red = 255;arc->high_end.green = 0;    arc->high_end.blue = 0; arc->high_end.alpha = 255;

    /* Set fixed end of bar in screen coords    */
    arc->centre[0] = screen_x;
    arc->centre[1] = screen_y;

    arc->max_ang = DegsToRads(max_ang);
    arc->width = width;
    arc->inner_rad = inner_rad;

    arc->max_val = max_val;

    /* Initialise point indices */
    i=0;
    for(j=0;j<2*NUM_ARC_SEGMENTS;j+=2)
    {

        arc->pt_index[i] = j;
        arc->pt_index[i+1] = j+1;
        arc->pt_index[i+2] = j+3;
        arc->pt_index[i+3] = j;
        arc->pt_index[i+4] = j+3;
        arc->pt_index[i+5] = j+2;
        i+=6;
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void SetMeterBar(METER_BAR *bar, RwReal val)
{
    RwInt32 len;
    RwInt32 lr,lg,lb,la,hr,hg,hb,ha; //colours

    val = LIMITS(0.0f, val, 1.0f);
    len = (RwInt32)(bar->max_len * val);

    /* Low end colour   */
    lr = bar->low_end.red;
    lg = bar->low_end.green;
    lb = bar->low_end.blue;
    la = bar->low_end.alpha;

    /* High end colour  */
    hr = (RwInt32)INTERPOLATE(bar->low_end.red, val, bar->high_end.red);
    hg = (RwInt32)INTERPOLATE(bar->low_end.green, val, bar->high_end.green);
    hb = (RwInt32)INTERPOLATE(bar->low_end.blue, val, bar->high_end.blue);
    ha = (RwInt32)INTERPOLATE(bar->low_end.alpha, val, bar->high_end.alpha);

    /* Low left corner  */
    RWIM2DVERTEXSetScreenX(&bar->corners[0], (RwReal)(bar->fixed_end[0] - bar->width/2));
    RWIM2DVERTEXSetScreenY(&bar->corners[0], (RwReal)(bar->fixed_end[1]));
    RWIM2DVERTEXSetScreenZ(&bar->corners[0], (RwReal)(10.0));
    RWIM2DVERTEXSetRecipCameraZ(&bar->corners[0], (RwReal)(0.1));
    RWIM2DVERTEXSetIntRGBA(&bar->corners[0], lr,lg,lb,la);

    /* Low right corner */
    RWIM2DVERTEXSetScreenX(&bar->corners[1], (RwReal)(bar->fixed_end[0] + bar->width/2));
    RWIM2DVERTEXSetScreenY(&bar->corners[1], (RwReal)(bar->fixed_end[1]));
    RWIM2DVERTEXSetScreenZ(&bar->corners[1], (RwReal)(10.0));
    RWIM2DVERTEXSetRecipCameraZ(&bar->corners[1], (RwReal)(0.1));
    RWIM2DVERTEXSetIntRGBA(&bar->corners[1], lr,lg,lb,la);

    /* High right corner */
    RWIM2DVERTEXSetScreenX(&bar->corners[2], (RwReal)(bar->fixed_end[0] + bar->width/2));
    RWIM2DVERTEXSetScreenY(&bar->corners[2], (RwReal)(bar->fixed_end[1] - len));
    RWIM2DVERTEXSetScreenZ(&bar->corners[2], (RwReal)(10.0));
    RWIM2DVERTEXSetRecipCameraZ(&bar->corners[2], (RwReal)(0.1));
    RWIM2DVERTEXSetIntRGBA(&bar->corners[2], hr,hg,hb,ha);

    /* High left corner */
    RWIM2DVERTEXSetScreenX(&bar->corners[3], (RwReal)(bar->fixed_end[0] - bar->width/2));
    RWIM2DVERTEXSetScreenY(&bar->corners[3], (RwReal)(bar->fixed_end[1] - len));
    RWIM2DVERTEXSetScreenZ(&bar->corners[3], (RwReal)(10.0));
    RWIM2DVERTEXSetRecipCameraZ(&bar->corners[3], (RwReal)(0.1));
    RWIM2DVERTEXSetIntRGBA(&bar->corners[3], hr,hg,hb,ha);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void SetMeterArc(METER_ARC *arc, RwReal val)
{
    RwInt32 inner_rad = arc->inner_rad, outer_rad = arc->inner_rad+arc->width;
    RwInt32 lr,lg,lb,la,hr,hg,hb,ha; //colours
    int i;
    RwReal y_scale=1;
    RwReal ang, seg_val, sin_ang, cos_ang;


#ifdef PS2
    y_scale = 0.55f;
#endif

    val = LIMITS(0.0f, val, arc->max_val);
    arc->val = val;
    val /= arc->max_val;

    /* Fixed end colour */
    lr = arc->low_end.red;
    lg = arc->low_end.green;
    lb = arc->low_end.blue;
    la = arc->low_end.alpha;

    /* Inner Fixed corner   */
    RWIM2DVERTEXSetScreenX(&arc->corners[0], (RwReal)(arc->centre[0]));
    RWIM2DVERTEXSetScreenY(&arc->corners[0], (RwReal)(arc->centre[1] + inner_rad*y_scale));
    RWIM2DVERTEXSetScreenZ(&arc->corners[0], (RwReal)(10.0));
    RWIM2DVERTEXSetRecipCameraZ(&arc->corners[0], (RwReal)(0.1));
    RWIM2DVERTEXSetIntRGBA(&arc->corners[0], lr,lg,lb,la);
    RWIM2DVERTEXSetU(&arc->corners[0], 0, 0.1f);
    RWIM2DVERTEXSetV(&arc->corners[0], 0, 0.1f);

    /* Outer fixed corner   */
    RWIM2DVERTEXSetScreenX(&arc->corners[1], (RwReal)(arc->centre[0]));
    RWIM2DVERTEXSetScreenY(&arc->corners[1], (RwReal)(arc->centre[1] + outer_rad*y_scale));
    RWIM2DVERTEXSetScreenZ(&arc->corners[1], (RwReal)(10.0));
    RWIM2DVERTEXSetRecipCameraZ(&arc->corners[1], (RwReal)(0.1));
    RWIM2DVERTEXSetIntRGBA(&arc->corners[1], lr,lg,lb,la);
    RWIM2DVERTEXSetU(&arc->corners[0], 0, 0.1f);
    RWIM2DVERTEXSetV(&arc->corners[0], 1, 0.1f);

    for(i = 1;i <= NUM_ARC_SEGMENTS; i++)
    {
        seg_val = val * i/NUM_ARC_SEGMENTS;
        ang = arc->max_ang*seg_val;
        sin_ang = (RwReal)MeSin(ang);
        cos_ang = (RwReal)MeCos(ang);

        /* High end colour  */
        hr = (RwInt32)INTERPOLATE(arc->low_end.red, seg_val, arc->high_end.red);
        hg = (RwInt32)INTERPOLATE(arc->low_end.green, seg_val, arc->high_end.green);
        hb = (RwInt32)INTERPOLATE(arc->low_end.blue, seg_val, arc->high_end.blue);
        ha = (RwInt32)INTERPOLATE(arc->low_end.alpha, seg_val, arc->high_end.alpha);

        RWIM2DVERTEXSetScreenX(&arc->corners[2*i], (RwReal)(arc->centre[0] - inner_rad*sin_ang));
        RWIM2DVERTEXSetScreenY(&arc->corners[2*i], (RwReal)(arc->centre[1] + inner_rad*cos_ang*y_scale));
        RWIM2DVERTEXSetScreenZ(&arc->corners[2*i], (RwReal)(10.0));
        RWIM2DVERTEXSetRecipCameraZ(&arc->corners[2*i], (RwReal)(0.1));
        RWIM2DVERTEXSetIntRGBA(&arc->corners[2*i], hr,hg,hb,ha);
        RWIM2DVERTEXSetU(&arc->corners[0], seg_val, 0.1f);
        RWIM2DVERTEXSetV(&arc->corners[0], 0, 0.1f);

        RWIM2DVERTEXSetScreenX(&arc->corners[2*i+1], (RwReal)(arc->centre[0] - outer_rad*sin_ang));
        cos_ang = (RwReal)MeCos(ang); //This fools the bug in the latest ee-gcc compiler!
        RWIM2DVERTEXSetScreenY(&arc->corners[2*i+1], (RwReal)(arc->centre[1] + outer_rad*cos_ang*y_scale));
        RWIM2DVERTEXSetScreenZ(&arc->corners[2*i+1], (RwReal)(10.0));
        RWIM2DVERTEXSetRecipCameraZ(&arc->corners[2*i+1], (RwReal)(0.1));
        RWIM2DVERTEXSetIntRGBA(&arc->corners[2*i+1], hr,hg,hb,ha);
        RWIM2DVERTEXSetU(&arc->corners[0], seg_val, 0.1f);
        RWIM2DVERTEXSetV(&arc->corners[0], 1, 0.1f);

    }

}

static int timer_start;

/***********************************************************************************************
*
*
*
************************************************************************************************/
void ResetTimer(void)
{
    timer_start = RsTimer();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int ReadTimer(void)
{
    return((RsTimer() - timer_start)); //milliseconds
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static RpCollisionTriangle *GroundHeightCheck(RpIntersection *intersection, RpWorldSector *sector,
                                             RpCollisionTriangle *coll_tri, RwReal dist, void *data)
{
    RwReal *val = (RwReal*)data;

    if(coll_tri->normal.y > 0)
    {
        dist = 1.0f - dist;
        if(dist < *val) *val = dist; //Pick the lowest value of ground height
    }
    return coll_tri;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
RwReal GetGroundHeight(RwReal x, RwReal z)
{
    const RwBBox *bb = RpWorldGetBBox(World);
    RpIntersection line;
    RwReal ground_height = 1.1f;
    RwReal len;

    line.type = rpINTERSECTLINE;
    line.t.line.start.x = x;
    line.t.line.start.z = z;
    line.t.line.end.x = x;
    line.t.line.end.z = z;


    /* Need a line directed downwards for RenderWare to detect collision    */
    if(bb->sup.y > bb->inf.y)
    {
        line.t.line.start.y = bb->sup.y;
        line.t.line.end.y = Worigin.y;
        len = bb->sup.y - Worigin.y;
    }
    else
    {
        line.t.line.start.y = bb->inf.y;
        line.t.line.end.y = Worigin.y;
        len = bb->inf.y - Worigin.y;
    }

//OLD_RW    RpWorldForAllIntersections(World, &line, GroundHeightCheck, (void*)&ground_height);

    RpCollisionWorldForAllIntersections(World, &line, GroundHeightCheck, (void*)&ground_height);

    if(ground_height == 1.1f) ground_height = 0.0f; //no intersections, assume zero
    ground_height *= len;
    ground_height += line.t.line.end.y;

    return(ground_height);
}

#ifdef _DEBUG_WINDOW
/***********************************************************************************************
*
*
*
************************************************************************************************/
void InitDebugStream(RwInt32 num_ch, RwReal x_range)
{

    if(x_range > 0)
    {
        debug_stream.x_scale = 1.0f/x_range;
    }
    else
    {
        debug_stream.x_scale = 1.0f;
    }

    debug_stream.channel = (DEBUG_CHANNEL*)malloc(num_ch*sizeof(DEBUG_CHANNEL));

    if(debug_stream.channel)
    {
        debug_stream.num_channels = num_ch;
    }
    else
    {
        debug_stream.num_channels = -1;
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void InitDebugChannel(RwInt32 ch, RwReal y_min, RwReal y_max, RwRGBA *col, char *label)
{
    RwInt32 i;

    if(ch > -1 && ch < debug_stream.num_channels)
    {
        DEBUG_CHANNEL *c = &debug_stream.channel[ch];

        if(y_min != y_max)
        {
            /* Set y scale and offset   */
            c->y_scale = 1.0f/(y_max-y_min);
            c->y_offset = -y_min*c->y_scale;
            /* Initialise links */
            for(i = 0; i < MAX_DEBUG_SAMPLES; i++)
            {
                c->data[i].y_val = 0.0f;
                c->data[i].delta_x = 1.0f;
                if(i>0 && i < MAX_DEBUG_SAMPLES-1)
                {
                    c->data[i].next = &c->data[i+1];
                    c->data[i].prev = &c->data[i-1];
                }
            }

            c->oldest = &c->data[0];
            c->oldest->prev = NULL;
            c->oldest->next = &c->data[1];

            c->newest = &c->data[MAX_DEBUG_SAMPLES-1];
            c->newest->next = NULL;
            c->newest->prev = &c->data[MAX_DEBUG_SAMPLES-2];

            c->col = *col;
            if(strlen(label) < 25)
            {
                strcpy(c->label, label);
            } else {
                sprintf(c->label,"Label too long");
            }
            c->initialised = TRUE;
        }
    }
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void DebugChannel(RwInt32 ch, RwReal delta_x, RwReal y_val)
{

    if(ch > -1 && ch < debug_stream.num_channels)
    {
        DEBUG_CHANNEL *c = &debug_stream.channel[ch];
        if(c->initialised)
        {
            DEBUG_SAMPLE *temp = c->oldest; //store link that moves from oldest to newest
            /* Update the oldest sample links   */
            c->oldest = temp->next;
            c->oldest->prev = NULL;
            temp->next = NULL;
            /* Update the newest sample links   */
            c->newest->next = temp;
            temp->prev = c->newest;
            c->newest = temp;

            /* Fill in the newest sample values */
            temp->y_val=y_val*c->y_scale + c->y_offset;
            temp->delta_x = delta_x*debug_stream.x_scale;

        }
    }
}
#endif

/***********************************************************************************************
*
*
*
************************************************************************************************/
void LogMessage(const int level,const char *const string,va_list ap)
{
#ifdef WIN32
  FILE *file;
  char buffer[200] = "";
  vsprintf(buffer,string,ap);
  file = fopen("output.txt","a+");
  fprintf(file,buffer);
  fprintf(file,"\n");
  fclose(file);
#endif
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void MEAPI OutputMessage(const int level,const char *const string,va_list ap)
{
#ifdef WIN32
  char buffer[2000];
  vsprintf(buffer,string,ap);
  OutputDebugString(buffer);
  OutputDebugString("\n");
#endif
}
