/*
  Copyright MathEngine Canada Inc   2000-2001
  Version information file used as a container for ident strings
 */

/**
 * McdTriangleMesh implementation identification string.
 */
static const char *strings[] =
{
  "@(#) MathEngine library\0"
  "$Description: MathEngine Collision Toolkit: TriangleMesh $\0"
  "$Name: t-stevet-RWSpre-030110 $\0"
  "$Last_compiled: " __DATE__ " " __TIME__ " $\0", /* internal only */
  0
};

#ifdef FORCE_VERSION_USE
const char *McdTriangleMeshGetVersionStrings(void) {
  return strings[0];
}
#endif
