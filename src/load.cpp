#include <svc.h>

#ifdef __cplusplus
extern "C" {
#endif

 int elf_mmap(int fd, Elf64_auxv_t* auxv)
 {
   int linker_fd;
   
   int size = _lseek(fd, 0, SEEK_END);
   
   _lseek(fd, 0, SEEK_SET);
   
   void *data = (void *)_mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, nullptr);
   
   void* ptr = (void *)_mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, nullptr, nullptr);
   
   Elf64_Ehdr *Ehdr = (Elf64_Ehdr *)data;
   
   Elf64_Phdr *Phdr = (Elf64_Phdr *)((__u64)data + Ehdr->e_phoff);
   
   for(int i = 0; i < Ehdr->e_phnum; i++)
   {
   
     if(__align_up(Phdr[i].p_vaddr + Phdr[i].p_memsz) > size)
     {
       ptr = (void *)_mremap(ptr,size,__align_up(Phdr[i].p_vaddr + Phdr[i].p_memsz),MREMAP_MAYMOVE);
       size = __align_up(Phdr[i].p_vaddr + Phdr[i].p_memsz);
     }
     
     _memcpy((void *)((__u64)ptr + Phdr[i].p_vaddr),(void *)((__u64)data + Phdr[i].p_offset),Phdr[i].p_filesz);
      
     if(Phdr[i].p_type == PT_INTERP)
     {
       linker_fd = _open((__u64)ptr + Phdr[i].p_vaddr, O_RDONLY);
     }
   }
   
   for(int i = 0;auxv[i].a_type != NULL;i++)
   {
      switch (auxv[i].a_type)
      {
        case AT_PHDR:
        auxv[i].a_un.a_val = (__u64)ptr + Ehdr->e_phoff;
        break;
       
        case AT_PHNUM:
        auxv[i].a_un.a_val = Ehdr->e_phnum;
        break;
       
        case AT_ENTRY:
        auxv[i].a_un.a_val = (__u64)ptr + Ehdr->e_entry;
        break;
       
        default:
        break;
      }
   }
   
   _munmap(data,_lseek(fd, 0, SEEK_END));
   
   _close(fd);
   
   return linker_fd;
 }

 __u64 load(Elf64_auxv_t* auxv)
 {

   int linker_fd = elf_mmap(_open("/system/bin/sh",O_RDONLY), auxv); // /system/bin/sh load
  
   int size = _lseek(linker_fd, 0, SEEK_END);

   Elf64_Ehdr *Ehdr = (Elf64_Ehdr *)_mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, linker_fd, nullptr);
   
   _close(linker_fd);
   
   for(int i = 0;auxv[i].a_type != 0;i++)
   {
      switch (auxv[i].a_type)
      {
        case AT_BASE:
        auxv[i].a_un.a_val = (__u64)Ehdr;
        break;
       
        default:
        break;
      }
   }

   Elf64_Phdr *Phdr = (Elf64_Phdr *)((__u64)Ehdr + Ehdr->e_phoff);
   
   for(int i = 0; i < Ehdr->e_phnum; i++)
   {
     _memset((void *)((__u64)Ehdr + Phdr[i].p_vaddr + Phdr[i].p_filesz),Phdr[i].p_memsz - Phdr[i].p_filesz);
   }
   
   return (__u64)Ehdr + Ehdr->e_entry;
 }

#ifdef __cplusplus
}
#endif

