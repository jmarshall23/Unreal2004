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

/** $Id: decore.h,v 1.45 2003/08/27 14:11:43 eagle Exp $
 **
 **/

/*************************************************************************/

/*
 *  Copyright (C) 2001 - DivXNetworks
 *
 * Adam Li
 * Andrea Graziani
 * Jonathan White
 *
 * DivX Advanced Research Center <darc@projectmayo.com>
 *
 */

/** \file decore.h
    This is the header file describing the entrance function of the decoder core
    or "decore".  All interaction with decore is performed using one function:
    decore().
	\verbatim
	$Id: decore.h,v 1.45 2003/08/27 14:11:43 eagle Exp $
	\endverbatim
*/

#ifndef _DECORE_H_
#define _DECORE_H_

#ifdef __cplusplus
extern "C" {
#endif 

#ifdef WIN32
#define STDCALL _stdcall
#else
#define STDCALL
#endif // WIN32

/**
 *
**/

// decore options
#define DEC_OPT_INIT         1 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_RELEASE      2 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_SETOUT       3 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_ADJUST       4 /**< See the \ref sect5 "API documentation". */ 
#define DEC_OPT_FRAME        5 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_INIT_VOL     6 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_FLUSH        7 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_VERSION      8 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_SETDEBUG     9 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_CONVERTYUV  10 /**< See the \ref sect5 "API documentation". */
#define DEC_OPT_CONVERTYV12 11 /**< See the \ref sect5 "API documentation". */

// decore return values
#define DEC_OK			0 /**< See the \ref sect5 "API documentation". */
#define DEC_MEMORY		1 /**< See the \ref sect5 "API documentation". */
#define DEC_BAD_FORMAT	2 /**< See the \ref sect5 "API documentation". */
#define DEC_INVALID_ARGUMENT 3 /**< See the \ref sect5 "API documentation". */
#define DEC_NOT_IMPLEMENTED 4 /**< See the \ref sect5 "API documentation". */

#define DECORE_VERSION		20021112 /**< See the \ref sect5 "API documentation". */

#define DEC_ADJ_POSTPROCESSING 0  /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_BRIGHTNESS 1 /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_CONTRAST 2 /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_SATURATION 3 /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_WARMTH 4 /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_POSTPROCESSING_ALLMODES 5 /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_POSTPROCESSING_THR 6 /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_SET 0 /**< See the \ref sect5 "API documentation". */
#define DEC_ADJ_RETRIEVE 0x80000000 /**< See the \ref sect5 "API documentation". */



// fixme: add watermarking to the API doc
#include "portab.h"

/** Structure with parameters used to initialize a decoder instance created by #decore(#DEC_OPT_INIT). */    
typedef struct
{
	uint32_t codec_version;
	uint32_t smooth_playback;
    uint32_t flag_logo;
    void* (*alloc) (uint32_t);
    void (*free) (void*);
#ifdef _WATERMARKING
	uint32_t watermarking_mode;
	void *watermarking_chain;
#endif
} DEC_INIT;


#ifndef  DivXBitmapInfoHeader_defined
#define DivXBitmapInfoHeader_defined

/**
	Structure used to describe format of uncompressed video frames that
	are inputted to the encoder via encore() and that are required on 
	the output from the decoder via decore(). 
	See the appropriate section in the \ref sect41 "encoder" or 
	\ref sect53 "decoder" documentation for more details. */
typedef struct 
{
    uint32_t biSize;     /**< Total size of the DivXBitmapInfoHeader structure in bytes. */
    int32_t  biWidth;    /**< Specifies the width of the bitmap, in pixels. */
    int32_t  biHeight;   /**< Specifies the height of the bitmap, in pixels. */ 
    uint16_t biPlanes;   /**< Number of planes.  Must be set to 1. */ 
    uint16_t biBitCount; /**< Number of bits-per-pixel.  Used to distinguish different RGB formats. */ 

	/**
		Video format.  Set to zero for RGB, or to a FOURCC value.  The following macro can be used to define a FOURCC:

		\code
		
#define FOURCC(A, B, C, D) ( \
		  ((uint8_t) (A)) | \
		  (((uint8_t) (B))<<8) | \
		  (((uint8_t) (C))<<16) | \
		  (((uint8_t) (D))<<24) )
uint32_t fccYV12=FOURCC(‘Y’, ‘V’, ‘1’, ‘2’); // Example macro usage: fourcc "YV12"
		\endcode
	*/
    uint32_t biCompression; 
    uint32_t biSizeImage;     /**< Specifies size of the image, in bytes. Ignored by encore(). */
    int32_t  biXPelsPerMeter; /**< Ignored by encore(). */
    int32_t  biYPelsPerMeter; /**< Ignored by encore(). */
    uint32_t biClrUsed;       /**< Ignored by encore(). */
    uint32_t biClrImportant;  /**< Ignored by encore(). */
} DivXBitmapInfoHeader; 
#endif

/** Structure to convey input bitstream and decoder buffer location to decoder when decoding a frame. */ 
typedef struct
{
	void *bmp; ///< decoded bitmap 
	const void *bitstream; ///< decoder buffer
	uint32_t length; ///< length of the decoder stream
	uint32_t render_flag;	///< 1: the frame is going to be rendered
	uint32_t stride; ///< decoded bitmap stride, in pixels ( not bytes! is it good? )
#ifdef _WATERMARKING
	void *output_stream;
	uint32_t output_length;
	uint32_t *output_used;
#endif
    uint32_t skip_decoding;
} DEC_FRAME;

/** Structure that can be used to get extended information about a decoded frame. */
typedef struct
{
	const char *quant_store;
	uint32_t quant_stride;
	uint32_t prediction_type;
	uint32_t frame_length;
	uint32_t frame_num;
	uint32_t vop_coded;

    void *y;
	void *u;
	void *v;
	uint32_t stride_y;
	uint32_t stride_uv;

} DEC_FRAME_INFO;

/** Structure that is populated by #decore(#DEC_OPT_INIT_VOL) using information extracted by parsing video headers. */
typedef struct
{
	uint32_t x_dim; 
	uint32_t y_dim;
	uint32_t time_incr;
	uint32_t codec_version;
	uint32_t build_number;
	uint32_t prefixed;
} DEC_VOL_INFO;
/**
 *
**/

/**
	Main decode engine entrance point.

	For full details of decoder operation see the \ref sect5 "API documentation".

	\param handle   Handle of the decore instance.
    \param dec_opt  Method specifier - controls function performed by this call to decore().  Use one of the "DEC_OPT" macros:
		- #DEC_OPT_INIT       
		- #DEC_OPT_RELEASE    
		- #DEC_OPT_SETOUT     
		- #DEC_OPT_ADJUST     
		- #DEC_OPT_FRAME      
		- #DEC_OPT_INIT_VOL   
		- #DEC_OPT_FLUSH      
		- #DEC_OPT_VERSION    
		- #DEC_OPT_SETDEBUG   
		- #DEC_OPT_CONVERTYUV 
		- #DEC_OPT_CONVERTYV12
    \param param1   First parameter (meaning depends on \p dec_opt)
    \param param2   First parameter (meaning depends on \p dec_opt)
    \return         Returns one of the following:
		- #DEC_OK			 
		- #DEC_MEMORY		 
		- #DEC_BAD_FORMAT	 
		- #DEC_INVALID_ARGUMENT 
		- #DEC_NOT_IMPLEMENTED  
*/
int decore(void* handle, int dec_opt, void *param1, void *param2);

#ifdef __cplusplus
}
#endif 

#endif // _DECORE_H_

