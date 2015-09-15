#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>

#include "pkx.h"


Result _srvGetServiceHandle(Handle* out, const char* name)
{
  Result rc = 0;

  u32* cmdbuf = getThreadCommandBuffer();
  cmdbuf[0] = 0x50100;
  strcpy((char*) &cmdbuf[1], name);
  cmdbuf[3] = strlen(name);
  cmdbuf[4] = 0x0;
  
  if((rc = svcSendSyncRequest(*srvGetSessionHandle())))return rc;

  *out = cmdbuf[3];
  return cmdbuf[1];
}

Result loadFile(char* path, void* dst, FS_archive* archive, Handle* fsHandle, u64 maxSize, u32 *bytesRead)
{
    // must malloc first! (and memset, if you'd like)
    if(!path || !dst || !archive)return -1;

    u64 size;
    Result ret;
    Handle fileHandle;

    ret=FSUSER_OpenFile(fsHandle, &fileHandle, *archive, FS_makePath(PATH_CHAR, path), FS_OPEN_READ, FS_ATTRIBUTE_NONE);
    if(ret!=0)return ret;

    ret=FSFILE_GetSize(fileHandle, &size);
    if(ret!=0)goto loadFileExit;
    if(size>maxSize){ret=-2; goto loadFileExit;}

    ret=FSFILE_Read(fileHandle, bytesRead, 0x0, dst, size);
    if(ret!=0)goto loadFileExit;
    if(*bytesRead<size){ret=-3; goto loadFileExit;}

    loadFileExit:
    FSFILE_Close(fileHandle);
    return ret;
}

Result saveFile(char *path, void *src, u64 size, FS_archive *archive, Handle *fsHandle, u32 *bytesWritten)
{
  if (!path || !src || !archive) return -1;

  Result ret;
  Handle fileHandle;

  ret = FSUSER_OpenFile(fsHandle, &fileHandle, *archive, FS_makePath(PATH_CHAR, path), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);
  if (ret) return ret;

  ret = FSFILE_Write(fileHandle, bytesWritten, 0, src, size, 0);

  FSFILE_Close(fileHandle);
  return ret;
}

s32	filesysInit(Handle *sd, Handle *save, FS_archive *sdarch, FS_archive *savearch)
{
  Result ret;
  ret = _srvGetServiceHandle(save, "fs:USER");
  if (ret) return ret;

  ret = FSUSER_Initialize(save);
  if (ret) return ret;

  *savearch = (FS_archive){0x4, (FS_path){PATH_EMPTY, 1, (u8*)""}, 0, 0};
  ret = FSUSER_OpenArchive(save, savearch);
  if (ret) return ret;

  ret = srvGetServiceHandle(sd, "fs:USER");
  if (ret) return ret;

  *sdarch = (FS_archive){0x00000009, (FS_path){PATH_EMPTY, 1, (u8*)""}, 0, 0};
  ret = FSUSER_OpenArchive(sd, sdarch);
  return ret;
}

s32 	filesysExit(Handle *sd, Handle *save, FS_archive *sdarch, FS_archive *savearch)
{
  FSUSER_CloseArchive(save, savearch);
  FSUSER_CloseArchive(sd, sdarch);
  svcCloseHandle(*save);
  svcCloseHandle(*sd);
  return 0;
}

s8 	loadLines(u8 *src, u8 *dst, u8 strlen,  u32 size)
{
  u16 	readnum = 3;
  u16 	line = 0, chr; 

  while (readnum < size)
  {
    chr = 0;
    while (readnum < size && src[readnum] != '\n')
    {
      dst[line * strlen + chr] = src[readnum];
      readnum++;
      chr++;
    }
    dst[line * strlen + chr] = 0;
    readnum++;
    line++;
  }
  return 0;
}

s8 	loadData(Handle *sdHandle, FS_archive *sdArchive)
{
  u8	tmp[12000];
  u32 	bytesRead;
  Result ret;

  printf("loading specie data...");
  ret = loadFile("/3ds/PCHex/data/personal", tmp, sdArchive, sdHandle, 12000, &bytesRead); 
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  memcpy(pkData.pkmData, tmp, bytesRead);
  printf(" OK\n");

  printf("loading species names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Species_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.species[0], 12, bytesRead);
  printf(" OK\n");

  printf("loading move names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Moves_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.moves[0], 17, bytesRead);
  printf(" OK\n");

  printf("loading ability names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Abilities_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.abilities[0], 15, bytesRead);
  printf(" OK\n");

  printf("loading item names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Items_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.items[0], 17, bytesRead);
  printf(" OK\n");

  return 0;
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

s32 	exportSave(u8 *save, u32 bytesRead, Handle *sdHandle, FS_archive *sdArchive)
{
  char 	path[] = "/3ds/PCHex/main";
  u32 	bytesWritten;
  s32 	ret;

  printf("Exporting save...");
  rewriteSaveCHK(save, bytesRead == 0x76000);
  ret = saveFile(path, save, bytesRead, sdArchive, sdHandle, &bytesWritten);
  if (ret) return ret;
  printf(" OK\n");
  return 0;
}

void 	waitKey(u32 keyWait)
{
 while (aptMainLoop())
  {
    hidScanInput();

    u32 kPressed = hidKeysDown();
    if (kPressed & keyWait) break;

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  } 
}

int 	main()
{
  char  path[] = "/main";
  u8 	*save = NULL;
  PrintConsole 	top, bot;
  Handle sdHandle, saveHandle;
  FS_archive sdArchive, saveArchive;

  gfxInitDefault();
  consoleInit(GFX_BOTTOM, &bot);
  consoleInit(GFX_TOP, &top);

  printf("Init Filesystem...");
  if (filesysInit(&sdHandle, &saveHandle, &sdArchive, &saveArchive))
    printf(" Failed\n");
  else
    printf(" OK\n");

  loadData(&sdHandle, &sdArchive);

  printf("Loading savefile...");
  u32 bytesRead;
  save = (u8 *) malloc(0xEB000);
  loadFile(path, save, &saveArchive, &saveHandle, 0xEB000, &bytesRead);
  printf(" OK, read %ld bytes\n", bytesRead);

  u8 game = getGame(bytesRead);
  s32 ret = 0;
  if (game >= 0)
    ret = startLoop(save, game, &top, &bot);
  consoleSelect(&top);
  if (ret)
    exportSave(save, bytesRead, &sdHandle, &sdArchive);

  printf("Program ended, press A to come back to HB menu\n");
  waitKey(KEY_A);
  free(save);
  gfxExit();
  filesysExit(&sdHandle, &saveHandle, &sdArchive, &saveArchive);
  return 0;
}
