/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// -*- c++ -*-

#ifndef FAISS_ON_DISK_INVERTED_LISTS_H
#define FAISS_ON_DISK_INVERTED_LISTS_H

#include <vector>
#include <list>

#include "IndexIVF.h"

namespace faiss {


struct LockLevels;

/** On-disk storage of inverted lists.
 *
 * The data is stored in a mmapped chunk of memory (base ptointer ptr,
 * size totsize). Each list is a range of memory that contains (object
 * List) that contains:
 *
 * - uint8_t codes[capacity * code_size]
 * - followed by idx_t ids[capacity]
 *
 * in each of the arrays, the size <= capacity first elements are
 * used, the rest is not initialized.
 *
 * Addition and resize are supported by:
 * - roundind up the capacity of the lists to a power of two
 * - maintaining a list of empty slots, sorted by size.
 * - resizing the mmapped block is adjusted as needed.
 *
 * An OnDiskInvertedLists is compact if the size == capacity for all
 * lists and there are no available slots.
 *
 * Addition to the invlists is slow. For incremental add it is better
 * to use a default ArrayInvertedLists object and convert it to an
 * OnDisk with merge_from.
 *
 * When it is known that a set of lists will be accessed, it is useful
 * to call prefetch_lists, that launches a set of threads to read the
 * lists in parallel.
 */
struct OnDiskInvertedLists: InvertedLists {

    struct List {
        int64_t size;     // size of inverted list (entries)
        int64_t capacity; // allocated size (entries)
        int64_t offset;   // offset in buffer (bytes)
        List ();
    };

    // size nlist
    std::vector<List> lists;

    struct Slot {
        int64_t offset;    // bytes
        int64_t capacity;  // bytes
        Slot (int64_t offset, int64_t capacity);
        Slot ();
    };

    // size whatever space remains
    std::list<Slot> slots;

    std::string filename;
    int64_t totsize;
    uint8_t *ptr; // mmap base pointer
    bool read_only;  /// are inverted lists mapped read-only

    OnDiskInvertedLists (int64_t nlist, int64_t code_size,
                         const char *filename);

    int64_t list_size(int64_t list_no) const override;
    const uint8_t * get_codes (int64_t list_no) const override;
    const idx_t * get_ids (int64_t list_no) const override;

    int64_t add_entries (
           int64_t list_no, int64_t n_entry,
           const idx_t* ids, const uint8_t *code) override;

    void update_entries (int64_t list_no, int64_t offset, int64_t n_entry,
                         const idx_t *ids, const uint8_t *code) override;

    void resize (int64_t list_no, int64_t new_size) override;

    // copy all inverted lists into *this, in compact form (without
    // allocating slots)
    int64_t merge_from (const InvertedLists **ils, int n_il, bool verbose=false);

    /// restrict the inverted lists to l0:l1 without touching the mmapped region
    void crop_invlists(int64_t l0, int64_t l1);

    void prefetch_lists (const idx_t *list_nos, int nlist) const override;

    virtual ~OnDiskInvertedLists ();

    // private

    LockLevels * locks;

    // encapsulates the threads that are busy prefeteching
    struct OngoingPrefetch;
    OngoingPrefetch *pf;
    int prefetch_nthread;

    void do_mmap ();
    void update_totsize (int64_t new_totsize);
    void resize_locked (int64_t list_no, int64_t new_size);
    int64_t allocate_slot (int64_t capacity);
    void free_slot (int64_t offset, int64_t capacity);

    // empty constructor for the I/O functions
    OnDiskInvertedLists ();
};


} // namespace faiss

#endif
