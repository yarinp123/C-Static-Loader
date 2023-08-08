#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg) {
  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
  Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + ehdr->e_phoff);
  printf("Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");

  for (int i = 0; i < ehdr->e_phnum; i++) {
    func(&phdr[i], i);
  }
  return 0;
}

void print_phdr(Elf32_Phdr *phdr, int i) {
  printf("Program header number %d at address %p\n", i, phdr);
}

char* typeString(Elf32_Phdr *phdr){
  if(phdr->p_type == PT_NULL) return "NULL";
  if(phdr->p_type == PT_LOAD) return "LOAD";
  if(phdr->p_type == PT_DYNAMIC) return "DYNAMIC";
  if(phdr->p_type == PT_INTERP) return "INTERP";
  if(phdr->p_type == PT_NOTE) return "NOTE";
  if(phdr->p_type == PT_SHLIB) return "SHLIB";
  if(phdr->p_type == PT_PHDR) return "PHDR";
  if(phdr->p_type == PT_TLS) return "TLS";
  if(phdr->p_type == PT_NUM) return "NUM";
  if(phdr->p_type == PT_LOOS) return "LOOS";
  if(phdr->p_type == PT_GNU_EH_FRAME) return "GNU_EH_FRAME";
  if(phdr->p_type == PT_GNU_STACK) return "GNU_STACK";
  if(phdr->p_type == PT_GNU_RELRO) return "GNU_RELRO";
  if(phdr->p_type == PT_GNU_PROPERTY) return "GNU_PROPERTY";
  if(phdr->p_type == PT_LOSUNW) return "LOSUNW";
  if(phdr->p_type == PT_SUNWBSS) return "SUNWBSS";
  if(phdr->p_type == PT_SUNWSTACK) return "SUNWSTACK";
  if(phdr->p_type == PT_HISUNW) return "HISUNW";
  if(phdr->p_type == PT_HIOS) return "HIOS";
  if(phdr->p_type == PT_LOPROC) return "LOPROC";
  if(phdr->p_type == PT_HIPROC) return "HIPROC";
  return "";
}

void readelf_flag_l (Elf32_Phdr *phdr , int i){    
    printf("%s\t\t",  typeString(phdr));
    printf("%#0*x\t", 8,phdr->p_offset);
    printf("%#0*x\t", 8,phdr->p_vaddr);
    printf("%#0*x\t", 8,phdr->p_paddr);
    printf("%#0*x\t", 6,phdr->p_filesz);
    printf("%#0*x\t", 6,phdr->p_memsz);
    
    if(phdr->p_flags & PF_R) printf("%c", 'R'); else printf(" ");
    if(phdr->p_flags & PF_W) printf("%c", 'W'); else printf(" ");
    if(phdr->p_flags & PF_X) printf("%c", 'E'); else printf(" ");
    printf("%d\t", phdr->p_flags);
    printf("0x%x\n", phdr->p_align);
}


void readelf_flag_l_2 (Elf32_Phdr *phdr , int i){
  int prot = PROT_NONE;
  if(phdr->p_flags & PF_R) prot = prot | PROT_READ;
  if(phdr->p_flags & PF_W) prot = prot | PROT_WRITE;
  if(phdr->p_flags & PF_X) prot = prot | PROT_EXEC;
  int flags = MAP_PRIVATE | MAP_FIXED;
  printf("protection = 0x%X\nflags = 0x%X\n", prot, flags);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <executable>\n", argv[0]);
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    perror("fstat");
    return 1;
  }

  void *map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (map_start == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  if (foreach_phdr(map_start, readelf_flag_l, 0) < 0) {
    perror("foreach_phdr");
    return 1;
  }
  if (foreach_phdr(map_start, readelf_flag_l_2, 0) < 0) {
    perror("foreach_phdr");
    return 1;
  }

  munmap(map_start, st.st_size);
  close(fd);
  return 0;
}