
#include <MeViewer.h>
#include <MeVersion.h>
#include <Render_xbox.h>
#include <xtl.h>
#include "RMouseCam.h"

#define AUTOREPEAT_FRAMES (5)
#define AUTOREPEAT_MIN (10)

#define BUTTON_THRESH XINPUT_GAMEPAD_MAX_CROSSTALK 
#define PAD_CENTRE 0
#define PAD_DEAD_OFFSET (0.24*32768)

#define SCREEN_WIDTH (640)
#define SCREEN_HEIGHT (224)

/* Just for testing */

LPDIRECT3DVERTEXBUFFER8 g_pVB        = NULL; // Buffer to hold vertices

void Xbox_SetWindowTitle(RRender *rc, const char * title)
{
}

HRESULT Xbox_SceneInit(XboxVars* Xbox)
{
    ZeroMemory( &Xbox->m_d3dmtrl, sizeof(D3DMATERIAL8) );
    Xbox->m_d3dmtrl.Power = 40.0f;

    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_LIGHTING, TRUE );
    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_DITHERENABLE, TRUE );
    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_ZENABLE, D3DZB_TRUE );
    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_CULLMODE, D3DCULL_NONE );
    //IDirect3DDevice8_SetRenderState(D3DRS_TEXTUREPERSPECTIVE, TRUE );

    return S_OK;
}

/* Xbox_DrawFrame
 * --------------
 *
 * On Entry:
 *
 * Xbox->rc->m_pRG_First - first in a linked list of RGraphics to render
 *
*/
void Xbox_DrawFrame(XboxVars* Xbox)
{
    HRESULT rval;
    int do2D;
    D3DMATRIX ident = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    D3DMATRIX cam = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1};

    IDirect3DDevice8_Clear(Xbox->m_pd3dDevice,0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                              D3DCOLOR_COLORVALUE(Xbox->rc->m_backgroundColour[0],
                                                  Xbox->rc->m_backgroundColour[1],
                                                  Xbox->rc->m_backgroundColour[2],
                                                  0.0f),1.0f, 0L );
    IDirect3DDevice8_BeginScene(Xbox->m_pd3dDevice);

    /* Set Projection Matrix */
    rval = IDirect3DDevice8_SetTransform(Xbox->m_pd3dDevice, D3DTS_PROJECTION, (const D3DMATRIX *)Xbox->rc->m_ProjMatrix );
    if( rval != D3D_OK )
        ME_REPORT(MeWarning(1,"Failed to set projection matrix."));

    { /* {} for local vars */
        RGraphic *current;
        D3DCOLOR ambcol;
        //BOOL bLightOn;
        //HRESULT hres;

        /* lighting */

        /* Ambient */
        if( Xbox->rc->m_bUseAmbientLight )
            ambcol = D3DCOLOR_COLORVALUE(Xbox->rc->m_rgbAmbientLightColor[0], Xbox->rc->m_rgbAmbientLightColor[1], Xbox->rc->m_rgbAmbientLightColor[2], 1.0f );
        else  /* set color to black */
            ambcol = D3DCOLOR_COLORVALUE(0,0,0,0);

        IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_AMBIENT, ambcol);


        /* First Directional Light */
        //hres = IDirect3DDevice8_GetLightEnable(0, &bLightOn);
        if( Xbox->rc->m_DirectLight1.m_bUseLight )
        {
            /* fill in d3dlight7 struct and enable light */
            D3DVECTOR  dirVec;

            Xbox->m_lgtDir1.Type = D3DLIGHT_DIRECTIONAL;

            dirVec.x = Xbox->rc->m_DirectLight1.m_Direction[0];
            dirVec.y = Xbox->rc->m_DirectLight1.m_Direction[1];
            dirVec.z = Xbox->rc->m_DirectLight1.m_Direction[2];

            Xbox->m_lgtDir1.Direction = dirVec;

            Xbox->m_lgtDir1.Ambient.r = Xbox->rc->m_DirectLight1.m_rgbAmbient[0];
            Xbox->m_lgtDir1.Ambient.g = Xbox->rc->m_DirectLight1.m_rgbAmbient[1];
            Xbox->m_lgtDir1.Ambient.b = Xbox->rc->m_DirectLight1.m_rgbAmbient[2];
            Xbox->m_lgtDir1.Ambient.a = 1.0f;

            Xbox->m_lgtDir1.Diffuse.r = Xbox->rc->m_DirectLight1.m_rgbDiffuse[0];
            Xbox->m_lgtDir1.Diffuse.g = Xbox->rc->m_DirectLight1.m_rgbDiffuse[1];
            Xbox->m_lgtDir1.Diffuse.b = Xbox->rc->m_DirectLight1.m_rgbDiffuse[2];
            Xbox->m_lgtDir1.Diffuse.a = 1.0f;

            Xbox->m_lgtDir1.Specular.r = Xbox->rc->m_DirectLight1.m_rgbSpecular[0];
            Xbox->m_lgtDir1.Specular.g = Xbox->rc->m_DirectLight1.m_rgbSpecular[1];
            Xbox->m_lgtDir1.Specular.b = Xbox->rc->m_DirectLight1.m_rgbSpecular[2];
            Xbox->m_lgtDir1.Specular.a = 1.0f;


            IDirect3DDevice8_SetLight(Xbox->m_pd3dDevice,0, &Xbox->m_lgtDir1);
            IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,0, TRUE);
            Xbox->rc->m_bForceDirectLight1Update = 0;
        }
        else if( !Xbox->rc->m_DirectLight1.m_bUseLight)
        {
            /* disable light */
            IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,0, FALSE);
        }


        /* Second Directional Light */
        //hres = IDirect3DDevice8_GetLightEnable(1, &bLightOn);
        if( Xbox->rc->m_DirectLight2.m_bUseLight )
        {
            /* fill in d3dlight7 struct and enable light */
            D3DVECTOR  dirVec;

            Xbox->m_lgtDir2.Type = D3DLIGHT_DIRECTIONAL;

            dirVec.x = Xbox->rc->m_DirectLight2.m_Direction[0];
            dirVec.y = Xbox->rc->m_DirectLight2.m_Direction[1];
            dirVec.z = Xbox->rc->m_DirectLight2.m_Direction[2];

            Xbox->m_lgtDir2.Direction = dirVec;

            Xbox->m_lgtDir2.Ambient.r = Xbox->rc->m_DirectLight2.m_rgbAmbient[0];
            Xbox->m_lgtDir2.Ambient.g = Xbox->rc->m_DirectLight2.m_rgbAmbient[1];
            Xbox->m_lgtDir2.Ambient.b = Xbox->rc->m_DirectLight2.m_rgbAmbient[2];
            Xbox->m_lgtDir2.Ambient.a = 1.0f;

            Xbox->m_lgtDir2.Diffuse.r = Xbox->rc->m_DirectLight2.m_rgbDiffuse[0];
            Xbox->m_lgtDir2.Diffuse.g = Xbox->rc->m_DirectLight2.m_rgbDiffuse[1];
            Xbox->m_lgtDir2.Diffuse.b = Xbox->rc->m_DirectLight2.m_rgbDiffuse[2];
            Xbox->m_lgtDir2.Diffuse.a = 1.0f;

            Xbox->m_lgtDir2.Specular.r = Xbox->rc->m_DirectLight2.m_rgbSpecular[0];
            Xbox->m_lgtDir2.Specular.g = Xbox->rc->m_DirectLight2.m_rgbSpecular[1];
            Xbox->m_lgtDir2.Specular.b = Xbox->rc->m_DirectLight2.m_rgbSpecular[2];
            Xbox->m_lgtDir2.Specular.a = 1.0f;


            IDirect3DDevice8_SetLight(Xbox->m_pd3dDevice,1, &Xbox->m_lgtDir2);
            IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,1, TRUE);
            Xbox->rc->m_bForceDirectLight2Update = 0;
        }
        else if( !Xbox->rc->m_DirectLight2.m_bUseLight)
        {
            /* disable light */
            IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,1, FALSE);
        }

        /* Point Light */
        //hres = IDirect3DDevice8_GetLightEnable(2, &bLightOn);
        if( Xbox->rc->m_PointLight.m_bUseLight  )
        {
            /* fill in d3dlight7 struct and enable light */
            D3DVECTOR  posVec;

            Xbox->m_lgtPt.Type = D3DLIGHT_POINT;

            posVec.x = Xbox->rc->m_PointLight.m_Position[0];
            posVec.y = Xbox->rc->m_PointLight.m_Position[1];
            posVec.z = Xbox->rc->m_PointLight.m_Position[2];

            Xbox->m_lgtPt.Position = posVec;

            Xbox->m_lgtPt.Ambient.r = Xbox->rc->m_PointLight.m_rgbAmbient[0];
            Xbox->m_lgtPt.Ambient.g = Xbox->rc->m_PointLight.m_rgbAmbient[1];
            Xbox->m_lgtPt.Ambient.b = Xbox->rc->m_PointLight.m_rgbAmbient[2];
            Xbox->m_lgtPt.Ambient.a = 1.0f;

            Xbox->m_lgtPt.Diffuse.r = Xbox->rc->m_PointLight.m_rgbDiffuse[0];
            Xbox->m_lgtPt.Diffuse.g = Xbox->rc->m_PointLight.m_rgbDiffuse[1];
            Xbox->m_lgtPt.Diffuse.b = Xbox->rc->m_PointLight.m_rgbDiffuse[2];
            Xbox->m_lgtPt.Diffuse.a = 1.0f;

            Xbox->m_lgtPt.Specular.r = Xbox->rc->m_PointLight.m_rgbSpecular[0];
            Xbox->m_lgtPt.Specular.g = Xbox->rc->m_PointLight.m_rgbSpecular[1];
            Xbox->m_lgtPt.Specular.b = Xbox->rc->m_PointLight.m_rgbSpecular[2];
            Xbox->m_lgtPt.Specular.a = 1.0f;

            Xbox->m_lgtPt.Attenuation0 = Xbox->rc->m_PointLight.m_AttenuationConstant;
            Xbox->m_lgtPt.Attenuation1 = Xbox->rc->m_PointLight.m_AttenuationLinear;
            Xbox->m_lgtPt.Attenuation2 = Xbox->rc->m_PointLight.m_AttenuationQuadratic;

            Xbox->m_lgtPt.Range = ((float)sqrt(FLT_MAX));

            IDirect3DDevice8_SetLight(Xbox->m_pd3dDevice,2, &Xbox->m_lgtPt);
            IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,2, TRUE);
            Xbox->rc->m_bForcePointLightUpdate = 0;
        }
        else if( !Xbox->rc->m_PointLight.m_bUseLight )
        {
            /* disable light */
            IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,2, FALSE);
        }
        
        /* Render the graphic list */

        do2D = 0;
        current = Xbox->rc->m_pRG_First;
        
        IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        
        if( Xbox->m_bAlphaBlend )
        {
            IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_ALPHABLENDENABLE, TRUE );
            IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
            IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
        }
        else
            IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_ALPHABLENDENABLE, FALSE );
        
        rval = IDirect3DDevice8_SetTransform(Xbox->m_pd3dDevice,D3DTS_VIEW, (D3DMATRIX *)Xbox->rc->m_CamMatrix );
        if( rval != D3D_OK )
            ME_REPORT(MeWarning(1,"Failed to set view matrix."));
        
        while( current )
        {
            CUSTOMVERTEX* pVertices;

            IDirect3DDevice8_SetTransform(Xbox->m_pd3dDevice,D3DTS_WORLD,  (D3DMATRIX *)current->m_pObject->m_Matrix);
            
            Xbox->m_d3dmtrl.Ambient.r = current->m_pObject->m_ColorAmbient[0];
            Xbox->m_d3dmtrl.Ambient.g = current->m_pObject->m_ColorAmbient[1];
            Xbox->m_d3dmtrl.Ambient.b = current->m_pObject->m_ColorAmbient[2];
            Xbox->m_d3dmtrl.Ambient.a = current->m_pObject->m_ColorAmbient[3];
            
            Xbox->m_d3dmtrl.Diffuse.r = current->m_pObject->m_ColorDiffuse[0];
            Xbox->m_d3dmtrl.Diffuse.g = current->m_pObject->m_ColorDiffuse[1];
            Xbox->m_d3dmtrl.Diffuse.b = current->m_pObject->m_ColorDiffuse[2];
            Xbox->m_d3dmtrl.Diffuse.a = current->m_pObject->m_ColorDiffuse[3];
            
            Xbox->m_d3dmtrl.Specular.r = current->m_pObject->m_ColorSpecular[0];
            Xbox->m_d3dmtrl.Specular.g = current->m_pObject->m_ColorSpecular[1];
            Xbox->m_d3dmtrl.Specular.b = current->m_pObject->m_ColorSpecular[2];
            Xbox->m_d3dmtrl.Specular.a = current->m_pObject->m_ColorSpecular[3];
            
            Xbox->m_d3dmtrl.Emissive.r = current->m_pObject->m_ColorEmissive[0];
            Xbox->m_d3dmtrl.Emissive.g = current->m_pObject->m_ColorEmissive[1];
            Xbox->m_d3dmtrl.Emissive.b = current->m_pObject->m_ColorEmissive[2];
            Xbox->m_d3dmtrl.Emissive.a = current->m_pObject->m_ColorEmissive[3];
            
            Xbox->m_d3dmtrl.Power = current->m_pObject->m_SpecularPower;
            
            IDirect3DDevice8_SetMaterial(Xbox->m_pd3dDevice,&Xbox->m_d3dmtrl );
            
            if( (current->m_pObject->m_bIsWireFrame && Xbox->m_bAllowWireFrame) || (Xbox->m_bForceWireFrame && !do2D) )
                IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_FILLMODE, D3DFILL_WIREFRAME );
            else
                IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_FILLMODE, D3DFILL_SOLID );
            
            
            if( (Xbox->m_bUseTextures || do2D) && current->m_pObject->m_nTextureID > -1 && current->m_pObject->m_nTextureID < 25 && !((current->m_pObject->m_bIsWireFrame && Xbox->m_bAllowWireFrame) || (Xbox->m_bForceWireFrame && !do2D ))  )
            {
                if( Xbox->m_bLinearFilter )
                {
                    IDirect3DDevice8_SetTextureStageState(Xbox->m_pd3dDevice,0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
                    IDirect3DDevice8_SetTextureStageState(Xbox->m_pd3dDevice,0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
                }
                else
                {
                    IDirect3DDevice8_SetTextureStageState(Xbox->m_pd3dDevice,0, D3DTSS_MINFILTER, D3DTEXF_POINT);
                    IDirect3DDevice8_SetTextureStageState(Xbox->m_pd3dDevice,0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
                }
                
                if( D3D_OK != IDirect3DDevice8_SetTexture(Xbox->m_pd3dDevice,0, Xbox->m_pddsTexture[current->m_pObject->m_nTextureID] ) )
                    ME_REPORT(MeWarning(1,"Problem setting texture."));
            }
            else
            {
                if( D3D_OK != IDirect3DDevice8_SetTexture(Xbox->m_pd3dDevice,0, NULL ) )
                    ME_REPORT(MeWarning(1,"Problem setting texture to NULL."));
            }
            
            /* Make the vertex buffer from the vertex list */

            IDirect3DVertexBuffer8_Lock(current->m_pVertexBuffer, 0, 0, (BYTE**)&pVertices, 0 );
            
            memcpy( pVertices, current->m_pVertices, current->m_pObject->m_nNumVertices*sizeof(CUSTOMVERTEX) );
            IDirect3DVertexBuffer8_Unlock(current->m_pVertexBuffer);

            /* Draw the vertex buffer */

            IDirect3DDevice8_SetStreamSource( Xbox->m_pd3dDevice,0, current->m_pVertexBuffer, sizeof(CUSTOMVERTEX) );
            IDirect3DDevice8_SetVertexShader( Xbox->m_pd3dDevice,D3DFVF_CUSTOMVERTEX );

            IDirect3DDevice8_DrawPrimitive(Xbox->m_pd3dDevice,D3DPT_TRIANGLELIST, 0, current->m_pObject->m_nNumVertices/3 );
            
            current = current->m_pNext;
            
            if( !do2D && !current && Xbox->m_bDisplay2D )
            {
                do2D = 1;
                current = Xbox->rc->m_pRG_First2D;
                
                rval = IDirect3DDevice8_SetTransform( Xbox->m_pd3dDevice,D3DTS_VIEW, (D3DMATRIX *)Xbox->rc->m_CamMatrix2D);
                if( rval != D3D_OK )
                    ME_REPORT(MeWarning(1,"Failed to set view matrix for 2D."));
                
                IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,0, FALSE);
                IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,1, FALSE);
                IDirect3DDevice8_LightEnable(Xbox->m_pd3dDevice,2, FALSE);
                ambcol = D3DCOLOR_COLORVALUE(1,1,1,1);
                IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_AMBIENT, ambcol);
                
                IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_ZFUNC, D3DCMP_ALWAYS);
                
                
                if( Xbox->m_bAlphaBlend2D )
                {
                    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_ALPHABLENDENABLE, TRUE );
                    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
                    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
                }
                else
                    IDirect3DDevice8_SetRenderState(Xbox->m_pd3dDevice,D3DRS_ALPHABLENDENABLE, FALSE );
            }
        }
    }


}

void Xbox_fpsCalc(XboxVars* Xbox,
                  MeI64 lastFrameStartTime,
                  MeI64 thisFrameStartTime)
{
    AcmeReal fFPS = 0.0f;
    static DWORD dwFrames  = 0L;
    int cyclesdiff;
    double tdiff;

    if( Xbox->m_bDisplayFps )
    {
        ++dwFrames;

        cyclesdiff = (int)(thisFrameStartTime - lastFrameStartTime);
        tdiff = (double)cyclesdiff/(double)(Xbox->m_uiTicksPerSec);
        if( tdiff == 0.0f )
            tdiff = 0.0001f;
        fFPS = 1.0f / (AcmeReal)tdiff;

        if( dwFrames > Xbox->m_dwFpsUpdate )
        {
            RRenderDisplayFps( Xbox->rc, fFPS );
            dwFrames = 0L;
        }
    }
}

HRESULT Xbox_RenderD3D(RRender* rc,RMainLoopCallBack func, void *userdata)
{
    MeProfileTimerResult timerResult;
    AcmeReal coltime, dyntime, rentime;

    MeProfileStartFrame();
    MeProfileGetTimerValue(&timerResult);
    rc->Xbox->m_lastFrameStartTime = rc->Xbox->m_thisFrameStartTime;
    rc->Xbox->m_thisFrameStartTime = timerResult.cpuCycles;

    /* 
     * Run the user's app (which includes calls to collision and physics) 
     *
    */

    Xbox_ReadPad(rc->Xbox);
    func(rc,userdata);

    /* 
     * Render the frame 
     *
    */

    MeProfileStartSection("Rendering", 0);

    RRenderUpdateGraphicMatrices(rc);
    Xbox_DrawFrame(rc->Xbox);
    
    MeProfileEndSection("Rendering");

    /* 
     * Idle time. Wait for vsync then update fps counter
     *
    */

    coltime = MeProfileGetSectionTime("Collision");
    dyntime = MeProfileGetSectionTime("Dynamics");
    rentime = MeProfileGetSectionTime("Rendering");
    RPerformanceBarUpdate(rc, coltime, dyntime, rentime, 0);    
    
    Xbox_fpsCalc(rc->Xbox,rc->Xbox->m_lastFrameStartTime,rc->Xbox->m_thisFrameStartTime);

    MeProfileStopTimers();
    MeProfileEndFrame();

    IDirect3DDevice8_BlockUntilVerticalBlank(rc->Xbox->m_pd3dDevice);
    IDirect3DDevice8_EndScene(rc->Xbox->m_pd3dDevice);
    IDirect3DDevice8_Present(rc->Xbox->m_pd3dDevice,NULL, NULL, NULL, NULL );

    return S_OK;
}

void Xbox_InitPad(XboxVars *Xbox)
{
    DWORD dwDeviceMask;

    XInitDevices( 0, NULL );

    // Get a mask of all currently available devices

    dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );

    while( (dwDeviceMask & (1<<0))==0)
    {
        dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );
    }

    ZeroMemory( &Xbox->m_InputStates[0], sizeof(XINPUT_STATE) );
    ZeroMemory( &Xbox->m_Gamepads[0], sizeof(XBGAMEPAD) );
    if( dwDeviceMask & (1<<0) ) 
    {
        // Get a handle to the device
        Xbox->m_Gamepads[0].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, 0, 
            XDEVICE_NO_SLOT, NULL );
        
        // Store capabilites of the device
        XInputGetCapabilities( Xbox->m_Gamepads[0].hDevice, &Xbox->m_Gamepads[0].caps );
    }
    XInputGetState( Xbox->m_Gamepads[0].hDevice, &Xbox->m_InputStates[0] );
}

/* Nasty hack. Should use RCameraRotate */

void Xbox_CameraRotate(XboxVars *Xbox,int x, int y)
{
    AcmeReal theta;
    AcmeReal phi;
    
    theta = RMouseDrag.m_camstartTheta + 3.5f*( (RMouseDrag.m_camstartX - x) / 640 );
    phi = RMouseDrag.m_camstartPhi + 3.5f*( (y - RMouseDrag.m_camstartY) / 448 );
    
    RCameraSetView( Xbox->rc, Xbox->rc->m_CameraDist, theta, phi );
}

/*
 * Xbox_EmulateMouseButtons
 * ------------------------
 *
 * Xbox doesnt have a mouse, so we map the mouse buttons to the joypad
 *
 * White Xbox button is left mouse button
 * Black Xbox button is right mouse button
 *
 * Middle mouse button is not emulated at the moment
 *
 * First we put the state of the 'mouse buttons' into 'button'
 *
 * We check the state of the buttons from last frame.
 * If a new button has been pressed, then we may be starting a mouse drag
 * If the same button is pressed as last frame, then we are 'dragging' the mouse
 *
 * Map the left stick to the mouse movement
*/
void Xbox_EmulateMouseButtons(XboxVars *Xbox)
{
    RMouseButtonWhich button;
    RMouseButtonEvent event;
    RRender *rc = Xbox->rc;

    /* Which button is being pressed now? */

    if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE]> BUTTON_THRESH )
    {
        button = kRLeftButton;
    }
    else if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]> BUTTON_THRESH)
    {
        button = kRRightButton;
    }
    else
    {
        button = (RMouseButtonWhich)0;
    }

    /* Check the left stick */

    if( (Xbox->m_InputStates[0].Gamepad.sThumbLX < PAD_CENTRE - PAD_DEAD_OFFSET) | 
        (Xbox->m_InputStates[0].Gamepad.sThumbLX > PAD_CENTRE + PAD_DEAD_OFFSET) |
        (Xbox->m_InputStates[0].Gamepad.sThumbLY < PAD_CENTRE - PAD_DEAD_OFFSET) | 
        (Xbox->m_InputStates[0].Gamepad.sThumbLY > PAD_CENTRE + PAD_DEAD_OFFSET)) 
    {
        /* Update the pointer position */
        Xbox->m_mousePointerTM[3][0] += (Xbox->m_InputStates[0].Gamepad.sThumbLX - PAD_CENTRE) >> 12;
        Xbox->m_mousePointerTM[3][1] -= (Xbox->m_InputStates[0].Gamepad.sThumbLY - PAD_CENTRE) >> 12;

        if(Xbox->m_mousePointerTM[3][0] < 0)
            Xbox->m_mousePointerTM[3][0] = 0;
        if(Xbox->m_mousePointerTM[3][0] > SCREEN_WIDTH)
            Xbox->m_mousePointerTM[3][0] = SCREEN_WIDTH;

        if(Xbox->m_mousePointerTM[3][1] < 0)
            Xbox->m_mousePointerTM[3][1] = 0;
        if(Xbox->m_mousePointerTM[3][1] > (SCREEN_HEIGHT << 1))
            Xbox->m_mousePointerTM[3][1] = (SCREEN_HEIGHT << 1);

    } /* Movement */

    /* Has a new button been pressed or are we 'dragging'? */

    if(button && (button != RMouseDrag.m_button)) 
    {
        /* We've overridden a previously pressed button */
        if(RMouseDrag.m_button)
        {
            RExecuteMouseCallback(
                rc,
                (int)Xbox->m_mousePointerTM[3][0], 
                (int)Xbox->m_mousePointerTM[3][1], 
                RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, 
                kRNewlyReleased);
        }
        event = kRNewlyPressed;
    } 
    else if(button && (button == RMouseDrag.m_button))
    {
        /* Staying button press? */
        /* Only need to do something if there's movement */

        event = kRStillPressed;

        if(button == kRLeftButton) 
        {
            RExecuteMouseCallback(
                rc,
                (int)Xbox->m_mousePointerTM[3][0], 
                (int)Xbox->m_mousePointerTM[3][1], 
                RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, 
                event);
        }
    }
    else if(!button && RMouseDrag.m_button)
    {
        /* Going button press? */
        event = kRNewlyReleased;
    }
    else
    {
        event = (RMouseButtonEvent)0;
    }

    /* If a button has just been pressed, then */

    if(event == kRNewlyPressed) 
    {
        /* Fill in data for this drag */
        RMouseDrag.m_meModifiers = 0;
#if 0
        if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]> BUTTON_THRESH)
            RMouseDrag.m_meModifiers |= RSHIFT;
        if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]> BUTTON_THRESH)
            RMouseDrag.m_meModifiers |= RCONTROL;
#endif

        RMouseDrag.m_startx = (int)Xbox->m_mousePointerTM[3][0];
        RMouseDrag.m_starty = (int)Xbox->m_mousePointerTM[3][1];

        /* On xbox, no modifier is needed for mouse forces */

        if(button == kRLeftButton) 
        {
            RExecuteMouseCallback(
                rc,
                (int)Xbox->m_mousePointerTM[3][0], 
                (int)Xbox->m_mousePointerTM[3][1], 
                RMouseDrag.m_meModifiers,
                button, 
                event);
        }
    } 
    else if(event == kRNewlyReleased) 
    {
        //if(RMouseDrag.m_meModifiers)
            RExecuteMouseCallback(
                rc,
                (int)Xbox->m_mousePointerTM[3][0], 
                (int)Xbox->m_mousePointerTM[3][1], 
                RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, 
                event);
    }

    /* And finally store the state of the button so we can tell if we're dragging next frame*/
    RMouseDrag.m_button = button;
}

void Xbox_ReadPad(XboxVars *Xbox)
{
    int camX,camY;

    RRender *rc = Xbox->rc;

    XInputGetState( Xbox->m_Gamepads[0].hDevice, &Xbox->m_InputStates[0] );

    /* Check Dpad */

    if(Xbox->m_InputStates[0].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
    {
        if(((!(Xbox->m_autorepeat[0] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[0] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[0] == 0)
        {
            RExecuteUpCallback(rc);
        }
        Xbox->m_autorepeat[0]++;
    } 
    else
    {
        Xbox->m_autorepeat[0] = 0;
    }


    if(Xbox->m_InputStates[0].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
    {
        if(((!(Xbox->m_autorepeat[1] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[1] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[1] == 0)
        {
            RExecuteDownCallback(rc);
        }
        Xbox->m_autorepeat[1]++;
    } 
    else
    {
        Xbox->m_autorepeat[1] = 0;
    }

    if(Xbox->m_InputStates[0].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
    {
        if(((!(Xbox->m_autorepeat[2] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[2] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[2] == 0)
        {
            RExecuteLeftCallback(rc);
        }
        Xbox->m_autorepeat[2]++;
    } 
    else
    {
        Xbox->m_autorepeat[2] = 0;
    }

    if(Xbox->m_InputStates[0].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
    {
        if(((!(Xbox->m_autorepeat[3] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[3] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[3] == 0)
        {
            RExecuteRightCallback(rc);
        }
        Xbox->m_autorepeat[3]++;
    } 
    else
    {
        Xbox->m_autorepeat[3] = 0;
    }

    /* Check A button (Green) */

    if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > BUTTON_THRESH)
    {
        if(((!(Xbox->m_autorepeat[4] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[4] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[4] == 0)
        {
            RExecuteRight2Callback(Xbox->rc);
            RExecuteActionNCallback(Xbox->rc, 5);
        }
        Xbox->m_autorepeat[4]++;
    } 
    else
    {
        Xbox->m_autorepeat[4] = 0;
    }

    /* Check X button (Blue) */

    if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > BUTTON_THRESH)
    {
        if(((!(Xbox->m_autorepeat[5] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[5] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[5] == 0)
        {
            RExecuteRight2Callback(Xbox->rc);
            RExecuteActionNCallback(Xbox->rc, 4);
        }
        Xbox->m_autorepeat[5]++;
    } 
    else
    {
        Xbox->m_autorepeat[5] = 0;
    }

    /* Check Y button (Yellow) */

    if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > BUTTON_THRESH)
    {
        if(((!(Xbox->m_autorepeat[6] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[6] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[6] == 0)
        {
            RExecuteRight2Callback(Xbox->rc);
            RExecuteActionNCallback(Xbox->rc, 2);
        }
        Xbox->m_autorepeat[6]++;
    } 
    else
    {
        Xbox->m_autorepeat[6] = 0;
    }

    /* Check B button (Red) */

    if(Xbox->m_InputStates[0].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > BUTTON_THRESH)
    {
        if(((!(Xbox->m_autorepeat[7] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[7] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[7] == 0)
        {
            RExecuteRight2Callback(Xbox->rc);
            RExecuteActionNCallback(Xbox->rc, 3);
        }
        Xbox->m_autorepeat[7]++;
    } 
    else
    {
        Xbox->m_autorepeat[7] = 0;
    }

    /* Check back button (select button on PS2) */
    
    if(Xbox->m_InputStates[0].Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
    {
        if(((!(Xbox->m_autorepeat[8] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[8] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[8] == 0)
        {
            RExecuteActionNCallback(rc, 0);
        }
        Xbox->m_autorepeat[8]++;
    } 
    else
    {
        Xbox->m_autorepeat[8] = 0;
    }


    /* Check start button */

    if(Xbox->m_InputStates[0].Gamepad.wButtons & XINPUT_GAMEPAD_START)
    {
        if(((!(Xbox->m_autorepeat[9] % AUTOREPEAT_FRAMES))&& (Xbox->m_autorepeat[9] > AUTOREPEAT_MIN))
            || Xbox->m_autorepeat[9] == 0)
        {
            RExecuteActionNCallback(rc, 1);
        }
        Xbox->m_autorepeat[9]++;
    } 
    else
    {
        Xbox->m_autorepeat[9] = 0;
    }

    /* Check shoulder buttons */

    /* Check the right analogue stick */

    if((Xbox->m_InputStates[0].Gamepad.sThumbRX < PAD_CENTRE - PAD_DEAD_OFFSET) | (Xbox->m_InputStates[0].Gamepad.sThumbRX > PAD_CENTRE + PAD_DEAD_OFFSET) |
        (Xbox->m_InputStates[0].Gamepad.sThumbRY < PAD_CENTRE - PAD_DEAD_OFFSET) | (Xbox->m_InputStates[0].Gamepad.sThumbRY > PAD_CENTRE + PAD_DEAD_OFFSET)) 
    {
        RMouseDrag.m_camstartDist = Xbox->rc->m_CameraDist;
        RMouseDrag.m_camstartPhi = Xbox->rc->m_CameraPhi;
        RMouseDrag.m_camstartTheta = Xbox->rc->m_CameraTheta;
        RMouseDrag.m_camstartX = 0;
        RMouseDrag.m_camstartY = 0;
        RCameraGetLookAt(Xbox->rc, RMouseDrag.m_camstartLookAt);
        camX = (Xbox->m_InputStates[0].Gamepad.sThumbRX - PAD_CENTRE)>> 12;
        camY = (Xbox->m_InputStates[0].Gamepad.sThumbRY - PAD_CENTRE)>> 12;

#if 0
        if(paddata & SCE_PADj)
            RCameraZoomDolly(camX, camY);
        else if(paddata & SCE_PADR2)
            RCameraPan(camX, camY);
        else
#endif
        Xbox_CameraRotate(Xbox,-camX, camY);
    }

    Xbox_EmulateMouseButtons(Xbox);

}

void Xbox_CreateMousePointer(XboxVars* Xbox)
{
    RGraphic *  rg;
    RRender *  rc = Xbox->rc;
    RObjectVertex *vtx;
    float color[4] = { 1.0f,1.0f,0.0f,1.0f };

    rg = RGraphicCreateEmpty(3);

    rg->m_pLWMatrix = 0;
    rg->m_pObject->m_nTextureID = -1; /* no texture */
    RGraphicSetColor(rg, color);
    rg->m_pObject->m_bIsWireFrame = 0; /* default to solid */

    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    rg->m_pObject->m_Matrix[3][0]=320.0f;
    rg->m_pObject->m_Matrix[3][1]=100.0f;

    Xbox->m_mousePointerTM=rg->m_pObject->m_Matrix;

    vtx = rg->m_pVertices;

    vtx[0].m_X = 0.0f;         
    vtx[0].m_Y = 0.0f;         
    vtx[0].m_Z = 0.0f;

    vtx[1].m_X = 20.0f; 
    vtx[1].m_Y = 10.0f; 
    vtx[1].m_Z = 0.0f;

    vtx[2].m_X = 10.0f; 
    vtx[2].m_Y = 20.0f; 
    vtx[2].m_Z = 0.0f;

    vtx[0].m_NX = 0.0f; vtx[0].m_NY = 0.0f; vtx[0].m_NZ = -1.0f;
    vtx[1].m_NX = 0.0f; vtx[1].m_NY = 0.0f; vtx[1].m_NZ = -1.0f;
    vtx[2].m_NX = 0.0f; vtx[2].m_NY = 0.0f; vtx[2].m_NZ = -1.0f;

    RGraphicAddToList(rc, rg, 1);
}

void Xbox_RunApp(RRender *rc, RMainLoopCallBack func, void *userdata)
{
    XboxVars* Xbox = rc->Xbox;

    if ( Xbox_LoadTextures(Xbox) != D3D_OK )
        ME_REPORT(MeWarning(1,"Error loading textures." ));

    Xbox_InitPad(Xbox);
    Xbox_CreateMousePointer(Xbox);

    while( !Xbox->m_bQuit )
    {        
        Xbox_RenderD3D(rc,func,userdata);
    }
}

DWORD test=10;

void Xbox_InitGlobals(RRender* rc)
{
    XboxVars* Xbox = rc->Xbox;
    memset(Xbox, 0, sizeof (struct XboxVars));

    //Xbox->m_hWnd = NULL;
    Xbox->m_bActive = FALSE;
    Xbox->m_bReady = FALSE;
    Xbox->m_bIsFrameLocked = TRUE;
    Xbox->m_bDisplayFps = TRUE;
    Xbox->m_dwFpsUpdate = 60;
    Xbox->m_uiTicksPerFrame = 0;
    //snprintf(Xbox->m_strWindowTitle, MAX_TITLE_LENGTH - 1
    //    , "%s (v%s) [Direct3D]", ME_PRODUCT_NAME, ME_VERSION_STRING);
    //Xbox->m_strWindowTitle[MAX_TITLE_LENGTH] = '\0';
    //Xbox->ConfirmDevFn = NULL;
    Xbox->m_bLockAspectRatio = TRUE;
    Xbox->m_bAlphaBlend = FALSE;
    Xbox->m_bAlphaBlend2D = TRUE;
    Xbox->m_bAllowWireFrame = TRUE;
    if(rc->m_options.m_bWireFrame != MEFALSE)
        Xbox->m_bForceWireFrame = !MEFALSE;
    else
        Xbox->m_bForceWireFrame = MEFALSE;
    if(rc->m_options.m_bTextures != MEFALSE)
        Xbox->m_bUseTextures = !MEFALSE;
    else
        Xbox->m_bUseTextures = MEFALSE;
    Xbox->m_bDisplayLookAt = FALSE;
    Xbox->m_bUseAntiAliasing = FALSE;
    Xbox->m_bLinearFilter = TRUE;
    Xbox->m_bDisplay2D = TRUE;
    Xbox->m_bDisplayPS = TRUE;
    Xbox->m_bDoStep = FALSE;
    //Xbox->m_bNoVSync = TRUE;
    Xbox->rc = rc;

#if 0
    RMouseDrag.p_rc = &Xbox->rc;
    RMouseDrag.p_width = &(Xbox->m_dwRenderWidth);
    RMouseDrag.p_height = &(Xbox->m_dwRenderHeight);
    RMouseDrag.m_allowContextMenu = 0;
#endif

    strncpy(rc->m_ButtonText[0], "D-pad Up", 20);
    strncpy(rc->m_ButtonText[1], "D-pad Down", 20);
    strncpy(rc->m_ButtonText[2], "D-pad Left", 20);
    strncpy(rc->m_ButtonText[3], "D-pad Right", 20);

    strncpy(rc->m_ButtonText[4], "Y (yellow)", 20);       
    strncpy(rc->m_ButtonText[5], "A (green)", 20);          
    strncpy(rc->m_ButtonText[6], "X (blue)", 20);         
    strncpy(rc->m_ButtonText[7], "B (red)", 20);         

    strncpy(rc->m_ButtonText[8], "Back", 20);         /* ACTION0 */
    strncpy(rc->m_ButtonText[9], "Start", 20);          /* ACTION1 */
    strncpy(rc->m_ButtonText[10], "Y (yellow)", 20);      /* ACTION2 */
    strncpy(rc->m_ButtonText[11], "B (red)", 20);        /* ACTION3 */
    strncpy(rc->m_ButtonText[12], "X (blue)", 20);        /* ACTION4 */
    strncpy(rc->m_ButtonText[13], "A (Green)", 20);         /* ACTION5 */

    strncpy(rc->m_ButtonText[14], "White Button + Left Stick", 20);    /* MOUSE */

}
void Xbox_initD3D()
{

}

HRESULT Xbox_setupD3D(XboxVars* Xbox)
{
    D3DPRESENT_PARAMETERS d3dpp;

    // Create the D3D object, which is used to create the D3DDevice.
    if( NULL == ( Xbox->m_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
     
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    
    // Set fullscreen 640x480x32 mode
    d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;

    // Create one backbuffer and a zbuffer
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Set up how the backbuffer is "presented" to the frontbuffer each frame
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    // Force D3D not to wait for vsync, so that we can wait ourselves and time it

    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the Direct3D device. Hardware vertex processing is specified 
    // since all vertex processing takes place on Xbox hardware.
    if( FAILED( IDirect3D8_CreateDevice(Xbox->m_pD3D, 0, D3DDEVTYPE_HAL, NULL,
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                           &d3dpp, &Xbox->m_pd3dDevice ) ) )
        return E_FAIL;

    // Set global render state

    Xbox_SceneInit(Xbox);

    // After creating the device, initial state would normally be set

    return S_OK;
    
}
void Xbox_CalibrateTimer(XboxVars* Xbox)
{
    MeProfileTimerMode tmode;
    tmode.counterMode = kMeProfileCounterModeCache;
#ifndef PS2
    tmode.granularity = 0;
#else
    tmode.granularity = 2;
#endif
    tmode.count0Label = 0;
    tmode.count1Label = 0;
    MeProfileStartTiming(tmode, Xbox->rc->m_bProfiling);

    Xbox->m_uiTicksPerSec = (unsigned int)MeProfileGetClockSpeed();
    Xbox->m_uiTicksPerFrame = Xbox->m_uiTicksPerSec / 60;
}

int Xbox_CreateRenderer(RRender *rc,void *pWnd)
{
    XboxVars* Xbox;
    Xbox = (XboxVars*)MeMemoryAPI.create(sizeof(XboxVars));

    rc->Xbox = Xbox;
    
    Xbox_InitGlobals(rc);

    Xbox_initD3D();

    Xbox_setupD3D(Xbox);

    //if(rc->m_options.m_bDisplayHelpOnConsole)
    //    D3D_DisplayConsoleHelp();
        
        if (rc->m_options.m_bCalibrateTimer)
        {
                MeInfo(1,"Calibrating timer...");
                Xbox_CalibrateTimer(Xbox);
                MeInfo(1,"%d ticks per second. (Ctrl-F9 recalibrates)", Xbox->m_uiTicksPerSec );
        }
        //MeInfo(1,"D3D is using %s\n", Xbox->m_pDeviceInfo->strDesc);

    return 0;
}

HRESULT Xbox_LoadTextures(XboxVars* Xbox)
{
   /* Walk through Xbox->rc->m_TextureList and create surfaces as appropriate */
    int i;
    int err = 0;

    for( i = 0; i < 25 ; i++ )
    {
        if( Xbox->rc->m_TextureList[i] )
        {
            HRESULT hr;

            char *bmpfilename = (char *)MeMemoryAPI.create(strlen(Xbox->rc->m_TextureList[i])+50);
            
            bmpfilename[0] = '\0';
            strcat(bmpfilename, "D:\\resources\\");
            strcat(bmpfilename, Xbox->rc->m_TextureList[i]);
            strcat(bmpfilename, ".bmp");


            hr = D3DXCreateTextureFromFileA(Xbox->m_pd3dDevice,
                                            bmpfilename,
                                            Xbox->m_pddsTexture + i);
            if(FAILED(hr))
            {
                err = 1;
            }

            MeMemoryAPI.destroy(bmpfilename);

        }
    }
    if( err )
        return E_FAIL;
    return D3D_OK;
}

