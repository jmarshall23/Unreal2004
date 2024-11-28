/*File:
BenMark5 extract.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
NOT BUILDABLE -- Just a code fragment
The relevant slice of the BenMark5 Ribbons plugin by Ben DeWaal

The key point here is NOT to use D3DVBCAPS_SYSTEMMEMORY.
  It will halve the triangle rate.
*/


#define _MFC_PROLOG         AFX_MANAGE_STATE(AfxGetStaticModuleState());
#define BREAK               __asm int 3
#define BREAK_ONCE          { static int i = 1; if (i) { i = 0; BREAK } }
#define RELEASE(x)          if (x) { (x)->Release(); (x) = NULL; }
#define DO(x)               { HRESULT res = (x); if (res) { g_strError.Format("%s(%d): %s",__FILE__,__LINE__,#x); return res; } }


//////////////////////////////////////////////////////////////////////////////
// CPlugIn::dxRun
//
HRESULT CPlugIn::dxRun
(
    INFO *pInfo
)
{
    // geometry constants
    const DWORD cdwRibbonCount = 25;

    const float cfRibbonCurl   = 60.0f * 3.14152683f;
    const float cfRibbonPitch  = 3.25f;
    const float cfRibbonWidth  = 0.04f;
    const float cfRibbonRadius = 0.05f;

    const DWORD cdwSubsInLength = 1200;
    const DWORD cdwSubsInWidth  = 5;

    // create vertex buffer
    DWORD dwNumVertices = (cdwSubsInLength + 1) * (cdwSubsInWidth + 1);
    D3DVERTEXBUFFERDESC vbdesc;
    memset (&vbdesc,0,sizeof(vbdesc));
    vbdesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);

		// Default when running is to NOT use D3DVBCAPS_SYSTEMMEMORY
		// Using this flag will slow the app to about half the triangle rate.
    vbdesc.dwCaps        = D3DVBCAPS_WRITEONLY | D3DVBCAPS_DONOTCLIP 
                         | (m_State.bSysMemVB ? D3DVBCAPS_SYSTEMMEMORY : 0);
    vbdesc.dwFVF         = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
    vbdesc.dwNumVertices = dwNumVertices;
    DO(m_pDirect3D->CreateVertexBuffer(&vbdesc,&m_pVertexBufferOutside,0));
    DO(m_pDirect3D->CreateVertexBuffer(&vbdesc,&m_pVertexBufferInside,0));

    // populate with vertices
    float *pfVerticesO;
    DO(m_pVertexBufferOutside->Lock(DDLOCK_SURFACEMEMORYPTR  | DDLOCK_WAIT,(void**)&pfVerticesO,NULL));
    float *pfVerticesI;
    DO(m_pVertexBufferInside->Lock(DDLOCK_SURFACEMEMORYPTR  | DDLOCK_WAIT,(void**)&pfVerticesI,NULL));

    for (DWORD y = 0; y <= cdwSubsInLength; y++)
    {
        for (DWORD x = 0; x <= cdwSubsInWidth; x++)
        {
            // compute point
            float angle  = cfRibbonCurl * float(y) / float(cdwSubsInLength);
            float height = cfRibbonWidth * float(x) / float(cdwSubsInWidth)
                         + cfRibbonPitch * float(y) / float(cdwSubsInLength)
                         - 0.5f * cfRibbonPitch;

            // start
            D3DXVECTOR3 p;
            p.x = height;
            p.y = cfRibbonRadius * (float)cos(angle);
            p.z = cfRibbonRadius * (float)sin(angle);
            D3DXVECTOR3 n;
            n.x = 0;
            n.y = (float)cos(angle);
            n.z = (float)sin(angle);

            float tv = float(y) / float(cdwSubsInLength);
            float tu = float(x) / float(cdwSubsInWidth);

            // copy to vertex buffer
            pfVerticesO[0]  =  p.x;
            pfVerticesO[1]  =  p.y;
            pfVerticesO[2]  =  p.z;
            pfVerticesO[3]  =  n.x;
            pfVerticesO[4]  =  n.y;
            pfVerticesO[5]  =  n.z;
            pfVerticesO[6]  =  tu;
            pfVerticesO[7]  =  tv;
            pfVerticesO    +=  8;

            pfVerticesI[0]  =  p.x;
            pfVerticesI[1]  =  p.y;
            pfVerticesI[2]  =  p.z;
            pfVerticesI[3]  = -n.x;
            pfVerticesI[4]  = -n.y;
            pfVerticesI[5]  = -n.z;
            pfVerticesI[6]  =  tu;
            pfVerticesI[7]  =  tv;
            pfVerticesI    +=  8;
        }
    }
    DO(m_pVertexBufferOutside->Unlock());
    DO(m_pVertexBufferInside->Unlock());

    // setup indices
    DWORD  dwIndexCount = 1 + cdwSubsInLength * (cdwSubsInWidth * 2 + 1);
    WORD  *pwIndices    = new WORD[dwIndexCount];
    WORD  *pwIndex      = pwIndices;
    DWORD  dwCurrent    = 0;

    pwIndex[0] = 0; pwIndex++;
    for (y = 0; y < cdwSubsInLength; y++)
    {
        if (dwCurrent > 0xffff)
        {
            g_strError = "Index out of range - reduce geometry complexity";
            return 1;
        }

        pwIndex[0] = WORD(dwCurrent + (cdwSubsInWidth + 1)); pwIndex++;
        dwCurrent ++;
        for (DWORD x = 0; x < cdwSubsInWidth; x++)
        {
            pwIndex[0]  = WORD(dwCurrent);
            pwIndex[1]  = WORD(dwCurrent + (cdwSubsInWidth + 1));
            pwIndex    += 2;
            dwCurrent  ++;
        }

        dwCurrent += (cdwSubsInWidth + 1) - 1;
        y++;
        if (y < cdwSubsInLength)
        {
            pwIndex[0] = WORD(dwCurrent + (cdwSubsInWidth + 1)); pwIndex++;
            dwCurrent --;
            for (DWORD x = 0; x < cdwSubsInWidth; x++)
            {
                pwIndex[0]  = WORD(dwCurrent);
                pwIndex[1]  = WORD(dwCurrent + (cdwSubsInWidth + 1));
                pwIndex    += 2;
                dwCurrent  --;
            }

            dwCurrent += (cdwSubsInWidth + 1) + 1;
        }
    }

    // if we use tri lists, we convert the strip to independent triangles here
    if (m_State.bUseTriList)
    {
        DWORD  dwNewIndexCount = (dwIndexCount - 2) * 3;
        WORD  *pwNewIndices    = new WORD[dwNewIndexCount];

        DWORD a = pwIndices[0];
        DWORD b = pwIndices[1];
        for (DWORD i = 2,j = 0; i < dwIndexCount; i++,j+=3)
        {
            DWORD c = pwIndices[i];
            pwNewIndices[j + 0] = (WORD)a;
            pwNewIndices[j + 1] = (WORD)b;
            pwNewIndices[j + 2] = (WORD)c;
            if (i & 1)
            {
                b = c;
            }
            else
            {
                a = c;
            }
        }

        // adopt new
        delete[] pwIndices;
        dwIndexCount = dwNewIndexCount;
        pwIndices    = pwNewIndices;
    }

    // set material
    D3DMATERIAL7 matOut[cdwRibbonCount];
    memset (&matOut,0,sizeof(matOut));
    for (DWORD i = 0; i < cdwRibbonCount; i++)
    {
        float r = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);
        float g = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);
        float b = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);

        matOut[i].dcvDiffuse.r  = r; 
        matOut[i].dcvDiffuse.g  = g; 
        matOut[i].dcvDiffuse.b  = b; 
        matOut[i].dcvDiffuse.a  = 1.0f; 
        matOut[i].dcvAmbient.r  = r * 0.3f;
        matOut[i].dcvAmbient.g  = g * 0.3f;
        matOut[i].dcvAmbient.b  = b * 0.3f;
        matOut[i].dcvAmbient.a  = 1.0f; 
    }

    // set material
    D3DMATERIAL7 matIn;
    memset (&matIn,0,sizeof(matIn));
    matIn.dcvDiffuse.r  = 0.7f; 
    matIn.dcvDiffuse.g  = 0.3f; 
    matIn.dcvDiffuse.b  = 0.3f; 
    matIn.dcvDiffuse.a  = 0.7f; 
    matIn.dcvAmbient.r  = 0.3f; 
    matIn.dcvAmbient.g  = 0.1f; 
    matIn.dcvAmbient.b  = 0.1f; 
    matIn.dcvAmbient.a  = 1.0f; 

    //
    // start draw
    //
    ID3DXMatrixStack *pMatrixStack;
    CStopWatch        stopWatch(pInfo->dwFlags & CBM5PlugIn::FLAG_TSC,pInfo->fdProcessorSpeed);
    DWORD             dwTriCount      = dwIndexCount - 2;
    DWORD             dwTotalTimes    = 0;
    DWORD             dwNext          = GetTickCount() + 2000;
    CString           str             = "Running...";
    float             fDelta          = 0.75f * cfRibbonPitch / (cdwRibbonCount - 1);

    DWORD dwAveCount       = 0;
    m_State.fdAveTriPerSec = 0.0;
    m_State.fdMaxTriPerSec = 0.0;
    m_State.fdMinTriPerSec = 1e199;

    stopWatch.start();
    DO(D3DXCreateMatrixStack (0,&pMatrixStack));
    for (BOOL bQuit = FALSE; !bQuit;)
    {
        // next experiment?
        DWORD dwNow = GetTickCount();
        if (dwNow > dwNext)
        {
            stopWatch.stop();
            double rcp = 1e-6 / stopWatch.getElaspedTime();
            double tps = double(dwTotalTimes * dwTriCount) * rcp / (m_State.bUseTriList ? 3 : 1);
            double sps = double(dwTotalTimes * dwIndexCount) * rcp;
            double ips = double(dwTotalTimes * dwIndexCount * 2) * rcp;
            double vps = double(dwTotalTimes * dwIndexCount * 32) * rcp;
            str.Format ("%.3lf MTri/s,  %.3lf MVerts/s,  %.3lfMB/s index traffic,  %.3lfMB/s [effective] vertex traffic",tps,sps,ips,vps);
            dwNext       = dwNow + 2000;
            dwTotalTimes = 0;

            dwAveCount++;
            m_State.fdAveTriPerSec += tps;
            m_State.fdMaxTriPerSec = max(tps,m_State.fdMaxTriPerSec);
            m_State.fdMinTriPerSec = min(tps,m_State.fdMinTriPerSec);

            // quit?
            EnterCriticalSection (&pInfo->cs);
            while (!pInfo->aVKList.IsEmpty())
            {
                CBM5PlugIn::EVENTRECORD& event = pInfo->aVKList.GetHead();
                pInfo->aVKList.RemoveHead();

                switch (event.dwEventID)
                {
                    case CBM5PlugIn::EVENT_CHAR:
                    {
                        if (event.adwEventData[0] == VK_ESCAPE) bQuit = TRUE;
                    }
                    break;
                }
            }
            LeaveCriticalSection (&pInfo->cs);

            stopWatch.start();
        }

        // update rotation
        //float fRotation1 = 2.0f * 3.141592653f / float(0x3fff) * float(dwNow & 0x3fff);
        float fRotation2 = 2.0f * 3.141592653f / 2047.0f * float(dwNow & 2047);

        // clear
        DO(m_pD3DXContext->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER));

        // begin scene
        DO(m_pDevice->BeginScene());

        // draw ribbons
        pMatrixStack->LoadIdentity();
        pMatrixStack->Translate (0.0f,-0.5f * 0.75f * cfRibbonPitch,0.0f);
        //pMatrixStack->RotateAxis (&D3DXVECTOR3(0.0f,0.0f,1.0f),0.3f);

        for (DWORD i = 0; i < cdwRibbonCount; i++)
        {
            // setup matrix
            pMatrixStack->Push();
            pMatrixStack->RotateAxisLocal (&D3DXVECTOR3(1.0f,0.0f,0.0f),fRotation2 + i * 0.4f);

            DO(m_pDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)pMatrixStack->GetTop()));

            // draw outside
            if (m_State.bApplyTextures)
            {
                DO(m_pDevice->SetTexture          (0,m_pTexture));
        
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE));
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,  D3DTOP_MODULATE));
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE));
                DO(m_pDevice->SetTextureStageState(1,D3DTSS_COLOROP,  D3DTOP_DISABLE));

                DO(m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE));
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,  D3DTOP_MODULATE));
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE));
                DO(m_pDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,  D3DTOP_DISABLE));
            }

            DO(m_pDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CW));
            DO(m_pDevice->SetMaterial(&matOut[i]));
            if (m_State.bUseTriList)
            {
                DO(m_pDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST,m_pVertexBufferOutside,0,dwNumVertices,pwIndices,dwIndexCount,0));
            }
            else
            {
                DO(m_pDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP,m_pVertexBufferOutside,0,dwNumVertices,pwIndices,dwIndexCount,0));
            }

            // draw inside
            if (m_State.bApplyTextures)
            {
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE));
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,  D3DTOP_SELECTARG1));
                DO(m_pDevice->SetTextureStageState(1,D3DTSS_COLOROP,  D3DTOP_DISABLE));

                DO(m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE));
                DO(m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1));
                DO(m_pDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,  D3DTOP_DISABLE));
            }

            DO(m_pDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW));
            DO(m_pDevice->SetMaterial(&matIn));
            if (m_State.bUseTriList)
            {
                DO(m_pDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST,m_pVertexBufferInside,0,dwNumVertices,pwIndices,dwIndexCount,0));
            }
            else
            {
                DO(m_pDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP,m_pVertexBufferInside,0,dwNumVertices,pwIndices,dwIndexCount,0));
            }

            // next
            dwTotalTimes += 2;
            pMatrixStack->Pop();
            pMatrixStack->Translate (0.0f,fDelta,0.0f);
        }

        // end
        DO(m_pDevice->EndScene());

        // show
        DO(m_pD3DXContext->DrawDebugText(0,0,0xffc0c0c0,(char*)(const char*)str));
        DO(m_pD3DXContext->UpdateFrame(D3DX_UPDATE_NOVSYNC));
    }
    stopWatch.stop();
    RELEASE (pMatrixStack);

    if (dwAveCount)
    {
        m_State.fdAveTriPerSec /= dwAveCount;
    }

    // done
    delete[] pwIndices;
    return DD_OK;
}
