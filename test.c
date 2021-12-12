#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>

static const unsigned char encrypt_map[0x100] = {
  ['1'] = ';',
  ['2'] = 'J',
  ['3'] = 'v',
  ['4'] = 'K',
  ['5'] = 'A',
  ['6'] = 'E',
  ['7'] = 'W',
  ['8'] = 'l',
  ['9'] = '0',
  ['0'] = '4',
  ['-'] = '+',
  ['='] = 'F',
  ['!'] = 'h',
  ['@'] = '}',
  ['#'] = ']',
  ['$'] = 'T',
  ['%'] = 'Y',
  ['^'] = 'o',
  ['&'] = 's',
  ['*'] = '%',
  ['('] = '!',
  [')'] = 'B',
  ['_'] = 'j',
  ['+'] = 'p',
  ['Q'] = 'w',
  ['W'] = 'u',
  ['E'] = 'b',
  ['R'] = 'D',
  ['T'] = '<',
  ['Y'] = '$',
  ['U'] = '#',
  ['I'] = '.',
  ['O'] = '8',
  ['P'] = 'q',
  ['['] = 'U',
  [']'] = '/',
  ['\\'] = 'M',
  ['q'] = 'G',
  ['w'] = '|',
  ['e'] = '&',
  ['r'] = 'c',
  ['t'] = 'd',
  ['y'] = '\'',
  ['u'] = 'z',
  ['i'] = '{',
  ['o'] = '6',
  ['p'] = 'g',
  ['{'] = ')',
  ['}'] = '7',
  ['|'] = 'k',
  ['a'] = '[',
  ['s'] = 'Q',
  ['d'] = '_',
  ['f'] = '3',
  ['g'] = 'x',
  ['h'] = 'X',
  ['j'] = 't',
  ['k'] = '>',
  ['l'] = '@',
  [';'] = 'm',
  ['\''] = '=',
  ['A'] = ',',
  ['S'] = '-',
  ['D'] = '\'',
  ['F'] = 'N',
  ['G'] = 'P',
  ['H'] = 'r',
  ['J'] = '(',
  ['K'] = 'n',
  ['L'] = '1',
  ['X'] = 'R',
  ['C'] = 'I',
  ['V'] = 'y',
  ['B'] = 'f',
  ['N'] = 'i',
  ['M'] = '2',
  [','] = '^',
  ['.'] = 'V',
  ['/'] = '5',
  ['x'] = '9',
  ['c'] = 'O',
  ['v'] = 'Z',
  ['b'] = 'H',
  ['n'] = 'S',
  ['m'] = 'e',
  ['<'] = '*',
  ['>'] = 'C',
  ['Z'] = 'a',
  ['z'] = 'L',
};

static const unsigned char decrypt_map[] = {
  [';'] = '1',
  ['J'] = '2',
  ['v'] = '3',
  ['K'] = '4',
  ['A'] = '5',
  ['E'] = '6',
  ['W'] = '7',
  ['l'] = '8',
  ['0'] = '9',
  ['4'] = '0',
  ['+'] = '-',
  ['F'] = '=',
  ['h'] = '!',
  ['}'] = '@',
  [']'] = '#',
  ['T'] = '$',
  ['Y'] = '%',
  ['o'] = '^',
  ['s'] = '&',
  ['%'] = '*',
  ['!'] = '(',
  ['B'] = ')',
  ['j'] = '_',
  ['p'] = '+',
  ['w'] = 'Q',
  ['u'] = 'W',
  ['b'] = 'E',
  ['D'] = 'R',
  ['<'] = 'T',
  ['$'] = 'Y',
  ['#'] = 'U',
  ['.'] = 'I',
  ['8'] = 'O',
  ['q'] = 'P',
  ['U'] = '[',
  ['/'] = ']',
  ['M'] = '\'',
  ['G'] = 'q',
  ['|'] = 'w',
  ['&'] = 'e',
  ['c'] = 'r',
  ['d'] = 't',
  ['\\'] = 'y',
  ['z'] = 'u',
  ['{'] = 'i',
  ['6'] = 'o',
  ['g'] = 'p',
  [')'] = '{',
  ['7'] = '}',
  ['k'] = '|',
  ['['] = 'a',
  ['Q'] = 's',
  ['_'] = 'd',
  ['3'] = 'f',
  ['x'] = 'g',
  ['X'] = 'h',
  ['t'] = 'j',
  ['>'] = 'k',
  ['@'] = 'l',
  ['m'] = ';',
  ['='] = '\'',
  [','] = 'A',
  ['-'] = 'S',
  ['\''] = 'D',
  ['N'] = 'F',
  ['P'] = 'G',
  ['r'] = 'H',
  ['('] = 'J',
  ['n'] = 'K',
  ['1'] = 'L',
  ['R'] = 'X',
  ['I'] = 'C',
  ['y'] = 'V',
  ['f'] = 'B',
  ['i'] = 'N',
  ['2'] = 'M',
  ['^'] = ',',
  ['V'] = '.',
  ['5'] = '/',
  ['9'] = 'x',
  ['O'] = 'c',
  ['Z'] = 'v',
  ['H'] = 'b',
  ['S'] = 'n',
  ['e'] = 'm',
  ['*'] = '<',
  ['C'] = '>',
  ['a'] = 'Z',
  ['L'] = 'z',
};

static void encrypt_buffer(unsigned char *map, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        unsigned char c = encrypt_map[(size_t)map[i]];
        if (!c)
            continue;
        map[i] = c;
    }
}

static int encrypt_file(const char *file)
{
    int fd;
    int ret;
    unsigned char *map;
    size_t file_size;
    struct stat statbuf;

    //printf("\n\n");
    printf("Encrypting file %s ...\n", file);

    fd = open(file, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    ret = fstat(fd, &statbuf);
    if (ret < 0) {
        perror("fstat");
        close(fd);
        return 1;
    }

    file_size = (size_t) statbuf.st_size;

    map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    encrypt_buffer(map, file_size);
    msync(map, file_size, MS_ASYNC);
    munmap(map, statbuf.st_size);
    return 0;
}

static int encrypt_files(const char *folder)
{
    int ret = 0;
    DIR *dirp;
    char fullpath[8192];
    char newpath[8192 + sizeof(".loli")];

    dirp = opendir(folder);
    if (!dirp) {
        perror("opendir");
        return 1;
    }

    printf("\n\n");
    printf("=============================================\n");
    while (1) {
        const char *file;
        struct dirent *dir;

        dir = readdir(dirp);
        if (!dir)
            break;

        file = dir->d_name;
        if (!strcmp(file, ".") || !strcmp(file, ".."))
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", folder, file);
        ret = encrypt_file(fullpath);
        if (ret)
            break;

        snprintf(newpath, sizeof(newpath), "%s.loli", fullpath);
        rename(fullpath, newpath);
    }

    closedir(dirp);
    printf("=============================================\n");
    return ret;
}

static int encrypt_folder(void)
{
    char folder[1024];
    size_t len;

    printf("Enter the folder name to be encrypted: ");
    if (!fgets(folder, sizeof(folder), stdin)) {
        puts("stdin closed!");
        return 1;
    }

    len = strlen(folder);

    if (folder[len - 1] == '\n') {
        folder[len - 1] = '\0';
    }

    return encrypt_files(folder);
}

static void decrypt_buffer(unsigned char *map, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        unsigned char c = decrypt_map[(size_t)map[i]];
        if (!c)
            continue;
        map[i] = c;
    }
}

static int decrypt_file(const char *file)
{
    int fd;
    int ret;
    unsigned char *map;
    size_t file_size;
    struct stat statbuf;

    printf("Decrypting file %s ...\n", file);

    fd = open(file, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    ret = fstat(fd, &statbuf);
    if (ret < 0) {
        perror("fstat");
        close(fd);
        return 1;
    }

    file_size = (size_t) statbuf.st_size;

    map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    decrypt_buffer(map, file_size);
    msync(map, file_size, MS_ASYNC);
    munmap(map, statbuf.st_size);
    return 0;
}

static int decrypt_files(const char *folder)
{
    int ret = 0;
    DIR *dirp;
    char fullpath[8192 + sizeof(".loli")];
    char newpath[8192];

    dirp = opendir(folder);
    if (!dirp) {
        perror("opendir");
        return 1;
    }

    while (1) {
        size_t len;
        const char *file;
        struct dirent *dir;

        dir = readdir(dirp);
        if (!dir)
            break;

        file = dir->d_name;
        if (!strcmp(file, ".") || !strcmp(file, ".."))
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", folder, file);
        ret = decrypt_file(fullpath);
        if (ret)
            break;

        len = strlen(fullpath);
        memcpy(newpath, fullpath, len - 5);
        newpath[len - 4] = '\0';
        printf("%s\n", newpath);
        rename(fullpath, newpath);
    }

    closedir(dirp);
    return ret;
}

static int decrypt_folder(void)
{
    char folder[1024];
    size_t len;

    printf("Enter the folder name to be decrypted: ");
    if (!fgets(folder, sizeof(folder), stdin)) {
        puts("stdin closed!");
        return 1;
    }

    len = strlen(folder);

    if (folder[len - 1] == '\n') {
        folder[len - 1] = '\0';
    }

    return decrypt_files(folder);
}

int main(int argc, char *argv[])
{
    int exit_code = 0;
    char text[255];
    FILE *name;
    char path[255];
    FILE *dir;
    FILE *distro;
    char buffer[255];
    distro = popen("cat /etc/os-release", "r");
    fgets(buffer, sizeof(buffer), distro);
    dir = popen("pwd", "r");
    fgets(path, sizeof(path), dir);
    name = popen("whoami", "r");
    fgets(text, sizeof(text), name);

    buffer[strlen(buffer) - 1] = '\0';

    printf("\n");
    printf("Name : %s \n", text);
    printf("Path : %s \n", path);
    #ifdef _WIN32
    printf("Systems : Windows32\n");
    #endif
    #ifdef _WIN64
    printf("Systems : Windows32\n");
    #endif
    #ifdef __linux__
    printf("Systems : Linux %s Based\n", buffer);
    #else
  	printf("Can't detect OS\n");
  	#endif

    if (argc == 1) {
        printf("\n");
        printf("Usage:\n");
        printf("./loli [option]\n\n");
        printf("[+] OPTION\n");
        printf("%s [encrypt|decrypt]\n\n", argv[0]);
        printf("Example : ./loli encrypt\n");
        return 0;
    }

    if (!strcmp(argv[1], "encrypt")) {
        exit_code = encrypt_folder();
    }
    else if (!strcmp(argv[1], "decrypt")) {
        exit_code = decrypt_folder();
    }

    return exit_code;
}
