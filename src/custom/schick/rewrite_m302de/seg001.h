#if !defined(__BORLANDC__)
namespace M302de {
#endif

//static
unsigned short CD_set_drive_no(void);
//static
void CD_driver_request(RealPt req);
//static
Bit32s CD_get_tod(void);
//static
void seg001_00c1(signed short);
void seg001_02c4(void);
signed short CD_bioskey(signed short);
//static
void CD_audio_stop_hsg(void);
void CD_audio_stop(void);
void CD_audio_pause(void);
void CD_audio_play(void);
void CD_0432(void);
void CD_set_track(signed short);
signed short CD_read_exe(char*);
void CD_insert_msg(void);
signed short CD_harderr_handler(void);
void CD_check(void);
signed short CD_init(void);

#if !defined(__BORLANDC__)
}
#endif
