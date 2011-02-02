// FAT-testing-terminal, hacked from Martin Thomas's example
// "ChaN's FAT-Module with STM32 SPI"
// http://gandalf.arubi.uni-kl.de/avr_projects/arm_projects/arm_memcards/index.html
// copyright (c) 2011 Aaron Marburg
//
// Martin's original comment:
/*----------------------------------------------------------------------*/
/* FAT file system test terminal for FatFs R0.07a  (C)ChaN, 2008        */
/* modified by Martin Thomas 4/2009 for the STM32 example               */
/*----------------------------------------------------------------------*/

#include <string.h> /* memset et al*/

#include "wirish.h"
#include "main.h"

#include "integer.h"
#include "term_io.h"

#include "diskio.h"
#include "ff.h"

//#include "rtc.h"
#include "fat_test_term.h"


DWORD acc_size;				/* Work register for fs command */
WORD acc_files, acc_dirs;
FILINFO Finfo;
#if _USE_LFN
char Lfname[512];
#endif


FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
FIL File1, File2;			/* File objects */
DIR Dir;					/* Directory object */
byte_t Buff[4*1024] __attribute__ ((aligned (4))) ;		/* Working buffer */

volatile UINT Timer;		/* Performance timer (1kHz increment) */

/* called by a timer interrupt-handler every 1ms */
extern "C" void fat_test_term_timerproc (void)
{
  Timer++;
}

static FRESULT scan_files (char* path)
{
  DIR dirs;
  FRESULT res;
  byte_t i;
  char *fn;


  if ((res = f_opendir(&dirs, path)) == FR_OK) {
    i = strlen(path);
    while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
#if _USE_LFN
      fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
      fn = Finfo.fname;
#endif
      if (Finfo.fattrib & AM_DIR) {
        acc_dirs++;
        *(path+i) = '/'; strcpy(path+i+1, fn);
        res = scan_files(path);
        *(path+i) = '\0';
        if (res != FR_OK) break;
      } else {
        //				xprintf("%s/%s\n", path, fn);
        acc_files++;
        acc_size += Finfo.fsize;
      }
    }
  }

  return res;
}



static void put_rc (FRESULT rc)
{
  const char *p;
  static const char str[] =
    "OK\0" "NOT_READY\0" "NO_FILE\0" "FR_NO_PATH\0" "INVALID_NAME\0" "INVALID_DRIVE\0"
    "DENIED\0" "EXIST\0" "RW_ERROR\0" "WRITE_PROTECTED\0" "NOT_ENABLED\0"
    "NO_FILESYSTEM\0" "INVALID_OBJECT\0" "MKFS_ABORTED\0";

  int i;

  for (p = str, i = 0; i != rc && *p; i++) {
    while(*p++);
  }
  xprintf("rc=%u FR_%s\n", (UINT)rc, p);
}

static void put_dresult( DRESULT rc )
{
  static const char *str[] = { "OK", "ERROR", "WRPRT", "NOTRDY", "PARERR" };

  xprintf("rc=%u, RF_%s\r", rc, str[(int)rc] );
}

static void put_dstatus( DSTATUS rc )
{
  xprintf("rc=%u: ", rc );
  if( rc & STA_NOINIT ) xprintf("NOINIT ");
  if( rc & STA_NODISK ) xprintf("NODISK ");
  if( rc & STA_PROTECT ) xprintf("PROTECT");
  xputc('\r');
}


#define LINEBUF_LEN 32

int fat_menu (void)
{
  
  // Input buffer handling
  static char linebuf[LINEBUF_LEN];
  static uint bufcount = 0;
  char c;
  bool do_process = false;

  char *ptr, *ptr2;
  long p1, p2, p3;
  FRESULT res;
  DRESULT dres;
  DSTATUS dstatus;

  byte_t b1;
  WORD w1;
  UINT s1, s2, cnt, blen = sizeof(Buff);
  DWORD ofs = 0, sect = 0;
  FATFS *fs;				/* Pointer to file system object */

  //RTC_t rtc;

    delay(100);

  while(COMM.available()) {
    c = COMM.read();
    if( (c==0x0A || c==0x0D) ) {
      linebuf[bufcount] = 0x00;
      do_process=true;
      bufcount = 0;
      COMM.println("");
    } else {
      linebuf[bufcount++] = c;
      COMM.print(c);
      if( bufcount == LINEBUF_LEN-1 ) {
        COMM.println("Input buffer overflow.");
        bufcount = 0;
      }
    }

    if(  do_process ) {
      do_process=false;

      ptr = linebuf;

      switch (*ptr++) {

        case 'm' :
          switch (*ptr++) {
            case 'd' :	/* md <address> [<count>] - Dump memory */
              if (!xatoi(&ptr, &p1)) break;
              if (!xatoi(&ptr, &p2)) p2 = 128;
              for (ptr=(char*)p1; p2 >= 16; ptr += 16, p2 -= 16)
                put_dump((byte_t*)ptr, (UINT)ptr, 16);
              if (p2) put_dump((byte_t*)ptr, (UINT)ptr, p2);
              break;
          }
          break;

        case 'd' :
          switch (*ptr++) {
            case 'd' :	/* dd [<lba>] - Dump sector */
              if (!xatoi(&ptr, &p2)) p2 = sect;
              dres = disk_read(0, Buff, p2, 1);
              if (dres) { xprintf("rc=%d\n", (WORD)dres); break; }
              sect = p2 + 1;
              xprintf("Sector:%lu\n", p2);
              for (ptr=(char*)Buff, ofs = 0; ofs < 0x200; ptr+=16, ofs+=16)
                put_dump((byte_t*)ptr, ofs, 16);
              break;

            case 'i' :	/* di - Initialize disk */
              dstatus = disk_initialize(0);
              put_dstatus( dstatus );
              break;

            case 's' :	/* ds - Show disk status */
              Buff[0]=2;
              if (disk_ioctl(0, CTRL_POWER, Buff) == RES_OK )
              { xprintf("Power is %s\r\n", Buff[1] ? "ON" : "OFF"); }
              if (disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK)
              { xprintf("Drive size: %lu sectors\r\n", p2); }
              if (disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK)
              { xprintf("Sector size: %u\r\n", w1); }
              if (disk_ioctl(0, GET_BLOCK_SIZE, &p2) == RES_OK)
              { xprintf("Erase block size: %lu sectors\r\n", p2); }
              if (disk_ioctl(0, MMC_GET_TYPE, &b1) == RES_OK)
              { xprintf("MMC/SDC type: %u\r\n", b1); }
              if (disk_ioctl(0, MMC_GET_CSD, Buff) == RES_OK)
              { xputs("CSD:\r\n"); put_dump(Buff, 0, 16); }
              if (disk_ioctl(0, MMC_GET_CID, Buff) == RES_OK)
              { xputs("CID:\r\n"); put_dump(Buff, 0, 16); }
              if (disk_ioctl(0, MMC_GET_OCR, Buff) == RES_OK)
              { xputs("OCR:\r\n"); put_dump(Buff, 0, 4); }
              if (disk_ioctl(0, MMC_GET_SDSTAT, Buff) == RES_OK) {
                xputs("SD Status:\r\n");
                for (s1 = 0; s1 < 64; s1 += 16) put_dump(Buff+s1, s1, 16);
              }
              break;
          }
          break;

        case 'b' :
          switch (*ptr++) {
            case 'd' :	/* bd <addr> - Dump R/W buffer */
              if (!xatoi(&ptr, &p1)) break;
              for (ptr=(char*)&Buff[p1], ofs = p1, cnt = 32; cnt; cnt--, ptr+=16, ofs+=16)
                put_dump((byte_t*)ptr, ofs, 16);
              break;

            case 'e' :	/* be <addr> [<data>] ... - Edit R/W buffer */
              if (!xatoi(&ptr, &p1)) break;
              if (xatoi(&ptr, &p2)) {
                do {
                  Buff[p1++] = (byte_t)p2;
                } while (xatoi(&ptr, &p2));
                break;
              }
              for (;;) {
                xprintf("%04X %02X-", (WORD)(p1), (WORD)Buff[p1]);
                get_line(linebuf, sizeof(linebuf));
                ptr = linebuf;
                if (*ptr == '.') break;
                if (*ptr < ' ') { p1++; continue; }
                if (xatoi(&ptr, &p2))
                  Buff[p1++] = (byte_t)p2;
                else
                  xputs("???\n");
              }
              break;

            case 'r' :	/* br <lba> [<num>] - Read disk into R/W buffer */
              if (!xatoi(&ptr, &p2)) break;
              if (!xatoi(&ptr, &p3)) p3 = 1;
              xprintf("rc=%u\n", (WORD)disk_read(0, Buff, p2, p3));
              break;

            case 'w' :	/* bw <lba> [<num>] - Write R/W buffer into disk */
              if (!xatoi(&ptr, &p2)) break;
              if (!xatoi(&ptr, &p3)) p3 = 1;
              xprintf("rc=%u\n", (WORD)disk_write(0, Buff, p2, p3));
              break;

            case 'f' :	/* bf <val> - Fill working buffer */
              if (!xatoi(&ptr, &p1)) break;
              memset(Buff, (byte_t)p1, sizeof(Buff));
              break;

          }
          break;

        case 'f' :
          switch (*ptr++) {

            case 'i' :	/* fi - Force initialized the logical drive */
              put_rc(f_mount(0, &Fatfs[0]));
              break;

            case 's' :	/* fs - Show logical drive status */
              res = f_getfree("", (DWORD*)&p2, &fs);
              if (res) { put_rc(res); break; }
              xprintf("FAT type = %u (%s)\r\n"
                  "Bytes/Cluster = %lu\r\n"
                  "Number of FATs = %u\r\n"
                  "Root DIR entries = %u\r\n"
                  "Sectors/FAT = %lu\r\n"
                  "Number of clusters = %lu\r\n"
                  "FAT start (lba) = %lu\r\n"
                  "DIR start (lba,clustor) = %lu\r\n"
                  "Data start (lba) = %lu\r\n\r\n",
                  (WORD)fs->fs_type,
                  (fs->fs_type==FS_FAT12) ? "FAT12" : (fs->fs_type==FS_FAT16) ? "FAT16" : "FAT32",
                  (DWORD)fs->csize * 512, (WORD)fs->n_fats,
                  fs->n_rootdir, fs->fsize, (DWORD)fs->n_fatent - 2,
                  fs->fatbase, fs->dirbase, fs->database
                  );
              acc_size = acc_files = acc_dirs = 0;
#if _USE_LFN
              Finfo.lfname = Lfname;
              Finfo.lfsize = sizeof(Lfname);
#endif
              res = scan_files(ptr);
              if (res) { put_rc(res); break; }
              xprintf("%u files, %lu bytes.\r\n"
                  "%u folders.\r\n"
                  "%lu KB total disk space.\r\n"
                  "%lu KB available.\r\n",
                  acc_files, acc_size, acc_dirs,
                  (fs->n_fatent - 2) * (fs->csize / 2), p2 * (fs->csize / 2)
                  );
              break;

            case 'l' :	/* fl [<path>] - Directory listing */
              while (*ptr == ' ') ptr++;
              res = f_opendir(&Dir, ptr);
              if (res) { put_rc(res); break; }
              p1 = s1 = s2 = 0;
              for(;;) {
#if _USE_LFN
                Finfo.lfname = Lfname;
                Finfo.lfsize = sizeof(Lfname);
#endif
                res = f_readdir(&Dir, &Finfo);
                if ((res != FR_OK) || !Finfo.fname[0]) break;
                if (Finfo.fattrib & AM_DIR) {
                  s2++;
                } else {
                  s1++; p1 += Finfo.fsize;
                }
                xprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s",
                    (Finfo.fattrib & AM_DIR) ? 'D' : '-',
                    (Finfo.fattrib & AM_RDO) ? 'R' : '-',
                    (Finfo.fattrib & AM_HID) ? 'H' : '-',
                    (Finfo.fattrib & AM_SYS) ? 'S' : '-',
                    (Finfo.fattrib & AM_ARC) ? 'A' : '-',
                    (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
                    (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
                    Finfo.fsize, &(Finfo.fname[0]));
#if _USE_LFN
                xprintf("  %s\r\n", Lfname);
#else
                xputc('\r');
                xputc('\n');
#endif
              }
              xprintf("%4u File(s),%10lu bytes total\r\n%4u Dir(s)", s1, p1, s2);
              if (f_getfree(ptr, (DWORD*)&p1, &fs) == FR_OK)
                xprintf(", %10lu bytes free\r\n", p1 * fs->csize * 512);
              break;

            case 'o' :	/* fo <mode> <file> - Open a file */
              if (!xatoi(&ptr, &p1)) break;
              while (*ptr == ' ') ptr++;
              put_rc(f_open(&File1, ptr, (byte_t)p1));
              break;

            case 'c' :	/* fc - Close a file */
              put_rc(f_close(&File1));
              break;

            case 'e' :	/* fe - Seek file pointer */
              if (!xatoi(&ptr, &p1)) break;
              res = f_lseek(&File1, p1);
              put_rc(res);
              if (res == FR_OK)
                xprintf("fptr=%lu(0x%lX)\n", File1.fptr, File1.fptr);
              break;

            case 'd' :	/* fd <len> - read and dump file from current fp */
              if (!xatoi(&ptr, &p1)) break;
              ofs = File1.fptr;
              while (p1) {
                if ((UINT)p1 >= 16) { cnt = 16; p1 -= 16; }
                else 				{ cnt = p1; p1 = 0; }
                res = f_read(&File1, Buff, cnt, &cnt);
                if (res != FR_OK) { put_rc(res); break; }
                if (!cnt) break;
                put_dump(Buff, ofs, cnt);
                ofs += 16;
              }
              break;

            case 'r' :	/* fr <len> - read file */
              if (!xatoi(&ptr, &p1)) break;
              p2 = 0;
              Timer = 0;
              while (p1) {
                if ((UINT)p1 >= blen) {
                  cnt = blen; p1 -= blen;
                } else {
                  cnt = p1; p1 = 0;
                }
                res = f_read(&File1, Buff, cnt, &s2);
                if (res != FR_OK) { put_rc(res); break; }
                p2 += s2;
                if (cnt != s2) break;
              }
              xprintf("%lu bytes read with %lu kB/sec.\n", p2, p2 / Timer);
              break;

            case 'w' :	/* fw <len> <val> - write file */
              if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
              memset(Buff, (byte_t)p2, blen);
              p2 = 0;
              Timer = 0;
              while (p1) {
                if ((UINT)p1 >= blen) {
                  cnt = blen; p1 -= blen;
                } else {
                  cnt = p1; p1 = 0;
                }
                res = f_write(&File1, Buff, cnt, &s2);
                if (res != FR_OK) { put_rc(res); break; }
                p2 += s2;
                if (cnt != s2) break;
              }
              xprintf("%lu bytes written with %lu kB/sec.\n", p2, p2 / Timer);
              break;

            case 'n' :	/* fn <old_name> <new_name> - Change file/dir name */
              while (*ptr == ' ') ptr++;
              ptr2 = strchr(ptr, ' ');
              if (!ptr2) break;
              *ptr2++ = 0;
              while (*ptr2 == ' ') ptr2++;
              put_rc(f_rename(ptr, ptr2));
              break;

            case 'u' :	/* fu <name> - Unlink a file or dir */
              while (*ptr == ' ') ptr++;
              put_rc(f_unlink(ptr));
              break;

            case 'v' :	/* fv - Truncate file */
              put_rc(f_truncate(&File1));
              break;

            case 'k' :	/* fk <name> - Create a directory */
              while (*ptr == ' ') ptr++;
              put_rc(f_mkdir(ptr));
              break;

            case 'a' :	/* fa <atrr> <mask> <name> - Change file/dir attribute */
              if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
              while (*ptr == ' ') ptr++;
              put_rc(f_chmod(ptr, p1, p2));
              break;

            case 't' :	/* ft <year> <month> <day> <hour> <min> <sec> <name> - Change timestamp */
              if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
              Finfo.fdate = ((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31);
              if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
              Finfo.ftime = ((p1 & 31) << 11) | ((p1 & 63) << 5) | ((p1 >> 1) & 31);
              put_rc(f_utime(ptr, &Finfo));
              break;

            case 'x' : /* fx <src_name> <dst_name> - Copy file */
              while (*ptr == ' ') ptr++;
              ptr2 = strchr(ptr, ' ');
              if (!ptr2) break;
              *ptr2++ = 0;
              while (*ptr2 == ' ') ptr2++;
              xprintf("Opening \"%s\"", ptr);
              res = f_open(&File1, ptr, FA_OPEN_EXISTING | FA_READ);
              xputc('\n');
              if (res) {
                put_rc(res);
                break;
              }
              xprintf("Creating \"%s\"", ptr2);
              res = f_open(&File2, ptr2, FA_CREATE_ALWAYS | FA_WRITE);
              xputc('\n');
              if (res) {
                put_rc(res);
                f_close(&File1);
                break;
              }
              xprintf("Copying file...");
              Timer = 0;
              p1 = 0;
              for (;;) {
                res = f_read(&File1, Buff, blen, &s1);
                if (res || s1 == 0) {
                  xprintf("EOF\n");
                  break;   /* error or eof */
                }
                res = f_write(&File2, Buff, s1, &s2);
                p1 += s2;
                if (res || s2 < s1) {
                  xprintf("disk full\n");
                  break;   /* error or disk full */
                }
              }
              xprintf("%lu bytes copied with %lu kB/sec.\n", p1, p1 / Timer);
              f_close(&File1);
              f_close(&File2);
              break;
#if _USE_MKFS
            case 'm' :	/* fm <partition rule> <cluster size> - Create file system */
              if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
              xprintf("The drive 0 will be formatted. Are you sure? (Y/n)=");
              get_line(ptr, sizeof(linebuf));
              if (*ptr == 'Y')
                put_rc(f_mkfs(0, (byte_t)p2, (WORD)p3));
              break;
#endif
            case 'z' :	/* fz [<rw size>] - Change R/W length for fr/fw/fx command */
              if (xatoi(&ptr, &p1) && p1 >= 1 && (size_t)p1 <= sizeof(Buff))
                blen = p1;
              xprintf("blen=%u\n", blen);
              break;
          }
          break;

          //      case 't' :	/* t [<year> <mon> <mday> <hour> <min> <sec>] */
          //.       if (xatoi(&ptr, &p1)) {
          //          rtc.year = (WORD)p1;
          //          xatoi(&ptr, &p1); rtc.month = (byte_t)p1;
          //          xatoi(&ptr, &p1); rtc.mday = (byte_t)p1;
          //          xatoi(&ptr, &p1); rtc.hour = (byte_t)p1;
          //          xatoi(&ptr, &p1); rtc.min = (byte_t)p1;
          //          if (!xatoi(&ptr, &p1)) break;
          //          rtc.sec = (byte_t)p1;
          //          rtc_settime(&rtc);
          //        }
          //        rtc_gettime(&rtc);
          //        xprintf("%u/%u/%u %02u:%02u:%02u\n", rtc.year, rtc.month, rtc.mday, rtc.hour, rtc.min, rtc.sec);
          //        break;

        case 'p' : /* p [<0|1>] - Card Power state, Card Power Control off/on */
          if (xatoi(&ptr, &p1)) {
            if (!p1) {
              f_sync(&File1);
            }
            Buff[0] = p1;
            disk_ioctl(0, CTRL_POWER, Buff);
          }
          Buff[0] = 2;
          if (disk_ioctl(0, CTRL_POWER, Buff) == RES_OK ) {
            COMM.print("Card power is ");
            if( Buff[1] ) 
              COMM.println("ON");
            else
              COMM.println("OFF");
          }
          break;
        case '?':
        case 'h':
          COMM.println("Help:");
          COMM.println(" p <0|1>         Turn power on and off");
          COMM.println(" di              Disk initialize");
          COMM.println(" ds              Disk status");
          COMM.println(" fi              FAT initialize");
          COMM.println(" fs              FAT status");
          COMM.println(" fl <path>       Directory listing");
          COMM.println(" fk <name>       Create directory");
          COMM.println(" fx <src> <dst>  Copy file");
          break;

        case 'x' :
          return_to_main_menu();
          return 0;
          break;
      }
      COMM.write("fat>");
    }
  }

    return 1;
}


