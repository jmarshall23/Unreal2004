/*           Copyright (C) 2001 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */

#ifndef ps2gl_h
#define ps2gl_h

#include "GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

   // immBufferVertexSize is the size in vertices of the buffers used to store
   // glBegin/glEnd geometry. there are currently 2 sets of buffers:
   // vertex, normal, tex coord, and color buffers.
   extern int	pglInit( int immBufferVertexSize );
   extern int	pglHasLibraryBeenInitted(void);
   extern void	pglFinish(void);

   extern void	pglWaitForVU1(void);
   extern void	pglWaitForVSync(void);
   extern void	pglSwapBuffers(void);

   // gs memory allocation

   extern void pglPrintGsMemAllocation(void);
   extern int pglHasGsMemBeenInitted(void);

   // gs mem slots

   typedef unsigned int pgl_slot_handle_t;

   extern pgl_slot_handle_t pglAddGsMemSlot( int startingPage, int pageLength, unsigned int pixelMode );
   extern void pglLockGsMemSlot( pgl_slot_handle_t slot_handle );
   extern void pglUnlockGsMemSlot( pgl_slot_handle_t slot_handle );
   extern void pglRemoveAllGsMemSlots();

   // gs mem areas

   typedef unsigned int pgl_area_handle_t;

   extern pgl_area_handle_t pglCreateGsMemArea( int width, int height, unsigned int pix_format );
   extern void pglDestroyGsMemArea( pgl_area_handle_t mem_area );

   extern void pglAllocGsMemArea( pgl_area_handle_t mem_area );
   extern void pglFreeGsMemArea( pgl_area_handle_t mem_area );

   extern void pglSetGsMemAreaWordAddr( pgl_area_handle_t mem_area, unsigned int addr );

   extern void pglBindGsMemAreaToSlot( pgl_area_handle_t mem_area, pgl_slot_handle_t mem_slot );
   extern void pglUnbindGsMemArea( pgl_area_handle_t mem_area );

   extern void pglLockGsMemArea( pgl_area_handle_t mem_area );
   extern void pglUnlockGsMemArea( pgl_area_handle_t mem_area );

   extern int pglGsMemAreaIsAllocated( pgl_area_handle_t mem_area );
   extern unsigned int pglGetGsMemAreaWordAddr( pgl_area_handle_t mem_area );

   // display and draw management

   extern void pglSetDisplayBuffers( pgl_area_handle_t frame0_mem, pgl_area_handle_t frame1_mem );
   extern void pglSetDrawBuffers( pgl_area_handle_t frame0_mem, pgl_area_handle_t frame1_mem,
				  pgl_area_handle_t depth_mem );

   // textures

   void pglTextureFromGsMemArea( pgl_area_handle_t tex_area_handle );

   // geometry

   void pglNormalPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ps2gl_h
