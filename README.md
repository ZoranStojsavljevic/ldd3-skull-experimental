### ldd3-skull-revisited

Here, the original skull driver by Alessandro Rubini and Jonathan Corbet (2001) is given.

Also the revisited version by the author:

	skull_genesis	==>> Alessandro Rubini and Jonathan Corbet (2001)
	skull_revisited	==>> Zoran Stojsavljevic (2020)

It is just an attempt to make this code alive, since from Year 2011 this code was obsolete,
due to the numerous Linux kernel API changes.

After 19 years many things have been changed, as well as some x86_64 HW platforms' definitions.

But the basic 1MB limit of PC XT (invented in 1981) stayed forever (well...). And the ISA region
stayed as well since the 80286 intro.

First 640KB + ISA 384KB (in total 1MB) architecture of the physical memory of x86_64 (since
INTEL 80286 silicon intro) seems carved in stone even these days, for the modern Giga Gate
silicons, inevitable.

Any suggestions, addendums, code improvement, or another functional suggestion of the presented
code are very welcome.

Author
