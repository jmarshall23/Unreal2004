#error this file is deprecated
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:53 $ - Revision: $Revision: 1.10.2.3 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#ifdef __cplusplus
extern "C" {
#endif

/**
    @file
    Loads a 24 bit per pixel.bmp file into a height matrix.
    This allocate memory that must be freed later.

    @param filename The name of the file to attempt to load.
    @param outHeightMatrix is allocated and contains the height array suitable
    for use with McdRGHeightFieldCreate.
    The height at a given position is a weighted sum of the color components,
    as follows:
     rMult*r + gMult*g + bMult*b + z0, where r,g,b are between 0 and 1.
    @param outXsize will contain the number of points along the x axis.
    @param outYsize will contain the number of points along the y axis.
    @a z0 is added to all heights.
    Returns 0 in case of failure, non zero otherwise.

*/
int MEAPI McduRGHeighFieldCreateHeightMatrixFromBmp(
                                char *filename,
                                MeReal rMult, MeReal gMult, MeReal bMult, MeReal z0,
                                MeReal **outHeightMatrix,
                                int *outXsize, int *outYsize);


/**
    Create a heightfield from a .bmp file.
    @a x0 and @y0 are the x and y coordinates of the left bottom corner of the
    heightfield.
    @a xSize and @a ySize are the sizes in each dimension.
    @a rMult, @a gMult, @bMult are multipliers of the red, greed, and blue
    components of the image, each being between 0 and 1.
    @a z0 is the offset height added to each height.
    Memory for the height matrix is allocated in this function.
    @see McduRGHeighFieldCreateHeightMatrixFromBmp for more detail.

*/
McdRGHeightFieldID MEAPI McduRGHeighFieldCreateFromBmp(McdFramework *frame,
                                char *bmpFileName,
                                MeReal x0, MeReal y0, MeReal xSize, MeReal ySize,
                                MeReal rMult, MeReal gMult, MeReal bMult, MeReal z0
                                );

#ifdef __cplusplus
}
#endif
