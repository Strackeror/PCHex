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
    printf("file found\n");

    ret=FSFILE_GetSize(fileHandle, &size);
    if(ret!=0)goto loadFileExit;
    if(size>maxSize){ret=-2; goto loadFileExit;}

    ret=FSFILE_Read(fileHandle, bytesRead, 0x0, dst, size);
    if(ret!=0)goto loadFileExit;
    if(*bytesRead<size){ret=-3; goto loadFileExit;}
    printf("file read : size %d \n", *bytesRead);
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

Result 	getPokemon(u32 offset, u8 *enc, u8 *save)
{
  printf("array test %d : ", offset);
  printf("%d\n", save[offset]);
  printf("copying...");
  for (int i = 0; i < 232; i++)
  {
    enc[i] = save[i + offset];
  }
  printf(" OK\n");
  return (0);
}

int 	startLoop(u8 *save, u32 boxOffset, PrintConsole *top, PrintConsole *bot)
{
  u16 pkSlot = 0, oldPkSlot = 1;

  u8 *enc = (u8 *)malloc(232);
  u8 *dec = (u8 *)malloc(232);

  u32 kPressed;

  while (aptMainLoop())
  {
    hidScanInput();
    kPressed = hidKeysDown();

    if (oldPkSlot != pkSlot)
    {
      getPokemon(boxOffset + pkSlot * 232, enc, save);
      decryptPokemon(enc, dec);
      consoleClear();
      printf("Dumping Pokemon on slot %d\n", pkSlot);
      pokemonDataDump(dec);
      oldPkSlot = pkSlot;
    }

    if (kPressed & KEY_LEFT)
      if (pkSlot > 0)
	pkSlot--;
    if (kPressed & KEY_RIGHT)
      if (pkSlot < 930)
	pkSlot++;
    if (kPressed & KEY_START)
      break;
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  }
  free(dec);
  free(enc);
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
 
  gfxInitDefault();
  fsInit();
  consoleInit(GFX_TOP, &top);
  consoleInit(GFX_TOP, &bot);

  printf("inited screen, press A to start file loading\n");

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
  
  printf("Left or Right to explore, START to exit\n\npress A to continue...");
  waitKey(KEY_A);
  if (boxOffset)
    startLoop(save, boxOffset, &top, &bot);
  free(save);
  gfxExit();
  fsExit();
  return 0;
}
