#if (!defined(_METRICS_H))
#define _METRICS_H

/****************************************************************************
 Includes
 */

#include <rwcore.h>

/****************************************************************************
 Function prototypes
 */
#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern void RsMetricsOpen(const RwCamera *camera);
extern void RsMetricsClose(void);
extern void RsMetricsRender(void);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* (!defined(_METRICS_H) */

