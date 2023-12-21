#include <common.h>
#include <linux/pci.h>
#include <io.h>
#include <module.h>

#define PCI_SLOT(devfn)		(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)		((devfn) & 0x07)

#define  PCIE_CONF_ADDR_EN		BIT(31)
#define  PCIE_CONF_REG(r)		((((r) & 0xf00) << 16) | ((r) & 0xfc))
#define  PCIE_CONF_BUS(b)		(((b) & 0xff) << 16)
#define  PCIE_CONF_DEV(d)		(((d) & 0x1f) << 11)
#define  PCIE_CONF_FUNC(f)		(((f) & 0x7) << 8)

#define  PCIE_CONF_ADDR(bus, devfn, where) \
	(PCIE_CONF_BUS(bus) | PCIE_CONF_DEV(PCI_SLOT(devfn))    | \
	 PCIE_CONF_FUNC(PCI_FUNC(devfn)) | PCIE_CONF_REG(where) | \
	 PCIE_CONF_ADDR_EN)

#define to_pci(_hc)	container_of(_hc, struct x86_pci_priv, pci)

struct x86_pci_priv {
	struct pci_controller pci;
};

static int x86_pci_rd_conf(struct pci_bus *bus, u32 devfn, int where, int size, u32 *val)
{
	__asm__ __volatile__ ("outl %0, %w1" : : "a" PCIE_CONF_ADDR(bus->number, devfn, where), "Nd" (0xcf8));
	__asm__ __volatile__ ("inl %w1, %0" : "=a"(*val) : "Nd" (0xcfc));

	if (size == 1)
		*val = (*val >> (8 * (where & 3))) & 0xff;
	else if (size == 2)
		*val = (*val >> (8 * (where & 3))) & 0xffff;

	return PCIBIOS_SUCCESSFUL;
}

static int x86_pci_wr_conf(struct pci_bus *bus, u32 devfn, int where, int size, u32 val)
{
	u32 _val, shift = 8 * (where & 3);

	if (size == 4)
		_val = val;
	else if (size == 2)
		_val = (_val & ~(0xffff << shift)) | ((val & 0xffff) << shift);
	else if (size == 1)
		_val = (_val & ~(0xff << shift)) | ((val & 0xff) << shift);
	else
		return PCIBIOS_BAD_REGISTER_NUMBER;

	__asm__ __volatile__ ("outl %0, %w1" : : "a" (_val), "Nd" (0xcf8));

	return PCIBIOS_SUCCESSFUL;
}

static const struct pci_ops x86_pci_ops = {
	.read = x86_pci_rd_conf,
	.write = x86_pci_wr_conf,
};

static int x86_pci_probe(struct device_d *dev)
{
	struct x86_pci_priv *priv;

	priv = xzalloc(sizeof(*priv));

	priv->pci.io_resource = request_ioport_region("x86-pci", 0xcf8, 0xcff);

	priv->pci.parent = dev;
	priv->pci.pci_ops = &x86_pci_ops;

	register_pci_controller(&priv->pci);

	return 0;
}

static struct driver_d x86_pci_driver = {
	.name = "x86-pci",
	.probe = x86_pci_probe,
};
device_platform_driver(x86_pci_driver);

