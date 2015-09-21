#ifndef OVERLAY_H
#define OVERLAY_H

#include <3ds.h>

struct s_overlay
{
  char 	title[17]; //title of the overlay
  char 	*dst; //string to write in
  u16 	maxlen; //maximum length of the string
  char 	list[128]; //list of chars to use
  u16 	offs; //current edited character
  s8	index; //index of the edited character in 'list'

  PrintConsole	win; //console to print in for the overlay

  s8 	dataIndex; //-1 if typing, 0+ if choosing a result
  u8 	*data; //data to search results in
  s16	datacount; //number of entries in data
  s16 	entrylen; //length of an entry in data
  s16 	foundIndex[11]; //Index of the results found in data
};

#endif /* end of include guard: OVERLAY_H */
