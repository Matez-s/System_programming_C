
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static const char hexdigits[] = "0123456789abcdef";
int hex_encode(const unsigned char *in, size_t inlen, char *out, size_t outlen) {
    if (outlen < inlen * 2 + 1) {
        return -1;
    }
    if (inlen > 0 && (in == NULL || out == NULL)) {
        return -1;
    }

    for (size_t i = 0; i < inlen; i++) {
        out[2 * i] = hexdigits[(in[i] >> 4) & 0xF]; // 0000 1011 AND 0000 1111 -> 1011 = B
        out[2 * i + 1] = hexdigits[in[i] & 0xF]; // 1011 0111 AND 0000 1111 -> 0111 = 7
    }
    out[inlen * 2] = 0;
    return 0;
}


int hex_decode(const char *in, unsigned char *out, size_t outlen) {
    size_t n = strlen(in);
    if (n % 2 != 0 || outlen < n / 2) {
        return -1;
    }
    for (size_t i = 0; i < n; i += 2) {
        char a = in[i];
        char b = in[i + 1];

        int va = (a >= '0' && a <= '9') ? a - '0' : (a >= 'a' && a <= 'f') ? a - 'a' + 10
        : (a >= 'A' && a <= 'F') ? a - 'A' + 10 : -1;
        int vb = (b >= '0' && b <= '9') ? b - '0' : (b >= 'a' && b <= 'f') ? b - 'a' + 10
        : (b >= 'A' && b <= 'F') ? b - 'A' + 10 : -1;

        if (va < 0 || vb < 0) {
            return -1;
        }
        out[i / 2] = (unsigned char) ((va << 4) | vb);
    }
    return 0;
}

void trim_newline(char *s) {
    //zistim velkost pola
    //cyklus
    //prechadzam odkonca --> kontroluj ci index nie je mensi ako 0 + ci som narazil \r || \n
    //nastavim posledny bajt na 0 a znizim index
    // koniec cyklu

    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

void split_first(char *line, char **head, char **tail) {
        //string: hello world hjsdkasdk |-> head: hello; tail: world.....
        //**cyklus
        // najdem prvy neprazdny znak
        //******************
        //ulozim head na ten prvy neprazdny znak
        // **cyklus
        // iterovat stringom do dalsieho bieleho znaku
        //******************
        // nedzeru nahradím 0
        // ** cyklus
        // najdem dalsi nebiely znak
        // *****************
        // nastavim ho na tail


    // 1. Preskoč úvodné medzery (iba ' ')
    while (*line && *line == ' ') {
        line++;
    }

    // 2. Nastav začiatok prvého slova
    *head = line;

    // 3. Prechádzaj až po prvú medzeru
    while (*line && *line != ' ') {
        line++;
    }

    // 4. Ak sme našli medzeru, ukonči head
    if (*line) {
        *line = '\0';
        line++;
    }

    // 5. Preskoč ďalšie medzery
    while (*line && *line == ' ') {
        line++;
    }

    // 6. Nastav tail (ak existuje)
    *tail = *line ? line : NULL;


}
void split_two(char *line, char **a, char **b, char **rest) {
    split_first(line, a, rest);
    split_first(*rest, b, rest);
}

//bezpecne kopirovanie stringov
char *strdup_safe(const char *s) {
    if (!s) {
        return NULL;
    }
    size_t len = strlen(s) + 1;
    char *p = malloc(len);
    if (p == NULL) {

        return NULL;
    } else {
        memcpy(p, s, len);
    }
    return p;

}

void print_help(void) {
    fprintf(stderr,
            "Available commands:\n"
            "  CREATE <username> <password>\n"
            "  LOGIN <username> <password>\n"
            "  LOGOUT\n"
            "  DELETE-ACCOUNT <password>\n"
            "  CONTACTS ADD <username>            # pošle žiadosť o kontakt\n"
            "  CONTACTS REQUESTS                  # zoznam prichádzajúcich žiadostí\n"
            "  CONTACTS ACCEPT <username>         # prijme žiadosť\n"
            "  CONTACTS DECLINE <username>        # odmietne žiadosť\n"
            "  CONTACTS LIST                      # potvrdené kontakty\n"
            "  MSG <username> <text>\n"
            "  FILE SEND <username> <path>\n"
            "  HELP\n"
    );
}

int read_file(const char *path, unsigned char **out, size_t *outlen) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Failed to open file '%s': %s\n", path, strerror(errno));
        return -1;
    }

    if (fseek(f, 0, SEEK_END) != 0) {  //tu ide kurzor až na seek and a ostáva na konci !!!
        fprintf(stderr, "Failed to seek to end of file '%s': %s\n", path, strerror(errno));
        fclose(f);
        return -1;
    }

    long sz = ftell(f);
    if (sz < 0) {

        fprintf(stderr, "Failed to get file size '%s': %s\n", path, strerror(errno));
        fclose(f);
        return -1;
    }

    if (fseek(f, 0, SEEK_SET) != 0) { //tu dávam kurzor zase na začiatok, aby som mohol čítať súbor
        fprintf(stderr, "Failed to seek to end of file '%s': %s\n", path, strerror(errno));
        fclose(f);
        return -1;
    }

    unsigned char *buf = malloc(sz);
    if (!buf) {
        // napisat chybove hlasenie
        fclose(f);
        return -1;
    }

    size_t read_count = fread(buf, 1, (size_t)sz, f);
    if (read_count != (size_t)sz) {
        fprintf(stderr, "Failed to read file '%s': %s\n", path, strerror(errno));
        free(buf);
        fclose(f);
        return -1;
    }

    if (fclose(f) != 0) {
        fprintf(stderr, "Failed to close file '%s': %s\n", path, strerror(errno));
        return -1;
    }

    *out = buf;
    *outlen = (size_t)sz;
    return 0;
}


int write_file(const char *path, const unsigned char *data, size_t len) {
    FILE *f = fopen(path, "wb");
    fwrite(data, 1, (size_t)len, f);
    fclose(f);

    return 0;

}

// int mkdir_p(const char *path) {
//     struct stat st;
//     if (stat(path, &st) == 0) {
//         if (S_ISDIR(st.st_mode)) { //kontrola či je to is directory
//             return 0;
//         }
//
//         errno = ENOTDIR;
//         return -1;
//     }
//     return mkdir(path, 0700);
// }

//ukážka random bajtov
void random_bytes(unsigned char *buf, size_t len) {
    FILE *f = fopen("/dev/urandom", "rb"); //berie čas, alebo teplotu... je to naozaj random
    if (!f) {
        srand((unsigned)time(NULL)); //pseudorandom
        for (size_t i = 0; i < len; i++) {
            buf[i] = (unsigned char) (rand() & 0xff); //volám klasický logický AND a klasické jednotky (?)
        }
        return;
    } else {
        fread(buf, 1, (size_t)len, f);
        fclose(f);
    }

}

void swap(void *a, void *b, size_t item_size) {
    unsigned char tmp[item_size];
    memcpy(tmp, a, item_size);
    memcpy(a, b, item_size);
    memcpy(b, tmp, item_size);
}

void insert_sort(void *array, size_t item_size, size_t n, int (*cmp_item) (const void *, const void*)) {
    for (size_t i = 0; i < n; i++) {
        unsigned char value[item_size];
        memcpy(value, (unsigned char*)array + i * item_size, item_size);

        size_t j = i;
        while (j >= 1 && cmp_item(value, (unsigned char*)array + (j - 1) * item_size) > 0) {
            memcpy((unsigned char*)array + j * item_size, (unsigned char*)array + (j-1) * item_size, item_size);
            --j;
        }
        memcpy((unsigned char*)(array + j * item_size), value, item_size);
    }
}

int cmp_int(const void *a, const void *b) {
    int a_val = *(const int*)a;
    int b_val = *(const int*)b;
    return a_val < b_val ? -1 : a_val == b_val ? 0 : 1;
}

int cmp_double(const void *a, const void *b) {
    double a_val = *(const double*)a;
    double b_val = *(const double*)b;

    if (a_val < b_val) {
        return -1;
    } else if (a_val > b_val) {
        return 1;
    } else {
        return 0;
    }
}
