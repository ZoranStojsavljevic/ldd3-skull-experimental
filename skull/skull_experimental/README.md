### Skull Experimental

This is the experimental code developed for some PHY MEM UEFI architecture's and memory
mapping exploration by the author. This code works for the author and particular PC
author uses for the PHY MEM UEFI exploration.

	*-cpu
		product: Intel(R) Core(TM) i5-4300U CPU @ 1.90GHz
		vendor: Intel Corp.

Author makes this code available for others for the (possible) experimental exploration
on their own perils and terms.

WARNING/DANGER: if someone does want to use this code as is or change it, someone
should definitely have very good understanding what is dealing with/is up to!

No warranty attached; no any one of authors can be in any way liable or/and take ANY
responsibility for damages, bugs, errors caused/done by this code or fitness for use.

### PHY_MEM range inspected

WARNING: Do note that this topology is proprietary to the model of the PC device used!

Please, execute the following command to see on used PC system:

	$ sudo cat /proc/iomem

	00000000-00000fff : Reserved
	00001000-00057fff : System RAM
	00058000-00058fff : Reserved
	00059000-0009cfff : System RAM
	0009d000-0009ffff : Reserved
	000a0000-000bffff : PCI Bus 0000:00
	000f0000-000fffff : System ROM
	00100000-9f3ca017 : System RAM
	9f3ca018-9f3d9457 : System RAM
	9f3d9458-9f3da017 : System RAM
	9f3da018-9f3e3457 : System RAM
	9f3e3458-9f3e4017 : System RAM
	9f3e4018-9f3f4057 : System RAM
	9f3f4058-aab7efff : System RAM
	aab7f000-abe7efff : Reserved
	abe7f000-abf7efff : ACPI Non-volatile Storage
	abf7f000-abffefff : ACPI Tables
	abfff000-abffffff : System RAM
	ad200000-af1fffff : Reserved
	  ad200000-af1fffff : Graphics Stolen Memory
	af200000-dfffffff : PCI Bus 0000:00
	  af210000-af21ffff : pnp 00:00
	  af300000-af4fffff : PCI Bus 0000:04
	  b0000000-bfffffff : PCI Bus 0000:03
	    b0000000-bfffffff : 0000:03:00.0
	  c0000000-cfffffff : 0000:00:02.0
	  d0000000-d03fffff : 0000:00:02.0
	  d0400000-d04fffff : PCI Bus 0000:04
	    d0400000-d0400fff : 0000:04:00.0
	      d0400000-d0400fff : rtsx_pci
	  d0500000-d05fffff : PCI Bus 0000:03
	    d0500000-d053ffff : 0000:03:00.0
	    d0540000-d0543fff : 0000:03:00.1
	      d0540000-d0543fff : ICH HD audio
	    d0560000-d057ffff : 0000:03:00.0
	  d0600000-d06fffff : PCI Bus 0000:02
	    d0600000-d0601fff : 0000:02:00.0
	      d0600000-d0601fff : iwlwifi
	  d0700000-d07fffff : PCI Bus 0000:01
	  d0800000-d081ffff : 0000:00:19.0
	    d0800000-d081ffff : e1000e
	  d0820000-d082ffff : 0000:00:14.0
	    d0820000-d082ffff : xhci-hcd
	  d0830000-d0833fff : 0000:00:03.0
	    d0830000-d0833fff : ICH HD audio
	  d0834000-d0837fff : 0000:00:1b.0
	    d0834000-d0837fff : ICH HD audio
	  d0838000-d08380ff : 0000:00:1f.3
	  d0839000-d083901f : 0000:00:16.0
	    d0839000-d083901f : mei_me
	  d083c000-d083c7ff : 0000:00:1f.2
	    d083c000-d083c7ff : ahci
	  d083d000-d083d3ff : 0000:00:1d.0
	    d083d000-d083d3ff : ehci_hcd
	  d083e000-d083efff : 0000:00:19.0
	    d083e000-d083efff : e1000e
	  d083f000-d083ffff : 0000:00:16.3
	e0000000-efffffff : PCI MMCONFIG 0000 [bus 00-ff]
	  e00f8000-e00f8fff : Reserved
	f0000000-fed3ffff : PCI Bus 0000:00
	  fec00000-fec003ff : IOAPIC 0	<<=======	#define IOAPIC	0xFEC00000, default physical address of IO APIC
	  fed00000-fed003ff : HPET 0
	    fed00000-fed003ff : PNP0103:00
	  fed10000-fed17fff : pnp 00:00	<<=======	#define MCHBAR	0xFED10000, size 32K
	  fed18000-fed18fff : pnp 00:00
	  fed19000-fed19fff : pnp 00:00
	  fed1c000-fed1ffff : Reserved
	    fed1c000-fed1ffff : pnp 00:00
	      fed1f410-fed1f414 : iTCO_wdt.1.auto
	        fed1f410-fed1f414 : iTCO_wdt.1.auto iTCO_wdt.1.auto
	      fed1f800-fed1f9ff : intel-spi
	  fed20000-fed3ffff : pnp 00:00
	fed40000-fed44fff : 00:06 GTPM
	fed45000-fedfffff : PCI Bus 0000:00
	  fed45000-fed8ffff : pnp 00:00
	  fed90000-fed90fff : dmar0
	  fed91000-fed91fff : dmar1
	fee00000-fee00fff : Local APIC
	fee01000-ffffffff : PCI Bus 0000:00
	  ff000000-ffffffff : INT0800:00
	    ffdb0000-ffdcefff : Reserved
	100000000-34edfffff : System RAM
	  12c000000-12ce00d16 : Kernel code
	  12d000000-12d86afff : Kernel rodata
	  12da00000-12dc70a7f : Kernel data
	  12e379000-12e7fffff : Kernel bss
	34ee00000-34fffffff : RAM buffer

### mmap() usage (after all)!

Author suggests here user space exploration by using user space powerfull mmap() f-n!

Please, do note that mmap() discussion is beyond the scope of this GitHub repo.

Short recap:

https://man7.org/linux/man-pages/man2/mmap.2.html

	void *mmap(void *addr,
		   size_t length,
		   int prot,
		   int flags,
		   int fd,
		   off_t offset);

	int munmap(void *addr, size_t length);

Additional net pointers for better understanding:

https://www.cs.uaf.edu/2016/fall/cs301/lecture/11_02_mmap.html
