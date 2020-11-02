/*
 * skull.c -- sample typeless module.
 * https://bootlin.com/doc/books/ldd3.pdf
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 * Copyright (C) 2020 Zoran Stojsavljevic
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini, Jonathan Corbet and Zoran
 * Stojsavljevic. No warranty is attached; no any one of authors
 * can take responsibility for errors or fitness for use.
 *
 * WARNING/DANGER: if someone does want to use this code as is
 * or change it, someone should definitely have very good
 * understanding what is dealing with/is up to!
 *
 * No warranty is attached; no any one of authors can be in any
 * way liable or/and take ANY responsibility for damages, bugs,
 * errors caused/done by this code or fitness for use.
 *
 * BUGS:
 *   -it only runs on intel platforms.
 *   -readb() should be used (see short.c).
 */

// #include <linux/config.h>
#include <linux/memblock.h>
#include <linux/module.h>
#include <linux/version.h> // Added from skull_clean.c
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/kernel.h> /* printk */
#include <linux/fs.h>
#include <linux/ioport.h> /* struct resource */
#include <linux/errno.h>
// #include <asm/system.h> /* cli(), *_flags */
#include <linux/mm.h> /* vremap (2.0) */
#include <asm/io.h> /* ioremap */

#include <linux/version.h>

#include <asm-generic/iomap.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("An advanced experimental Linux skull driver");

/* The region we look at - Do NOT Start below 0x0A0000 address! */
#define ISA_REGION_BEGIN 0x00A0000 // ISA REGION BEGIN
#define PCIE_0000_00_END 0x00C0000
#define ISA_EMPTY_BEGIN  0x00C0000
#define ISA_EMPTY_END    0x00F0000
#define SYSTEM_ROM_BEGIN 0x00F0000
#define ISA_REGION_END   0x0100000 // ISA REGION END/System ROM END
// #define SYSTEM_RAM_1M    0x0100000
// #define SYSTEM_RAM_2M    0x0200000

#define BASIC_STEP 0x10000

/* have three symbols to export */
	void skull_fn1(void){}
	void skull_fn2(void){}
	int  skull_variable;

EXPORT_SYMBOL (skull_fn1);
EXPORT_SYMBOL (skull_fn2);
EXPORT_SYMBOL (skull_variable);


/* Perform hardware autodetection - TBD */
int skull_probe_hw(unsigned int port, unsigned int range)
{
	/* Do smart probing here - not yet determined */
	return -1; /* not found :-) */
}

/* Perform hardware initalizazion - TBD */
int skull_init_board(unsigned int port)
{
	/* Do smart initalization here - not yet determined */
	return 0; /* done :-) */
}

/* Detect the the device if the region is still free */
static int skull_detect(unsigned int port, unsigned int range)
{
	/*
	 * Obsolete code
	 *
	 * int err;
	 *
	 * if ((err = request_region (port, range, "COM2")) < 0)
	 *	return err; // busy
	 */

	if (skull_probe_hw(port, range) != 0)
		return -ENODEV; /* not found */
	request_region(port, range, "COM2"); /* "Can't fail" */

	return 0;
}

/*
 * Port ranges: the device can reside between 0x280
 * and 0x300, in step of 0x10. It uses 0x10 ports.
 */
#define SKULL_PORT_FLOOR 0x280
#define SKULL_PORT_CEIL  0x300
#define SKULL_PORT_RANGE 0x010

/*
 * The following function performs autodetection, unless a specific
 * value was assigned by insmod to "skull_port_base"
 */

static int skull_port_base = 0; /* 0 forces autodetection */
module_param(skull_port_base, int, 0);

static int skull_find_hw(void) /* returns the # of devices */
{
	/* Base is either the load-time value or the first trial */
	int base = skull_port_base ? skull_port_base : SKULL_PORT_FLOOR;
	int result = 0;

	/* Loop one time if value assigned, try them all if autodetecting */
	do {
		if (skull_detect(base, SKULL_PORT_RANGE) == 0) {
			skull_init_board(base);
			result++;
		}
		printk(KERN_INFO "result: 0x%x, base: 0x%x\n", result, base);
		base += SKULL_PORT_RANGE; /* prepare for next trial */
	}
	while (skull_port_base == 0 && base < SKULL_PORT_CEIL);

	return result;
}

static unsigned long local_ioremap(
				unsigned long begin,
				unsigned long end,
				unsigned long step,
				char *name,
				struct resource *local_resource)
{
	unsigned long add;
	unsigned char oldval, newval; /* Values read from memory   */
	unsigned long flags;          /* Used to hold system flags */
	unsigned long i;
	volatile void __iomem *base;

	/*
	 * Use ioremap to get a handle on the ISA region
	 * void __iomem *ioremap(resource_size_t phys_addr, unsigned long size);
	 */
	base = ioremap(begin, end - begin);
	if(NULL == base) {
		printk(KERN_INFO "--- [NULL pointer] ioremap addr = %p NULL ---\n", base);
		return begin;
	}

	base -= begin;  /* Do the offset once */

	/* Probe potential memory holes in step chunks */
	for (add = begin; add < end; add += step) {
		/*
		 * Function check_mem_region(start,n) Was removed from the kernel APIs from 4.1 onwards
		 * #define check_mem_region(start,n)   __check_region(&iomem_resource, (start), (n))
		 *
		 * Replaced by the function request_region(start,n,name)
		 * #define request_region(start,n,name)   __request_region(&ioport_resource, (start), (n), (name), 0)
		 */

		/*
		 *  Check for an already allocated region
		 * if (check_mem_region(add, 2048)) {
		 */
		local_resource = request_region(add, step, name);
		if (NULL != local_resource) {
			printk(KERN_INFO "0x%lx: Region Allocated\n", add);
			printk(KERN_INFO "local_resource->start 0x%llx\n", local_resource->start);
			printk(KERN_INFO "local_resource->end 0x%llx\n", local_resource->end);
			printk(KERN_INFO "local_resource->name %s\n", local_resource->name);
			printk(KERN_INFO "local_resource->flags 0x%lx\n", local_resource->flags);
			printk(KERN_INFO "local_resource->desc 0x%lx\n", local_resource->desc);
			printk(KERN_INFO "local_resource->parent %p\n", local_resource->parent);
			printk(KERN_INFO "local_resource->sigling %p\n", local_resource->sibling);
			printk(KERN_INFO "local_resource->child %p\n", local_resource->child);
			return 0;
		}

		/* Read and write the beginning of the region and see what happens */

		/*
		 * Old kernel code, prior to 3.x
		 * save_flags(flags);
		 * cli() disables all irqs on IOAPIC (globally)
		 * cli() replaced by local_irq_save(flags)
		 *
		 * The following code must run irq un-preemptable within the current HW thread
		 */
		local_irq_save(flags); // Disables for a bit all irqs on a current HW thread's LAPIC

		oldval = readb (base + add);  /* Read a byte */
		writeb (oldval^0xff, base + add);
		mb();
		newval = readb (base + add);
		writeb (oldval, base + add);

		/*
		 * Old kernel code, prior to 3.x
		 * restore_flags(flags) replaced by local_irq_restore(flags)
		 */
		local_irq_restore(flags); // Enables all irqs on a current HW thread's LAPIC

		/* From now on current HW thread could be preempted by irqs delivered from IOAPIC */
		if (0xff == (oldval^newval)) {  /* Re-read the change: it's ram */
			printk(KERN_INFO "0x%lx: RAM\n", add);
			printk(KERN_INFO "--- ioremap addr = 0x%lx ---\n", (unsigned long)(base + add));
			continue;
		}

		if (0 != (oldval^newval)) {  /* Random bits changed: it's empty */
			printk(KERN_INFO "0x%lx: EMPTY\n", add);
			printk(KERN_INFO "--- ioremap addr = 0x%lx ---\n", (unsigned long)(base + add));
			continue;
		}

		/*
		 * Expansion rom (executed at boot time by the bios) has a
		 * signature where the first byte is 0x55, the second 0xaa,
		 * and the third byte indicates the size of such rom
		 */
		if ((oldval == 0x55) && (readb (base + add + 1) == 0xaa)) {
			int size = 512 * readb (base + add + 2);
			printk(KERN_INFO "0x%lx: Expansion ROM, %i bytes\n", add, size);
			printk(KERN_INFO "--- ioremap addr = 0x%lx ---\n", (unsigned long)(base + add));
			add += (size & ~step) - step; /* skip it */
			continue;
		}

		/*
		 * If the tests above failed, we still don't know if it is ROM or
		 * empty. Since empty memory can appear as 0x00, 0xff, or the low
		 * address byte, we must probe multiple bytes: if at least one of
		 * them is different from these three values, then this is rom
		 * (though not boot rom).
		 */
		for (i=0; i<5; i++) {
			unsigned long radd = add + 57*(i+1);  /* a "random" value */
			unsigned char val = readb (base + radd);
			if (val && val != 0xff && val != ((unsigned long)radd & 0xff))
			break;
		}
		printk(KERN_INFO "0x%lx: %s\n", add, i==5 ? "EMPTY" : "ROM");
		printk(KERN_INFO "--- ioremap addr = 0x%lx ---\n", (unsigned long)(base + add));
	}

	/* iounmap the region */
	iounmap(base + begin);

	return 0;
}

static int __init skull_init(void)
{
	/*
	 * Print the isa region map, in blocks of step bytes. This
	 * is still not the best code, but it prints fewer lines,
	 * it deserves to remain short to be included in the book.
	 * Note also that read() should be used instead of pointers
	 */
	struct resource *local_resource;
	unsigned long   phy_mem, i;

	printk(KERN_INFO "Insert the skull loadable module\n");

	local_resource = (struct resource *)vmalloc(sizeof(struct resource));

	// 000a0000-000bffff : PCI Bus 0000:00
	printk(KERN_INFO "--- range 0x000a0000 - 0x000bffff ---\n");
	phy_mem = local_ioremap(ISA_REGION_BEGIN, PCIE_0000_00_END, BASIC_STEP, "PCI Bus 0000:00", local_resource);
	if (phy_mem)
		printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);

	// 000c0000-000effff - gap according to the /proc/iomem mapping
	printk(KERN_INFO "--- range 0x000c0000 - 0x000effff ---\n");
	phy_mem = local_ioremap(ISA_EMPTY_BEGIN, ISA_EMPTY_END, BASIC_STEP, "System RAM", local_resource);
	if (phy_mem)
		printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);

	// 000f0000-000fffff : System ROM
	printk(KERN_INFO "--- range 0x000f0000 - 0x000fffff ---\n");
	phy_mem = local_ioremap(SYSTEM_ROM_BEGIN, ISA_REGION_END, BASIC_STEP, "System ROM", local_resource);
	if (phy_mem)
		printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);

#if 0
	// 10000000-e0000000 : ???
	printk(KERN_INFO "--- range 0x10000000 - 0xe0000000 ---\n");
	for (i = 1; i < 14; i++) {
		phy_mem = local_ioremap(i * 0x10000000, (i + 1)*0x10000000, 0x1000000, "System RAM", local_resource);
		if (phy_mem)
			printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);
	}
#endif

	// e0000000-e01fffff : PCI MMCONFIG 0000 [bus 00-ff]
	printk(KERN_INFO "--- range 0xe0000000-0xe01fffff ---\n");
	phy_mem = local_ioremap(0xe0000000, 0xe0200000, BASIC_STEP, "System RAM", local_resource);
	if (phy_mem)
		printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);

	// feb00000-febfffff
	printk(KERN_INFO "--- range 0xfeb00000 - 0xfebfffff ---\n");
	phy_mem = local_ioremap(0xfeb00000, 0xfec00000, BASIC_STEP, "System RAM", local_resource);
	if (phy_mem)
		printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);

	// fec00000-fec003ff : IOAPIC 0 <<== #define IOAPIC 0xFEC00000 - default physical address of IOAPIC
	printk(KERN_INFO "--- range 0xfec00000 - 0xfec003ff --- #define IOAPIC 0xFEC00000\n");
	phy_mem = local_ioremap(0xfec000000, 0xfec003ff, 0x100, "System RAM", local_resource);
	if (phy_mem)
		printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);

	// fed10000-fed17fff : MCHBAR <<== #define MCHBAR 0xFED10000 - default physical address of MCHBAR
	printk(KERN_INFO "--- range 0xfed10000 - 0xfed17fff --- #define MCHBAR 0xFED10000\n");
	phy_mem = local_ioremap(0xfed10000, 0xfed18000, 0x1000, "System RAM", local_resource);
	if (phy_mem)
		printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);

	/*
	 * DANGER: Do NOT test/touch this area!
	 * fed3ffff-ffffffff INTEL System Space Mem Controller, IOAPIC etc.
	 * phy_mem = local_ioremap(0xfed3ffff, 0x100000000, BASIC_STEP, "System RAM", local_resource);
	 * if (phy_mem)
	 *	printk(KERN_INFO "--- phy_mem bus fault @ 0x%lx ---\n", phy_mem);
	 */

	/* free vmalloc() area */
	vfree(local_resource);

	/* Find the hardware */
	skull_find_hw();

	/* Always fail to load (or suceed) */
	return 0;
}

void skull_release(unsigned int port, unsigned int range)
{
	release_region(port, range);
}

static void __exit skull_cleanup(void)
{
	/* Should put real values here ... */
	/* Skull_release(0,0); */
	printk(KERN_INFO "Release/remove the skull loadable module\n");
}

module_init(skull_init);
module_exit(skull_cleanup);
