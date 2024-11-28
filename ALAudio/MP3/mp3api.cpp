//############################################################################
//##                                                                        ##
//##  Miles Sound System                                                    ##
//##                                                                        ##
//##  API.CPP: ASI decoder module for MPEG audio                            ##
//##                                                                        ##
//##  16-bit protected-mode source compatible with MSC 7.0                  ##
//##  32-bit protected-mode source compatible with MSC 11.0/Watcom 10.6     ##
//##                                                                        ##
//##  Version 1.00 of 8-Apr-98: Initial                                     ##
//##                                                                        ##
//##  Author: John Miles                                                    ##
//##                                                                        ##
//############################################################################
//##                                                                        ##
//##  Copyright (C) RAD Game Tools, Inc.                                    ##
//##                                                                        ##
//##  Contact RAD Game Tools at 425-893-4300 for technical support.         ##
//##                                                                        ##
//############################################################################

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "mp3dec.h"

S32 ASI_started = 0;

C8 ASI_error_text[256];

//############################################################################
//#                                                                          #
//# Read one frame's worth of audio data into stream buffer                  #
//#                                                                          #
//# Return byte offset of beginning of new data in buffer, or -1 on error    #
//#                                                                          #
//############################################################################

S32 fetch_audio_data(ASISTREAM *STR)
{
   //
   // See if incoming data block will fit in buffer at current write position
   // If not, discard data in lower half of buffer and move newer data down to
   // make room
   //
   // Ensure that 4 bytes of valid overrun data always exists at end of
   // buffer, to avoid page faults in read_bits()
   //
   // Incoming data block is assumed to be smaller than (STREAM_BUFSIZE / 2)!
   // Furthermore, we assume that read and write cursors are in the second
   // half of the buffer by the time an overflow condition occurs.  Two 4K
   // buffer halves will be more than sufficient to guarantee these criteria
   // for all valid MPEG frame sizes.
   //

   S32 half = STREAM_BUFSIZE / 2;

   if (STR->write_cursor + STR->data_size >= (STREAM_BUFSIZE-4))
      {
      memcpy(STR->audio_buffer,
                &STR->audio_buffer[half],
                 half);

      STR->write_cursor -= half;

      STR->apos -= (half * 8);
      }

   S32 result = fread( &STR->audio_buffer[STR->write_cursor], 1, STR->data_size, STR->file_handle );

   if (result < STR->data_size)
      {
#if 0
      sprintf(ASI_error_text,"Truncated MPEG audio stream, %ld bytes read, %ld expected",
         result, STR->data_size);
      return -1;
#else
   //
   // Reference encoder seems to truncate final frame, so we'll do the padding
   // here...
   //

   memset(&STR->audio_buffer[result],
               0,
               STR->data_size - result);
#endif
      }

   STR->current_offset += STR->data_size;

   //
   // Advance write cursor and return its original value
   //

   STR->write_cursor += STR->data_size;

   return STR->write_cursor - STR->data_size;
}

//############################################################################
//#                                                                          #
//# Seek next frame header in stream, and write current frame information    #
//# to ASISTREAM structure                                                   #
//#                                                                          #
//############################################################################

S32 seek_frame(ASISTREAM *STR, //)
               S32            offset)
{
   //
   // Fetch data from stream until valid header encountered
   //

   if (offset != -1)
      {
      STR->current_offset = offset;
      }

  S32 found_layer = -1;
  S32 start_offset = STR->current_offset;
  S32 result;
  U32 seek_data = 0;
  U32 dest_data = 0;

  again:

   S32 seek_offset = offset;

   S32 Dummy = 1;

   while( Dummy == 1 )
      {
#ifdef IS_LE
      seek_data >>= 8;
#else
      seek_data <<= 8;
#endif

      if ( ( STR->current_offset - start_offset ) > AIL_MAX_FILE_HEADER_SIZE )
      {
        goto not_found;
      }

	  if( seek_offset != -1 )
		fseek( STR->file_handle, seek_offset, SEEK_SET );

	  result = fread( ((U8*)&dest_data)+3, 1, 1, STR->file_handle );

      if (result < 1)
         {
         //
         // (Not considered an error)
         //

//       strcpy(ASI_error_text,"End of data reached");
         return 0;
         }

      seek_data |= dest_data;

      seek_offset = -1;
      STR->current_offset++;

#ifdef IS_LE
      if ( (seek_data & 0x0000e0ff ) == 0x0000e0ff )
#else
      if ( (seek_data & 0xffe00000 ) == 0xffe00000 )
#endif
         {
         break;
         }

      }

   //
   // Read rest of header dword
   //

   *((U32 *)STR->header_buffer) = seek_data;


   //
   // Parse header, first skipping 11-bit sync field
   //

   STR->hpos = 11;

   STR->MPEG25             = !H(1);
   STR->MPEG1              =  H(1);
   STR->layer              =  H(2);
   STR->protection_bit     =  H(1);
   STR->bitrate_index      =  H(4);
   STR->sampling_frequency =  H(2);
   STR->padding_bit        =  H(1);
   STR->private_bit        =  H(1);
   STR->mode               =  H(2);
   STR->mode_extension     =  H(2);
   STR->copyright          =  H(1);
   STR->original           =  H(1);
   STR->emphasis           =  H(2);

   //
   // Perform sanity check on header, since most encoders seem to be written
   // with complete disregard to the rule against >= 12 consecutive 1 bits
   // in stream....
   //
   // We assume the first header found in the stream is valid, and use its
   // contents to check the fields of all subsequent headers.  The fields
   // tested are those which should not ordinarily change in the course of
   // a single stream.
   //

   if ((STR->bitrate_index      == 0x0f) ||
       (STR->sampling_frequency == 0x03) )
   {
      //
      // Header contains one or more invalid bitfields, so skip it
      //
      // (Note that this will skip a valid frame if it begins within
      // the first 4 bytes of a false header)
      //

      offset = -1;
      goto again;
   }

   // keep searching if we find a non layer 3 block
   if ( STR->layer!= 1)
   {
     found_layer = STR->layer;
     offset = -1;
     goto again;
   }

   if (!STR->check_valid)
      {
      STR->check_valid = 1;

      STR->check_MPEG25             = STR->MPEG25;
      STR->check_MPEG1              = STR->MPEG1;
      STR->check_layer              = STR->layer;
      STR->check_protection_bit     = STR->protection_bit;
      STR->check_sampling_frequency = STR->sampling_frequency;
      STR->check_mode               = STR->mode;
      STR->check_copyright          = STR->copyright;
      STR->check_original           = STR->original;
      }
   else
      {
      if ((STR->MPEG1              != STR->check_MPEG1)              ||
          (STR->MPEG25             != STR->check_MPEG25)             ||
          (STR->layer              != STR->check_layer)              ||

//          (STR->protection_bit     != STR->check_protection_bit)     ||
//          (STR->mode               != STR->check_mode)               ||
//          (STR->copyright          != STR->check_copyright)          ||
//          (STR->original           != STR->check_original)           ||

          (STR->sampling_frequency != STR->check_sampling_frequency) )
          {

          //
          // Header does not match characteristics of first one found in
          // stream -- keep looking
          //
          // (Note that this will skip a valid frame if it begins within
          // the first 4 bytes of a false header)
          //

          offset = -1;
          goto again;
          }

      }

   //
   // Skip CRC word if present
   //

   STR->header_size = 4;

   if (STR->protection_bit == 0)
      {

     result = fread( &STR->header_buffer[4], 1, 2, STR->file_handle );

     if (result < 1)
         {
          not_found:

           if (found_layer == 2)
             {
             strcpy(ASI_error_text,"MPEG Layer 2 files not supported");
             return 0 ; // we don't support layer 2
             }
           else if (found_layer == 3)  // the 3 means layer 1 - cool!
             {
             strcpy(ASI_error_text,"MPEG Layer 1 files not supported");
             return 0 ; // we don't support layer 1
             }
           else  if (found_layer == 0) // reserved layer
             {
             strcpy(ASI_error_text,"Undefined/reserved MPEG layer not supported");
             return 0;
             }

           strcpy(ASI_error_text,"MPEG audio header not found or is badly formatted");
           return 0;
         }

      STR->current_offset += 2;

      STR->hpos += 16;

      STR->header_size = 6;
      }

   STR->frame_function      = L3_frame;
   STR->frame_info_function = L3_frame_info;

   //
   // Call frame info function to fetch stream data
   //

   if (!STR->frame_info_function(STR))
      {
      //
      // End of stream reached, or error occurred
      //

      return 0;
      }

   //
   // Return success
   //

   return 1;
}

//############################################################################
//#                                                                          #
//# Initialize ASI stream decoder                                            #
//#                                                                          #
//############################################################################

ASIRESULT  ASI_startup     (void)
{
   if (ASI_started++)
      {
      strcpy(ASI_error_text,"Already started");
      return ASI_ALREADY_STARTED;
      }

   //
   // Init static prefs/attributes
   //

   ASI_error_text[0] = 0;

   //
   // Init layer-specific data
   //

   L3_init();

   return ASI_NOERR;
}

//############################################################################
//#                                                                          #
//# Shut down ASI stream decoder                                             #
//#                                                                          #
//############################################################################

ASIRESULT       ASI_shutdown    (void)
{
   if (!ASI_started)
      {
      strcpy(ASI_error_text,"Not initialized");
      return ASI_NOT_INIT;
      }

   --ASI_started;

   //
   // Destroy layer-specific data
   //

   L3_destroy();

   return ASI_NOERR;
}

//############################################################################
//#                                                                          #
//# Open a stream, returning handle to stream                                #
//#                                                                          #
//############################################################################

HASISTREAM    ASI_stream_open (
                                       FILE* file_handle,
                                        U32           total_size)
{
   //
   // Allocate ASISTREAM descriptor
   //

   ASISTREAM *STR = (ASISTREAM *) malloc(sizeof(ASISTREAM));

   if (STR == NULL)
      {
      strcpy(ASI_error_text,"Out of memory");
      return 0;
      }

   memset(STR, 0, sizeof(ASISTREAM));

   //
   // Init prefs
   //

   STR->requested_rate = -1;
   STR->requested_bits = -1;
   STR->requested_chans = -1;

   //
   // Copy params to descriptor fields
   //

   STR->file_handle = file_handle;
   STR->total_size  = total_size;

   //
   // Alloc frame buffer
   //

   STR->frame_buffer = (U8 *) malloc(FRAME_BUFSIZE);

   if (STR->frame_buffer == NULL)
      {
      strcpy(ASI_error_text,"Out of memory");
      free(STR);
      return 0;
      }

   //
   // Initialize input buffer
   //

   STR->write_cursor = 0;
   STR->apos         = 0;

   //
   // Initialize output buffer
   //

   STR->frame_size = 0;
   STR->output_cursor = 0;

	
   // Handle ID3.
	unsigned char buffer[10];
	int mp3_offset = 0;
	fread( buffer, 1, 10, STR->file_handle );
   
	if(	( buffer[ 0 ] == 0x49 ) && ( buffer[ 1 ] == 0x44 ) 
	&&	( buffer[ 2 ] == 0x33 ) && ( buffer[ 3 ] < 0xff ) 
	&&	( buffer[ 4 ] < 0xff ) && ( buffer[ 6 ] < 0x80 ) 
	&&	( buffer[ 7 ] < 0x80 ) && ( buffer[ 8 ] < 0x80 ) 
	&& ( buffer[ 9 ] < 0x80 ) )
	{
		// yes, this is an ID3
		mp3_offset = 10 + ( (U32) buffer[9] ) | ( ( (U32) buffer[8] ) << 7 ) | ( ( (U32) buffer[7] ) << 14 ) | ( ( (U32) buffer[6] ) << 21 );
	}
	else
		fseek( STR->file_handle, 0, SEEK_SET );
	
   //
   // Decode first frame header to obtain stream information
   //
   if (!seek_frame(STR,mp3_offset))
   {
      free(STR);
      return 0;
   }

   //
   // Force rewind to beginning of stream when first frame decoded
   //

   STR->seek_param = 0;

   memset(STR->s,0,sizeof(STR->s));
   memset(STR->res,0,sizeof(STR->res));

   STR->u_start[0] = STR->u_start[1] = 0;
   STR->u_div  [0] = STR->u_div  [1] = 0;

   //
   // Return descriptor address cast to handle
   //

   return (U32) STR;
}

//############################################################################
//#                                                                          #
//# Close stream, freeing handle and all internally-allocated resources      #
//#                                                                          #
//############################################################################

ASIRESULT       ASI_stream_close(HASISTREAM stream)
{
   ASISTREAM *STR = (ASISTREAM *) stream;

   if (STR->frame_buffer != NULL)
      {
      free(STR->frame_buffer);
      STR->frame_buffer = NULL;
      }

   free(STR);

   return ASI_NOERR;
}

//############################################################################
//#                                                                          #
//# Decode data from stream, returning # of bytes actually decoded           #
//#                                                                          #
//############################################################################

S32  ASI_stream_process (HASISTREAM  stream, //)
                                  void   *buffer,
                                  S32         request_size)
{
   ASISTREAM *STR = (ASISTREAM *) stream;

   //
   // Keep track of # of bytes originally requested
   //

   S32 original_request = request_size;

   //
   // Init buffer output offset
   //

   S32 write_cursor = 0;

   U8 *dest = (U8  *) buffer;

   //
   // If any data from last frame remains in buffer, copy it first
   //
   // Otherwise fetch and decode frame data until request size satisfied
   //

   while (request_size > 0)
      {
      //
      // Copy as much data as possible from currently-buffered frame
      //

      if (STR->output_cursor < STR->frame_size)
         {
         S32 avail = STR->frame_size - STR->output_cursor;

         if (avail > request_size)
            {
            avail = request_size;
            }

         memcpy(&dest[write_cursor],
                    &STR->frame_buffer[STR->output_cursor],
                     avail);

         STR->output_cursor += avail;
         write_cursor       += avail;
         request_size       -= avail;
         }

      //
      // Exit from loop if request completely fulfilled by existing buffer
      // contents
      //

      if (!request_size)
         {
         break;
         }

      //
      // Else initialize output frame buffer and fetch next frame
      //

      STR->frame_size = 0;
      STR->output_cursor = 0;

      //
      // Seek next frame based on current offset
      //

      if (!seek_frame(STR,STR->seek_param))
         {
         //
         // End of stream reached, or error occurred
         //

         break;
         }

      //
      // Set offset parameter to -1 for all subsequent frames, to allow
      // application to perform continuous streaming without explicit seeks
      //

      STR->seek_param = -1;

      //
      // Call frame-processing function
      //

      if (!STR->frame_function(STR))
         {
         //
         // End of stream reached, or error occurred
         //

         break;
         }
      }

   //
   // If source stream exhausted, pad with zeroes to end of buffer
   //

   if (request_size > 0)
      {
      memset(&dest[write_cursor],
                  0,
                  request_size);
      }

   //
   // Return # of bytes fetched from source stream
   //

   return original_request - request_size;
}

//############################################################################
//#                                                                          #
//# Restart stream decoding process at new offset                            #
//#                                                                          #
//############################################################################

ASIRESULT  ASI_stream_seek (HASISTREAM stream, //)
                                     S32        stream_offset)
{
   ASISTREAM  *STR = (ASISTREAM  *) stream;

   //
   // Initialize input buffer
   //

   STR->write_cursor = 0;
   STR->apos         = 0;

   //
   // Initialize output buffer
   //

   if (stream_offset!=-2)
   {
     STR->frame_size = 0;
     STR->output_cursor = 0;
   }
   else
   {
     stream_offset=-1;
   }

   //
   // Set up to resume frame processing at specified seek position
   //
   // -1 causes current position to be maintained
   //

   STR->seek_param = stream_offset;

   //
   // Init output filter
   //

   memset(STR->s,   0, sizeof(STR->s));
   memset(STR->res, 0, sizeof(STR->res));

   STR->u_start[0] = STR->u_start[1] = 0;
   STR->u_div  [0] = STR->u_div  [1] = 0;

   //
   // Return success
   //

   return ASI_NOERR;
}

#define max(a,b)  (((a) > (b)) ? (a) : (b))

//############################################################################
//#                                                                          #
//# Retrieve an ASI stream attribute or preference value by index            #
//#                                                                          #
//############################################################################

S32      ASI_stream_attribute (HASISTREAM stream, //)
                                        HATTRIB    attribute)
{
   ASISTREAM  *STR = (ASISTREAM  *) stream;

   //
   // Sample rate in samples/second for [MPEG25][MPEG version][value]
   //

   const S32 sample_rate[2][2][4] =
   {{
      { 22050L,24000L,16000L,22050L },
      { 44100L,48000L,32000L,44100L }
   },
   {
      { 11025L,12000L, 8000L,11025L },
      { 44100L,48000L,32000L,44100L }
   }};

   switch ((ATTRIB) attribute)
      {
      //
      // Attributes
      //

      case INPUT_BIT_RATE:     return STR->bit_rate;
      case INPUT_SAMPLE_RATE:  return sample_rate[STR->MPEG25][STR->MPEG1][STR->sampling_frequency];
      case INPUT_BITS:         return 16 / max(1,((sample_rate[STR->MPEG25][STR->MPEG1][STR->sampling_frequency] * 16 * STR->nch) / STR->bit_rate));
      case INPUT_CHANNELS:     return STR->nch;

      case OUTPUT_BIT_RATE:    return sample_rate[STR->MPEG25][STR->MPEG1][STR->sampling_frequency] * 16 * STR->nch;
      case OUTPUT_SAMPLE_RATE: return sample_rate[STR->MPEG25][STR->MPEG1][STR->sampling_frequency];
      case OUTPUT_BITS:        return 16;
      case OUTPUT_CHANNELS:    return STR->nch;

      case POSITION:      return STR->current_offset;

      case MPEG_VERSION:  return 2 - STR->MPEG1;
      case MPEG_LAYER:    return 4 - STR->layer;

      case MIN_INPUT_BLOCK_SIZE: return (STREAM_BUFSIZE / 2);

      case PERCENT_DONE:

         if ((STR->current_offset == 0) || (STR->total_size == 0))
            {
            return 0;
            }
         else
            {
            F32 percent = F32(STR->current_offset) * 100.0F / F32(STR->total_size);

            return *(U32  *) (&percent);
            }

      //
      // Preferences
      //

      case REQUESTED_RATE:  return STR->requested_rate;
      case REQUESTED_BITS:  return STR->requested_bits;
      case REQUESTED_CHANS: return STR->requested_chans;
      }

   return -1;
}
