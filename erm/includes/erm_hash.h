/**
 * erm_hash.h
 *
 * Dave Taht
 * 2017-03-20
 */

#ifndef ERM_HASH_H
#define ERM_HASH_H

// The underlying IP hashing engine may be one of several types:
// presently jhash3, spookyhash, and crc32 are all candidates.
// They are not intended to be cryptographically secure hashes
// and meant merely as an additional means to classify flows.

// create expects one value to hash (src or dst)
// create3 expects 3 values to hash separately and return 1
// create5 expects 5 values to hash separately and return 1

ermhash_t erm_hash_create(erm_t handle, int size, int number) COLD;
ermhash_t erm_hash_create2(erm_t handle, int size, int number) COLD;
ermhash_t erm_hash_create3(erm_t handle, int size, int number) COLD;
ermhash_t erm_hash_create5(erm_t handle, int size, int number) COLD;
ermhash_t erm_hash_destroy(erm_t handle) COLD;

ermhash_t erm_hash_put(ermhash_t handle, ... ) HOT;
ermhash_t erm_hash_get(ermhash_t handle, ... ) VERYHOT;
ermhash_t erm_hash_tag(ermhash_t handle, ... ) VERYHOT;

// I need to think about this in the context of cake, which
// needs to return 3 partial hashes.

ermhash3_t erm_hash_cake_create(erm_t handle, int size, int number) COLD;
ermhash3_t erm_hash_cake_put(ermhash_t handle, ... ) HOT;
ermhash3_t erm_hash_cake_get(erm_t handle, ...) VERYHOT;


#endif
