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

Result 	getFile(char *path, u8 *save, u32 *bytesRead)
{
  if (save <= 0)

    return -1;
  Handle saveGameHandle;
  Result ret;
  ret = _srvGetServiceHandle(&saveGameHandle, "fs:USER");
  printf("loaded fs %x\n", (u16) ret);
  if (ret) return ret;
  ret = FSUSER_Initialize(&saveGameHandle);
  printf("init fs %x\n", (u16) ret);
  if (ret) return ret;
  FS_archive archive = (FS_archive){0x4, (FS_path){PATH_EMPTY, 1, (u8*)""}, 0, 0};
  ret = FSUSER_OpenArchive(&saveGameHandle, &archive);
  printf("loaded archive %x\n", (u16) ret);
  if (ret) return ret;
  ret = loadFile(path, save, &archive, &saveGameHandle, 0xEB000, bytesRead);
  printf("loaded file %s %x\n", path, (u16) ret);
  if (ret) return ret;
  FSUSER_CloseArchive(&saveGameHandle, &archive);
  svcCloseHandle(saveGameHandle);
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
  Handle sdHandle;
  FS_archive sdArchive;
  
 
  gfxInitDefault();
  fsInit();

  srvGetServiceHandle(&sdHandle, "fs:USER");
  sdArchive = (FS_archive){0x00000009, (FS_path){PATH_EMPTY, 1, (u8*)""}, 0, 0};
  FSUSER_OpenArchive(&sdHandle, &sdArchive);

  consoleInit(GFX_BOTTOM, &bot);
  consoleInit(GFX_TOP, &top);

  printf("inited screen, press A to start file loading\n");
  
  loadData(&sdHandle, &sdArchive);

  printf("test top x:%d y:%d  bot x:%d y:%d\n", top.consoleWidth, top.consoleHeight, bot.consoleWidth, bot.consoleHeight);
  waitKey(KEY_A);

  u32 bytesRead;
  save = (u8 *) malloc(0xEB000);
  getFile(path, save, &bytesRead);

  u32 boxOffset = 0;

  if (bytesRead == 0x76000)
  {
    printf("found OR/AS save, offset is 0x33000\n");
    boxOffset = 0x33000;
  }
  else if (bytesRead == 0x65600)
  {
    printf("found X/Y save, offset is 0x22600\n");
    boxOffset = 0x22600;
  }
  else 
    printf("found no suitable save\n");
  if (boxOffset)
    startLoop(save, boxOffset, &top, &bot);

  FSUSER_CloseArchive(&sdHandle, &sdArchive);
  svcCloseHandle(sdHandle);
  free(save);
  gfxExit();
  fsExit();
  return 0;
}
