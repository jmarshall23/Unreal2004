//-----------------------------------------------------------------------------
// File: XBFont.h
//
// Desc: Texture-based font class. This class reads .abc font files that are
//       generated by the FontMaker tool. These .abc files are used to create
//       a texture with all the font's glyph, and also extract information on
//       the dimensions of each glyph.
//
//       Once created, this class is used to render text in a 3D scene with the
//       following function:
//          DrawText( fScreenY, fScreenSpaceY, dwTextColor, strText,
//                    dwJustificationFlags );
//
//       For performance, you can batch multiple DrawText calls together
//       between Begin() and End() calls, as in the following example:
//          pFont->Begin();
//          pFont->DrawText( ... );
//          pFont->DrawText( ... );
//          pFont->DrawText( ... );
//          pFont->End();
//
//       The size (extent) of the text can be computed without rendering with
//       the following function:
//          GetTextExtent( strText, pfReturnedWidth, pfReturnedHeight,
//                         bComputeExtentUsingFirstLineOnly );
//
//       Finally, the font class can create a texture to hold rendered text,
//       which is useful for static text that must be rendered for many
//       frames, or can even be used within a 3D scene. (For instance, for a
//       player's name on a jersey.) Use the following function for this:
//          CreateTexture( strText, d3dTextureFormat );
//
//       See the XDK docs for more information.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       02.18.01 - Changes for March XDK release
//       04.15.01 - Using packed resources for May XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBFONT_H
#define XBFONT_H
#include "XBResource.h"
#include "XBUtil.h"




//-----------------------------------------------------------------------------
// Number of vertex buffers for rendering text. Having this number be greater
// than 1 can reduce potential stalling of the GPU.
//-----------------------------------------------------------------------------
#define NUM_FONT_BUFFERS 2



//-----------------------------------------------------------------------------
// Flags for the CXBFont::DrawText() function
//-----------------------------------------------------------------------------
#define XBFONT_LEFT     0x00000000
#define XBFONT_RIGHT    0x00000001
#define XBFONT_CENTER_X 0x00000002
#define XBFONT_CENTER_Y 0x00000004




//-----------------------------------------------------------------------------
// Custom vertex type for rendering text
//-----------------------------------------------------------------------------
struct XBFONTVERTEX 
{ 
    D3DXVECTOR4 p;
    DWORD       color;
    FLOAT       tu, tv; 
};

#define D3DFVF_XBFONTVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)




//-----------------------------------------------------------------------------
// Name: struct GLYPH_ATTR
// Desc: Structure to hold information about one glyph (font character image)
//-----------------------------------------------------------------------------
struct GLYPH_ATTR
{
    FLOAT left, top, right, bottom; // Texture coordinates for the image
    SHORT wOffset;                  // Pixel offset for glyph start
    SHORT wWidth;                   // Pixel width of the glyph
    SHORT wAdvance;                 // Pixels to advance after the glyph
};




//-----------------------------------------------------------------------------
// Name: class CXBFont
// Desc: Class to implement texture-based font rendering. A .tga image file of 
//       the pre-rendered font is used to create the texture. A .abc file
//       contains information for spacing the font characters (aka glyphs).
//-----------------------------------------------------------------------------
class CXBFont
{
public:
    // Font and texture dimensions
    DWORD         m_dwFontHeight;
    DWORD         m_dwTexWidth;
    DWORD         m_dwTexHeight;

    // Unicode ranges
    WCHAR         m_cLowChar;
    WCHAR         m_cHighChar;

    // Glyph data for the font
    DWORD         m_dwNumGlyphs;
    GLYPH_ATTR*   m_Glyphs;

    // D3D rendering objects
    CXBPackedResource       m_xprResource;
    LPDIRECT3DDEVICE8       m_pd3dDevice;
    LPDIRECT3DTEXTURE8      m_pTexture;
    LPDIRECT3DVERTEXBUFFER8 m_pVBs[NUM_FONT_BUFFERS];
    LPDIRECT3DVERTEXBUFFER8 m_pVB;
    DWORD                   m_dwCurrentBuffer;
    XBFONTVERTEX*           m_pVertices;
    DWORD                   m_dwNumQuads;
    DWORD                   m_dwNestedBeginCount;
    BOOL                    m_bTextureFromFile;

    // Internal call to trigger rendering of the vertex buffer
    HRESULT Render();

    // Access functions for debugging purposes
    LPDIRECT3DTEXTURE8 GetTexture() const    { return m_pTexture; }
    DWORD              GetFontHeight() const { return m_dwFontHeight; }

public:
    // Constructor/destructor
    CXBFont();
    ~CXBFont();

    // Functions to create and destroy the internal objects
    HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, 
                    const CHAR* strFontResourceFileName );
    HRESULT Destroy();

    // Replaces invalid (outside the valid glyph range) characters in a string
    VOID    ReplaceInvalidChars( WCHAR* strUpdate, WCHAR cReplacement ) const;

    // Returns the dimensions of a text string
    HRESULT GetTextExtent( const WCHAR* strText, FLOAT* pWidth, 
                           FLOAT* pHeight, BOOL bFirstLineOnly=FALSE ) const;

    // Function to create a texture containing rendered text
    LPDIRECT3DTEXTURE8 CreateTexture( const WCHAR* strText, 
                                      D3DCOLOR dwBackgroundColor = 0x00000000,
                                      D3DCOLOR dwTextColor = 0xffffffff,
                                      D3DFORMAT d3dFormat = D3DFMT_LIN_A8R8G8B8 );

    // Public calls to render text. Callers can simply call DrawText(), but for
    // performance, they should batch multiple calls together, bracketed by 
    // calls to Begin() and End().
    HRESULT Begin();
    HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, 
                      const WCHAR* strText, DWORD dwFlags=0L );
    HRESULT End();
};




#endif


