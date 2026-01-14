#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <elf.h>
extern "C" __u64 _svc(...);

#define __align_up(x)         ((x + 0x1000 - 1) & ~(0x1000 - 1))

#define _exit_group(sig) _svc(__NR_exit_group,sig)

#define _mprotect(addr,len,prot) _svc(__NR_mprotect,addr,len,prot)

#define _open(name,flag) _svc(__NR_openat,AT_FDCWD,name,flag)

#define _close(fd) _svc(__NR_close,fd)

#define _lseek(fd,offset,whence) _svc(__NR_lseek,fd,offset,whence)

#define _read(fd,buf,count) _svc(__NR_read,fd,buf,count)

#define _mmap(addr,length,prot,flags,fd,offset) _svc(__NR_mmap,addr,length,prot,flags,fd,offset)

#define _mremap(addr, old_size, new_size, flags) _svc(__NR_mremap, addr, old_size, new_size, flags)

#define _munmap(addr,length) _svc(__NR_munmap,addr,length)

static inline void _memcpy(void *dest,void *src,size_t n)
{
  for(int i = 0; i < n; i++)
  {
   *(char *)((uint64_t)dest + i) = *(char *)((uint64_t)src + i);
  }
}

static inline void _memset(void *dest,size_t n)
{
  for(int i = 0; i < n; i++)
  {
   *(char *)((uint64_t)dest + i) = 0;
  }
}
