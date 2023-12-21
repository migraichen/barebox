#include <init.h>
#include <clock.h>
#include <common.h>
#include <io.h>

static int x86_pci_init(void)
{
	struct device_d *dev;
	dev = device_alloc("x86-pci", DEVICE_ID_SINGLE);
	return platform_device_register(dev);
}
coredevice_initcall(x86_pci_init);
