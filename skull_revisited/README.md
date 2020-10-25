### skull_revisited

	[1] The Makefile is corrected;
	[2] The appropriate functions are added to make the skull module again alive;
	[3] Some requested definitions are added to avoid warnings;
	[4] The STEP is enhanced from 2K to 64K. It is, after all, 19 years later!
	[5] Some code cleanup is done to make it more understandable (improving printks).

### Some additional concerns

Please, do note that the legacy x86_64 IO space of 1024 ports remains intact, while PCIe is mainly
used for MMIO.

please, execute the command:

	$ cat /proc/ioports

To get familiar with PCIe ports (PCH feature) on modern PC platforms.

The legacy COM ports in PC-compatible computers are typically defined as:

https://en.wikipedia.org/wiki/COM_(hardware_interface)

	COM1: I/O port 0x3F8, IRQ 4
	COM2: I/O port 0x2F8, IRQ 3
	COM3: I/O port 0x3E8, IRQ 4
	COM4: I/O port 0x2E8, IRQ 3

### HW initialization

In the context of the skull driver it is still not known how the IO HW initialization should be
performed. Probably none, since it is done by the UEFI/kernel bringup.

Please, do note that there is NO BIOS driving FW, rather UEFI FW. Thus, Extension ROMs are (for
already a long time) obsolete.

### Handling

	$ make clean
	make -C /lib/modules/5.8.16-200.fc32.x86_64/build/ M=/home/vuser/projects/github/linux-cip-misc/Education/lang-tools/test-drivers/skull/skull_revisited clean
	make[1]: Entering directory '/usr/src/kernels/5.8.16-200.fc32.x86_64'
	make[1]: Leaving directory '/usr/src/kernels/5.8.16-200.fc32.x86_64'

	$ make
	make -C /lib/modules/5.8.16-200.fc32.x86_64/build/ M=/home/vuser/projects/github/linux-cip-misc/Education/lang-tools/test-drivers/skull/skull_revisited modules
	make[1]: Entering directory '/usr/src/kernels/5.8.16-200.fc32.x86_64'
	  CC [M]  /home/vuser/projects/github/linux-cip-misc/Education/lang-tools/test-drivers/skull/skull_revisited/skull.o
	  MODPOST /home/vuser/projects/github/linux-cip-misc/Education/lang-tools/test-drivers/skull/skull_revisited/Module.symvers
	  CC [M]  /home/vuser/projects/github/linux-cip-misc/Education/lang-tools/test-drivers/skull/skull_revisited/skull.mod.o
	  LD [M]  /home/vuser/projects/github/linux-cip-misc/Education/lang-tools/test-drivers/skull/skull_revisited/skull.ko
	make[1]: Leaving directory '/usr/src/kernels/5.8.16-200.fc32.x86_64'

	$ sudo insmod skull.ko

	$ dmesg

	...[snap]...

	[19631.497415] Insert the skull loadable module
	[19631.497429] resource sanity check: requesting [mem 0x000a0000-0x000fffff], which spans more than PCI Bus 0000:00 [mem 0x000a0000-0x000bffff window]
	[19631.497433] caller skull_init+0x1d/0x120 [skull] mapping multiple BARs
	[19631.497439] a0000: empty
	[19631.497442] b0000: empty
	[19631.497443] c0000: empty
	[19631.497444] d0000: ROM
	[19631.497445] e0000: RAM
	[19631.497446] f0000: ROM
	[19631.497447] result: 0, base: 280
	[19631.497447] result: 0, base: 290
	[19631.497448] result: 0, base: 2a0
	[19631.497448] result: 0, base: 2b0
	[19631.497449] result: 0, base: 2c0
	[19631.497449] result: 0, base: 2d0
	[19631.497449] result: 0, base: 2e0
	[19631.497450] result: 0, base: 2f0

	$ ls -al /sys/module | grep skull
	drwxr-xr-x.   5 root root 0 Oct 25 10:30 skull
