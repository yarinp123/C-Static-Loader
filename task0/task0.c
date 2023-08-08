#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg) {
  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
  Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + ehdr->e_phoff);
  for (int i = 0; i < ehdr->e_phnum; i++) {
    func(&phdr[i], i);
  }
  return 0;
}

void print_phdr(Elf32_Phdr *phdr, int i) {
  printf("Program header number %d at address %p\n", i, phdr);
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

  if (foreach_phdr(map_start, print_phdr, 0) < 0) {
    perror("foreach_phdr");
    return 1;
  }

  munmap(map_start, st.st_size);
  close(fd);
  return 0;
}