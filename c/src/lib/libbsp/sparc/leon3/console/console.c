/*
 *  This file contains the TTY driver for the serial ports on the LEON.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <amba.h>

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
int syscon_uart_index __attribute__((weak)) = 0;

/*
 *  Should we use a polled or interrupt drived console?
 *
 *  NOTE: This is defined in the custom/leon.cfg file.
 */

/*
 *  apbuart_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

extern void apbuart_outbyte_polled(
  ambapp_apb_uart *regs,
  char ch
);

/* body is in debugputs.c */

/*
 *  apbuart_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

extern int apbuart_inbyte_nonblocking(ambapp_apb_uart *regs);

/* body is in debugputs.c */

struct apbuart_priv {
  ambapp_apb_uart *regs;
  unsigned int freq_hz;
};
static struct apbuart_priv apbuarts[BSP_NUMBER_OF_TERMIOS_PORTS];
static int uarts = 0;

/*
 *  Console Termios Support Entry Points
 *
 */

ssize_t console_write_support (int minor, const char *buf, size_t len)
{
  int nwrite = 0, port;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  while (nwrite < len) {
    apbuart_outbyte_polled(apbuarts[port].regs, *buf++);
    nwrite++;
  }
  return nwrite;
}

int console_pollRead(int minor)
{
  int port;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  return apbuart_inbyte_nonblocking(apbuarts[port].regs);
}

int console_set_attributes(int minor, const struct termios *t)
{
  unsigned int scaler;
  unsigned int ctrl;
  int baud;
  struct apbuart_priv *uart;

  switch(t->c_cflag & CSIZE) {
    default:
    case CS5:
    case CS6:
    case CS7:
      /* Hardware doesn't support other than CS8 */
      return -1;
    case CS8:
      break;
  }

  if (minor == 0)
    uart = &apbuarts[syscon_uart_index];
  else
    uart = &apbuarts[minor - 1];

  /* Read out current value */
  ctrl = uart->regs->ctrl;

  switch(t->c_cflag & (PARENB|PARODD)){
    case (PARENB|PARODD):
      /* Odd parity */
      ctrl |= LEON_REG_UART_CTRL_PE|LEON_REG_UART_CTRL_PS;
      break;

    case PARENB:
      /* Even parity */
      ctrl &= ~LEON_REG_UART_CTRL_PS;
      ctrl |= LEON_REG_UART_CTRL_PE;
      break;

    default:
    case 0:
    case PARODD:
      /* No Parity */
      ctrl &= ~(LEON_REG_UART_CTRL_PS|LEON_REG_UART_CTRL_PE);
  }

  if ( !(t->c_cflag & CLOCAL) ){
    ctrl |= LEON_REG_UART_CTRL_FL;
  }else{
    ctrl &= ~LEON_REG_UART_CTRL_FL;
  }

  /* Update new settings */
  uart->regs->ctrl = ctrl;

  /* Baud rate */
  switch(t->c_cflag & CBAUD){
    default:      baud = -1;      break;
    case B50:     baud = 50;      break;
    case B75:     baud = 75;      break;
    case B110:    baud = 110;     break;
    case B134:    baud = 134;     break;
    case B150:    baud = 150;     break;
    case B200:    baud = 200;     break;
    case B300:    baud = 300;     break;
    case B600:    baud = 600;     break;
    case B1200:   baud = 1200;    break;
    case B1800:   baud = 1800;    break;
    case B2400:   baud = 2400;    break;
    case B4800:   baud = 4800;    break;
    case B9600:   baud = 9600;    break;
    case B19200:  baud = 19200;   break;
    case B38400:  baud = 38400;   break;
    case B57600:  baud = 57600;   break;
    case B115200: baud = 115200;  break;
    case B230400: baud = 230400;  break;
    case B460800: baud = 460800;  break;
  }

  if ( baud > 0 ){
    /* Calculate Baud rate generator "scaler" number */
    scaler = (((uart->freq_hz * 10)/(baud * 8)) - 5) / 10;

    /* Set new baud rate by setting scaler */
    uart->regs->scaler = scaler;
  }

  return 0;
}

/* AMBA PP find routine. Extract AMBA PnP information into data structure. */
int find_matching_apbuart(struct ambapp_dev *dev, int index, void *arg)
{
  struct ambapp_apb_info *apb = (struct ambapp_apb_info *)dev->devinfo;

  /* Extract needed information of one APBUART */
  apbuarts[uarts].regs = (ambapp_apb_uart *)apb->start;
  /* Get APBUART core frequency, it is assumed that it is the same
   * as Bus frequency where the UART is situated
   */
  apbuarts[uarts].freq_hz = ambapp_freq_get(&ambapp_plb, dev);
  uarts++;

  if (uarts >= BSP_NUMBER_OF_TERMIOS_PORTS)
    return 1; /* Satisfied number of UARTs, stop search */
  else
    return 0; /* Continue searching for more UARTs */
}

/* Find all UARTs */
int console_scan_uarts(void)
{
  memset(apbuarts, 0, sizeof(apbuarts));

  /* Find APBUART cores */
  ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS), VENDOR_GAISLER,
                  GAISLER_APBUART, find_matching_apbuart, NULL);

  return uarts;
}

/*
 *  Console Device Driver Entry Points
 *
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  int i;
  char console_name[16];

  rtems_termios_initialize();

  /* Find UARTs */
  console_scan_uarts();

  /* Update syscon_uart_index to index used as /dev/console
   * Let user select System console by setting syscon_uart_index. If the
   * BSP is to provide the default UART (syscon_uart_index==0):
   *   non-MP: APBUART[0] is system console
   *   MP: LEON CPU index select UART
   */
  if (syscon_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
    syscon_uart_index = LEON3_Cpu_Index;
#else
    syscon_uart_index = 0;
#endif
  } else {
    syscon_uart_index = syscon_uart_index - 1; /* User selected sys-console */
  }

  /*  Register Device Names
   *
   *  0 /dev/console   - APBUART[USER-SELECTED, DEFAULT=APBUART[0]]
   *  1 /dev/console_a - APBUART[0] (by default not present because is console)
   *  2 /dev/console_b - APBUART[1]
   *  ...
   *
   * On a MP system one should not open UARTs that other OS instances use.
   */
  if (syscon_uart_index < uarts) {
    status = rtems_io_register_name( "/dev/console", major, 0 );
    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
  }
  strcpy(console_name,"/dev/console_a");
  for (i = 0; i < uarts; i++) {
    if (i == syscon_uart_index)
      continue; /* skip UART that is registered as /dev/console */
    console_name[13] = 'a' + i;
    status = rtems_io_register_name( console_name, major, i+1);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  int port;

  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    console_pollRead,            /* pollRead */
    console_write_support,       /* write */
    console_set_attributes,      /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };

  assert(minor <= uarts);
  if (minor > uarts || minor == (syscon_uart_index + 1))
    return RTEMS_INVALID_NUMBER;

  sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  /* Initialize UART on opening */
  apbuarts[port]->regs->ctrl |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE;
  apbuarts[port]->regs->status = 0;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}

