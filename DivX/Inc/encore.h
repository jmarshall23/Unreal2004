/**
 ** Copyright (C) 2002 DivXNetworks, all rights reserved.
 **
 ** DivXNetworks, Inc. Proprietary & Confidential
 **
 ** This source code and the algorithms implemented therein constitute
 ** confidential information and may comprise trade secrets of DivXNetworks
 ** or its associates, and any use thereof is subject to the terms and
 ** conditions of the Non-Disclosure Agreement pursuant to which this
 ** source code was originally received.
 **
 **/

/** $Id: encore.h,v 1.64 2003/08/18 20:30:45 eagle Exp $
 **
 **/

/*************************************************************************/

#ifndef _ENCORE_ENCORE_H
#define _ENCORE_ENCORE_H

/** \file encore.h
    This is the header file describing the entrance function of the encoder core
    or "encore".  All interaction with encore is performed using one function:
    encore().
*/

/** \mainpage The DivX Video Encoder: libdivxencore
 
	This documentation was generated from comments in the encoder source code using an
	open-source tool called Doxygen.

	Starting Points:

	The API to libdivxencore may be found in encore.h.

	The only entry point to the library is encore().  This is called with different 
	options to initialise and release an encoder instance, and to encode video frames.

	CVS tag used to check out the source from which this documentation was generated: 
	\verbatim $Name: b947-Tikehau $ \endverbatim

	(c) DivXNetworks 2001-2003
*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "portab.h"

#ifndef  DivXBitmapInfoHeader_defined
#define DivXBitmapInfoHeader_defined
typedef struct tagDivXBitmapInfoHeader 
{
    uint32_t biSize; 
    int32_t  biWidth; 
    int32_t  biHeight; 
    uint16_t biPlanes; 
    uint16_t biBitCount;
    uint32_t biCompression; 
    uint32_t biSizeImage; 
    int32_t  biXPelsPerMeter; 
    int32_t  biYPelsPerMeter; 
    uint32_t biClrUsed; 
    uint32_t biClrImportant; 
} DivXBitmapInfoHeader; 
#endif

/* Rate control modes */
#define RCMODE_VBV_1PASS            0 /**< VBV 1-pass */
#define RCMODE_1PASS_CONSTANT_Q     1 /**< 1-pass constant quality */
#define RCMODE_VBV_MULTIPASS_1ST    2 /**< VBV multipass 1st-pass */
#define RCMODE_VBV_MULTIPASS_NTH    3 /**< VBV multipass nth-pass */
#define RCMODE_502_1PASS            4 /**< original 1-pass */
#define RCMODE_502_2PASS_1ST        5 /**< original 1st pass */
#define RCMODE_502_2PASS_2ND        6 /**< original 2nd pass */
#define RCMODE_IMAGE_COMPRESS       7 /**< 1-pass constant frame size */

/** macro to check that a particular value for vbr_mode is good */
#define IS_GOOD_RCMODE(vbr_mode) ((vbr_mode)>=0 && (vbr_mode)<=7) 


/**
    Structure optionally passed as an argument when creating encoder.
    You can set all values to 0, in which case they'll be initialized
    to default values, or specify them directly.
*/
typedef struct
{
	/** \name General */
	/*\{*/
	/** Rate control mode.  Use one of the following macros to set the mode:
		RCMODE_VBV_1PASS         
		RCMODE_1PASS_CONSTANT_Q  
		RCMODE_VBV_MULTIPASS_1ST 
		RCMODE_VBV_MULTIPASS_NTH 
 		RCMODE_502_1PASS         
 		RCMODE_502_2PASS_1ST     
 		RCMODE_502_2PASS_2ND
		RCMODE_IMAGE_COMPRESS
		RCMODE_PSNR
	*/
	int vbr_mode;                  
	int bitrate;                   /**< The bitrate of the target encoded stream, in bits/second.  Ignored in constant quantiser mode. */
	float quantizer;               /**< Used in constant quantiser mode. */
	int use_bidirect;              /**< >0 enables usage of b-frames. */
	/*\}*/

	/** @name Timing */
	/*@{*/
	int input_clock;               /**< Input clock to use - units Hz. */
	int input_frame_period;        /**< Period of one input frame according to input_clock. Use zero for variable framerate.*/
	int internal_timescale;        /**< Timescale to use inside encoder (default 30000 Hz). */     
	/*@}*/

    /** @name Keyframe insertion parameters **/
    /*@{*/
    int max_key_interval;          /**< Sets the maximum interval between key frames. */
	int key_frame_threshold;       /**< Sets a threshold for the codec to decide between I and P frames. Higher values will generally produce fewer I frames.  Default value is 50, ignored after first pass. */
    /*@}*/

	/** @name VBV model 
	    Parameters of the VBV model that is used to control bitrate in vbr_modes RCMODE_VBV_1PASS and RCMODE_VBV_MULTIPASS_NTH.
	    The VBV model may be disabled to provide unrestricted bitrate by setting vbv_bitrate to -1.
	 */
	/*@{*/
	int vbv_bitrate;               /**< Channel bitrate in bits/second.  (use multiple of 400). */
	int vbv_size;                  /**< Size of decoder's VBV buffer in bits (use multiple of 16384). */
	int vbv_occupancy;             /**< Initial occupancy of buffer in bits (use multiple of 64). */
	/*@}*/

	/** @name Multipass Frame Level Control */
	/*@{*/
	double complexity_modulation;  /**< Controls influence of motion complexity on quantiser for nth pass.  Use range 0.0 to 1.0, default 0.5. */
	const char* log_file_read;     /**< Filename of log (map) file to read. */
	const char* log_file_write;    /**< Filename of log (map) file to write. */
	const char* mv_file;           /**< Filename of file used to save information that will speed encoding in subsequent passes of the same video sequence. */
	/*@}*/

	/** @name EncoderMPEG4 */
	/*@{*/
	int deinterlace;               /**< Set non-zero to enables fast deinterlacing performed inside the MPEG-4 encoder at encoding resolution. Currently ignored by x86 non-MMX machines. */

	/** Quality of compression.  Additional legal values of 'quality' are defined to support new motion search modes while retaining API compatibility.
	0: same as 5
	1-5: standard SAD motion search ( 1 - fastest, 5 - best )
	quality & 256: full search
	quality & 192 == 0: SAD minimization
	quality & 192 == 64: shortcut minimization ( not fully implemented )
	quality & 192 == 128: shortcut minimization except for I/P decisions
	quality & 192 == 192: direct bits+L*error minimization

	attention: setting 192 or 256 will significantly decrease performance
	Having  192 AND 256 turned on at the same time has no practical use whatsoever ( expect 10-20 seconds per frame )
	**/	
	int quality;
	int data_partitioning;         /**< Enables Data Partitioning */
	int quarter_pel;               /**< Enables quarter pixel motion compensation mode. */ 
	int use_gmc;                   /**< Enables use of GMC (Global Motion Compensation) */
	/*@}*/

	/** @name Psychovisual.
	Note that negative values of pv_strength_frame and pv_strength_MB are used to activate new psychovisual modes:
	 pv_strength_frame < 0 && pv_strength_MB > 0 - TrellisQuantisingPv (do not use)
	 pv_strength_frame > 0 && pv_strength_MB < 0 - SimpleCoeffAttenuation (aka "slow" PV)
	 pv_strength_frame < 0 && pv_strength_MB < 0 - NoiseShapingPv activated (aka "fast" PV)
	 */
	/*@{*/
	int psychovisual;              /**< Enables psychovisual enhancements. */
	double pv_strength_frame;      /**< Strength of the psychovisual enhancement at frame level. */
	double pv_strength_MB;         /**< Strength of the psychovisual enhancement at macroblock level.  */
	/*@}*/

	/** @name Preprocessing: Interlace and IVTC mode. */
	/*@{*/
	/** IVTC/deinterlace mode.  
	     - 0 - Encode as progressive
	     - 1 - Encode as interlaced
	     - 2 - Deinterlace all frames
	     - 3 - Intelligent IVTC / deinterlace
	    The Intelligent IVTC/deinterlace mode allows the encoder to 
	    process content that is any mixture of interlaced, frame 
	    progressive and 3:2 pulldown material.  Interlaced sections will be 
	    deinterlaced and encoded at frame rate.  Frame progressive sections 
		are encoded as-is.  An IVTC process is applied to 3:2 pulldown 
		material before encoding.  The IVTC processes means that the 
		one-to-one relationship between input bitmaps and output encoded 
		frames is broken.  This mode will only work if the input frames are
		always at 30fps (which is not what is commonly received from MPEG-2
		decoders).

	*/
	int interlace_mode;            
	/*@}*/

	/** @name Preprocessing: Crop and Resize */
	/*@{*/
	/** To crop the image, first set enable_crop to 1 (default is 0 - off).  The 
	    four crop parameters crop_left, crop_right, crop_top and crop_bottom must 
	    then be set.  For example, setting crop_top = 8 and the others to 0 will 
	    remove the top 8 lines from the picture. */
	int enable_crop;               

	/** Enables resizing of the image.  By default, the size of the encoded image
	will be the dimensions of the input image after subtracting any crop values.  
	To encode at a different resolution, set enable_resize = 1 (default is 0 - off)
	and the resize_width and resize_height to the desired encoding resolution. */
	int enable_resize;              

	int resize_width;              /**< desired width of (cropped and) resized image. */
	int resize_height;             /**< desired height of (cropped and) resized image. */
	int crop_left;                 /**< number of pixels to crop from left side of input image. */
	int crop_right;                /**< number of pixels to crop from right side of input image. */ 
	int crop_top;                  /**< number of pixels to crop from top of input image. */ 
	int crop_bottom;               /**< number of pixels to crop from bottom of input image. */ 

	/**< resize algorithm to use: 0==bilinear, 1==bicubic. 
	    Currently, two resize algorithms are implanted:  \e bilinear and \e bicubic.   
	    To use the bilinear resize, set \p resize_mode = 0 (default).  For the bicubic 
	    resize, set \[ resize_mode = 1.  The constants \p bicubic_B and bicubic_C are 
	    spline parameters which influence the characteristics of the resized image.
	    It is recommended to use the Cartmull-Rom Spline (\p resize_mode = 0, 
	    \p bicubic_B = 0, \p bicubic_C = 0.5) or the bilinear algorithm (
	    \p resize_mode = 1) according to personal preference. */
	int resize_mode;               /**< desired resizing algorithm */
	double bicubic_B;              /**< spline parameter for bicubic resize. */
	double bicubic_C;              /**< spline parameter for bicubic resize. */
	/*@}*/

	/** @name Preprocessing: temporal/spatial filtering 
		Two noise-reduction prefilters are incorporated into the encoder.  
		One is a temporal filter and the other spatial.  To enable the 
		temporal filter, set temporal_enable = 1 (default is 0 - off).
		spatial_level should then be set in the range 0.0 (off) to 1.0 
		(full strength spatial filtering).  To enable the spatial filter, 
		set  spatial_passes in the range 1 to 3 (default is 0 - off).  
		Likewise, then set spatial_level in the range 0.0 (off) to 1.0 
		(full strength spatial filtering).  
	*/
	/*@{*/
	int temporal_enable;    
	int spatial_passes;
	double temporal_level;
	double spatial_level;
	/*@}*/

	/** @name Custom Modules
	    Set to NULL or 0 if not used.  Specification available separately.
	*/
	/*@{*/
	void* custom_rc;       /**< Custom rate control module (Baseclass IFrameLevelControl). */
	void* custom_preproc;  /**< Custom preprocessing module (Baseclass IPreprocessor). */
	void* custom_encoder;  /**< Custom encoder module (Baseclass ILowLevelEncoder). */
	void* custom_callback; /**< Custom callback object for warnings, errors and progress box (Baseclass IEncoreCallback). */
	/*@}*/
} SETTINGS;



/** Convenience function to derive floating point framerate from SETTINGS structure. 
    @param pSettings Pointer to populated SETTINGS structure.
    @return Framerate in frames-per-second.
  */
static float framerate(SETTINGS *pSettings) {
	return (float)pSettings->input_clock / (float)pSettings->input_frame_period;
}



/** Structure passed as a first argument when using encore() to encode a frame. */
typedef struct _ENC_FRAME_
{	
	/** Image frame to be encoded.  Set to 0 to flush out any further
	    compressed frames that can be generated by the encoder. 
	*/
	const void *image;

	/** Buffer for encoded bitstream.  This should be allocated by the
	    caller of encore() and it should be large enough to hold any
	    compressed video frame that might be produced by the encoder.
          The theoretical upper limit of frame size is around 6 bytes/pixel
	    or 2.5 Mb for 720x576 frame.  For performance reasons, checks 
	    for buffer overflow are not performed.         

  maximum possible amount of texture is 30 bits/code * 64 codes/block * 6 blocks = 45 bits/pixel = 5.625 bytes/pixel.
  longest possible motion vector component is 12+fcode bits <= 16 bits ( we don't use fcode>4 ). It gives
  32 bits/macroblock or 0.0156 bytes/pixel.
  the rest ( macroblock type, dquant, ... ) is less than 32 bits/macroblock.
  
	*/
	void *bitstream;

	/** Length of the encoded bitstream in bytes.  If (length < 0) then bitstream 
	    is invalid and the encoder is not able to produce further frames 
	    until it receives more input images. 
	*/
	int length;             	
	
	/** Requests the encoder to produce a not-coded P-frame with identical 
	    timestamp to previously encoded P-frame.  Such frames may be used 
	    in the AVI file format to indicate the end of a sequence of
	    consequetive B-frames. Set to 0 for normal operation.  Encoder will
		buffer the frame in image as usual.
	*/
	int produce_empty_frame;

 	/** Timestamp of the frame data contained in image.  Units are input_clock Hz.
 	    Note that this integer value may overflow as it is incremented by the user.
 		This is not a problem as the encoder only takes account of differences 
 		between timestamps which are unaffected by the arithmetic overflow.
 	*/
 	int timestamp;
}
ENC_FRAME;




/**
    Structure passed as a second optional argument when encoding a frame.
    On successful return its members are filled with information about
    the encoded frame.
*/
typedef struct
{
	int iMotionBits;       /**< Number of bits of encoded frame used for describing motion */
	int iTextureBits;      /**< Number of bits of encoded frame used for describing texture */
	int iStuffingBits;     /**< Number of bits of encoded frame used by stuffing */
	int iTotalBits;        /**< Total number of bits in encoded frame */
	int iQuant;            /**< Actual frame-level quantizer used to encode frame */
	int iSequenceNumber;   /**< Display-order sequence number of encoded frame */
	int iMvSum;
	int iMvCount;

	char cType;            /**< Actual frame-type ('I'/'B'/'P') of encoded frame '\0' means no frame produced */
	int iDisplayTimestamp; /**< Display/composition timestamp of encoded frame */

	//float fPsnr;           /**< PSNR of encoded frame.  If zero, PSNR is unavailable. */

} ENC_RESULT;
typedef ENC_RESULT FrameResult;




/** Main encode engine entrance point.  The action taken and meaning of parameters depends on \p enc_opt.

    - Use enc_opt = ENC_OPT_VERSION to check that the API described in encore.h is compatible with the
      binary DivX library that you are using.   See the example below. 

    - Use enc_opt = ENC_OPT_INIT to instantiate and and initialise an encoder instance.  
      - Set \p handle to point to a void* variable that will recieve a handle to the encoder instance.
      - Set \p param1 to point to a BITMAPINFOHEADER structure that describes the input image format.
      - Set \p param2 to point to a SETTINGS structure that contains settings the encoder settings.

    - Use enc_opt = ENC_OPT_ENCODE to encode a frame of video.  Because of IVTC and B-Frame reordering,
      the encoder operates on a 1-in, n-out basis.  This means that each input bitmap may
	produce zero or more compressed video frames.  The caller must therefore call encore() at least
	twice for each input frame to see if the encoder can produce any more frames from buffered data.
	The frames are output in bitstream order.
      - Set \p handle as the handle to the previously initialised encoder instance.
      - Set \p param1 to point to a ENC_FRAME structure 
      - Set \p param2 to 0, or to point to a ENC_RESULT structure
     
    - Use enc_opt = ENC_OPT_RELEASE to release the resources associated with an encoder instance.
      - Set \p handle as the handle to the previously initialised encoder instance.
      - \p param1 and \p param2 are ignored 

    Example encore() usage:

\code
#include <encore.h>

void* encore_handle;
DivXBitmapInfoHeader format;
SETTINGS settings;
ENC_FRAME frame;
ENC_RESULT result;
int n, nframes = 10;

if (encore(0, ENC_OPT_VERSION, 0, 0) != ENCORE_VERSION) {
	printf("API in encore.h is not compatible with encore library");
	return 0;
}

memset(&settings, 0, sizeof(SETTINGS));

// ... configure format and settings here ...

encore((void*)&encore_handle, ENC_OPT_INIT, &format, &settings);

for (n=0; n<nframes; n++) {

	// encode one input frame...
	frame.image = bitmap[n];
	frame.bitstream = malloc(2500000);
	frame.produce_empty_frame = 0;

	do {
		encore(encore_handle, ENC_OPT_ENCODE, &frame, &result);
		printf("encore produced %d bytes ", frame.length);
		printf("containing a %c frame of size %d bits\n", result.cType, result.iTotalBits);

		// ... do something with the bitstream here ...

		frame.image = (void*)0;
	} while (frame.length >= 0);

	free(frame.bitstream);

}

encore(encore_handle, ENC_OPT_RELEASE, 0, 0);
\endcode

    For single-pass encoding:
    -# encore() is called with \p handle = ENC_OPT_INIT to initialize a new 
       instance and its coding parameters,  references and other necessary information.
    -# encore() is called with \p handle = ENC_OPT_ENCODE for each frame 
       to be encoded. The input will be the video frame to codec and its coding 
       parameter. The output will be compressed bitstream. 
    -# After all the video frames are completed. encore() is called one more time 
       with \p handle = ENC_OPT_RELEASE to end the instance and release all the
       resources allocated for it.

    For multipass encoding:
    - The above single-pass encoding will be executed more than once. In the 
      first pass, the codec will measure and record the complexity of the video 
	The result is the analyzed to determine the best parameters for each 
	frame of encoding. In subsequent passes, the codec will encode the video 
	accordingly.

    \param handle   Handle of the encore instance
    \param enc_opt  Option for encoding: use one of the ENC_OPT* macros
    \param param1   First parameter (meaning depends on enc_opt)
    \param param2   First parameter (meaning depends on enc_opt)
    \return         Returns ENC_OK on success
*/
int encore(void *handle, int enc_opt, void *param1, void *param2);	




/* encore() options (the enc_opt parameter of encore()) */
#define ENC_OPT_INIT      0 /**< initialize the encoder, return a handle */
#define ENC_OPT_RELEASE   1	/**< release all the resource associated with the handle */
#define ENC_OPT_ENCODE    2	/**< encode a single frame */
#define ENC_OPT_ERRORCODE 3 /**< reserved for future usage */
#define ENC_OPT_VERSION	  4 /**< return information on version of codec interface */  
#define ENC_OPT_SETFEEDBACK  5 /**< not used in public releases */
#define ENC_OPT_TAGNAME   6   
#define ENC_OPT_PROFILE   7 /**< see paragraph 4.4 of API documentation */
#define ENC_OPT_BITRATE   8 /**< change target bitrate */
#define ENC_OPT_VBVRATE   9 /**< change VBV channel bitrate */
#define ENC_OPT_QUANT    10 /**< change quantiser used in constant-Q mode */
#define ENC_OPT_FORCE_I  12 /**< force the next frame to be an I frame */




/* encore version information */
#define ENCORE_VERSION		20021024    /**< Version number of API encore.h */
#define ENCORE_MAJOR_VERSION	5200    /**< DivX major release number */
     

/* return code of encore() */
#define ENC_BUFFER         -2
#define ENC_FAIL           -1
#define ENC_OK              0
#define ENC_MEMORY          1
#define ENC_BAD_FORMAT      2
#define ENC_INTERNAL        3

//used by the IDivXFilterInterface
struct STATE_INFO
{
	SETTINGS settings;
	int use_mvfile;
	int write_log;
	int psy_mode;
	int preproc_mode;
	int resize_mode_5;
	int write_nth_pass_log;
	char logfilename[256];
	char mvfilename[256];
	char nthpassfilename[256];
};

//Struture used by the VfW interface ICSetState and ICGetState

/*
struct STATE_INFO
{
	int fixed_size; // must be first field in the structure
	SETTINGS settings;
	int vbr_crispness;
	int postprocessing;
	int cpu_limit;
	int has_cpu_limit;
	int dirty;
	int do_add;
	int do_mc;
	int do_mc_b;
	int show_gmc;
	int do_writeyuv;
	int do_writeyuvmul;
	int write_mp4;
	int no_vbr_rc;
	int use_mvfile;
	int write_log;
	int use_dialogs;
	int disable_feedback;
	int psy_mode;
	int preproc_mode;
	int resize_mode_5;
	int write_nth_pass_log;
	int disable_profile_page;
	int active_profile;
	// ADD ONLY CHAR[]'S BELOW THIS LINE
	char logfilename[256];
	char mp4filename[256];
	char yuvoutdirname[256];
	char mvfilename[256]; // non-empty even if MV reuse is disabled
	char nthpassfilename[256];
};


struct STATE_DATA: STATE_INFO
{
	STATE_DATA()
	{
		memset(this, 0, sizeof(STATE_INFO));
		fixed_size = (char*)&logfilename[0]-(char*)&fixed_size;
	}
	void write(char* pointer)
	{
		memcpy(pointer, &fixed_size, fixed_size);
		pointer+=fixed_size;
		strcpy(pointer, logfilename);
		pointer+=strlen(logfilename)+1;
		strcpy(pointer, mp4filename);
		pointer+=strlen(mp4filename)+1;
		strcpy(pointer, mvfilename);
		pointer+=strlen(mvfilename)+1;
		strcpy(pointer, nthpassfilename);
		pointer+=strlen(nthpassfilename)+1;
	}

	int read(const char* pointer)
	{
		const char* save_pointer=pointer;
		memcpy(&fixed_size, pointer, fixed_size);
		pointer+=fixed_size;
		strcpy(logfilename, pointer);
		pointer+=strlen(logfilename)+1;
		strcpy(mp4filename, pointer);
		pointer+=strlen(mp4filename)+1;
		strcpy(mvfilename, pointer);
		pointer+=strlen(mvfilename)+1;
		return pointer-save_pointer;
	}
};
*/
#ifdef __cplusplus
};
#endif

#endif
