// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 NotToBT <nottobf@gmail.com>

/*
 * KallyLC (Kally Linux Crasher)
 * MAIN C FILE <kallylc.c>
 */

#include "include/kallylc.h"

int logs(long long errcode, const char *format, ...) {
    char formlog[1024];
    char message[512];
    char formtime[64];
    time_t rawtime;
    time(&rawtime);
    struct tm *time1 = localtime(&rawtime);
    strftime(formtime, sizeof(formtime), "%Y-%m-%d %H:%M:%S", time1);
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    if (errcode == 0) {
        snprintf(formlog, sizeof(formlog), "[%s] INFO: %s\n", formtime, message);
    } else {
        snprintf(formlog, sizeof(formlog), "[%s] ERR %lld: %s\n", formtime, errcode, message);
    }
    FILE *LOGFILE1 = fopen(LOGFILE, "a");
    if (LOGFILE1 == NULL) {
        perror("Logfile append failed");
        exit(13);
    }
    fprintf(LOGFILE1, "%s", formlog);
    fflush(LOGFILE1);
    fsync(fileno(LOGFILE1));
    fclose(LOGFILE1);
    return 0;
}

int remdir(const char *path) {
    DIR *dir1 = opendir(path);
    size_t pathlen = strlen(path);
    int r = -1;
    if (dir1) {
        struct dirent *p;
        r = 0;
        while (!r && (p = readdir(dir1))) {
            int len;
            char *buf;
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;
            len = pathlen + strlen(p->d_name) + 2;
            buf = malloc(len);
            if (buf) {
                struct stat statbuf;
                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        r = remdir(buf);
                    } else {
                        r = unlink(buf);
                    }
                }
                free(buf);
            } else {
                r = -1;
            }
        }
        closedir(dir1);
    }
    if (!r) {
        r = rmdir(path);
    }
    return r;
}

int sbcheck(void) {
    if (access(EFI_SB_VAR, F_OK) != 0) {
        return -1;
    }
    FILE *fp = fopen(EFI_SB_VAR, "rb");
    if (!fp) {
        return -1;
    }
    uint8_t buffer[5];
    size_t read_bytes = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (read_bytes < 5) {
        return -1;
    }
    return (int)buffer[4];
}

int getroot(void) {
    if (getuid() != 0) {
        printf("Permission denied. Are you root?\n");
        logs(1, "Permission denied. User isn't running the program as root.");
        exit(1);
    }
    return 0;
}

void init() {
    getroot();
    int result = remdir(TMPDIR);
    if (result == 0) {
        printf("Deleting directory completed successfully.\n");
    }
    else if (result == -1) {
        printf("Directory already deleted or not found.\n");
    }
    else {
        printf("Error: Unable to delete directory. Do you have access to /tmp/kallylc?\n");
        exit(0);
    }
    result = mkdir(TMPDIR, 0777);
    if (result == 0) {
        printf("Making directory completed successfully.\n");
        logs(0, "---------- NEW LOG ENTRY ----------\n");
        logs(0, "Making directory: %s complete.", TMPDIR);
    } else {
        printf("Error code %i: Unable to make directory. Do you have access to /tmp or are you running out of space?\n", result);
        exit(0);
    }
    system("clear");
}

void print_usage(char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf("  -o, --option <integer>  Set choice value\n");
    printf("  -v, --version           Show version\n");
    printf("  -h, --help              Show this help text\n");
}

int opt1(void) {
    long pagesize = sysconf(_SC_PAGESIZE);
    char *buffer = mmap(NULL, pagesize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap failed");
        logs(1, "Nmap Failure: exiting...");
        return 1;
    }
    buffer[0] = 'X';
    madvise(buffer, pagesize, MADV_DONTFORK);
    printf("Simulating active MCE hardware corruption at RAM location: %p\n", buffer);
    logs(0, "Simulating active MCE hardware corruption in RAM");
    if (madvise(buffer, pagesize, MADV_HWPOISON) < 0) {
        perror("madvise HWPOISON failed");
        return 1;
    }
    printf("Reading memory address to provoke active handling path...\n");
    char fault = *buffer;
    return 0;
}

int opt2(void) {
    int *sigsegv = (int *)0xFFFF6FFFFFFFFFFF;
    printf("Triggering General Protection Fault (#GP) @ Non-canonical void address: %p\n", sigsegv);
    logs(0, "Triggering General Protection Fault (#GP) @ Address: %p", sigsegv);
    *sigsegv = 2;
    return 0;
}

int opt3(void) {
    int *sigsegv = NULL;
    printf("Triggering NULL pointer dereference...\n");
    logs(0, "Triggering userspace segmentation fault via NULL pointer dereference");
    *sigsegv = 45;
    return 0;
}

int opt4(void) {
    char errstr[128];
    char cmd[256];
    char cmd1[512];
    int opt = 0;
    int sel_ko = 0;
    int opt4mkdirres = mkdir("/tmp/kallylc/moduletmp", 0777);
    if (opt4mkdirres != 0) {
        printf("Directory creation failure. The program will halt.\n");
        logs(1, "The program will halt now.");
    }
    FILE *make = fopen("/tmp/kallylc/moduletmp/Makefile", "w");
    if (make == NULL) {
        perror("Error creating makefile!");
        logs(1, "Makefile creation failed. Permission denied or disk full.");
        exit(1);
    }
    printf("Kernel-level warning/crash initiated.\n");
    printf("Before we start, we must verify that secureboot is disabled.\n");
    int status = sbcheck();
    switch (status) {
        case 1:
            printf("Secure Boot is ENABLED.\n");
            logs(1, "Secure Boot is ENABLED. Kally can't continue Option 4.");
            fclose(make);
            return 0;
        case 0:
            printf("Secure Boot is DISABLED.\n");
            break;
        default:
            printf("Could not determine Secure Boot state (Non-UEFI system or insufficient permissions).\n");
            logs(1, "Could not determine Secure Boot state.");
            fclose(make);
            return 1;
    }
    printf("You have 4 options!\n"
    "Option 1: Trigger a Kernel Warning [Function WARN()]\n"
    "Option 2: Trigger a Kernel Bug [Function BUG()]\n"
    "Option 3: Trigger a Kernel Oops via Divide by zero error\n"
    "Option 4: Trigger a Kernel Panic [Function panic()]\n"
    "Option 5: Trigger kernel preemption (Preempt count) & Out-of-memory (OOM) in Ring-0\n"
    "Choose one: ");
    if (scanf("%i", &opt) != 1) {
        printf("Invalid input selection.\n");
        fclose(make);
        return 1;
    }
    switch (opt) {
        case 1:
            fprintf(make, "obj-m += kallylclkm1.o\n\n");
            sel_ko = 1;
            break;
        case 2:
            fprintf(make, "obj-m += kallylclkm2.o\n\n");
            sel_ko = 2;
            break;
        case 3:
            fprintf(make, "obj-m += kallylclkm3.o\n\n");
            sel_ko = 3;
            break;
        case 4:
            fprintf(make, "obj-m += kallylclkm4.o\n\n");
            sel_ko = 4;
            break;
        case 5:
            fprintf(make, "obj-m += kallylclkm5.o\n\n");
            sel_ko = 5;
            break;
        default:
            printf("Wrong option: %i!\n", opt);
            snprintf(errstr, sizeof(errstr), "User chose option %i, but it is invalid", opt);
            logs(1, errstr);
            fclose(make);
            exit(1);
    }
    printf("Building makefile...\n");
    logs(0, "User initiated Option 4 and selected kernel sub-option: %i", opt);
    fprintf(make, "KDIR := /lib/modules/$(shell uname -r)/build\n");
    fprintf(make, "all:\n");
    fprintf(make, "\tmake -C $(KDIR) M=/tmp/kallylc/moduletmp modules\n");
    fprintf(make, "clean:\n");
    fprintf(make, "\tmake -C $(KDIR) M=/tmp/kallylc/moduletmp clean\n");
    fclose(make);
    snprintf(cmd1, sizeof(cmd1), "sudo cp modules/kallylclkm%i.c /tmp/kallylc/moduletmp/kallylclkm%i.c && sudo make -C /tmp/kallylc/moduletmp", sel_ko, sel_ko);
    snprintf(cmd, sizeof(cmd), "sudo insmod /tmp/kallylc/moduletmp/kallylclkm%i.ko", sel_ko);
    system(cmd1);
    system(cmd);
    return 0;
}

int opt5(void) {
    int cho;
    system("clear");
    printf("Memory Leaker!");
    logs(0, "Memory leaker chosen. (opt4)");
    printf("Select the memory size:");
    printf("Option 1: 8mb (Slowest)\n"
    "Option 2: 64mb (Faster)\n"
    "Option 3: 512mb (Fastest)\n");
    printf("Choose one\n");
    scanf("%i", &cho);
    size_t allocsize;
    switch (cho) {
        case 1:
            allocsize = 1024 * 1024 * 8;
            break;
        case 2:
            allocsize = 1024 * 1024 * 64;
            break;
        case 3:
            allocsize = 1024 * 1024 * 512;
            break;
        default:
            printf("Wrong option.\nExiting...");
            exit(0);
    }
    printf("Executing memory leak... Ctrl + C if you choose not to execute, or\nwait for the OOM reaper to reap the executable.\n");
    logs(0, "Executing memory leak now...");
    printf("Memory leak allocations will be printed in: %s.\n", LOGFILE);
    struct timespec salloc, ealloc;
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &salloc);
        void *ptr = mmap(NULL, allocsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        memset(ptr, 0xF, allocsize);
        clock_gettime(CLOCK_MONOTONIC, &ealloc);
        double ttaken = (ealloc.tv_sec - salloc.tv_sec) * 1000.0 + (ealloc.tv_nsec - salloc.tv_nsec) / 1000000.0;
        logs(0, "Allocated memory size %zu bytes in duration: %.3f milliseconds.", allocsize, ttaken);
    }
    exit(0);
}

int opt6(void) {
    printf("Fork bomb!\n");
    logs(0, "User has initiated Fork Bomb");
    printf("WARNING: The fork bomb will violently implode your system (as it executes multiple programs.\nPress any key to continue...\n");
    getchar();
    printf("Executing fork bomb now...\n");
    while (1) {
        pid_t process = fork();
        if (process == 0) {
            logs(1, "Kernel scheduler exhausted. PID = 0.");
            return 1;
        }
        logs(0, "Forked process for fork bomb: PID %d", process);
    }
}

int opt7(void) {
    printf("File Descriptor Exhaustion!\n");
    logs(0, "User has initiated File Descriptor Exhaustion)");
    printf("WARNING: This will lock up file handles and suffocate the process table.\nWhether the system/file handles lock up depends on your console file descriptor limit.\nPress any key to continue...\n");
    getchar();
    printf("Executing now...\n");
    logs(0, "File descriptor exhaustion has started.");
    while (1) {
        int fd = open("/dev/null", O_RDONLY);
        if (fd < 0) {
            perror("Open failed");
            logs(1, "File descriptor table completely exhausted. Exiting...");
            break;
        }
    }
    return 0;
}

int opt8(void) {
    printf("Buffer overflow!\n");
    logs(0, "Triggering buffer overflow....\n");
    char *arr = malloc(5);
    strcpy(arr, "WINDOWS");
    return 0;
}

int ch(const int usrch) {
    switch (usrch) {
        case 1:
            opt1();
            break;
        case 2:
            opt2();
            break;
        case 3:
            opt3();
            break;
        case 4:
            opt4();
            break;
        case 5:
            opt5();
            break;
        case 6:
            opt6();
            break;
        case 7:
            opt7();
            break;
        case 8:
            opt8();
            break;
        default:
            printf("Wrong choice. Exiting...");
            logs(1, "User chose wrong choice. The program will exit.");
            exit(1);
    }
    return 0;
}

void ver() {
    printf("KallyLC %s\nCopyright (C) 2026 NotToBT.\nLicense GPLv2 ONLY: GNU GPL version 2 only <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>\nThis is free software; see the source for copying conditions.  There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n", VER);
}

int main(int argc, char *argv[]) {
    int choice = 0;
    int opt;
    struct option long_options[] = {
        {"option",  required_argument, NULL, 'o'},
        {"version", no_argument,       NULL, 'v'},
        {"help",    no_argument,       NULL, 'h'},
        {NULL,      0,                 NULL,  0 }
    };
    while ((opt = getopt_long(argc, argv, "o:vh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'o': {
                char *endptr;
                long value = strtol(optarg, &endptr, 10);
                if (endptr == optarg || *endptr != '\0') {
                    fprintf(stderr, "Error: Option requires an integer. Found '%s'\n", optarg);
                    return 1;
                }
                choice = (int)value;
                printf("Choice set to: %d\n", choice);
                break;
            }
            case 'v':
                ver();
                return 0;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case '?':
                return 1;
            default:
                return 1;
        }
    }
    for (int i = optind; i < argc; i++) {
        printf("Non-option/Positional argument: %s\n", argv[i]);
    }
    char str2[256];
    system("clear");
    init();
    printf("Welcome to KallyLC %s!\n", VER);
    if (DEBUG_MODE == 1) {
        printf("You're in DEBUG mode. (Your version of KallyLC was compiled with debug mode)\n");
        logs(0, "Debug mode is active.");
    }
    if (PROT_DISABLE == 1) {
        system("clear");
        logs(1, "Protection mode is active. The program cannot continue.");
        exit(1);
    }
    if (choice != 0) {
        snprintf(str2, sizeof(str2), "User chose choice: %i", choice);
        logs(0, str2);
        printf("Choice %i is defined in argument --option. Continuing...", choice);
        printf("Press any key to continue...\n");
        getchar();
        ch(choice);
    }
    printf("You have %d options:\n", OPTCOUNT);
    printf("The options are:\n");
    printf("1: MCE hardware corruption\n[User-Kernel space]\n2: General Protection Fault (#GP) [User space]\n3: Null Pointer Dereference / Segmentation fault [User space]\n4: Kernel-level errors (%i choices) [Kernel space]\n5: Memory leak via memory allocation\n6: Fork bomb\n7: File descriptor exhaustion\n8: Buffer overflow\n", KRNLCH);
    printf("Choice? ");
    scanf("%i", &choice);
    snprintf(str2, sizeof(str2), "User chose choice: %i", choice);
    logs(0, str2);
    system("clear");
    ch(choice);
    return 0;
}
