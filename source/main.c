#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>

#include "pkx.h"

u32 __stacksize__ = 0x40000;

void __appInit()
{
  // Initialize services
  srvInit();
  aptInit();
  gfxInit(GSP_RGB565_OES, GSP_RGB565_OES, false);
  hidInit(NULL);
  fsInit();
}


void __appExit()
{
  // Exit services
  fsExit();
  hidExit();
  gfxExit();
  aptExit();
  srvExit();
}

Result loadFile(char* path, void* dst, FS_archive* archive, Handle* fsHandle, u64 maxSize)
{
    // must malloc first! (and memset, if you'd like)
    if(!path || !dst || !archive)return -1;

    u64 size;
    u32 bytesRead;
    Result ret;
    Handle fileHandle;

    ret=FSUSER_OpenFile(fsHandle, &fileHandle, *archive, FS_makePath(PATH_CHAR, path), FS_OPEN_READ, FS_ATTRIBUTE_NONE);
    if(ret!=0)return ret;
    printf("file found\n");

    ret=FSFILE_GetSize(fileHandle, &size);
    if(ret!=0)goto loadFileExit;
    if(size>maxSize){ret=-2; goto loadFileExit;}

    ret=FSFILE_Read(fileHandle, &bytesRead, 0x0, dst, size);
    if(ret!=0)goto loadFileExit;
    if(bytesRead<size){ret=-3; goto loadFileExit;}
    printf("file read : size %d \n", bytesRead);
    loadFileExit:
    FSFILE_Close(fileHandle);
    return ret;
}

Result 	getFile(char *path, u8 *save)
{
  if (save <= 0)
    return -1;
  printf("memory allocated\n");
  Handle saveGameHandle;
  Result ret;
  ret = srvGetServiceHandle(&saveGameHandle, "fs:USER");
  printf("loaded fs %x\n", (u16) ret);
  if (ret) return ret;
  ret = FSUSER_Initialize(&saveGameHandle);
  printf("init fs %x\n", (u16) ret);
  if (ret) return ret;
  FS_archive archive = (FS_archive){0x4, (FS_path){PATH_EMPTY, 1, (u8*)""}, 0, 0};
  ret = FSUSER_OpenArchive(&saveGameHandle, &archive);
  printf("loaded archive %x\n", (u16) ret);
  if (ret) return ret;
  ret = loadFile(path, save, &archive, &saveGameHandle, 0xEB000);
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
  Result ret;
 
  consoleInit(GFX_TOP, NULL);

  printf("inited screen, press A to start\n");

  waitKey(KEY_A);
  save = (u8 *) malloc(0xEB000);
  ret = getFile(path, save);

  printf("press A to get box 1 slot 1 info \n");

  waitKey(KEY_A);

  u8 *enc = (u8 *)malloc(232);
  u8 *dec = (u8 *)malloc(232);
  printf("loading box 1 slot 1\n");
  getPokemon(0x33000 + 30 * 232, enc, save);
  printf("trying to decrypt...\n");
  decryptPokemon(enc, dec);

  pokemonDataDump(dec);
  printf("done press START to finish\n");
  waitKey(KEY_START); 
  free(save);
  free(enc);
  free(dec);
  gfxExit();
  return 0;
}
