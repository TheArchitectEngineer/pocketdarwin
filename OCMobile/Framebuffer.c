// Framebuffer definition

#if defined("harpia")
	#include <harpia/framebuffer.h>
#if defined("generic")
	#include <genericintel/framebuffer.h>
	
	int framrbufferMain() {
		fprintf("[OpenCore] WARNING: Will not use framebuffer!\n");

		continueGenericIntelBoot();
	}
#endif



