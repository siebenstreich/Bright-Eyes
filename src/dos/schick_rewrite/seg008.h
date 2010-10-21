extern unsigned short swap_u16(unsigned short val);
extern void set_video_mode(unsigned char mode);
extern void set_video_page(unsigned char mode);
extern void set_color(Bit8u *ptr, unsigned char color);
extern void set_palette(Bit8u *ptr, unsigned char first_color, unsigned short colors);

extern void draw_h_line(PhysPt ptr, unsigned short count, unsigned char color);
extern void draw_h_spaced_dots(PhysPt ptr, unsigned short count, unsigned char color, unsigned short space);
