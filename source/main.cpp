#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>

extern "C"
{
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

Result 	shuffleArray(u8 *array, u8 sv)
{
  u8 aloc[] = { 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3 };
  u8 bloc[] = { 1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2 };
  u8 cloc[] = { 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1 };
  u8 dloc[] = { 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0 };
  printf("locations loaded\n");

  u8 ord[] = {aloc[sv], bloc[sv], cloc[sv], dloc[sv]};
  u8 tmp[56];
  printf("locations found\n");
  for (int b = 0; b < 4; b++)
  {
    memcpy(tmp, array + 8 + 56 * ord[b], 56);
    memcpy(array + 8 + 56 * b, tmp, 56);
  }
  return 0;	
}
u32 seedStep(u32 seed)
{
  u32 a = 0x41C64E6D;
  u32 c = 0x00006073;

  return ((seed * a + c) & 0xFFFFFFFF);
}

Result decryptPokemon(u8 *enc, u8 *dec)
{
  
  memcpy(dec, enc, 232);
  printf("poke copied\n");

  u32 pv = *(u32 *)dec;
  u8 sv = (((pv & 0x3E000) >> 0xD) % 24);
  printf("seeds extracted\n");

  u32 seed = pv;
  u16 tmp;
  for (int i = 8; i < 232; i += 2)
  {
    memcpy(&tmp, dec + i, 2);
    tmp ^= (seedStep(seed) >> 16);
    seed = seedStep(seed);
    memcpy(dec + i, &tmp, 2);
  }
  printf("blocks decrypted\n");

  shuffleArray(dec, sv);
  printf("arrays shuffled\n");
  return (0);
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
// Array.Copy(BitConverter.GetBytes((ushort)(BitConverter.ToUInt16(pkx, i) ^ (LCRNG(ref seed) >> 16))), 0, pkx, i, 2);
  ret = getFile(path, save);

  printf("press A to get box 1 slot 1 info \n");

  waitKey(KEY_A);

  u8 *enc = (u8 *)malloc(232);
  u8 *dec = (u8 *)malloc(232);
  printf("loading box 1 slot 1 at offset 0x38400\n");
  getPokemon(0x33000 + 30 * 232, enc, save);
  printf("trying to decrypt...\n");
  decryptPokemon(enc, dec);
  for (int i = 0x40; i < 0x58; i += 2)
    printf("%c", dec[i]);

  waitKey(KEY_START); 
  free(save);
  free(enc);
  free(dec);
  gfxExit();
  return 0;
}
