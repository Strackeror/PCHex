#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <3ds.h>
#include "pchex.h"

u32 	getCHKOffset(u8 game, u8 type, u8 index)
{
  u32 startoras[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1B600, 0x1BE00, 0x1C000, 0x1C200, 0x1C800, 0x1CA00, 0x1CE00, 0x1D600, 0x1D800, 0x1DA00, 0x1DC00, 0x1DE00, 0x1E000, 0x1E800, 0x1EE00, 0x1F200, 0x20E00, 0x21000, 0x21400, 0x21800, 0x22000, 0x23C00, 0x24000, 0x24800, 0x24C00, 0x25600, 0x25A00, 0x26200, 0x27000, 0x27200, 0x27400, 0x28200, 0x28A00, 0x28E00, 0x30A00, 0x38400, 0x6D000, };
  u32 startxy[] = { 0x05400, 0x05800, 0x06400, 0x06600, 0x06800, 0x06A00, 0x06C00, 0x06E00, 0x07000, 0x07200, 0x07400, 0x09600, 0x09800, 0x09E00, 0x0A400, 0x0F400, 0x14400, 0x19400, 0x19600, 0x19E00, 0x1A400, 0x1AC00, 0x1B400, 0x1B600, 0x1B800, 0x1BE00, 0x1C000, 0x1C400, 0x1CC00, 0x1CE00, 0x1D000, 0x1D200, 0x1D400, 0x1D600, 0x1DE00, 0x1E400, 0x1E800, 0x20400, 0x20600, 0x20800, 0x20C00, 0x21000, 0x22C00, 0x23000, 0x23800, 0x23C00, 0x24600, 0x24A00, 0x25200, 0x26000, 0x26200, 0x26400, 0x27200, 0x27A00, 0x5C600, };
  u32 lengthoras[] = { 0x000002C8, 0x00000B90, 0x0000002C, 0x00000038, 0x00000150, 0x00000004, 0x00000008, 0x000001C0, 0x000000BE, 0x00000024, 0x00002100, 0x00000130, 0x00000440, 0x00000574, 0x00004E28, 0x00004E28, 0x00004E28, 0x00000170, 0x0000061C, 0x00000504, 0x000011CC, 0x00000644, 0x00000104, 0x00000004, 0x00000420, 0x00000064, 0x000003F0, 0x0000070C, 0x00000180, 0x00000004, 0x0000000C, 0x00000048, 0x00000054, 0x00000644, 0x000005C8, 0x000002F8, 0x00001B40, 0x000001F4, 0x000003E0, 0x00000216, 0x00000640, 0x00001A90, 0x00000400, 0x00000618, 0x0000025C, 0x00000834, 0x00000318, 0x000007D0, 0x00000C48, 0x00000078, 0x00000200, 0x00000C84, 0x00000628, 0x00000400, 0x00007AD0, 0x000078B0, 0x00034AD0, 0x0000E058, };
  u32 lengthxy[] = { 0x000002C8, 0x00000B88, 0x0000002C, 0x00000038, 0x00000150, 0x00000004, 0x00000008, 0x000001C0, 0x000000BE, 0x00000024, 0x00002100, 0x00000140, 0x00000440, 0x00000574, 0x00004E28, 0x00004E28, 0x00004E28, 0x00000170, 0x0000061C, 0x00000504, 0x000006A0, 0x00000644, 0x00000104, 0x00000004, 0x00000420, 0x00000064, 0x000003F0, 0x0000070C, 0x00000180, 0x00000004, 0x0000000C, 0x00000048, 0x00000054, 0x00000644, 0x000005C8, 0x000002F8, 0x00001B40, 0x000001F4, 0x000001F0, 0x00000216, 0x00000390, 0x00001A90, 0x00000308, 0x00000618, 0x0000025C, 0x00000834, 0x00000318, 0x000007D0, 0x00000C48, 0x00000078, 0x00000200, 0x00000C84, 0x00000628, 0x00034AD0, 0x0000E058, };
  if (game)
  {
    if (type)
      return lengthoras[index];
    else
      return startoras[index] - 0x5400;
  }
  else
  {
    if (type)
      return lengthxy[index];
    else
      return startxy[index] - 0x5400;
  }
}

s8	getGame(u32 bytesRead)
{
  u8 game = -1;
  if (bytesRead == 0x76000)
  {
    printf("found OR/AS save\n");
    game = 1;
  }
  else if (bytesRead == 0x65600)
  {
    printf("found X/Y save\n");
    game = 0;
  }
  else 
    printf("found no suitable save\n");
  return game;
}

u16 	ccitt16(u8 *data, u32 len)
{
  u16 	crc = 0xFFFF;
  for (u32 i = 0; i < len; i++)
  {
    crc ^= (u16)data[i] << 8;
    for (int j = 0; j < 8; j++)
      if (crc & 0x8000)
	crc = crc << 1 ^ 0x1021;
      else
	crc = crc << 1;
  }
  return crc;
}

s32 	rewriteSaveCHK(u8 *save, u8 game)
{
  u8 	blockCount = (game) ? 58 : 55;
  u32 	csoff = (game ? 0x7B21A : 0x6A81A) - 0x5400;
  u8 	*tmp = malloc(0x35000);
  u16 	cs;

  if (!tmp)
    return -1;
  for (int i = 0; i < blockCount; i++)
  {
    memcpy(tmp, save + getCHKOffset(game, 0, i), getCHKOffset(game, 1, i));
    cs = ccitt16(tmp, getCHKOffset(game, 1, i));
    memcpy(save + csoff + i * 8, &cs, 2);
  }
  free(tmp);
  return (0);
}

s32 	backupSave(u8 *save, u8 game, Handle *fshdl, FS_archive *fsarch)
{
  char 	dir[] = "/3ds/PCHex/";
  char 	path[100];
  u32 	len = game ? 0x76000 : 0x65600;
  u32 	bytesWritten;
  s32 	ret;

  printf("Backing save up...");
  sprintf(path, "%smain_%d", dir,(int) time(NULL));
  ret = saveFile(path, save, len, fsarch, fshdl, &bytesWritten); //actually write the file
  if (ret) return ret;
  printf(" OK\n");
  printf("backed save up to %s\n", path);
  return 0;
}

s32 	exportSave(u8 *save, u8 game, Handle *fshdl, FS_archive *fsarch)
{
  char 	path[] = "/main";
  u32 	len = game ? 0x76000 : 0x65600;
  u32 	bytesWritten;
  s32 	ret;

  printf("Exporting save...");
  rewriteSaveCHK(save, game); //rewrite checksum, necessary or the save is considered corrupted
  deleteFile(path, fshdl, fsarch);
  ret = saveSFile(path, save, len, fsarch, fshdl, &bytesWritten); //actually write the file
  if (ret) return ret;
  printf(" OK\n");
  return 0;
}

s32 	loadSave(u8 *save, Handle *fshdl, FS_archive *fsarch)
{
  char 	path[] = "/main";
  u32 	bytesRead;
  s32 	ret;

  printf("Loading savefile...");
  ret = loadFile(path, save, fsarch, fshdl, 0xEB000, &bytesRead); //actually loading the file
  if (ret) return -1;
  printf(" OK, read %ld bytes\n", bytesRead);
  return (getGame(bytesRead)); //we return the which game was found
}
