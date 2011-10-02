/*
 *	Status: Up-to-date
 *
 *	Linux PCI Utilities -- List All PCI Devices
 *
 *	Copyright (c) 1997--1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "pciutils.h"
#include "../syslib.h"

static int verbose;			      /* Show detailed information */
static int show_hex;			  /* Show contents  as hexadecimal numbers */
static struct pci_filter filter;  /* Device filter */
static struct pci_access *pacc;
static int error = 0;

#ifdef ARCH_SPARC64
#define IRQ_FORMAT "%08x"
#else
#define IRQ_FORMAT "%d"
#endif

#ifdef HAVE_64BIT_ADDRESS
#ifdef HAVE_LONG_ADDRESS
#define ADDR_FORMAT "%016Lx"
#else
#define ADDR_FORMAT "%016lx"
#endif
#else
#define ADDR_FORMAT "%08lx"
#endif

#ifdef ARCH_SPARC64
#define IO_FORMAT "%016Lx"
#elif defined(HAVE_LONG_ADDRESS)
#define IO_FORMAT "%04Lx"
#else
#define IO_FORMAT "%04lx"
#endif

#ifndef __GNUC__
#define alloca malloc
#endif

#define FLAG(x,y) ((x & y) ? '+' : '-')


//=============================
// Structures...
//-----------------------------
struct device {
  struct device *next;
  struct pci_dev *dev;
  unsigned int config_cnt;
  byte config[256];
};

struct bridge {
  struct bridge *chain;         /* Single-linked list of bridges */
  struct bridge *next, *child;      /* Tree of bridges */
  struct bus *first_bus;        /* List of busses connected to this bridge */
  unsigned int primary, secondary, subordinate; /* Bus numbers */
  struct device *br_dev;
};

struct bus {
  unsigned int number;
  struct bus *sibling;
  struct device *first_dev, **last_dev;
};

//=============================
// Globals...
//-----------------------------
static struct device *first_dev;


//=============================
// Functions...
//-----------------------------
static struct device * scan_device(struct pci_dev *p) {
 int how_much = (show_hex > 2) ? 256 : 64;
 struct device *d = NULL;

 if (!pci_filter_match(&filter, p)) {
  return NULL;
 }
 d = (struct device *)xmalloc(sizeof(struct device));
 bzero(d, sizeof(*d));
 d->dev = p;
 if (!pci_read_block(p, 0, d->config, how_much)) {
  //die("Unable to read %d bytes of configuration space.", how_much);
  error++; return d;
 }
 if (how_much < 128 && (d->config[PCI_HEADER_TYPE] & 0x7f) == 
      PCI_HEADER_TYPE_CARDBUS) {
  if (!pci_read_block(p, 64, d->config+64, 64)) {
   //die("Unable to read cardbus bridge extension data.");
  } else {
   how_much = 128;
  }
 }
 d->config_cnt = how_much;
 pci_setup_cache(p, d->config, d->config_cnt);
 pci_fill_info(p, 
  PCI_FILL_IDENT    | 
  PCI_FILL_IRQ      | 
  PCI_FILL_BASES    | 
  PCI_FILL_ROM_BASE | 
  PCI_FILL_SIZES
 );
 return d;
}

static void scan_devices(void) {
 struct device *d;
 struct pci_dev *p;

 pci_scan_bus(pacc);
 for(p=pacc->devices; p; p=p->next) {
  if (d = scan_device(p)) {
   d->next = first_dev;
   first_dev = d;
  }
 }
}

static inline byte get_conf_byte(struct device *d, unsigned int pos) {
 return d->config[pos];
}

static word get_conf_word(struct device *d, unsigned int pos) {
 return d->config[pos] | (d->config[pos+1] << 8);
}

static int compare_them(const void *A, const void *B) {
 const struct pci_dev *a = (*(const struct device **)A)->dev;
 const struct pci_dev *b = (*(const struct device **)B)->dev;

 if (a->bus < b->bus)
   return -1;
 if (a->bus > b->bus)
   return 1;
 if (a->dev < b->dev)
   return -1;
 if (a->dev > b->dev)
   return 1;
 if (a->func < b->func)
   return -1;
 if (a->func > b->func)
   return 1;
 return 0;
}

static void sort_them(void) {
 struct device **index, **h, **last_dev;
 int cnt;
 struct device *d;

 cnt = 0;
 for(d=first_dev; d; d=d->next)
   cnt++;
 h = index = (struct device **)alloca(sizeof(struct device *) * cnt);
 for(d=first_dev; d; d=d->next)
   *h++ = d;
 qsort(index, cnt, sizeof(struct device *), compare_them);
 last_dev = &first_dev;
 h = index;
 while (cnt--) {
  *last_dev = *h;
  last_dev = &(*h)->next;
  h++;
 }
 *last_dev = NULL;
}

static struct bus * find_bus(struct bridge *b, unsigned int n) {
 struct bus *bus;

 for(bus=b->first_bus; bus; bus=bus->sibling)
  if (bus->number == n)
   break;
 return bus;
}

static struct bus * new_bus(struct bridge *b, unsigned int n) {
 struct bus *bus = (struct bus *)xmalloc(sizeof(struct bus));

 bus = (struct bus *)xmalloc(sizeof(struct bus));
 bus->number = n;
 bus->sibling = b->first_bus;
 bus->first_dev = NULL;
 bus->last_dev = &bus->first_dev;
 b->first_bus = bus;
 return bus;
}

static void insert_dev(struct device *d, struct bridge *b) {
 struct pci_dev *p = d->dev;
 struct bus *bus;

 if (! (bus = find_bus(b, p->bus))) {
  struct bridge *c;
 for(c=b->child; c; c=c->next)
  if (c->secondary <= p->bus && p->bus <= c->subordinate) {
   insert_dev(d, c);
   return;
  }
  bus = new_bus(b, p->bus);
 }
 *bus->last_dev = d;
 bus->last_dev = &d->next;
 d->next = NULL;
}

PciData* PciCollectData (struct device *d) {
 PciData* data = NULL;
 struct pci_dev *p = d->dev;
 word cls   = get_conf_word(d, PCI_CLASS_DEVICE);
 byte htype = get_conf_byte(d, PCI_HEADER_TYPE) & 0x7f;
 byte max_lat, min_gnt;
 byte int_pin = get_conf_byte(d, PCI_INTERRUPT_PIN);
 unsigned int irq = p->irq;
 word subsys_v = 0;
 word subsys_d = 0;
 
 data = (PciData*)malloc(sizeof(PciData));
 data->next = NULL;

 data->domain = p->domain;
 data->bus    = p->bus;
 data->slot   = p->dev;
 data->func   = p->func;
 data->vid    = p->vendor_id;
 data->did    = p->device_id;
 data->cls    = get_conf_word(d, PCI_CLASS_DEVICE);
 
 switch (htype) {
  case PCI_HEADER_TYPE_NORMAL:
   if (cls == PCI_CLASS_BRIDGE_PCI)
    // Invalid class %04x for header type %02x\n", class, htype
   max_lat = get_conf_byte(d, PCI_MAX_LAT);
   min_gnt = get_conf_byte(d, PCI_MIN_GNT);
   subsys_v = get_conf_word(d, PCI_SUBSYSTEM_VENDOR_ID);
   subsys_d = get_conf_word(d, PCI_SUBSYSTEM_ID);
   break;
  case PCI_HEADER_TYPE_BRIDGE:
   if (cls != PCI_CLASS_BRIDGE_PCI)
    // Invalid class %04x for header type %02x\n", class, htype
   irq = int_pin = min_gnt = max_lat = 0;
   subsys_v = subsys_d = 0;
   break;
  case PCI_HEADER_TYPE_CARDBUS:
   if ((cls >> 8) != PCI_BASE_CLASS_BRIDGE)
    // Invalid class %04x for header type %02x\n", class, htype
   min_gnt = max_lat = 0;
   subsys_v = get_conf_word(d, PCI_CB_SUBSYSTEM_VENDOR_ID);
   subsys_d = get_conf_word(d, PCI_CB_SUBSYSTEM_ID);
   break;
  default:
    // unknown header
    data->svid = 0;
    data->sdid = 0;
 }
 
 if (subsys_v && subsys_v != 0xffff) {
  data->svid = subsys_v;
  data->sdid = subsys_d;
 }
 return(data);
}

PciData* PciGetData(void) {
 struct device  *d;
 PciData *p;
 PciData *first = NULL;
 PciData *last  = NULL;

 pacc = pci_alloc();
 pacc->error = die;
 pci_filter_init(pacc, &filter);

 pacc->numeric_ids = 1;
 pacc->numeric_ids = 1;
 verbose++;

 pci_init(pacc);
 scan_devices();
 sort_them();

 for(d=first_dev; d; d=d->next) {
  p = PciCollectData(d);
  if (last != NULL) 
   last->next = p;
  if (d == first_dev)
   first = p;
  last = p;
 }
 /* we can't clean this up as we use the return data
 pci_cleanup(pacc);
 */
 pacc = 0;
 return(first);
}

