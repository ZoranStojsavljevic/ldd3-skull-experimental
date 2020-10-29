### Skull Revisited

	[1] The Makefile is corrected;
	[2] The appropriate functions are added to make the skull module again alive;
	[3] Some requested definitions are added to avoid warnings;
	[4] The STEP is enhanced from 2K to 64K. It is, after all, 19 years later!
	[5] Some code cleanup is done to make it more understandable (improving printks).

### ISA range inspected

Please, execute the following command:

	$ sudo cat /proc/iomem

	00000000-00000fff : Reserved
	00001000-00057fff : System RAM
	00058000-00058fff : Reserved
	00059000-0009cfff : System RAM
	0009d000-0009ffff : Reserved
	000a0000-000bffff : PCI Bus 0000:00	<<== ISA range inspected
	000f0000-000fffff : System ROM		<<== ISA range inspected
	00100000-9f3ca017 : System RAM
	9f3ca018-9f3d9457 : System RAM
	9f3d9458-9f3da017 : System RAM
	9f3da018-9f3e3457 : System RAM
	9f3e3458-9f3e4017 : System RAM
	9f3e4018-9f3f4057 : System RAM
	9f3f4058-aab7efff : System RAM
	aab7f000-abe7efff : Reserved
	...[continues]...

WARNING: Do note that this topology is proprietary to the model of the PC device used!

### Some additional concerns

Please, do note that the legacy x86(_64) 64K IO port space remains intact, while PCIe is mainly
used for MMIO.

Please, execute the following command:

	$ sudo cat /proc/ioports

To get familiar with PCIe ports (PCH feature) on modern PC platforms.

The legacy COM ports in PC-compatible computers are typically defined as:

https://en.wikipedia.org/wiki/COM_(hardware_interface)

	COM1: I/O port 0x3F8, IRQ 4
	COM2: I/O port 0x2F8, IRQ 3
	COM3: I/O port 0x3E8, IRQ 4
	COM4: I/O port 0x2E8, IRQ 3

### HW initialization

In the context of the skull driver it is still not known how the IO HW initialization should be
performed. Probably none, since it is done by the UEFI/kernel bringup (devices were already
allocated and initialized), thus the additional HW initialization (in this context) will fail.

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
	[ 2763.827372] Insert the skull loadable module
	[ 2763.827385] resource sanity check: requesting [mem 0x000a0000-0x000fffff], which spans more than PCI Bus 0000:00 [mem 0x000a0000-0x000bffff window]
	[ 2763.827388] caller skull_init+0x35/0x1000 [skull] mapping multiple BARs
	[ 2763.827389] --- ioremap addr = ffffb8e6c3480000 ---
	[ 2763.827393] a0000: EMPTY
	[ 2763.827396] b0000: EMPTY
	[ 2763.827397] c0000: EMPTY
	[ 2763.827398] d0000: ROM
	[ 2763.827399] e0000: RAM
	[ 2763.827400] f0000: ROM
	[ 2763.827400] result: 0, base: 280
	[ 2763.827401] result: 0, base: 290
	[ 2763.827401] result: 0, base: 2a0
	[ 2763.827402] result: 0, base: 2b0
	[ 2763.827402] result: 0, base: 2c0
	[ 2763.827402] result: 0, base: 2d0
	[ 2763.827403] result: 0, base: 2e0
	[ 2763.827403] result: 0, base: 2f0

	$ ls -al /sys/module | grep skull
	drwxr-xr-x.   5 root root 0 Oct 25 10:30 skull
