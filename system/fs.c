#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#if FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;
int next_open_inum = 0;
int next_open_dblock = 0;

#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2
#define FIRST_DATA_BLOCK (FIRST_INODE_BLOCK + NUM_INODE_BLOCKS)

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);


/* YOUR CODE GOES HERE */
int fs_open(char *filename, int flags);
int fs_close(int fd);
int fs_create(char *filename, int mode);
int fs_seek(int fd, int offset);
int fs_read(int fd, void *buf, int nbytes);
int fs_write(int fd, void *buf, int nbytes);

// change filetable.state to FSTATE_OPEN
int fs_open(char *filename, int flags){
  for (int i = 0; i < next_open_fd; i++) {
    if (*filename == fsd.root_dir.entry[i].name) {
      oft[i].state = FSTATE_OPEN;
      return i;
    }
  }
  return SYSERR;
}

// change filetable.state to FSTATE_CLOSED
int fs_close(int fd){
  if (fd < 0 || fd >= NUM_FD) {
    return SYSERR;
  }
  oft[fd].state = FSTATE_CLOSED;
  return OK;
}

int fs_create(char *filename, int mode){
  int fd = next_open_fd;
  int inode_id = next_open_inum;
  next_open_fd++;
  next_open_inum++;

  struct filetable *file = &oft[fd];
  file->state = FSTATE_OPEN;
  file->fileptr = 0;

  // using dir in oft instead of directory
  file->de = &fsd.root_dir.entry[fd];
  strncpy(file->de->name, filename, strlen(filename));
  file->de->inode_num = inode_id;

  struct inode *in = &file->in;
  in->id = inode_id;
  in->size = 0;
  in->type = 0;
  in->nlink = 0;
  in->blocks[0] = next_open_dblock;
  next_open_dblock++;

  for (int i = 1; i < INODEBLOCKS; i++) {
    in->blocks[i] = -1;
  }
  fs_put_inode_by_num(dev0, inode_id, in);

  return fd;
}

int fs_seek(int fd, int offset){
  struct filetable *file = &oft[fd];
  if (file->state == FSTATE_CLOSED) {
    return SYSERR;
  }
  if (file->fileptr + offset < 0) {
    return SYSERR;
  }

  file->fileptr += offset;
  return OK;
}

int fs_read(int fd, void *buf, int nbytes){
  struct filetable *file = &oft[fd];
  if (file->state == FSTATE_CLOSED) {
    return SYSERR;
  }

  if (file->fileptr % MDEV_BLOCK_SIZE == 0) {
    int blocks = nbytes / MDEV_BLOCK_SIZE + 1;
    int blocks_over = file->fileptr / MDEV_BLOCK_SIZE;
    for (int i = blocks_over; i < blocks+blocks_over; i++) {
      bs_bread(dev0, file->in.blocks[i], 0, buf+((i-blocks_over)*MDEV_BLOCK_SIZE), MDEV_BLOCK_SIZE);
      if (file->in.blocks[i] == -1) {
        printf("read too much. out of file boundary.\n");
      }
    }
    file->fileptr += nbytes;
    return nbytes;
  }
  else {
    int blocks = nbytes / MDEV_BLOCK_SIZE + 1;
    int blocks_over = file->fileptr / MDEV_BLOCK_SIZE;
    int len = MDEV_NUM_BLOCKS - file->fileptr;
    bs_bread(dev0, file->in.blocks[blocks_over], file->fileptr, buf, len);
    file->fileptr += len;
    fs_read(fd, buf+len, nbytes-len);
    return nbytes;
  }
}

int fs_write(int fd, void *buf, int nbytes){
  struct filetable *file = &oft[fd];
  if (file->state == FSTATE_CLOSED) {
    return SYSERR;
  }

  if (file->fileptr % MDEV_BLOCK_SIZE == 0) {
    int beginning_block;
    int blocks = nbytes / MDEV_BLOCK_SIZE + 1;
    for (int i = file->fileptr/MDEV_BLOCK_SIZE; i < blocks; i++) {
      if (file->in.blocks[i] == -1){
        beginning_block = next_open_dblock + FIRST_DATA_BLOCK;
        next_open_dblock++;
        file->in.blocks[i] = beginning_block;
        if (next_open_dblock > MDEV_BLOCK_SIZE-1) {
          printf("running out of data blocks\n");
          return SYSERR;
        }
      }
      else {
        beginning_block = file->in.blocks[i];
          }
      bs_bwrite(dev0, beginning_block, 0, buf + MDEV_NUM_BLOCKS*(i-file->fileptr/MDEV_NUM_BLOCKS), MDEV_BLOCK_SIZE);
    }
    file->fileptr += nbytes;
    return nbytes;
  }
  else{
    int beginning_block = file->in.blocks[0];
    int len = MDEV_NUM_BLOCKS - file->fileptr;
    bs_bwrite(dev0, beginning_block, file->fileptr, buf, len);
    buf = buf + len;
    file->fileptr += len;
    fs_write(fd, buf, nbytes-len);
    return nbytes;
  }
}


int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}

int fs_mkfs(int dev, int num_inodes) {
  int i;

  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8;

  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }

  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }

  fsd.inodes_used = 0;

  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));

  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);


  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit); /* 128 >> */
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

#endif /* FS */
