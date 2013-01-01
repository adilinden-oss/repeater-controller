/*
    irlpdev.c : This code retires the R/W operations through the
    /dev/irlp-port device in favour    of ioctl() on /dev/parport0 .

    To catch situations where IRLP-Nodes are still configured to
    load the irlp-port driver which prevents parport0 from working,
    the legacy irlp-port is tried if accessing parport0 fails.
    
    2004-03-27, DL2KCD: Creation.
    2004-04-06, DL2KCD: Added fallback to legacy device.
    2004-04-17, DL2KCD: Added fcntl() locking to work around Linux kernel bug.
    2013-01-01, VA3ADI: Removed legacy irlp-port
*/

/*
   A note of frustration about Linux, a popular imitation (not more) of Unix:
   Linux offers direct access to the parallel port data and status lines
   using ioctl() on /dev/parport0. However, the designers of Linux found
   it to be a good idea to force us to use a peculiar locking mechanism
   whenever using this device - the device has to be claimed first, and
   no other process can use it until it is released. But the claim/release
   mechanism contains a race condition, causing the error message
   "parport0: don't know how to wake ppdev0" do be displayed when the
   system is heavily exercised. Look at the function parport_claim() in
   /usr/src/linux-2.4/drivers/parport/share.c, and read the comments there
   about locking to be necessary but not done.

   As a workaround, we use fcntl() on a lockfile in /tmp to make sure that
   never more than one process attempts to claim the device at the same time.
   We hope their fcntl() is not broken as well...

   Why on earth is Linux so much popular than FreeBSD? Sigh... (DL2KCD)
*/

#include "irlpdev.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/ppdev.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <stdlib.h>

static int fd = -1;
#define LOCKFILE "/tmp/irlp-lockfile-parport0"
static int lockfilefd = -1;

int ppclaim() {
    if( fd < 0 )         // device must be open to claim it
        return -1;
    if( lockfilefd < 0 ) { // open lockfile once and never close
        lockfilefd = open(LOCKFILE, O_WRONLY|O_CREAT, 0664);
        if( lockfilefd < 0 ) { // We give up if this happens
            perror("Can't open lock file "LOCKFILE);
            exit(errno);
        }
    }
    if( flock(lockfilefd, LOCK_EX) < 0 ) {  // We give up if this happens
        perror("Can't optain exclusive lock on "LOCKFILE);
        exit(errno);
    }
    if( ioctl(fd, PPCLAIM) ) {
        perror("PPCLAIM");
        // Release the lockfile if claiming the device failed.
           flock(lockfilefd, LOCK_UN);
        return -1;    
    }
    return 0;
}

int pprelease() {
    int ret = 0;

    if( fd < 0 )
        ret = -1;
    else if( ioctl(fd, PPRELEASE) ) {
        perror("PPRELEASE");
        ret = -1;
    }
    flock(lockfilefd, LOCK_UN); // Make sure we always unlock.
    return ret;
}

int irlpdev_open() {
    if( fd >= 0 )
        return fd; /* already open */
    if( (fd = open("/dev/parport0", O_RDWR)) < 0 ) {
        perror("open(\"/dev/parport0\") failed.");
        return -1;
    }
    if( ppclaim() < 0 ) { /* trial claim */
        perror("Trial PPCLAIM failed on /dev/parport0");
        close(fd);
        fd = -1;
        return -1;
    }
    pprelease();
    return fd;
}

int read_irlpdev(unsigned char *buff, int n) {
    int k;

    if( ppclaim() < 0 )
        return -1;

    k = 0;
    if( n > 0 ) {
        if( ioctl(fd, PPRSTATUS, buff) ) {
            perror("PPRSTATUS");
            pprelease();
            return -1;
        }
        k++;
    }    
    if( n > 1 ) {
        if( ioctl(fd, PPRDATA, buff + 1) ) {
            perror("PPRDATA");
            pprelease();
            return -1;
        }
        k++;
    }
    pprelease();
    return k;
}

int write_irlpdev(unsigned char *buff, int n) {
    int k;

    if( ppclaim() < 0 )
        return -1;

    k = 0;
    if( n > 0 ) {
        if( ioctl(fd, PPWDATA, buff) ) {
            perror("PPWDATA");
            pprelease();
            return -1;
        }
        k++;
    }
    pprelease();
    return k;
}
