#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libelf.h>
#include <gelf.h>
#include <stdbool.h>
#include <json-c/json.h>

#define USAGE "Usage: %s [-hv] [-o] [-p] FILE\n"

int main(int argc, char *argv[]) {
    int opt;
    bool print_owner_opt = false;
    bool pretty_print_opt = false;
    char *filename;
    int fd;
    Elf *elf;
    size_t phnum;
    int r;
    char *owner = NULL;
    char *value = NULL;

    while((opt = getopt(argc, argv, "hvop")) != -1) {
        switch(opt) {
            case 'h':
                printf(USAGE, argv[0]);
                exit(0);
            case 'v':
                puts("pkgnote-v0.1.0");
                exit(0);
            case 'o':
                print_owner_opt = true;
                break;
            case 'p':
                pretty_print_opt = true;
                break;
        }
    }

    if(argc - optind != 1) {
        fprintf(stderr, USAGE, argv[0]);
        exit(1);
    }

    filename = argv[optind];

    fd = open(filename, O_RDONLY);
    if(fd < 0) {
        fprintf(stderr, "Cannot open %s: %s\n", filename, strerror(errno));
        exit(1);
    }

    if(elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed: %s\n",
                elf_errmsg(-1));
        exit(1);
    }

    elf = elf_begin(fd, ELF_C_READ, NULL);
    if(!elf) {
        fprintf(stderr, "elf_begin() failed: %s\n", elf_errmsg(-1));
        exit(1);
    }

    if(elf_kind(elf) != ELF_K_ELF) {
        fprintf(stderr, "File %s is not an ELF\n", filename);
        exit(1);
    }

    r = elf_getphdrnum(elf, &phnum);
    if(r < 0) {
        fprintf(stderr, "Cannot get the number of ELF headers: %s\n",
                elf_errmsg(-1));
        exit(1);
    }

    for(size_t i = 0; i < phnum; i++) {
        GElf_Phdr dest;
        GElf_Phdr *ph;
        Elf_Data *data;
        GElf_Nhdr nh;
        size_t nh_off = 0;
        size_t name_off;
        size_t desc_off;

        ph = gelf_getphdr(elf, i, &dest);
        if(!ph || ph->p_type != PT_NOTE)
            continue;

        data = elf_getdata_rawchunk(elf, ph->p_offset, ph->p_filesz,
                                    ELF_T_NHDR);

        while(nh_off < data->d_size && (nh_off = gelf_getnote(data, nh_off,
                                                              &nh, &name_off,
                                                              &desc_off))) {
            if(nh.n_namesz == 0 || nh.n_descsz == 0)
                continue;

            if(nh.n_type != 0xCAFE1A7E)
                continue;

            owner = (char *) data->d_buf + name_off;
            value = (char *) data->d_buf + desc_off;

            break;
        }
    }

    if(!(owner && value))
        goto end;

    if(print_owner_opt)
        puts(owner);
    else {
        json_object *obj = NULL;
        int flags = JSON_C_TO_STRING_NOSLASHESCAPE;

        obj = json_tokener_parse(value);
        if(!obj)
            goto end;

        if(pretty_print_opt)
            flags |= JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED;
        else
            flags |= JSON_C_TO_STRING_PLAIN;

        puts(json_object_to_json_string_ext(obj, flags));

        json_object_put(obj);
    }

end:
    elf_end(elf);
    close(fd);

    exit(0);
}
