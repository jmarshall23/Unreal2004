/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/04/04 15:29:39 $ $Revision: 1.45.2.6 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#define DO_BENCHMARK_TXT 0

/* OpenGL Back-end rendering */

#ifdef WITH_OPENGL

#include <stdlib.h>

#include <MeProfile.h>
#include <MeVersion.h>
#include <MeString.h>
#include <MeSimpleFile.h>

#ifdef PS2
#   include <GL/ps2gl.h>
#endif

#include "Render_ogl.h"

#ifdef _ME_API_DOUBLE
#define ME_GL_REAL GL_DOUBLE
#else
#define ME_GL_REAL GL_FLOAT
#endif

#ifdef PS2
void PS2_ReadPad();
#endif

void OGL_SetWindowTitle(RRender* rc, const char* title)
{
#ifndef PS2
    if (!(title && *title)) {
        MeWarning(0, "OGL_SetWindowTitle: You need to pass a valid char* in title.");
        return;
    }

/*    snprintf(gOGL.m_strWindowTitle, MAX_TITLE_LENGTH - 1
        , "%s - %s (v%s) [OpenGL / GLUT]", title, ME_PRODUCT_NAME, ME_VERSION_STRING);*/
        snprintf(gOGL.m_strWindowTitle, MAX_TITLE_LENGTH - 1
        , "%s (v %s) [OpenGL]", ME_PRODUCT_NAME, ME_VERSION_STRING);

    gOGL.m_strWindowTitle[MAX_TITLE_LENGTH - 1] = '\0';

    strncpy(rc->m_AppName, title, sizeof(rc->m_AppName));
    rc->m_AppName[sizeof(rc->m_AppName) -1] = '\0';

    glutSetWindowTitle(gOGL.m_strWindowTitle);
#endif
}

#if DO_BENCHMARK_TXT
int file;
#endif

/* This is called as RRun by application */
void OGL_RunApp(RRender*rc, RMainLoopCallBack func, void *userdata)
{
    gOGL.callbackfunc = func;
    gOGL.userdata = userdata;

    OGL_PreMainLoopInit();

#if DO_BENCHMARK_TXT
    file=MeOpen("benchmark.txt",kMeOpenModeWRONLY);
#endif

    OGL_glutMainLoop();

    /* The following will not be called because currently
       OGL_glutMainLoop() does not return.... */
    OGL_PostMainLoopCleanup();
}


/* This is registered as GLUT's display callback */
void OGL_DisplayFunc(void)
{
    MeProfileTimerResult start;

#ifdef PS2
    PS2_ReadPad();
#endif

    /* While glutMainLoop() doesn't return, we need to put
       exit code here */
    if( gOGL.m_bQuit )
    {
        OGL_PostMainLoopCleanup();
        exit(0);
    }

/* Can include this if you want... */
#if 0
/* Get rid of close button */
#if defined(WIN32)
    {
        HWND hWnd = GetForegroundWindow();
        LONG l = GetWindowLong(hWnd, GWL_STYLE);
        l &= ~WS_SYSMENU;
        SetWindowLong(hWnd, GWL_STYLE, l);
    }
#endif
#endif

    /* swap to the front-buffer */
    glFlush();
    glutSwapBuffers();

    MeProfileGetTimerValue(&start);

    MeProfileStartFrame();

    if( !gOGL.rc->m_bPause || gOGL.m_bDoStep )
    {
        gOGL.m_bDoStep = 0;

        /* callback */
        if( gOGL.callbackfunc )
            gOGL.callbackfunc( gOGL.rc, gOGL.userdata );
    }

    MeProfileStartSection("Rendering", 0);

    /* update matrices */
    RRenderUpdateGraphicMatrices(gOGL.rc);

    OGL_DrawFrame();

    /* see if we have to quit next time */
    if( gOGL.rc->m_bQuitNextFrame )
        gOGL.m_bQuit = 1;

    MeProfileEndSection("Rendering");

#ifndef PS2
    MeProfileStartSection("Idle Time",0);
    if( gOGL.m_bIsFrameLocked )
    {
        MeProfileTimerResult timerResult;
       	MeProfileGetTimerValue(&timerResult);
        while( (timerResult.cpuCycles - start.cpuCycles)
	    	< gOGL.m_uiTicksPerFrame )
        {
            MeProfileGetTimerValue(&timerResult);
        }
    }
    MeProfileEndSection("Idle Time");
#endif

#ifndef PS2
    if( gOGL.m_bDisplayFps )
        OGL_fpsCalc(start.cpuCycles);
#endif

    MeProfileStopTimers();
    MeProfileEndFrame();

    {
        AcmeReal coltime, dyntime, rentime, idletime;
#if DO_BENCHMARK_TXT
        int count;
        char buf[256];
#endif

        coltime = MeProfileGetSectionTime("Collision");
        dyntime = MeProfileGetSectionTime("Dynamics");
        rentime = MeProfileGetSectionTime("Rendering");
        idletime = MeProfileGetSectionTime("Idle Time");

        if (gOGL.rc->m_bProfiling != kMeProfileLogTotals)
            RPerformanceBarUpdate(gOGL.rc,
                coltime,dyntime,rentime,idletime);
        else
        {
            static AcmeReal colprev = 0.0f, dynprev = 0.0f;
            static AcmeReal renprev = 0.0f, idleprev = 0.0f;
            const AcmeReal
                colthis = coltime-colprev,
                dynthis = dyntime-dynprev,
                renthis = rentime-renprev,
                idlethis = idletime-idleprev;

            RPerformanceBarUpdate(gOGL.rc,
                colthis,dynthis,renthis,idlethis);

            colprev = coltime; dynprev = dyntime;
            renprev = rentime; idleprev = idletime;
        }

#if DO_BENCHMARK_TXT
        count = sprintf(buf,"%f\n",MeProfileGetSectionTime("Dynamics"));
        MeWrite(file,buf,count);
#endif
    }

    glutPostRedisplay();
}


/* This is registered as GLUT's reshape callback */
void OGL_ReshapeFunc(int width, int height)
{
    gOGL.m_width = width;
    gOGL.m_height = height;
    /* set up GL's viewport appropriately */
    if( gOGL.m_bLockAspectRatio )
    {
        gOGL.rc->m_AspectRatio = (float)640/(float)448;

        RRenderUpdateProjectionMatrices( gOGL.rc );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        APPEND_F_OR_D(glMultMatrix)((MeReal*)gOGL.rc->m_ProjMatrix);

        if( width > (height * gOGL.rc->m_AspectRatio) )
            glViewport(0.5f*(width-height*(float)gOGL.rc->m_AspectRatio),
                0.0f,
                height*(float)gOGL.rc->m_AspectRatio,
                height);
        else
            glViewport(0.0f,
                0.5f*(height - width/(float)gOGL.rc->m_AspectRatio),
                width,
                width/(float)gOGL.rc->m_AspectRatio);
    }
    else
    {
        gOGL.rc->m_AspectRatio = (float)(width)/(float)(height);

        RRenderUpdateProjectionMatrices( gOGL.rc );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        APPEND_F_OR_D(glMultMatrix)((MeReal*)gOGL.rc->m_ProjMatrix);

        glViewport(0,0,width,height);
    }
#ifndef PS2
    glDepthRange(0.0f,1.0f);
#endif
    glutPostRedisplay();
}


void OGL_fpsCalc(MeU64 starttime)
{
    AcmeReal fFPS = 0.0f;
    static int nFrames  = 0L;
    int cyclesdiff;
    double tdiff;
    MeProfileTimerResult ctTime;
   
    MeProfileGetTimerValue(&ctTime);

    ++nFrames;

    cyclesdiff = (int)(ctTime.cpuCycles - starttime);
    tdiff = (double)cyclesdiff/(double)(gOGL.m_uiTicksPerSec);
    if( tdiff == 0.0f )
        tdiff = 0.0001f;
    fFPS = 1.0f / tdiff;

    //if( nFrames > gOGL.m_nFpsUpdate )
    //{
        RRenderDisplayFps( gOGL.rc, fFPS );
        nFrames = 0L;
    //}
}

void OGL_ToggleFullScreen(void)
{
    static int oldwidth = 640;
    static int oldheight = 448;

    gOGL.m_bIsFullscreen = !gOGL.m_bIsFullscreen;

    #if defined LINUX && defined FX
    if (fullscreen)
        XMesaSetFXmode(XMESA_FX_FULLSCREEN);
    else
        XMesaSetFXmode(XMESA_FX_WINDOW);
    #else
    if (!gOGL.m_bIsFullscreen)
    {
#ifndef PS2
        glutReshapeWindow(oldwidth, oldheight);
#endif
        gOGL.m_width = oldwidth;
        gOGL.m_height = oldheight;
    }
    else
    {
        oldwidth = gOGL.m_width;
        oldheight = gOGL.m_height;
#ifndef PS2
        glutFullScreen();
#endif
    }
    #endif
}

/* Called immediately before main render loop */
void OGL_PreMainLoopInit()
{
    GLfloat ambient[4];

    if (OGL_LoadTextures())
        ME_REPORT(MeWarning(1,"Error loading textures."));

    glClearColor(gOGL.rc->m_backgroundColour[0], 
        gOGL.rc->m_backgroundColour[1],
        gOGL.rc->m_backgroundColour[2], 0.0f);

#ifndef PS2
    glClearDepth(1.0f);
#endif

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

#ifdef PS2
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DITHER);
    glDisable(GL_CULL_FACE);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_COLOR_MATERIAL);

#ifdef PGL_CLIPPING
    pglDisable(PGL_CLIPPING);
#endif
#else
    glEnable(GL_DEPTH_TEST);
#endif

    ambient[0] = ambient[1] = ambient[2] = ambient[3] = 0.0f;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    APPEND_F_OR_D(glMultMatrix)((MeReal*)gOGL.rc->m_ProjMatrix);

    gOGL.rc->m_bForceDirectLight1Update = 1;
    gOGL.rc->m_bForceDirectLight2Update = 1;
    gOGL.rc->m_bForcePointLightUpdate = 1;
}

/* Called just before shutdown */
void OGL_PostMainLoopCleanup()
{
    OGL_FreeTextures();

    if(gOGL.rc->m_bProfiling)
        MeProfileOutputResults();
    MeProfileStopTiming();
}

void OGL_DrawParticleList(MeBool bWasLinearFilter)
{
#if 0
    /* Render Particle Systems */
    if( gOGL.m_bDisplayPSystems )
    {
        RParticleSystem *ps = gOGL.rc->m_pPS_First;
        MeVector3 *curr_pos;
        AcmeReal x,y,z,x1,y1,z1;
        int i;

        glDepthFunc(GL_LEQUAL);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

#ifdef PS2
        glDisable(GL_ALPHA_TEST);
#else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GEQUAL, 0.5f);
#endif

        while( ps )
        {
            glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat *)ps->m_rgbAmbient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat *)ps->m_rgbDiffuse);
            glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat *)ps->m_rgbEmissive);
#ifndef PS2
            glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat *)ps->m_rgbSpecular);
            glMaterialf(GL_FRONT, GL_SHININESS, 40.0f);
#endif

            if (ps->m_nTextureID > -1
                && ps->m_nTextureID < 25 && gOGL.m_bAllowTextures
            )
            {
                glEnable(GL_TEXTURE_2D);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glBindTexture(GL_TEXTURE_2D,
                    gOGL.m_glTexNames[ps->m_nTextureID]);
                if( bWasLinearFilter != gOGL.m_bUseLinearFilter )
                {
                    if( gOGL.m_bUseLinearFilter )
                    {
                        glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    }
                    else
                    {
                        glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    }
                }
            }
            else
                glDisable(GL_TEXTURE_2D);

            /* loop through particle positions */
            curr_pos = ps->m_Positions;
            for( i = 0; i < ps->m_nNumParticles; i++ )
            {
                x = (*curr_pos)[0];
                y = (*curr_pos)[1];
                z = (*curr_pos)[2];
                curr_pos++;

                x1 = x*gOGL.rc->m_CamMatrix[0][0] +
                    y*gOGL.rc->m_CamMatrix[1][0] +
                    z*gOGL.rc->m_CamMatrix[2][0] +
                    gOGL.rc->m_CamMatrix[3][0];
                y1 = x*gOGL.rc->m_CamMatrix[0][1] +
                    y*gOGL.rc->m_CamMatrix[1][1] +
                    z*gOGL.rc->m_CamMatrix[2][1] +
                    gOGL.rc->m_CamMatrix[3][1];
                z1 = x*gOGL.rc->m_CamMatrix[0][2] +
                    y*gOGL.rc->m_CamMatrix[1][2] +
                    z*gOGL.rc->m_CamMatrix[2][2] +
                    gOGL.rc->m_CamMatrix[3][2];

                /* create triangle */
                glBegin(GL_TRIANGLES);

                glNormal3f(0.0f, 0.0f, -1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f( (float)(x1 - 0.5f*ps->m_TriangleSize), (float)(y1 - 0.5f*ps->m_TriangleSize), (float)z1 );

                glNormal3f(0.0f, 0.0f, -1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f( (float)(x1 + 0.5f*ps->m_TriangleSize), (float)(y1 - 0.5f*ps->m_TriangleSize), (float)z1 );

                glNormal3f(0.0f, 0.0f, -1.0f);
                glTexCoord2f(1.0f, 0.5f);
                glVertex3f( (float)x1, (float)(y1 + 0.5f*ps->m_TriangleSize), (float)z1 );

                glEnd();
            }

            ps = ps->m_pNextSystem;
        }

#ifndef PS2
        glDisable(GL_ALPHA_TEST);
#endif

    }
#endif

}

__inline void drawArrays(RGraphic* rg)
{
#ifndef PS2
    glVertexPointer  (3,ME_GL_REAL,sizeof(RObjectVertex),&rg->m_pVertices->m_X);
    glNormalPointer  (  ME_GL_REAL,sizeof(RObjectVertex),&rg->m_pVertices->m_NX);
    glTexCoordPointer(2,ME_GL_REAL,sizeof(RObjectVertex),&rg->m_pVertices->m_U);
#else
    glVertexPointer  (3,ME_GL_REAL,0,rg->m_pVertexArray);
    glNormalPointer  (  ME_GL_REAL,0,rg->m_pNormalArray);
    glTexCoordPointer(2,ME_GL_REAL,0,rg->m_pTexCoordArray);
#endif
    
    glDrawArrays(GL_TRIANGLES, 0, rg->m_pObject->m_nNumVertices);
}

void OGL_Draw3DList(MeBool bWasLinearFilter)
{
    RGraphic *currentRG;

    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    APPEND_F_OR_D(glMultMatrix)((MeReal*)gOGL.rc->m_CamMatrix);

    currentRG = gOGL.rc->m_pRG_First;

    while( currentRG )
    {
        glPushMatrix();

#ifndef PS2
        if ((currentRG->m_pObject->m_bIsWireFrame && gOGL.m_bAllowWireFrame)
            || (gOGL.m_bForceWireFrame)
        )
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

        glMaterialfv(GL_FRONT, GL_AMBIENT,
            (GLfloat *)currentRG->m_pObject->m_ColorAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,
            (GLfloat *)currentRG->m_pObject->m_ColorDiffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,
            (GLfloat *)currentRG->m_pObject->m_ColorEmissive);
#ifndef PS2
        glMaterialfv(GL_FRONT, GL_SPECULAR,
            (GLfloat *)currentRG->m_pObject->m_ColorSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS,
            (GLfloat)currentRG->m_pObject->m_SpecularPower);
#endif
        
        if( currentRG->m_pObject->m_nTextureID > -1
            && currentRG->m_pObject->m_nTextureID < 25
            && gOGL.m_bAllowTextures )
        {
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBindTexture(GL_TEXTURE_2D,
                gOGL.m_glTexNames[currentRG->m_pObject->m_nTextureID]);
            
            if( gOGL.m_bUseLinearFilter )
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }
        
        APPEND_F_OR_D(glMultMatrix)((MeReal*)currentRG->m_pObject->m_Matrix);
        /* Display lists cannot hold VertexPointer, etc. calls ! */
#if 0
        if(currentRG->m_DisplayList!=-1) 
            glCallList(currentRG->m_DisplayList);
        else
#else
            drawArrays(currentRG);
#endif
        glPopMatrix();
        
        currentRG = currentRG->m_pNext;
    }
}

void OGL_Draw2DList(MeBool bWasLinearFilter)
{
    RGraphic *currentRG;
    GLfloat ambient[4];

    if( gOGL.m_bDisplay2D )
    {

        /* Render 2D list */
#ifndef PS2
        glDepthFunc(GL_ALWAYS);
#endif
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        APPEND_F_OR_D(glMultMatrix)((MeReal*)gOGL.rc->m_CamMatrix2D);

        ambient[0] = 1.0f;
        ambient[1] = 1.0f;
        ambient[2] = 1.0f;
        ambient[3] = 1.0f;
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);

#ifdef PS2
        glDisable(GL_BLEND);
#else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
        glEnable(GL_BLEND);
#endif

        currentRG = gOGL.rc->m_pRG_First2D;
        while( currentRG )
        {
            glPushMatrix();
            APPEND_F_OR_D(glMultMatrix)((MeReal*)currentRG->m_pObject->m_Matrix);

            glMaterialfv(GL_FRONT, GL_AMBIENT,
                (GLfloat *)currentRG->m_pObject->m_ColorAmbient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE,
                (GLfloat *)currentRG->m_pObject->m_ColorDiffuse);
            glMaterialfv(GL_FRONT, GL_EMISSION,
                (GLfloat *)currentRG->m_pObject->m_ColorEmissive);
#ifndef PS2
            glMaterialfv(GL_FRONT, GL_SPECULAR,
                (GLfloat *)currentRG->m_pObject->m_ColorSpecular);
            glMaterialf(GL_FRONT, GL_SHININESS,
                (GLfloat)currentRG->m_pObject->m_SpecularPower);
#endif
 
            if (currentRG->m_pObject->m_nTextureID > -1
                && currentRG->m_pObject->m_nTextureID < 25
                && gOGL.m_bAllowTextures)
            {

                glEnable(GL_TEXTURE_2D);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glBindTexture(GL_TEXTURE_2D, gOGL.m_glTexNames[currentRG->m_pObject->m_nTextureID]);

                if( gOGL.m_bUseLinearFilter )
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                }
                else
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                }
            }
            else
            {
                glDisable(GL_TEXTURE_2D);
            }
        /* Display lists cannot hold VertexPointer, etc. calls ! */
#if 0
        if(currentRG->m_DisplayList!=-1) 
            glCallList(currentRG->m_DisplayList);
        else
#else
            drawArrays(currentRG);
#endif
            glPopMatrix();
            currentRG = currentRG->m_pNext;
        }

#ifndef PS2
        glDisable(GL_BLEND);
#endif
        
    } /* end if 2D */
    
}

void OGL_DrawLineList(MeBool bWasLinearFilter)
{
    RNativeLine *currentLine;
    RRender *rc;

    rc = gOGL.rc;
    
    glDepthFunc(GL_LEQUAL);
    
    glMatrixMode(GL_MODELVIEW);
    
    currentLine = rc->m_pNL_First;
    
    while( currentLine )
    {
        glPushMatrix();
        
#ifdef PS2
        glColor3f(currentLine->m_color[0],currentLine->m_color[1],
            currentLine->m_color[2]);
#else
        glColor4f(currentLine->m_color[0],currentLine->m_color[1],
            currentLine->m_color[2],currentLine->m_color[3]);
#endif
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        
        glMultMatrixf((AcmeReal*)currentLine->m_matrix);
        
        glBegin(GL_LINES);
        glVertex3f(currentLine->m_vectors->m_start[0],
            currentLine->m_vectors->m_start[1],
            currentLine->m_vectors->m_start[2]);
        glVertex3f(currentLine->m_vectors->m_end[0],
            currentLine->m_vectors->m_end[1],
            currentLine->m_vectors->m_end[2]);
        glEnd();
        
        glPopMatrix();
        
        currentLine = currentLine->m_next;
    }
    glEnable(GL_LIGHTING);
}

/* This is where we do the drawing */
void OGL_DrawFrame()
{
    GLfloat ambient[4], diffuse[4], specular[4], position[4];
    static MeBool bWasLinearFilter = 1;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    APPEND_F_OR_D(glMultMatrix)((MeReal*)gOGL.rc->m_CamMatrix);

    /*lighting */
    ambient[0] = gOGL.rc->m_rgbAmbientLightColor[0];
    ambient[1] = gOGL.rc->m_rgbAmbientLightColor[1];
    ambient[2] = gOGL.rc->m_rgbAmbientLightColor[2];
    ambient[3] = 1.0f;
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    if( gOGL.rc->m_bForceDirectLight1Update )
    {
        ambient[0] = gOGL.rc->m_DirectLight1.m_rgbAmbient[0];
        ambient[1] = gOGL.rc->m_DirectLight1.m_rgbAmbient[1];
        ambient[2] = gOGL.rc->m_DirectLight1.m_rgbAmbient[2];
        ambient[3] = 1.0f;
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

        diffuse[0] = gOGL.rc->m_DirectLight1.m_rgbDiffuse[0];
        diffuse[1] = gOGL.rc->m_DirectLight1.m_rgbDiffuse[1];
        diffuse[2] = gOGL.rc->m_DirectLight1.m_rgbDiffuse[2];
        diffuse[3] = 1.0f;
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

#ifndef PS2
        specular[0] = gOGL.rc->m_DirectLight1.m_rgbSpecular[0];
        specular[1] = gOGL.rc->m_DirectLight1.m_rgbSpecular[1];
        specular[2] = gOGL.rc->m_DirectLight1.m_rgbSpecular[2];
        specular[3] = 1.0f;
        glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
#endif

        gOGL.rc->m_bForceDirectLight1Update = 0;
    }

    if( gOGL.rc->m_bForceDirectLight2Update )
    {
        ambient[0] = gOGL.rc->m_DirectLight2.m_rgbAmbient[0];
        ambient[1] = gOGL.rc->m_DirectLight2.m_rgbAmbient[1];
        ambient[2] = gOGL.rc->m_DirectLight2.m_rgbAmbient[2];
        ambient[3] = 1.0f;
        glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);

        diffuse[0] = gOGL.rc->m_DirectLight2.m_rgbDiffuse[0];
        diffuse[1] = gOGL.rc->m_DirectLight2.m_rgbDiffuse[1];
        diffuse[2] = gOGL.rc->m_DirectLight2.m_rgbDiffuse[2];
        diffuse[3] = 1.0f;
        glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);

#ifndef PS2
        specular[0] = gOGL.rc->m_DirectLight2.m_rgbSpecular[0];
        specular[1] = gOGL.rc->m_DirectLight2.m_rgbSpecular[1];
        specular[2] = gOGL.rc->m_DirectLight2.m_rgbSpecular[2];
        specular[3] = 1.0f;
        glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
#endif

        gOGL.rc->m_bForceDirectLight2Update = 0;
    }

    if( gOGL.rc->m_bForcePointLightUpdate )
    {
        ambient[0] = gOGL.rc->m_PointLight.m_rgbAmbient[0];
        ambient[1] = gOGL.rc->m_PointLight.m_rgbAmbient[1];
        ambient[2] = gOGL.rc->m_PointLight.m_rgbAmbient[2];
        ambient[3] = 1.0f;
        glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);

        diffuse[0] = gOGL.rc->m_PointLight.m_rgbDiffuse[0];
        diffuse[1] = gOGL.rc->m_PointLight.m_rgbDiffuse[1];
        diffuse[2] = gOGL.rc->m_PointLight.m_rgbDiffuse[2];
        diffuse[3] = 1.0f;
        glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse);

#ifndef PS2
        specular[0] = gOGL.rc->m_PointLight.m_rgbSpecular[0];
        specular[1] = gOGL.rc->m_PointLight.m_rgbSpecular[1];
        specular[2] = gOGL.rc->m_PointLight.m_rgbSpecular[2];
        specular[3] = 1.0f;
        glLightfv(GL_LIGHT3, GL_SPECULAR, specular);
#endif

        glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION,
            gOGL.rc->m_PointLight.m_AttenuationConstant);
        glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION,
            gOGL.rc->m_PointLight.m_AttenuationLinear);
        glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION,
            gOGL.rc->m_PointLight.m_AttenuationQuadratic);

        gOGL.rc->m_bForcePointLightUpdate = 0;
    }

    if( gOGL.rc->m_bUseAmbientLight )
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);

    if( gOGL.rc->m_DirectLight1.m_bUseLight )
    {
        position[0] = -gOGL.rc->m_DirectLight1.m_Direction[0];
        position[1] = -gOGL.rc->m_DirectLight1.m_Direction[1];
        position[2] = -gOGL.rc->m_DirectLight1.m_Direction[2];
        position[3] = 0.0f;
        glLightfv(GL_LIGHT1, GL_POSITION, position);
        glEnable(GL_LIGHT1);
    }
    else
        glDisable(GL_LIGHT1);

    if( gOGL.rc->m_DirectLight2.m_bUseLight )
    {
        position[0] = -gOGL.rc->m_DirectLight2.m_Direction[0];
        position[1] = -gOGL.rc->m_DirectLight2.m_Direction[1];
        position[2] = -gOGL.rc->m_DirectLight2.m_Direction[2];
        position[3] = 0.0f;
        glLightfv(GL_LIGHT2, GL_POSITION, position);
        glEnable(GL_LIGHT2);
    }
    else
        glDisable(GL_LIGHT2);

    if( gOGL.rc->m_PointLight.m_bUseLight )
    {
        position[0] = gOGL.rc->m_PointLight.m_Position[0];
        position[1] = gOGL.rc->m_PointLight.m_Position[1];
        position[2] = gOGL.rc->m_PointLight.m_Position[2];
        position[3] = 1.0f;
        glLightfv(GL_LIGHT3, GL_POSITION, position);
        glEnable(GL_LIGHT3);
    }
    else
        glDisable(GL_LIGHT3);

    OGL_DrawParticleList(bWasLinearFilter);
    OGL_DrawLineList(bWasLinearFilter);    
    OGL_Draw3DList(bWasLinearFilter);
    OGL_Draw2DList(bWasLinearFilter);

    bWasLinearFilter = gOGL.m_bUseLinearFilter;

#ifdef _DEBUG
#ifdef PGL_CLIPPING
    {
        static unsigned first = 1;

        if (first)
        {
            MeDebug(0,"*** current renderer '%s'",pglGetCurRendererName());
            first = 0;
        }
    }
#endif
#endif
}

void OGL_glutMainLoop()
{
    /* Ideally this would behave like the glutMainLoop, but return. however,
     * it just calls glutMainLoop() at the moment :(
     */

    glutMainLoop();
}

void MEAPI RGraphicCalcOGLDisplayList(RGraphic *rg)
{

#ifdef PS2
    int vert;

    for(vert=0;vert!=rg->m_pObject->m_nNumVertices;vert++)
    {
        rg->m_pVertexArray[vert*3+0] = rg->m_pVertices[vert].m_X;
        rg->m_pVertexArray[vert*3+1] = rg->m_pVertices[vert].m_Y;
        rg->m_pVertexArray[vert*3+2] = rg->m_pVertices[vert].m_Z;

        rg->m_pNormalArray[vert*3+0] = rg->m_pVertices[vert].m_NX;
        rg->m_pNormalArray[vert*3+1] = rg->m_pVertices[vert].m_NY;
        rg->m_pNormalArray[vert*3+2] = rg->m_pVertices[vert].m_NZ;

        rg->m_pTexCoordArray[vert*2+0] = rg->m_pVertices[vert].m_U;
        rg->m_pTexCoordArray[vert*2+1] = rg->m_pVertices[vert].m_V;
    }        
#endif

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

/* Display lists cannot hold VertexPointer, etc. calls ! */
#if 0
    rg->m_DisplayList = glGenLists(1);
    if( rg->m_DisplayList == 0 )
    {
#ifdef PS2
        ME_REPORT(MeWarning(1, "'glGenLists()' returned zero"));
#endif
        rg->m_DisplayList = -1;
        return;
    }

    glNewList(rg->m_DisplayList, GL_COMPILE);
    {
#ifndef PS2
        glVertexPointer  (3,ME_GL_REAL,sizeof(RObjectVertex),&rg->m_pVertices->m_X);
        glNormalPointer  (  ME_GL_REAL,sizeof(RObjectVertex),&rg->m_pVertices->m_NX);
        glTexCoordPointer(2,ME_GL_REAL,sizeof(RObjectVertex),&rg->m_pVertices->m_U);
#else
        glVertexPointer  (3,ME_GL_REAL,0,rg->m_pVertexArray);
        glNormalPointer  (  ME_GL_REAL,0,rg->m_pNormalArray);
        glTexCoordPointer(2,ME_GL_REAL,0,rg->m_pTexCoordArray);
#endif

        glDrawArrays(GL_TRIANGLES, 0, rg->m_pObject->m_nNumVertices);
    }
    glEndList();
#else
    rg->m_DisplayList = -1;
#endif
}

#endif
