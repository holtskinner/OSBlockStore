#include "block_store.h"
#include "bitmap.h"

#define BITMAP_SIZE_BYTES 256         // 2^8 blocks.
#define BLOCK_STORE_NUM_BLOCKS 256    // 2^8 blocks.
#define BLOCK_STORE_AVAIL_BLOCKS 255  // Last block consumed by the FBM
#define BLOCK_STORE_NUM_BYTES 65536   // 2^8 blocks of 2^8 bytes.
#define BLOCK_SIZE_BITS 2048          // 2^8 BYTES per block
#define BLOCK_SIZE_BYTES 256          // 2^8 BYTES per block

typedef struct block_store {
  unsigned char *data;  // unsigned char is one byte
  bitmap_t *bitmap;     // bitmap is at end of blockstore
} block_store_t;

block_store_t *block_store_create() {
  block_store_t *block = calloc(1, sizeof(block_store_t));

  if (!block) {
    return NULL;
  }

  block->data = calloc(BLOCK_STORE_NUM_BLOCKS, BLOCK_SIZE_BYTES);

  if (!block->data) {
    return NULL;
  }

  block->bitmap = bitmap_create(BLOCK_STORE_NUM_BYTES);

  if (!block->bitmap) {
    return NULL;
  }

  // initialize bitmap to 0
  // 8 blocks to store the actual bitmap
  for (size_t i = 0; i < BITMAP_SIZE_BYTES; i++) {
    bitmap_reset(block->bitmap, i);
  }

  return block;
}

void block_store_destroy(block_store_t *const bs) {
  if (!bs) {
    return;
  }

  bitmap_destroy(bs->bitmap);
  free(bs->data);
  free(bs);

  return;
}

size_t block_store_allocate(block_store_t *const bs) {
  if (!bs) {
    return SIZE_MAX;
  }

  size_t index = bitmap_ffz(bs->bitmap);

  if (index >= BLOCK_STORE_AVAIL_BLOCKS) {
    return SIZE_MAX;
  }

  bitmap_set(bs->bitmap, index);

  return index;
}

bool block_store_request(block_store_t *const bs, const size_t block_id) {
  if (!bs || 0 == block_id || block_id > BLOCK_STORE_AVAIL_BLOCKS ||
      bitmap_test(bs->bitmap, block_id)) {
    return false;
  }
  bitmap_set(bs->bitmap, block_id);
  return true;
}

void block_store_release(block_store_t *const bs, const size_t block_id) {
  if (!bs || block_id > BLOCK_STORE_AVAIL_BLOCKS) {
    return;
  }

  bitmap_reset(bs->bitmap, block_id);
  return;
}

size_t block_store_get_used_blocks(const block_store_t *const bs) {
  if (!bs) {
    return SIZE_MAX;
  }
  return bitmap_total_set(bs->bitmap);
}

size_t block_store_get_free_blocks(const block_store_t *const bs) {
  if (!bs) {
    return SIZE_MAX;
  }
  return BLOCK_STORE_AVAIL_BLOCKS - bitmap_total_set(bs->bitmap);
}

size_t block_store_get_total_blocks() { return BLOCK_STORE_AVAIL_BLOCKS; }

size_t block_store_read(const block_store_t *const bs, const size_t block_id,
                        void *buffer) {
  if (!bs || 0 == block_id || !buffer) {
    return 0;
  }

  if (!memcpy(buffer, bs->data + block_id, BLOCK_SIZE_BYTES)) {
    return 0;
  }

  return BLOCK_STORE_NUM_BLOCKS;
}

size_t block_store_write(block_store_t *const bs, const size_t block_id,
                         const void *buffer) {
  if (!bs || block_id > BLOCK_STORE_NUM_BLOCKS || !buffer) {
    return 0;
  }

  if (!memcpy(bs->data + block_id, buffer, BLOCK_SIZE_BYTES)) {
    return 0;
  }

  return BLOCK_STORE_NUM_BLOCKS;
}

// bonus
block_store_t *block_store_deserialize(const char *const filename) {
  if (!filename) {
    return NULL;
  }

  // int fd = open(filename, O_RDONLY);

  // if (fd < 0) {
  //   return NULL;
  // }

  // block_store_t *bs = block_store_create();
  // if (!bs) {
  //   return NULL;
  // }

  // size_t index = block_store_allocate(bs);
  // if (index == SIZE_MAX) {
  //   close(fd);
  //   return NULL;
  // }

  // void *buffer = calloc(1, BLOCK_SIZE_BYTES);

  // if (!buffer) {
  //   return NULL;
  // }

  // if (read(fd, buffer, BLOCK_SIZE_BYTES) < 0) {
  //   free(buffer);
  //   return NULL;
  // }

  // close(fd);

  // if (block_store_write(bs, index, buffer) == 0) {
  //   free(buffer);
  //   return NULL;
  // }
  // block_store_write(bs, index, buffer);
  // free(buffer);
  // return bs;
  return NULL;
}

size_t block_store_serialize(const block_store_t *const bs,
                             const char *const filename) {
  if (!bs || !filename) {
    return 0;
  }

  // int fd = open(filename, O_WRONLY | O_CREAT, 0644);

  // if (fd < 0) {
  //   return 0;
  // }

  // size_t numberBytesWritten = write(fd, bs, BLOCK_SIZE_BYTES);

  // close(fd);
  // return numberBytesWritten;
  return BLOCK_SIZE_BYTES;
}
