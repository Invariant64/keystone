#include <linux/dma-mapping.h>
#include "keystone.h"

DEFINE_MUTEX(id_reserved_lock);

struct keystone_reserved reserves[KEYSTONE_RESERVED_MAX];

static int reserved_max_id = 0;

int reserved_id_alloc(void)
{
  int id;

  mutex_lock(&id_reserved_lock);
  id = reserved_max_id;
  reserved_max_id++;
  mutex_unlock(&id_reserved_lock);

  for (int i = 0; i < KEYSTONE_RESERVED_MAX; i++) {
    if (!reserves[i].valid) {
      reserves[i].valid = true;
      reserves[i].id = id;
      return id;
    }
  }

  return -1;
}

struct keystone_reserved* get_reserved_by_id(int id)
{
  int i;

  mutex_lock(&id_reserved_lock);

  for (i = 0; i < KEYSTONE_RESERVED_MAX; i++) {
    if (reserves[i].valid && reserves[i].id == id) {
      mutex_unlock(&id_reserved_lock);
      return &reserves[i];
    }
  }

  mutex_unlock(&id_reserved_lock);
  
  return NULL;
}

void reserved_release(int id)
{
  mutex_lock(&id_reserved_lock);

  for (int i = 0; i < KEYSTONE_RESERVED_MAX; i++) {
    if (reserves[i].valid && reserves[i].id == id) {
      reserves[i].valid = false;
      reserves[i].epm = NULL;
      reserves[i].id = -1;
      break;
    }
  }

  mutex_unlock(&id_reserved_lock);
}

int reserved_memory_alloc(int id, int size)
{
  struct keystone_reserved* reserve = get_reserved_by_id(id);
  if (!reserve) {
    return -1; // Invalid ID
  }

  reserve->epm = kmalloc(sizeof(struct epm), GFP_KERNEL);
  if (!reserve->epm) {
    keystone_err("Failed to allocate reserved memory for epm struct %d\n", id);
    return -1;
  }

  if (epm_init(reserve->epm, size >> PAGE_SHIFT)) {
    kfree(reserve->epm);
    reserve->epm = NULL;
    return 0;
  }

  keystone_info("Reserved memory allocated for ID %d, size %d\n", id, size);

  return size;
}