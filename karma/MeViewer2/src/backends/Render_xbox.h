#ifndef RENDER_XBOX_H
#define RENDER_XBOX_H

#include <xtl.h>

typedef struct 
{
    // The following members are inherited from XINPUT_GAMEPAD:
    WORD    wButtons;
    BYTE    bAnalogButtons[8];
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;

    // Thumb stick values converted to range [-1,+1]
    FLOAT      fX1;
    FLOAT      fY1;
    FLOAT      fX2;
    FLOAT      fY2;
    
    // State of buttons tracked since last poll
    WORD       wLastButtons;
    BOOL       bLastAnalogButtons[8];
    WORD       wPressedButtons;
    BOOL       bPressedAnalogButtons[8];

    // Rumble properties
    XINPUT_RUMBLE   Rumble;
    XINPUT_FEEDBACK Feedback;

    // Device properties
    XINPUT_CAPABILITIES caps;
    HANDLE     hDevice;

    // Flags for whether gamepad was just inserted or removed
    BOOL       bInserted;
    BOOL       bRemoved;
} XBGAMEPAD;

typedef struct XboxVars
{
    BOOL                 m_bActive;
    BOOL                 m_bReady;
    BOOL                 m_bQuit;
    BOOL                 m_bIsFrameLocked;
    BOOL                 m_bDisplayFps;
    DWORD                m_dwFpsUpdate;
    unsigned int         m_uiTicksPerFrame;
    unsigned int         m_uiTicksPerSec;

    BOOL                 m_bLockAspectRatio;
    BOOL                 m_bDisplay2D;
    BOOL                 m_bDisplayPS;
    BOOL                 m_bAlphaBlend;
    BOOL                 m_bAlphaBlend2D;
    BOOL                 m_bAllowWireFrame;
    BOOL                 m_bForceWireFrame;
    BOOL                 m_bUseAntiAliasing;
    BOOL                 m_bUseTextures;
    BOOL                 m_bLinearFilter;
    BOOL                 m_bDisplayLookAt;
    BOOL                 m_bDoStep; /* for single-frame advance */

    DWORD                m_dwNumDevicesEnumerated;
    DWORD                m_dwNumDevices;

    char                 m_strDevicename[256];

    BOOL                 m_bIsFullscreen;
    DWORD                m_dwRenderWidth;
    DWORD                m_dwRenderHeight;
    RECT                 m_rcScreenRect;

    MeI64                m_thisFrameStartTime;
    MeI64                m_lastFrameStartTime;

    /* DirectX stuff */

    LPDIRECT3D8          m_pD3D;
    LPDIRECT3DDEVICE8    m_pd3dDevice;
    D3DMATERIAL8         m_d3dmtrl;  /* Default material */
    D3DBaseTexture *     m_pddsTexture[25]; /* Texture surfaces */
    D3DLIGHT8            m_lgtDir1;  /* First directional light */
    D3DLIGHT8            m_lgtDir2;  /* Sencond directional light */
    D3DLIGHT8            m_lgtPt;    /* Point light */

    /* Input stuff */

    XINPUT_STATE         m_InputStates[4];
    XBGAMEPAD            m_Gamepads[4];
    int                  m_autorepeat[10];

    /* Mouse pointer */
    MeMatrix4Ptr         m_mousePointerTM;

    /* Render context */

    RRender              *rc;

} XboxVars;

// A structure for our custom vertex type
typedef struct CUSTOMVERTEX
{
    FLOAT x, y, z; 
    FLOAT nx, ny, nz;
    FLOAT tu, tv;
} CUSTOMVERTEX;


// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)


#ifdef __cplusplus
extern "C"
{
#endif

void       Xbox_RunApp(RRender *rc, RMainLoopCallBack func,void *userdata);
int        Xbox_CreateRenderer(RRender *rc,void *pWnd);
void       Xbox_SetWindowTitle(RRender *rc, const char * title);
HRESULT    Xbox_LoadTextures(struct XboxVars* Xbox);
void       Xbox_ReadPad(struct XboxVars *Xbox);

#ifdef __cplusplus
}
#endif


#endif