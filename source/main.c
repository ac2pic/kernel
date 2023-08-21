//#define DEBUG_SOCKET
#define DEBUG_IP "192.168.2.2"
#define DEBUG_PORT 9023


#include "ps4.h"
#define KCALL_REL32(k, src, dest) do { *(uint32_t *)(k + src + 1) = (dest - src - k - 5); } while(0);

static inline __attribute__((always_inline)) uint64_t kernbase() {
	return __readmsr(0xC0000082) - 0x1C0;
}


__attribute__((noinline, section("kp"))) int inject(int *param_1,int *param_2, void * param_3,void * param_4,int param_5, int param_6, uint8_t * param_7,unsigned long param_8,int param_9,int param_10,
                int param_11) {
  	int (*printf)(const char *fmt, ...);
  	printf = (void *)(kernbase() + 0x000B7A30);
	char abc[] = "%x \0";
	for(unsigned long i = 0; i < 0x5A0; i++) {
		printf(abc, param_7[i]);
	}
	return -256;
}


int patch_mount() {
  int (*printf)(const char *fmt, ...);
  extern uint8_t __start_kp[];
  extern uint8_t __stop_kp[];
  // Disable write protection
  uint64_t cr0 = readCr0();
  writeCr0(cr0 & ~X86_CR0_WP);
  uint64_t kb = kernbase();
  printf = (void *)(kb + 0x000B7A30);
  // Free space for our function
  uint64_t startAddr = kb + 0x00cfcef8;
  int len = &__stop_kp[0] - &__start_kp[0];
  for (int i = 0; i < len; i++) {
	uint8_t * kernFunc = (uint8_t *)startAddr;
	kernFunc[i] = __start_kp[i];
  }
  KCALL_REL32(kb, 0x006c412a, startAddr);
  writeCr0(cr0);
  printf("done writing patches");
  return 0;
}

int _main(struct thread *td) {
  UNUSED(td);

  kexec(patch_mount, 0);


  return 0;
}
