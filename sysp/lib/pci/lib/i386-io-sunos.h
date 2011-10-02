/*
 *	The PCI Library -- Access to i386 I/O ports on Solaris
 *
 *	Copyright (c) 2003 Bill Moore <billm@eng.sun.com>
 *	Copyright (c) 2003 Martin Mares <mj@ucw.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include <sys/sysi86.h>
#include <sys/psw.h>

static int intel_iopl_set = -1;

static int
intel_setup_io(void)
{
  if (intel_iopl_set < 0)
    {
      if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) < 0)
	intel_iopl_set = 0;
      else
	intel_iopl_set = 1;
    }
  return intel_iopl_set;
}

static inline void
intel_cleanup_io(void)
{
  if (intel_iopl_set > 0)
    {
      /* FIXME: How to switch off I/O port access? */
    }
  intel_iopl_set = -1;
}

static inline u8
inb (u16 port)
{
  u8 v;
  __asm__ __volatile__ ("inb (%w1)":"=a" (v):"Nd" (port));
  return v;
}

static inline u16
inw (u16 port)
{
  u16 v;
  __asm__ __volatile__ ("inw (%w1)":"=a" (v):"Nd" (port));
  return v;
}

static inline u32
inl (u16 port)
{
  u32 v;
  __asm__ __volatile__ ("inl (%w1)":"=a" (v):"Nd" (port));
  return v;
}

static inline void
outb (u8 value, u16 port)
{
  __asm__ __volatile__ ("outb (%w1)": :"a" (value), "Nd" (port));
}

static inline void
outw (u16 value, u16 port)
{
  __asm__ __volatile__ ("outw (%w1)": :"a" (value), "Nd" (port));
}

static inline void
outl (u32 value, u16 port)
{
  __asm__ __volatile__ ("outl (%w1)": :"a" (value), "Nd" (port));
}
