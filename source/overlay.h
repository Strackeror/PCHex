#ifndef OVERLAY_H
#define OVERLAY_H

#include <3ds.h>

struct s_overlay
{
  char 	title[17];
  char 	*dst;
  u16 	maxlen;
  char 	list[128];
  u16 	offs;
  s8	index;

  PrintConsole	win;

  s8 	dataIndex;
  u8 	*data;
  s16	datacount;
  s16 	entrylen;
  s16 	foundIndex[11];
};

#endif /* end of include guard: OVERLAY_H */
