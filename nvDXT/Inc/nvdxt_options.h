#ifndef NVDXT_OPTIONS_H
#define NVDXT_OPTIONS_H
enum
{
    dSaveButton = 1,
    dCancelButton = 2,
    dBackgroundName = 3,

    dSaveTextureFormatCombo = 600,


    // 3d viewing options
    d3DPreviewButton = 300, 
    d2DPreviewButton = 301, 
    dPreviewRefresh = 302, 


    dViewDXT1 = 200,
    dViewDXT2 = 201,
    dViewDXT3 = 202,
    dViewDXT5 = 203,
    dViewA4R4G4B4 = 204,
    dViewA1R5G5B5 = 205,
    dViewR5G6B5 = 206,
    dViewA8R8G8B8 = 207,


    dGenerateMipMaps = 30,
    dMIPMapSourceFirst = dGenerateMipMaps,
    //dSpecifyMipMaps = 31,
    dUseExistingMipMaps = 31,
    dNoMipMaps = 32,
    dMIPMapSourceLast = dNoMipMaps,

    dSpecifiedMipMapsCombo = 39,


    dMIPFilterCombo = 601,


    dShowDifferences = 40,
    dShowFiltering = 41,
    dShowMipMapping = 42,
    dShowAnisotropic = 43,

    dChangeClearColorButton = 50,
    dDitherColor = 53,

    dLoadBackgroundImageButton = 54,
    dUseBackgroundImage = 55,

    dBinaryAlpha = 56,
    dAlphaBlending = 57,
    dFadeColor = 58,
    dFadeAlpha = 59,

    dFadeToColorButton = 60,
    dAlphaBorder = 61,
    dBorder = 62,
    dBorderColorButton = 63,
    dbNormalizeMIPMaps = 64,

    dDitherEachMIPLevel = 66,
    dGreyScale = 67,
    dQuickCompress = 68,
    dLightingEnable = 69,

    dbApplySharpenFilterToMIP0 = 70,

    // WarpSharp is currently disabled
    /*dSharpenEdgeRadius = 71,
    dSharpenLambda = 72,
    dSharpenMu = 73,
    dSharpenTheta = 74,
    dbSharpenUseTwoComponents = 75,
    dbSharpenNonMaximalSuppression = 76,
    dbSharpenFlavor2 = 77,
    dbSharpenSharpBlur = 78,*/

    dZoom = 79,


    dTextureType2D = 80,
    dTextureTypeFirst = dTextureType2D,
    dTextureTypeCube = 81,
    ///dTextureTypeImage = 82,
    dTextureTypeVolume = 83,  
    dTextureTypeLast = dTextureTypeVolume,

    dFadeAmount = 90,
    dFadeToAlpha = 91,
    dFadeToDelay = 92,

    dBinaryAlphaThreshold = 94,

    dFilterGamma = 100,
    dFilterBlur = 101,
    dFilterWidth = 102,
    dbOverrideFilterWidth = 103,
    dLoadProfile = 104,
    dSaveProfile = 105,
    dSharpenMethodCombo = 106,

    dAskToLoadMIPMaps = 400,
    dShowAlphaWarning = 401,
    dShowPower2Warning = 402,
    dTextureFormatBasedOnAlpha = 403,
    dSystemMessage = 404,

    dAdvancedBlendingButton = 500,
    dUserSpecifiedFadingAmounts = 501,
    dSharpenSettingsButton = 502,
    dFilterSettingsButton = 503,
    dNormalMapGenerationSettingsButton = 504,


    ///////////  Normal Map

    dOK = 1001,
    dCancel = 1002,
    dScaleEditText = 1003,
    dProxyItem = 1005,
    dMinZEditText = 1008,

    dFilter4x = 1040,
    dFirstFilterRadio = dFilter4x,
    dFilter3x3 = 1041,
    dFilter5x5 = 1042,
    dFilterDuDv = 1043,
    dFilter7x7 = 1044,
    dFilter9x9 = 1045,
    dLastFilterRadio = dFilter9x9,

    dALPHA = 1009,
    dFirstCnvRadio = dALPHA,
    dAVERAGE_RGB = 1010,
    dBIASED_RGB = 1011,
    dRED = 1012,
    dGREEN = 1013,
    dBLUE = 1014,
    dMAX = 1015,
    dCOLORSPACE = 1016,
    dNORMALIZE = 1017,
    dLastCnvRadio = dNORMALIZE,

    d3DPreview = 1021,      
    dDecalTexture = 1022,
    dbUseDecalTexture = 1023,
    dbBrighten = 1024,
    dbAnimateLight = 1025,
    dStaticDecalName = 1026,


    dbWrap = 1030,
    dbMultipleLayers = 1031,
    db_16_16 = 1032,

    dAlphaNone = 1033,
    dFirstAlphaRadio = dAlphaNone,
    dAlphaHeight = 1034,
    dAlphaClear = 1035,
    dAlphaWhite = 1036,
    dLastAlphaRadio = dAlphaWhite,

    dbInvertY = 1037,
    db_12_12_8 = 1038,
    dbInvertX = 1039,

    dbNormalMapConversion = 1050,

    dCustomFilterStart = 2000,
    // 5x5  Filter 0- 24

    dCustomDiv = 2025,
    dCustomBias = 2025,

    dCustomUnSharpRadius = 2027,
    dCustomUnSharpAmount = 2028,
    dCustomUnSharpThreshold = 2029,

    dCustomXSharpenStrength = 2030,
    dCustomXSharpenThreshold = 2031,

    dCustomWarpSharpenDisplaceAmount = 2032,
    dCustomWarpSharpenEdgeStrength = 2033,
    dCustomWarpSharpenRadius = 2034,
    dCustomWarpSharpenDepth = 2035,
    dCustomWarpSharpenElevation = 2036,

    dCustomFilterEnd = dCustomWarpSharpenElevation,  

    dDeriveDiv = 2050,
    dDeriveBias = 2051,



};



#ifndef TRGBA
#define TRGBA
typedef	struct	
{
	unsigned char	rgba[4];
} rgba_t;
#endif

#ifndef TPIXEL
#define TPIXEL
union tPixel
{
  unsigned long u;
  rgba_t c;
};
#endif


// Windows handle for our plug-in (seen as a dynamically linked library):
extern HANDLE hDllInstance;
class CMyD3DApplication;

typedef enum RescaleTypes
{
    RESCALE_NONE,               // no rescale
    RESCALE_NEAREST_POWER2,     // rescale to nearest power of two
    RESCALE_BIGGEST_POWER2,   // rescale to next bigger power of 2
    RESCALE_SMALLEST_POWER2,  // rescale to smaller power of 2 
    RESCALE_NEXT_SMALLEST_POWER2,  // rescale to next smaller power of 2
    RESCALE_PRESCALE,           // rescale to this size
    RESCALE_RELSCALE,           // relative rescale
    RESCALE_CLAMP,              //
    RESCALE_LAST,              //


} RescaleTypes;


enum SharpenFilterTypes
{
    kSharpenFilterNone,
    kSharpenFilterNegative,
    kSharpenFilterLighter,
    kSharpenFilterDarker,
    kSharpenFilterContrastMore,
    kSharpenFilterContrastLess,
    kSharpenFilterSmoothen,
    kSharpenFilterSharpenSoft,
    kSharpenFilterSharpenMedium,
    kSharpenFilterSharpenStrong,
    kSharpenFilterFindEdges,
    kSharpenFilterContour,
    kSharpenFilterEdgeDetect,
    kSharpenFilterEdgeDetectSoft,
    kSharpenFilterEmboss,
    kSharpenFilterMeanRemoval,
    kSharpenFilterUnSharp,
    kSharpenFilterXSharpen,
    kSharpenFilterWarpSharp,
    kSharpenFilterCustom,
    kSharpenFilterLast,
};



enum MIPFilterTypes
{
    kMIPFilterPoint ,    
    kMIPFilterBox ,      
    kMIPFilterTriangle , 
    kMIPFilterQuadratic ,
    kMIPFilterCubic ,    

    kMIPFilterCatrom ,   
    kMIPFilterMitchell , 

    kMIPFilterGaussian , 
    kMIPFilterSinc ,     
    kMIPFilterBessel ,   

    kMIPFilterHanning ,  
    kMIPFilterHamming ,  
    kMIPFilterBlackman , 
    kMIPFilterKaiser,
    kMIPFilterLast,
};


enum TextureFormatTypes
{
    kDXT1 ,
    kDXT1a ,  // DXT1 with one bit alpha
    kDXT3 ,   // explicit alpha
    kDXT5 ,   // interpolated alpha
    k4444 ,   // a4 r4 g4 b4
    k1555 ,   // a1 r5 g5 b5
    k565 ,    // a0 r5 g6 b5
    k8888 ,   // a8 r8 g8 b8
    k888 ,    // a0 r8 g8 b8
    k555 ,    // a0 r5 g5 b5
    k8   ,   // paletted
    kV8U8 ,   // DuDv 
    kCxV8U8 ,   // normal map
    kA8 ,            // alpha only
    k4  ,            // 16 bit color
    kTextureFormatLast
};


#define CUSTOM_FILTER_ENTRIES 27
#define UNSHARP_ENTRIES 3
#define XSHARP_ENTRIES 3
#define WARPSHARP_ENTRIES 5

#define CUSTOM_DATA_ENTRIES (CUSTOM_FILTER_ENTRIES+UNSHARP_ENTRIES+XSHARP_ENTRIES+WARPSHARP_ENTRIES)

typedef struct CompressionOptions
{
    CompressionOptions()
    {

        rescaleImageType = RESCALE_NONE; 
        scaleX = 1;
        scaleY = 1;

        bMipMapsInImage = false;    // mip have been loaded in during read

        MipMapType = dGenerateMipMaps;         // dNoMipMaps, dUseExistingMipMaps, dGenerateMipMaps
        SpecifiedMipMaps = 0;   // if dSpecifyMipMaps or dUseExistingMipMaps is set (number of mipmaps to generate)

        MIPFilterType = kMIPFilterTriangle;      // for MIP maps
        /* 
            for MIPFilterType, specify one betwee dMIPFilterFirst and dMIPFilterLast
        */


        bBinaryAlpha = false;       // zero or one alpha channel

        bNormalizeMIPMaps = false;         // Is a normal Map
        bDuDvMap= false;           // Is a DuDv (EMBM) map

        bAlphaBorder= false;       // make an alpha border
        bBorder= false;            // make a color border
        BorderColor.u = 0;        // color of border


        bFadeColor = false;         // fade color over MIP maps
        bFadeAlpha= false;         // fade alpha over MIP maps

        FadeToColor.u = 0;        // color to fade to
        FadeToAlpha = 0;        // alpha value to fade to (0-255)

        FadeToDelay = 0;        // start fading after 'n' MIP maps

        FadeAmount = 0;         // percentage of color to fade in

        BinaryAlphaThreshold = 128;  // When Binary Alpha is selected, below this value, alpha is zero


        bDitherColor = false;       // enable dithering during 16 bit conversion
        bDitherEachMIPLevel = false;// enable dithering during 16 bit conversion for each MIP level (after filtering)
        bGreyScale = false;         // treat image as a grey scale
        bQuickCompress = false;         // Fast compression scheme
        bForceDXT1FourColors = false;  // do not let DXT1 use 3 color representation


        bApplySharpenFilterToMIP0 = false;
        SharpenFilterType = kSharpenFilterNone;

        // sharpening after creating each MIP map level
        // warp sharp filter parameters
        // look here for details:
        //          
        // "Enhancement by Image-Dependent Warping", 
        // IEEE Transactions on Image Processing, 1999, Vol. 8, No. 8, S. 1063
        // Nur Arad and Craig Gotsman
        // http://www.cs.technion.ac.il/~gotsman/AmendedPubl/EnhancementByImage/EnhancementByI-D.pdf




        /*SharpenEdgeRadius = 2;
        SharpenLambda = 10.0f;
        SharpenMu = 0.01f;
        SharpenTheta =  0.75;
        bSharpenUseTwoComponents = false;
        bSharpenNonMaximalSuppression = false;
        bSharpenSharpBlur = false;
        bSharpenFlavor2 = false;*/

        // gamma value for Kaiser, Light Linear
        FilterGamma = 2.2f;
        // alpha value for 
        FilterBlur = 1.0f;
        // width of filter
        FilterWidth = 10.0f;
        bOverrideFilterWidth = false;

        TextureType = dTextureType2D;        // regular decal, cube or volume  
        /*
        for TextureType, specify one of:
        dTextureType2D 
        dTextureTypeCube 
        dTextureTypeImage 
        dTextureTypeVolume
        */

        TextureFormat = kDXT1;	    
        /* 
        for TextureFormat, specify any from dTextureFormatFirst to 
        dTextureFormatLast

  
        */

        bSwapRGB = false;           // swap color positions R and G
        user_data = 0;

    };

    RescaleTypes   rescaleImageType;     // changes to just rescale
    float   scaleX;
    float   scaleY;

    bool            bMipMapsInImage;    // mip have been loaded in during read
    short           MipMapType;         // dNoMipMaps, dSpecifyMipMaps, dUseExistingMipMaps, dGenerateMipMaps

    short           SpecifiedMipMaps;   // if dSpecifyMipMaps or dUseExistingMipMaps is set (number of mipmaps to generate)

    MIPFilterTypes   MIPFilterType;      // for MIP map, select from MIPFilterTypes

    bool        bBinaryAlpha;       // zero or one alpha channel

    bool        bNormalizeMIPMaps;  // Is a normal Map
    bool        bDuDvMap;           // Is a DuDv (EMBM) map

    bool        bAlphaBorder;       // make an alpha border
    bool        bBorder;            // make a color border
    tPixel      BorderColor;        // color of border


    bool        bFadeColor;         // fade color over MIP maps
    bool        bFadeAlpha;         // fade alpha over MIP maps

    tPixel      FadeToColor;        // color to fade to
    int         FadeToAlpha;        // alpha value to fade to (0-255)

    int         FadeToDelay;        // start fading after 'n' MIP maps

    int         FadeAmount;         // percentage of color to fade in

    int         BinaryAlphaThreshold;  // When Binary Alpha is selected, below this value, alpha is zero


    bool        bDitherColor;       // enable dithering during 16 bit conversion
    bool        bDitherEachMIPLevel;// enable dithering during 16 bit conversion for each MIP level (after filtering)
    bool        bGreyScale;         // treat image as a grey scale
    bool        bQuickCompress;         // Fast compression scheme
    bool        bForceDXT1FourColors;  // do not let DXT1 use 3 color representation


    // sharpening after creating each MIP map level

    bool bApplySharpenFilterToMIP0;

    float custom_data[CUSTOM_DATA_ENTRIES]; 

    SharpenFilterTypes  SharpenFilterType; 



    // warp sharp filter parameters (disabled)
    // look here for details:
    //          
    // "Enhancement by Image-Dependent Warping", 
    // IEEE Transactions on Image Processing, 1999, Vol. 8, No. 8, S. 1063
    // Nur Arad and Craig Gotsman
    // http://www.cs.technion.ac.il/~gotsman/AmendedPubl/EnhancementByImage/EnhancementByI-D.pdf

    /*int SharpenEdgeRadius;
    float SharpenLambda;
    float SharpenMu;
    float SharpenTheta;
    bool bSharpenUseTwoComponents;
    bool bSharpenNonMaximalSuppression;
    bool bSharpenSharpBlur;
    bool bSharpenFlavor2;*/

    // gamma value for Kaiser, Light Linear
    float FilterGamma;
    // alpha value for kaiser filter
    float FilterBlur;
    // width of filter
    float FilterWidth;

    bool bOverrideFilterWidth; // use the specified width,instead of the default




	short 		TextureType;        // regular decal, cube or volume  
	/*
        for TextureType, specify one of:
            dTextureType2D 
    	    dTextureTypeCube 
            dTextureTypeImage 
            dTextureTypeVolume
     */

	TextureFormatTypes 		TextureFormat;	    
    /* 
        for TextureFormat, specify any from dTextureFormatFirst to dTextureFormatLast
  
        */

    bool        bSwapRGB;           // swap color positions R and G
    void * user_data;

} CompressionOptions;


#endif
