#include "LRUCache.h"
#include <iostream>

LRUCache::LRUCache(size_t capacity)
    : slots(capacity), max_size(capacity), access_counter(0) {}

bool LRUCache::contains(const std::string& track_id) const {
    return findSlot(track_id) != max_size;
}

AudioTrack* LRUCache::get(const std::string& track_id) {
    size_t idx = findSlot(track_id);
    if (idx == max_size) return nullptr;
    return slots[idx].access(++access_counter);
}

/**
 * TODO: Implement the put() method for LRUCache
 */
bool LRUCache::put(PointerWrapper<AudioTrack> track) {
    if (!track) return false;

    // Case 1: HIT - Track already exists
    // We access it to update the timestamp (MRU)
    size_t existing_idx = findSlot(track->get_title());
    if (existing_idx != max_size) {
        slots[existing_idx].access(++access_counter);
        return false; // No eviction occurred
    }

    // Case 2: MISS - Need to insert
    bool evicted = false;
    size_t insert_idx = findEmptySlot();

    // If cache is full, we must evict first
    if (insert_idx == max_size) {
        if (!evictLRU()) {
            // Should not happen if size > 0, but safety check
            return false; 
        }
        insert_idx = findEmptySlot();
        evicted = true;
    }

    // Perform the insertion
    // Note: track is a PointerWrapper, so we move it to transfer ownership
    if (insert_idx != max_size) {
        slots[insert_idx].store(std::move(track), ++access_counter);
    }

    return evicted;
}

bool LRUCache::evictLRU() {
    size_t lru = findLRUSlot();
    if (lru == max_size || !slots[lru].isOccupied()) return false;
    slots[lru].clear();
    return true;
}

size_t LRUCache::size() const {
    size_t count = 0;
    for (const auto& slot : slots) if (slot.isOccupied()) ++count;
    return count;
}

void LRUCache::clear() {
    for (auto& slot : slots) {
        slot.clear();
    }
}

void LRUCache::displayStatus() const {
    std::cout << "[LRUCache] Status: " << size() << "/" << max_size << " slots used\n";
    for (size_t i = 0; i < max_size; ++i) {
        if(slots[i].isOccupied()){
            std::cout << "  Slot " << i << ": " << slots[i].getTrack()->get_title()
                      << " (last access: " << slots[i].getLastAccessTime() << ")\n";
        } else {
            std::cout << "  Slot " << i << ": [EMPTY]\n";
        }
    }
}

size_t LRUCache::findSlot(const std::string& track_id) const {
    for (size_t i = 0; i < max_size; ++i) {
        if (slots[i].isOccupied() && slots[i].getTrack()->get_title() == track_id) return i;
    }
    return max_size;

}

/**
 * TODO: Implement the findLRUSlot() method for LRUCache
 */
size_t LRUCache::findLRUSlot() const {
    size_t lru_index = max_size;
    size_t min_time = -1; // Max possible size_t value (effectively infinity)

    for (size_t i = 0; i < max_size; ++i) {
        if (slots[i].isOccupied()) {
            size_t time = slots[i].getLastAccessTime();
            if (time < min_time) {
                min_time = time;
                lru_index = i;
            }
        }
    }
    return lru_index;
}

size_t LRUCache::findEmptySlot() const {
    for (size_t i = 0; i < max_size; ++i) {
        if (!slots[i].isOccupied()) return i;
    }
    return max_size;
}

void LRUCache::set_capacity(size_t capacity){
    if (max_size == capacity)
        return;
    //udpate max size
    max_size = capacity;
    //update the slots vector
    slots.resize(capacity);
}