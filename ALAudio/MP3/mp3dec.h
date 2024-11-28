//############################################################################
//##                                                                        ##
//##  Miles Sound System                                                    ##
//##                                                                        ##
//##  MPAUDIO.H: Private header file for MPEG decoder                       ##
//##                                                                        ##
//##  Version 1.00 of 6-Apr-98: Initial                                     ##
//##                                                                        ##
//##  Author: John Miles                                                    ##
//##                                                                        ##
//############################################################################
//##                                                                        ##
//##  Contact RAD Game Tools at 425-893-4300 for technical support.         ##
//##                                                                        ##
//############################################################################

#include "mss.h"

#pragma pack(push,1)

#define SBLIMIT 32                // 32 subbands
#define SSLIMIT 18                // 18 frequency lines/subband

#define HEAD_BUFSIZE   64         // Header buffer = 64 bytes
#define STREAM_BUFSIZE 8192       // Stream buffer = 2 x 4K blocks
#define FRAME_BUFSIZE  32768      // Output audio frame buffer = 32K bytes

#define MPG_MD_STEREO       0
#define MPG_MD_JOINT_STEREO 1
#define MPG_MD_DUAL_CHANNEL 2
#define MPG_MD_MONO         3

#define MPG_MD_LR_LR        0
#define MPG_MD_LR_I         1
#define MPG_MD_MS_LR        2
#define MPG_MD_MS_I         3

#ifndef PI
#define PI 3.14159265358979f
#endif

extern S32 ASI_started;

extern C8 ASI_error_text[256];

typedef S32 (*FRAMEFN)(struct ASISTREAM *STR);
typedef S32 (*FINFOFN)(struct ASISTREAM *STR);

//
// Huffman source table representation
//

struct HTABLE
{
   U8 x;
   U8 y;
   U8 len;
   C8 *string;
};

//
// Huffman tree node
//
// Root node is index 0; any child pointer to node 0 indicates that
// this is a terminal node with valid x,y members
//

struct HUFFNODE
{
   S32 child[2];
   S16 x;
   S16 y;
};

//
// Structure for layer-3 scale factors, from ISO reference source
//

typedef struct
{
   S32 l[23];            // [cb]
   S32 s[3][13];         // [window][cb]
}
III_scalefac_t[2];       // [ch]

//
// Attribute tokens
//

enum ATTRIB
{
   //
   // Additional decoder attribs (beyond standard RIB PROVIDER_ attributes)
   //

   IN_FTYPES,               // STRING supported input file types
   IN_WTAG,                 // Tag used for this data in a wave file
   OUT_FTYPES,              // STRING supported output file types
   FRAME_SIZE,              // S32 worst-case frame buffer size

   //
   // Generic ASI stream attribs
   //

   INPUT_BIT_RATE,          // S32 input bit rate
   INPUT_SAMPLE_RATE,       // S32 sample rate of input data
   INPUT_BITS,              // S32 bit width per sample of input data
   INPUT_CHANNELS,          // S32 # of channels in input data

   OUTPUT_BIT_RATE,         // S32 output bit rate
   OUTPUT_SAMPLE_RATE,      // S32 output sample rate
   OUTPUT_BITS,             // S32 bit width per sample of output data
   OUTPUT_CHANNELS,         // S32 # of channels in output data

   POSITION,                // S32 bytes processed so far
   PERCENT_DONE,            // % percent done
   MIN_INPUT_BLOCK_SIZE,    // S32 minimum block size for input

   //
   // Codec-specific stream attribs
   //

   MPEG_VERSION,            // S32 MPEG version
   MPEG_LAYER,              // S32 MPEG layer

   //
   // Stream preferences
   //

   REQUESTED_RATE,          // S32 requested rate for output data
   REQUESTED_BITS,          // S32 requested bit width for output data
   REQUESTED_CHANS,         // S32 requested # of channels for output data
};


extern ASIRESULT	ASI_startup(void);
extern ASIRESULT	ASI_shutdown(void);
extern HASISTREAM	ASI_stream_open(FILE* file_handle,U32 total_size);
extern ASIRESULT	ASI_stream_close(HASISTREAM stream);
extern S32			ASI_stream_process(HASISTREAM  stream, void *buffer, S32 request_size);
extern ASIRESULT	ASI_stream_seek(HASISTREAM stream, S32 stream_offset);
extern S32			ASI_stream_attribute(HASISTREAM stream, HATTRIB attribute);

//
// Stream structure
//

typedef struct ASISTREAM
{
   //
   // Stream creation parameters
   //

   FILE*		 file_handle;
   U32           total_size;

   //
   // Preferences (not currently supported)
   //

   S32 requested_rate;
   S32 requested_bits;
   S32 requested_chans;

   //
   // Other stream source parms
   //

   U32 current_offset;

   //
   // Input stream buffer
   //
   // Buffer is split into two halves to ensure that previous buffer is
   // always available for audio data retrieval.  As soon as a read-request
   // would cause a buffer overflow, the second half of the buffer is copied
   // to the first half to free up sufficient space for the incoming data.
   //

   U8  audio_buffer[STREAM_BUFSIZE];
   S32 write_cursor;                // Byte offset for incoming data

   S32 apos;                        // Current bit-fetch position in audio buffer

   U8  header_buffer[HEAD_BUFSIZE]; // Raw header and side info for current frame
                                    // (Actual size of side info data depends on MPEG layer #)
   S32 hpos;                        // Current bit position in header/side buffer

   S32 header_size;                 // 4 or 6 bytes, depending on CRC
   S32 side_info_size;              // Valid for layer 3 side info only

   //
   // Output frame buffer
   //
   // MPEG streams are decoded one frame at a time, and returned to the
   // application on demand
   //

   U8  *frame_buffer;
   S32 frame_size;                  // Size of output frame in bytes
   S32 output_cursor;               // Next location from which to copy output data

   //
   // Header information for current frame
   //

   S32 MPEG1;                       // 1=MPEG1, 0=MPEG2 or MPEG2.5
   S32 MPEG25;                      // 1=MPEG2.5, 0=MPEG1 or MPEG2
   S32 layer;
   S32 protection_bit;
   S32 bitrate_index;
   S32 sampling_frequency;
   S32 padding_bit;
   S32 private_bit;
   S32 mode;
   S32 mode_extension;
   S32 copyright;
   S32 original;
   S32 emphasis;

   //
   // Copy of certain fields from first header encountered in stream,
   // used for validation of subsequent headers
   //

   S32 check_valid;
   S32 check_MPEG1;
   S32 check_MPEG25;
   S32 check_layer;
   S32 check_protection_bit;
   S32 check_sampling_frequency;
   S32 check_mode;
   S32 check_copyright;
   S32 check_original;

   //
   // Side information for current frame
   //

   S32 main_data_begin;
   S32 scfsi                 [2][32];
   S32 part2_3_length        [2][2];
   S32 big_values            [2][2];
   S32 global_gain           [2][2];
   S32 scalefac_compress     [2][2];
   S32 window_switching_flag [2][2];
   S32 block_type            [2][2];
   S32 mixed_block_flag      [2][2];
   S32 table_select          [2][2][3];
   S32 subblock_gain         [2][2][3];
   S32 region0_count         [2][2];
   S32 region1_count         [2][2];
   S32 preflag               [2][2];
   S32 scalefac_scale        [2][2];
   S32 count1table_select    [2][2];
   S32 allocation            [2][32];

   //
   // Layer 3 scale factors for long and short blocks
   //

   III_scalefac_t scalefac[2];      // [granule]

   //
   // MPEG2-specific data
   //

   S32 intensity_scale;
   S32 pos_limit[32];

   //
   // Miscellaneous data for current frame, derived by seek_frame()
   //

   S32 bit_rate;
   S32 sample_rate;
   S32 average_frame_size;
   S32 data_size;
   S32 nch;
   S32 ngr;

   //
   // Integer frequency coefficients
   //
   // Note extra 2 words at end to accommodate last half of final quadword
   // in count1 partition
   //

   S16 is     [2][578];
   U8  is_sign[2][578];

   S32 zero_part[2];
   S32 zero_count[2];

   //
   // Real frequency-coefficient workspace
   //

   F32 xr[2][32][18];
   F32 lr[2][32][18];

   F32 sample[2][32][36];

   //
   // Persistent storage for IMDCT -- must be cleared to zero at seek time
   //

   F32 s[2][32][18];

   //
   // Input to poly() for a single channel
   //
   // This also serves as the output array for the IMDCT prefilter stage in
   // Layer 3
   //

   F32 res[32][18];

   //
   // Persistent storage for POLY
   //

   F32    u      [2][2][17][16];
   S32    u_start[2];
   S32    u_div  [2];

   //
   // Output pointer
   //

   S16 *samples;

   //
   // Miscellaneous stream decoder data
   //

   S32 seek_param;

   //
   // Layer-dependent function pointers
   //

   FRAMEFN frame_function;
   FINFOFN frame_info_function;
} ASISTREAM;

//
// General prototypes
//

S32 fetch_audio_data(ASISTREAM *STR);
S32 seek_frame(ASISTREAM *STR, S32 offset);

//
// Layer-specific prototypes
//

S32 L3_init(void);
S32 L3_destroy(void);

S32 L3_frame(ASISTREAM *STR);

S32 L3_frame_info(ASISTREAM *STR);

//
// Math prototypes
//

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef IS_X86

extern void __stdcall x86_poly_filter(const F32 *src,
                                    const F32 *b,
                                    S32       phase,
                                    F32       *out1,
                                    F32       *out2);

extern void __stdcall x86_dewindow_and_write(F32 *u,
                                            F32 *dewindow,
                                            S32 start,
                                            S16 *samples,
                                            S32 output_step,
                                            S32 div);

extern void __stdcall x86_IMDCT_3x12        (F32 *in,
                                           S32 sb,
                                           F32 *result,
                                           F32 *save);

extern void __stdcall x86_IMDCT_1x36        (F32 *in,
                                           S32 sb,
                                           F32 *result,
                                           F32 *save,
                                           F32 *window);

#else


extern void AILCALL PPC_poly_filter(const F32 *src,
                                    const F32 *b,
                                    S32       phase,
                                    F32       *out1,
                                    F32       *out2);

extern void AILCALL PPC_dewindow_and_write(F32 *u,
                                           F32 *dewindow,
                                           S32 start,
                                           S16 *samples,
                                           S32 output_step,
                                           S32 div);

extern void AILCALL PPC_IMDCT_3x12    (F32 *in,
                                       S32 sb,
                                       F32 *result,
                                       F32 *save);

extern  void AILCALL PPC_IMDCT_1x36    (register F32 *input,
                                        register S32 sb,
                                        register F32 *result,
                                        register F32 *last,
                                        register F32 *window);

#endif

#ifdef __cplusplus
}
#endif

//############################################################################
//#                                                                          #
//# Macros to acquire bitfield data of length n from header/side or audio    #
//# buffers, n <= 24                                                         #
//#                                                                          #
//# Bit position 0 is MSB of byte 0                                          #
//#                                                                          #
//# Request for 0 bits is considered valid, returning 0                      #
//#                                                                          #
//############################################################################

#define H(n) (n ? read_bits(STR->header_buffer, &STR->hpos, (n)) : 0)
#define A(n) (n ? read_bits(STR->audio_buffer,  &STR->apos, (n)) : 0)
#define V(n) (n ? view_bits(STR->audio_buffer,  &STR->apos, (n)) : 0)

#ifdef IS_LE

U32 inline read_bits(U8 *data, S32 *bitpos, S32 n)
{
   U32 val;

#ifdef IS_WIN32

   _asm
      {
      mov edx,bitpos
      mov ebx,data

      mov ecx,[edx]
      mov eax,ecx

      and ecx,7
      shr eax,3

      mov eax,[ebx][eax]
      bswap eax

      shl eax,cl

      mov ecx,n
      add [edx],ecx

      mov ecx,32
      sub ecx,n
      shr eax,cl
      mov val,eax
      }

#else

   S32     b = *bitpos;
   U8	  *p = &data[b >> 3];

   val = ((U32)(p[3]))        +
        (((U32)(p[2])) << 8)  +
        (((U32)(p[1])) << 16) +
        (((U32)(p[0])) << 24);

   val <<= b & 7;
   val >>= 32 - n;

   *bitpos = b + n;

#endif

   return val;
}

U32 inline view_bits(U8 *data, S32 *bitpos, S32 n)
{
   U32 val;

#ifdef IS_WIN32

   _asm
      {
      mov edx,bitpos
      mov ebx,data

      mov ecx,[edx]
      mov eax,ecx

      and ecx,7
      shr eax,3

      mov eax,[ebx][eax]
      bswap eax

      shl eax,cl

      mov ecx,32
      sub ecx,n
      shr eax,cl
      mov val,eax
      }

#else

   S32     b = *bitpos;
   U8	  *p = &data[b >> 3];

   val = ((U32)(p[3]))        +
        (((U32)(p[2])) << 8)  +
        (((U32)(p[1])) << 16) +
        (((U32)(p[0])) << 24);

   val <<= b & 7;
   val >>= 32 - n;

#endif

   return val;
}

#else

U32 inline read_bits(U8 *data, S32 *bitpos, S32 n)
{
   U32 val;
   S32     b = *bitpos;

   val = *(U32*)&data[b>>3];

   val <<= b & 7;
   val >>= 32 - n;

   *bitpos = b + n;

   return val;
}

U32 inline view_bits(U8 *data, S32 *bitpos, S32 n)
{
   U32 val;

   S32     b = *bitpos;
   U8     *p = &data[b >> 3];

   val = *(U32*)&data[b>>3];

   val <<= b & 7;
   val >>= 32 - n;

   return val;
}

#endif

#pragma pack(pop)
