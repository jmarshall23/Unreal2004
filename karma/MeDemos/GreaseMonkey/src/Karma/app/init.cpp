/***********************************************************************************************
*
*   $Id: init.cpp,v 1.1.2.2 2002/03/13 13:17:33 richardm Exp $
*
***********************************************************************************************/
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include "rplogo.h"
#include "rtbmp.h"

#include "MePrecision.h"
#include "parser.hpp"
#include "utils.hpp"
#include "RwFuncs.hpp"
#include "skeleton.h"
#include "metrics.h"

#include "platxtra.h"
#include "McdFrame.h"

#include "MdtCar.hpp"
#include "carAPI.hpp"
#include "car.hpp"
#include "driver.hpp"
#include "smoke.hpp"
#include "init.hpp"
#include "control.hpp"
#include "MeMemory.h"
#include "McdSpace.h"

#ifdef USE_SOUND
    #include "carsound.hpp"
#endif

extern McdSpaceID mcd_space;

char *track_textures[NUM_TRACK_TEXTURES] = {
    {"stands2"},
    {"cement"},
    {"concsmall"},
    {"road_main"},
    {"road_main_b"},
    {"roaddirtoff"},
    {"road_arrow"},
    {"road_start"},
    {"crisscross"}
};

char *chassis_tex_names[NUM_CAR_TEXTURES] = {
    {"car2map"},
    {"car3map"},
    {"car4map"},
    {"car5map"},
    {"car6map"},
    {"car7map"}
};

RwTexture *chassis_textures[NUM_CAR_TEXTURES];

RwUInt8 track_mats[NUM_TRACK_TEXTURES];

/* Cfg parser functions */
static void xPCVideoMode(Parser *pf);
static void xCfgShapeDir(Parser *pf);
static void xCfgTexDir(Parser *pf);
static void xCfgWorldBsp(Parser *pf);
static void xCfgLightsFile(Parser *pf);
static void xCfgTracksFile(Parser *pf);
//static void xCfgCarDataFile(Parser *pf);
//static void xCfgDashboardShape(Parser *pf);
static void xCfgDriver(Parser *pf);
static void xCfgPlayerCar(Parser *pf);
static void xCfgRearViewMirror(Parser *pf);

/* Lighting Parser functions    */
static void xLtsAtomics(Parser *pf);
static void xLtsWorld(Parser *pf);
static void xLtsLight(Parser *pf);
static void xLtsEndLight(Parser *pf);
static void xLtsPosition(Parser *pf);
static void xLtsType(Parser *pf);
static void xLtsDirection(Parser *pf);
static void xLtsRGB(Parser *pf);
static void xLtsAlpha(Parser *pf);
static void xLtsPitch(Parser *pf);
static void xLtsCone(Parser *pf);
static void xLtsRadius(Parser *pf);

/* Track Parser Files   */
static void xTrkTrack(Parser *pf);
static void xTrkEndTrack(Parser *pf);
static void xTrkPoint(Parser *pf);

static TEXT_2_PFUNC lts_comtable[] = {
    {"Light",       xLtsLight},
    {"World",       xLtsWorld},
    {"Atomics",     xLtsAtomics},
    {"EndLight",    xLtsEndLight},
    {"Type",        xLtsType},
    {"Position",    xLtsPosition},
    {"Direction",   xLtsDirection},
    {"RGB",         xLtsRGB},
    {"Alpha",       xLtsAlpha},
    {"Pitch",       xLtsPitch},
    {"Radius",      xLtsRadius},
    {"Cone",        xLtsCone},
    {0,0}
};

static TEXT_2_PFUNC cfg_comtable[] = {
    {"PC_VIDEOMODE",xPCVideoMode},
    {"SHAPE_DIR",   xCfgShapeDir},
    {"TEXTURE_DIR", xCfgTexDir},
    {"WORLD_BSP",   xCfgWorldBsp},
    {"LIGHTS_FILE", xCfgLightsFile},
    {"TRACKS_FILE", xCfgTracksFile},
//  {"CAR_DATA_FILE",   xCfgCarDataFile},
//  {"DASHBOARD_SHAPE", xCfgDashboardShape},
    {"DRIVER",      xCfgDriver},
    {"PLAYER_CAR",      xCfgPlayerCar},
    {"REAR_VIEW_MIRROR",xCfgRearViewMirror},
    {0,0}
};

static TEXT_2_PFUNC trk_comtable[] = {
    {"TRACK",       xTrkTrack},
    {"END_TRACK",   xTrkEndTrack},
    {"POINT",       xTrkPoint},
    {0,0}
};

TEXT_2_ENUM light_types[] = {
    {"AMBIENT",     rpLIGHTAMBIENT},
    {"DIRECTIONAL", rpLIGHTDIRECTIONAL},
    {"SPOT",        rpLIGHTSPOT},
    {"POINT",       rpLIGHTPOINT},
    {"SPOTSOFT",    rpLIGHTSPOTSOFT},
    {0,0}
};

RwInt32 num_lights, light_type, light_flags;
RwV3d light_pos;
RwReal light_dir, light_pitch, light_rad, light_cone;
RwRGBAReal light_col;

RwV3d cam_def_start_pos = {-600,100,400};

//RwInt32 num_points;
course_point curr_track[MAX_COURSE_POINTS];

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xTrkTrack(Parser *pf)
{
    tracks[num_tracks].num_points = 0;
    sprintf(tracks[num_tracks].name,"%s",pf->GetWord());
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xTrkEndTrack(Parser *pf)
{
    course_point *cp;
    RwInt32 id = 0;
    int i;
    COURSE_POINT *p1,*p2,*p3;
    RwReal hdg1,hdg2;
    RwReal hdg_factor=1.0f, spd_factor=0;

    cp = tracks[num_tracks].course_points = (course_point*)RwMalloc(tracks[num_tracks].num_points*sizeof(course_point));

    if(cp)
    {
        memcpy(tracks[num_tracks].course_points, curr_track, tracks[num_tracks].num_points*sizeof(course_point));

        for(i=0;i<tracks[num_tracks].num_points;i++)
        {
            /* Set desirred heading going through each point    */
            p1 = &cp[cp[id].id];
            p2 = &cp[cp[id].next_id];
            id = cp[id].next_id;
            p3 = &cp[cp[id].next_id];

            hdg1 = (RwReal)MeAtan2(p2->pos.y - p1->pos.y, p2->pos.x - p1->pos.x);
            hdg2 = (RwReal)MeAtan2(p3->pos.y - p2->pos.y, p3->pos.x - p2->pos.x);
            cp[id].hdg = hdg1 + hdg_factor*SmallestAngleRads((RwReal)hdg1, (RwReal)hdg2);

            /* Set desired speed going through each point   */
        }

        num_tracks++;
    }
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xTrkPoint(Parser *pf)
{
    course_point *cp;
    if(tracks[num_tracks].num_points < MAX_COURSE_POINTS)
    {
        cp = &(curr_track[tracks[num_tracks].num_points]);
        cp->id      = pf->GetInt();
        cp->next_id = pf->GetInt();
        cp->pos.x   = (RwReal)pf->GetFloat();
        cp->pos.y   = (RwReal)pf->GetFloat();
        cp->hdg     = 0;
        cp->spd     = (RwReal)pf->GetFloat();
        cp->tol1    = (RwReal)pf->GetFloat();
        cp->tol2    = (RwReal)pf->GetFloat();

        tracks[num_tracks].num_points++;
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
RwInt32 InitialiseCoursePoints(char *fname)
{
    Parser pf;

    num_tracks = 0;

    if(pf.Open(fname))
    {
        pf.SetComTable(trk_comtable);
        pf.Parse();

        if(num_tracks > 0) return 1;
    }

    return(0);

}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsLight(Parser *pf) {
    light_type = rpLIGHTAMBIENT;
    light_pos.x = light_pos.y = light_pos.z = 0.0f;
    light_dir = 0.0f;
    light_pitch = 0.0f;
    light_col.red = light_col.blue = light_col.green = light_col.alpha = 0.0f;
    light_rad = 0.0f;
    light_cone = 0.0f;
    light_flags = 0;
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsWorld(Parser *pf) {
    if(pf->GetInt()) light_flags |= rpLIGHTLIGHTWORLD;
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsAtomics(Parser *pf) {
    if(pf->GetInt()) light_flags |= rpLIGHTLIGHTATOMICS;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsEndLight(Parser *pf) {

    if(num_lights < MAX_LIGHTS) {
        lights[num_lights] = CreateLight(World, light_type, &light_pos, light_dir, light_pitch,
                        &light_col, light_rad , light_cone, light_flags);

        if(lights[num_lights]) num_lights++;
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsType(Parser *pf) {
    pf->GetWord();
    light_type = pf->WordToEnum(light_types);
    if(light_type < 0) light_type = rpLIGHTAMBIENT;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsPosition(Parser *pf) {
    light_pos.x = (RwReal)pf->GetInt();
    light_pos.y = (RwReal)pf->GetInt();
    light_pos.z = (RwReal)pf->GetInt();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsDirection(Parser *pf) {
    light_dir = (RwReal)pf->GetInt();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsRGB(Parser *pf) {
    light_col.red = (RwReal)pf->GetInt()/255.0f;
    light_col.green = (RwReal)pf->GetInt()/255.0f;
    light_col.blue = (RwReal)pf->GetInt()/255.0f;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsAlpha(Parser *pf) {
    light_col.alpha = (RwReal)pf->GetInt()/255.0f;
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsPitch(Parser *pf) {
    light_pitch = (RwReal)pf->GetInt();
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsCone(Parser *pf) {
    light_cone = DegsToRads((RwReal)pf->GetInt());
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xLtsRadius(Parser *pf) {
    light_rad = (RwReal)pf->GetInt();
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
RwInt32 LoadAndInitialiseLighting(const char *fname)
{
    Parser pf;

    num_lights = 0;

    if(pf.Open(fname))
    {
        pf.SetComTable(lts_comtable);
        pf.Parse();
        return 1;
    }

    return(0);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xPCVideoMode(Parser *pf)
{
    game_vars.vm_width = pf->GetInt();
    game_vars.vm_height = pf->GetInt();
    game_vars.vm_depth = pf->GetInt();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgShapeDir(Parser *pf)
{
    sprintf(game_vars.shape_dir,"%s/",pf->GetWord());
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgTexDir(Parser *pf)
{
    sprintf(game_vars.texture_path,"%s/",pf->GetWord());
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgWorldBsp(Parser *pf)
{
    sprintf(game_vars.world_bsp,"%s%s",game_vars.shape_dir,pf->GetWord());
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgLightsFile(Parser *pf)
{
    #ifdef _XBOX
        sprintf(game_vars.lights_file,"D:\\Scripts\\%s",pf->GetWord());
    #else
        sprintf(game_vars.lights_file,"./Scripts/%s",pf->GetWord());
    #endif
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgTracksFile(Parser *pf)
{
    #ifdef _XBOX
        sprintf(game_vars.tracks_file,"D:\\Scripts\\%s",pf->GetWord());
    #else
        sprintf(game_vars.tracks_file,"./Scripts/%s",pf->GetWord());
    #endif
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgDriver(Parser *pf)
{
    if(game_vars.num_cars < MAX_CARS-1)
    {
        #ifdef _XBOX
            sprintf(game_vars.driver_files[game_vars.num_cars],"D:\\Scripts\\%s",pf->GetWord());
        #else
            sprintf(game_vars.driver_files[game_vars.num_cars],"./Scripts/%s",pf->GetWord());
        #endif
        sprintf(game_vars.driver_tracks[game_vars.num_cars],"%s",pf->GetWord());
        game_vars.start_point[game_vars.num_cars] = pf->GetInt();
        game_vars.num_cars++;
    }
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgPlayerCar(Parser *pf)
{
    game_vars.player_car = pf->GetInt();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCfgRearViewMirror(Parser *pf)
{
    game_vars.rear_view_mirror = 1;
    rvm.x_frac = (RwReal)pf->GetFloat();
    rvm.y_frac = (RwReal)pf->GetFloat();
    rvm.w_frac = (RwReal)pf->GetFloat();
    rvm.h_frac = (RwReal)pf->GetFloat();

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
RwInt32 LoadConfigFile(const char *fname)
{
    Parser pf;

    game_vars.num_cars = 0;
    game_vars.player_car = 0;

    if(pf.Open(fname))
    {
        pf.SetComTable(cfg_comtable);
        pf.Parse();
        return 1;
    }

    return(0);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void InitialiseGameCamera()
{
    RwReal ar = (RwReal)RsGlobal.maximumHeight/(RwReal)RsGlobal.maximumWidth;
    RwFrame *c_frame = RwCameraGetFrame(game_cam.cam);


    CameraSetViewFrustrum(game_cam.cam, 60.0f, ar, 0.5f, 2500.0f);  // fov, aspect ratio, near clip, far clip
    RwCameraSetFogDistance(game_cam.cam, 0.0f);
    game_cam.ang_lag = 5.0f;
    game_cam.pos_lag = 3.0f;
    game_cam.pitch  = 0.0f;
    game_cam.dir    = 0.0f;
    game_cam.dist   = 10.0f;

    /* Give camera a default pos*/
    RwFrameTranslate(c_frame, &Worigin, rwCOMBINEREPLACE);
    RwFrameTranslate(c_frame, &cam_def_start_pos, rwCOMBINEPOSTCONCAT);
}

/******************************************************************************
*
*
*
********************************************************************************/
RwBool TextureIsTrack(char *name)
{
    int i;

    for(i=0; i<NUM_TRACK_TEXTURES; i++)
    {
        if(!stricmp(name, track_textures[i]))
        {
            return(TRUE);
        }
    }

    return FALSE;

}

/******************************************************************************
*
*
*
********************************************************************************/
RpMaterial *MatEnumCB(RpMaterial *mat, void *data)
{
    RwTexture *tex;
    char *name;
    int *index;
    static RwUInt8 mat_count = 0;

    tex = RpMaterialGetTexture(mat);
    if(tex)
    {
        name = RwTextureGetName(tex);
        if(TextureIsTrack(name))
        {
            index = (int *)data;
            track_mats[*index] = mat_count; //Store this material as a track material
            (*index)++;
        }
    }

    mat_count++;
    return(mat);
}
/******************************************************************************
*
*
*
********************************************************************************/
void InitialiseMaterials(void)
{
    int i,count=0;
    for(i=0;i<NUM_TRACK_TEXTURES;i++) track_mats[i] = 0xFF;
    RpWorldForAllMaterials(World, MatEnumCB, (void*)&count);
}

/******************************************************************************
*
*
*
********************************************************************************/
RwBool Init3DAndSimulation(void *devParam)
{
    RwChar *fname;

    int i;
    /*
     * Initialize all RenderWare components, specifying the platform dependent
     * output device, path separator and VRML write destination directory...
     */

#ifdef _XBOX
    if(!LoadConfigFile("D:\\Scripts\\grmonkey.cfg"))
    {
        return FALSE;
    }
#else
    if(!LoadConfigFile("./Scripts/grmonkey.cfg"))
    {
        return FALSE;
    }
#endif

    if(RsRwInitialize(devParam))
    {
        /* We want to read textures from image files of BMP format...*/
    	RwImageRegisterImageFormat(RWSTRING("bmp"), RtBMPImageRead, RtBMPImageWrite);
		/* Create a raster charset for displaying on-screen information */
        Charset = RtCharsetCreate(&col_RED, &BackgroundColor);
        if(Charset == NULL )
        {
            RsErrorMessage(RWSTRING("Cannot create raster charset."));
            return FALSE;
        }

        /* Register path to find textures   */
        fname = RsPathnameCreate(RWSTRING(game_vars.texture_path));

        RwImageSetPath(fname);

        RsPathnameDestroy(fname);

        /* Create world */
		
       fname = RsPathnameCreate(RWSTRING(game_vars.world_bsp));

        if(!CreateWorldFromTableBSP(fname))
        {
			RsErrorMessage(RWSTRING("World Not created"));
            RsPathnameDestroy(fname);
            return FALSE;
        }
        RsPathnameDestroy(fname);

        LoadAndInitialiseLighting(game_vars.lights_file); //

        InitialiseSmoke();

        if(!InitialiseCoursePoints(game_vars.tracks_file))
        {
            RsErrorMessage(RWSTRING("No Tracks Loaded"));
            RsPathnameDestroy(fname);
            return FALSE;
        }

        //printf("Num Lights %d\n",num_lights);
    #ifdef _XBOX
        if(!LoadConfigFile("D:\\Scripts\\grmonkey.cfg"))
        {
         return FALSE;
        }
    #else
        if(!LoadConfigFile("./Scripts/grmonkey.cfg"))
        {
            return FALSE;
        }
    #endif
        
        if(CreateCameras())
        {

            InitialiseCarSim(World);    //Set up Me world and bodies

#ifdef USE_SOUND
            /* set up sound system */
            InitSoundSystem();
            int v8sound = LoadWavFromFile("Samples/v8engine.wav");
            int idle = LoadWavFromFile("Samples/idle.wav");
            int jet = LoadWavFromFile("Samples/jet.wav");
            int skid = LoadWavFromFile("Samples/loopskid.wav");

            //float pos[3] = {0.0, 0.0, 0.0};
            //float vel[3] = {0.0, 0.0, 0.0};
            //PlayWav(v8sound, 10, pos, vel, 1);

#endif // USE_SOUND


            drivers = (Driver**)malloc(sizeof(Driver*)*game_vars.num_cars);
            for(i=0; i<game_vars.num_cars; i++)
            {

                drivers[i] = new Driver;
                if(!drivers[i]) return FALSE;
            }

            if(game_vars.player_car && game_vars.player_car <= game_vars.num_cars)
            {
                player = drivers[game_vars.player_car-1];
                player->SetControlMethod(PLAYER_CONTROL);
            }
            else
            {
                player = (Driver *)NULL;
                game_vars.player_car = 0; // if player car is invalid
            }

            InitialiseFrameRate();
            InitialisePerformanceTimer();

#ifdef RWMETRICS
		    RsMetricsOpen(game_cam.cam);
#endif /* RWMETRICS */

            InitialiseGameCamera();

            InitialiseMaterials();

            InitialiseMeterArc(&speedo, (int)(0.9125f*RsGlobal.maximumWidth), (int)(0.9083f*RsGlobal.maximumHeight), (int)(0.05f*RsGlobal.maximumWidth), 350, (int)(0.0125f*RsGlobal.maximumWidth), 150);
            SetMeterArc(&speedo, 0);

            InitialiseMeterArc(&tacho, (int)(0.7875f*RsGlobal.maximumWidth), (int)(0.9083f*RsGlobal.maximumHeight), (int)(0.05f*RsGlobal.maximumWidth), 270, (int)(0.0125f*RsGlobal.maximumWidth), 8500);
            SetMeterArc(&tacho, 0);

            UpdateAllShapes();

//			McdSpaceEndChanges(mcd_space);

            RwResourcesSetArenaSize(1024 * 1024 * 8);

            if(game_vars.rear_view_mirror)
            {
                if(CreateRearViewMirror())
                {
                    RwV3d c_pos = {0.0f, 1.0f, 0.0f};
                    RwMatrixSetIdentity(&rvm.offset);
                    RwMatrixRotate(&rvm.offset, RwMatrixGetUp(&rvm.offset), -90, rwCOMBINEREPLACE);
                    RwMatrixTranslate(&rvm.offset, &c_pos, rwCOMBINEPOSTCONCAT);
                }
            }

#ifdef RWLOGO
            RpLogoSetPosition(rpLOGOTOPRIGHT);
#endif
            /* Set up initile camera    */
            if(game_vars.player_car) {
                game_cam.view_driver = game_vars.player_car-1;
            } else {
                game_cam.view_driver = 0;
            }

            ResetCamera(CM_FOLLOW, drivers[game_cam.view_driver]->GetChassisClump());
            //ResetCamera(CM_FIXED_TO_CAR, drivers[game_cam.view_driver]->GetChassisClump());

#ifdef _DEBUG_WINDOW
            InitDebugStream(4, 3.0f);
//          InitDebugChannel(0, 0.0f, 0.05f, &col_WHITE, "FRAME_TIME");
//          InitDebugChannel(1, 0.0f, 0.05f, &col_RED, "RENDER_TIME");
//          InitDebugChannel(2, 0.0f, 0.05f, &col_GREEN, "COLLISION_TIME");
//          InitDebugChannel(3, 0.0f, 0.05f, &col_YELLOW, "DYNAMICS_TIME");
            InitDebugChannel(0, -1.0f, 1.0f, &col_WHITE,    "P INPUT");
            InitDebugChannel(1, -1.0f, 1.0f, &col_RED,      "WHEEL ANGLE");
            InitDebugChannel(2, -1.0f, 1.0f, &col_GREEN,    "D INPUT");
            InitDebugChannel(3, -1.0f, 1.0f, &col_YELLOW,   "SLIP INPUT");

#endif
            return TRUE;
        }
    }

    return FALSE;
}
