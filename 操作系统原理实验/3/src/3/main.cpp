#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include "dev.cpp"

#if __x86_64__
#define ElfW(type) Elf64_##type
#else
#define ElfW(type) Elf32_##type
#endif

char* shstrtbl[300];
char* strtbl;
ElfW(Sym)* symtbl; 




int main(int argc,char *argv[]) {
    if(argc != 2) {
        printf("usage: %s <ELF_FILE>\n", argv[0]);
        exit(1);
    }
    
    char *thefilename = argv[1];
    FILE *thefile;
    struct stat statbuf;
    

    ElfW(Ehdr) *ehdr = 0;
    ElfW(Phdr) *phdr = 0;
    ElfW(Shdr) *shdr = 0;
    
    if (!(thefile = fopen(thefilename, "r")))
    {
        perror(thefilename);
        exit(EXIT_FAILURE);
    }
    
    if (fstat(fileno(thefile), &statbuf) < 0)
    {
        perror(thefilename);
        exit(EXIT_FAILURE);
    }
    pid_t pid,pidme;
    pidme = getpid();
    if((pid = fork()) == 0) {
        if(execve(thefilename,argv,environ)) {
            printf("execve failure\n");
            exit(0);
        }
    }
    printf("pid:%d\n",pid);

//
    ehdr = (ElfW(Ehdr) *)mmap(0, statbuf.st_size,
                              PROT_READ | PROT_WRITE, MAP_PRIVATE, fileno(thefile), 0);

    phdr = (ElfW(Phdr)*)(ehdr->e_phoff + (char *)ehdr);
    shdr = (ElfW(Shdr)*)(ehdr->e_shoff + (char *)ehdr);                               
    
    ElfW(Shdr) *shstrtab =  &shdr[ehdr->e_shstrndx];
    char *shstr = (char *)(shstrtab->sh_offset + (char *)ehdr);
    int index = 0;

    shstrtbl[index] = shstr;
    shstr++;
    index++;

    while(1) {
        size_t len = strlen(shstr);
        if(len == 0) {
            break;
        }
        shstrtbl[index] = shstr;
        index++;
        shstr += len+1;
    }

    ElfW(Shdr) *strtab;
    ElfW(Shdr) *symtab;

    Elf64_Word secidx = 0;
    
    shdr++;
    for(int i = 0;i < ehdr->e_shnum;i++) {
        secidx = shdr->sh_name;
        if(strcmp((shstrtbl[0]+secidx), ".strtab") == 0) {
            strtab = shdr;
        } else if(strcmp(shstrtbl[0]+secidx,".symtab") == 0) {
            symtab = shdr;
        }
        shdr++;
    }
    
    //symtab --> symtbl
    symtbl = (ElfW(Sym)*)(symtab->sh_offset + (char*)ehdr);
    size_t symsize = symtab->sh_size/symtab->sh_entsize;

    //strtab --> strtbl
    char *str = (char*)(strtab->sh_offset + (char*)ehdr); 
    // index = 0;
    strtbl = str;

    char name[50];
    bool find = false;
    uintptr_t vaddr,paddr;
    while(1) {
        printf("dest:");
        scanf("%s",name);
        for(int i = 0;i < symsize;i++) {
            int stridx = symtbl[i].st_name;
            // printf("str-tbl:%s\n",strtbl+stridx);
            if(strstr(strtbl+stridx,name) != NULL) {
                find = true;
                vaddr = symtbl[i].st_value;
                
                if(vaddr == 0) {
                    vaddr = (uintptr_t)dlsym(RTLD_DEFAULT,name);
                    if(virt_to_phys_user(&paddr,pidme,vaddr) == 1) {
                        printf("symbol:%s convert physical addr failure\n",name);
                        continue;
                    }                    
                } else {
                    if(virt_to_phys_user(&paddr,pid,vaddr) == 1) {
                        printf("symbol:%s convert physical addr failure\n",name);
                        continue;
                    }
                }
                printf("symbol:%s  vaddr:0x%lx paddr:0x%lx\n",strtbl+stridx,vaddr,paddr);
            }  
        }
        if(!find) {
            vaddr = (uintptr_t)dlsym(RTLD_DEFAULT,name);
            if(vaddr != 0) {
                if(virt_to_phys_user(&paddr,pidme,vaddr) == 1) {
                    printf("symbol:%s convert physical addr failure\n",name);
                    continue;
                } else {
                    printf("symbol:%s  vaddr:0x%lx paddr:0x%lx\n",name,vaddr,paddr);
                }
            } else {
                printf("not found\n");
            }
        }
        find = false;
    }
}