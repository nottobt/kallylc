// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: 2026 NotToBT <nottobf@gmail.com>

/*
 * KallyLC (Kally Linux Crasher)
 * HEADER FILE <kallylc.h>
 */

#ifndef KALLYLC_H
#define KALLYLC_H

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <getopt.h>

// General defines
#define EFI_SB_VAR "/sys/firmware/efi/efivars/SecureBoot-8be4df61-93ca-11d2-aa0d-00e098032b8c"
#define VER "Version 2.0.0b"
#define KRNLCH 4
#define LOGFILE "/tmp/kallylc/kallylc.log"
#define TMPDIR "/tmp/kallylc"
#define MODDIR "/tmp/kallylc/moduletmp"
#define OPTCOUNT 7

// Modes (Will be used with cmake/gcc)
#ifdef DEBUG
	#define DEBUG_MODE 1
#else
	#define DEBUG_MODE 0
#endif

#ifdef PROT
	#define PROT_DISABLE 1
#else
	#define PROT_DISABLE 0
#endif

int logs(long long errcode, const char *format, ...);
int remdir(const char *path);
int sbcheck(void);
int getroot(void);
void init();
int opt1(void);
int opt2(void);
int opt3(void);
int opt4(void);
int opt5(void);
int opt6(void);
int opt7(void);
int opt8(void);
void ver();

#endif // KALLYLC_H
