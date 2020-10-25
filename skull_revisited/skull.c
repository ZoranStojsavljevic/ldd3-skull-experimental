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
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates. No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * BUGS:
 *   -it only runs on intel platforms.
 *   -readb() should be used (see short.c): skull doesn't work with 2.1
 */

/* jc: cleaned up, but not yet run for anything */

// #include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h> // Added from skull_clean.c
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/kernel.h> /* printk */
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/errno.h>
// #include <asm/system.h> /* cli(), *_flags ==>> obsolete dir/file.h */
#include <linux/mm.h> /* vremap (2.0) */
#include <asm/io.h> /* ioremap */

#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A very simple Linux skull driver - revisited");

/* The region we look at - Do NOT BEGIN below 0x0A0000 address! */
#define ISA_REGION_BEGIN 0x0A0000
#define ISA_REGION_END   0x100000
#define STEP 0x10000

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
//	int err;

//	if ((err = request_region (port, range, "COM2")) < 0)
//		return err; /* busy */
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

static int skull_port_base=0; /* 0 forces autodetection */
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
		printk(KERN_INFO "result: %x, base: %x\n", result, base);
		base += SKULL_PORT_RANGE; /* prepare for next trial */
	}
	while (skull_port_base == 0 && base < SKULL_PORT_CEIL);

	return result;
}

static int __init skull_init(void)
{
	/*
	 * Print the isa region map, in blocks of STEP bytes. This
	 * is still not the best code, but it prints fewer lines,
	 * it deserves to remain short to be included in the book.
	 * Note also that read() should be used instead of pointers
	 */
	unsigned char oldval, newval; /* Values read from memory   */
	unsigned long flags;          /* Used to hold system flags */
	unsigned long add, i;
	void *base;

	printk(KERN_INFO "Insert the skull loadable module\n");

	/* Use ioremap to get a handle on the ISA region */
	base = ioremap(ISA_REGION_BEGIN, ISA_REGION_END - ISA_REGION_BEGIN);
	base -= ISA_REGION_BEGIN;  /* Do the offset once */

	/* Probe all the memory hole in STEP steps */
	for (add = ISA_REGION_BEGIN; add < ISA_REGION_END; add += STEP) {

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
		if (request_region(add, STEP, "skull")) {
			printk(KERN_INFO "%lx: Allocated\n", add);
			continue;
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
		if ((oldval^newval) == 0xff) {  /* Re-read the change: it's ram */
			printk(KERN_INFO "%lx: RAM\n", add);
			continue;
		}
		if ((oldval^newval) != 0) {  /* Random bits changed: it's empty */
			printk(KERN_INFO "%lx: empty\n", add);
			continue;
		}

		/*
		 * Expansion rom (executed at boot time by the bios) has a
		 * signature where the first byte is 0x55, the second 0xaa,
		 * and the third byte indicates the size of such rom
		 */
		if ( (oldval == 0x55) && (readb (base + add + 1) == 0xaa)) {
			int size = 512 * readb (base + add + 2);
			printk(KERN_INFO "%lx: Expansion ROM, %i bytes\n", add, size);
			add += (size & ~STEP) - STEP; /* skip it */
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
		printk(KERN_INFO "%lx: %s\n", add, i==5 ? "empty" : "ROM");
	}

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
