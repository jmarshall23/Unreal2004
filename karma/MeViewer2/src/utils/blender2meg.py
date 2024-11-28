# 
#  Copyright (c) 2001 MathEngine PLC
# 
#  $Name: t-stevet-RWSpre-030110 $
# 
#  Date: $Date: 2001/09/07 18:40:44 $ - Revision: $Revision: 1.3 $
# 
#  This software and its accompanying manuals have been developed
#  by MathEngine PLC ("MathEngine") and the copyright and all other
#  intellectual property rights in them belong to MathEngine. All
#  rights conferred by law (including rights under international
#  copyright conventions) are reserved to MathEngine. This software
#  may also incorporate information which is confidential to
#  MathEngine.
#  
#  Save to the extent permitted by law, or as otherwise expressly
#  permitted by MathEngine, this software and the manuals must not
#  be copied (in whole or in part), re-arranged, altered or adapted
#  in any way without the prior written consent of the Company. In
#  addition, the information contained in the software may not be
#  disseminated without the prior written consent of MathEngine.
# 
   

# Blender Export Script
# produces .meg objects for MeViewer

import Blender

print "\nMathEngine Blender Export"
filename = "blender.meg"
print "Writing file %s" % filename
file = open(filename, "w")
# add some padding for vertex count
file.write("              \n")
objs = Blender.Object.Get()
objects = []
vcount = 0
for obj in objs:
    name = obj.name
    mesh = Blender.NMesh.GetRaw(obj.name)
    if mesh:
        print "Mesh found."
        index = 0
        if mesh.faces:
            for face in mesh.faces:
                index = index + 1
                verticesinface = len(face.v)
                if verticesinface == 3:
                    vcount = vcount + 3
                    for v in face.v:
                        file.write("x:%s y:%s z:%s nx:%s ny:%s nz:%s u:%s v:%s /\n" % (v.co[0],
                                                      v.co[1], v.co[2], v.no[0], v.no[1],
                                                      v.no[2], v.uvco[0], v.uvco[1]))
                    file.write("\n")
                elif verticesinface == 4:
                    vcount = vcount + 6
                    # make two triangles (1,2,3) & (1,3,4)
                    v1 = face.v[0]
                    v2 = face.v[1]
                    v3 = face.v[2]
                    v4 = face.v[3]

                    # first triangle
                    file.write("x:%s y:%s z:%s nx:%s ny:%s nz:%s u:%s v:%s /\n" % (v1.co[0],
                                                      v1.co[1], v1.co[2], v1.no[0], v1.no[1],
                                                      v1.no[2], v1.uvco[0], v1.uvco[1]))
                    file.write("x:%s y:%s z:%s nx:%s ny:%s nz:%s u:%s v:%s /\n" % (v2.co[0],
                                                      v2.co[1], v2.co[2], v2.no[0], v2.no[1],
                                                      v2.no[2], v2.uvco[0], v2.uvco[1]))
                    file.write("x:%s y:%s z:%s nx:%s ny:%s nz:%s u:%s v:%s /\n" % (v3.co[0],
                                                      v3.co[1], v3.co[2], v3.no[0], v3.no[1],
                                                      v3.no[2], v3.uvco[0], v3.uvco[1]))
                    file.write("\n")

                    # second triangle
                    file.write("x:%s y:%s z:%s nx:%s ny:%s nz:%s u:%s v:%s /\n" % (v1.co[0],
                                                      v1.co[1], v1.co[2], v1.no[0], v1.no[1],
                                                      v1.no[2], v1.uvco[0], v1.uvco[1]))
                    file.write("x:%s y:%s z:%s nx:%s ny:%s nz:%s u:%s v:%s /\n" % (v3.co[0],
                                                      v3.co[1], v3.co[2], v3.no[0], v3.no[1],
                                                      v3.no[2], v3.uvco[0], v3.uvco[1]))
                    file.write("x:%s y:%s z:%s nx:%s ny:%s nz:%s u:%s v:%s /\n" % (v4.co[0],
                                                      v4.co[1], v4.co[2], v4.no[0], v4.no[1],
                                                      v4.no[2], v4.uvco[0], v4.uvco[1]))
                    file.write("\n")

                else:
                    print "ERROR: Found face with %s vertices. Skipping." % verticesinface
        else:
            print "----ERROR: Mesh has no faces."
    else:
        print "Non-Mesh object found. (%s)" % obj.name
print "Vertex count is %s" % vcount
file.seek(0)
file.write("vc:%s /\n" % vcount )  
file.close()
print "Done.\n"
# --END--
