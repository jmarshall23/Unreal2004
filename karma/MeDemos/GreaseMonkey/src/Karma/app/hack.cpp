#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) unsigned short ** __cdecl __p__pctype(void);

unsigned short *_pctype = *__p__pctype();

__declspec(dllimport) int * __cdecl __p___mb_cur_max(void);
int __mb_cur_max = *__p___mb_cur_max();

#ifdef __cplusplus
}
#endif

