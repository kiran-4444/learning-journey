// :bustub-keep-private:
//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// arc_replacer.cpp
//
// Identification: src/buffer/arc_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/arc_replacer.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <ostream>
#include "common/config.h"
#include "common/exception.h"

namespace bustub {

/**
 *
 * TODO(P1): Add implementation
 *
 * @brief a new ArcReplacer, with lists initialized to be empty and target size to 0
 * @param num_frames the maximum number of frames the ArcReplacer will be required to cache
 */
ArcReplacer::ArcReplacer(size_t num_frames) : replacer_size_(num_frames) {}

/**
 * TODO(P1): Add implementation
 *
 * @brief Performs the Replace operation as described by the writeup
 * that evicts from either mfu_ or mru_ into its corresponding ghost list
 * according to balancing policy.
 *
 * If you wish to refer to the original ARC paper, please note that there are
 * two changes in our implementation:
 * 1. When the size of mru_ equals the target size, we don't check
 * the last access as the paper did when deciding which list to evict from.
 * This is fine since the original decision is stated to be arbitrary.
 * 2. Entries that are not evictable are skipped. If all entries from the desired side
 * (mru_ / mfu_) are pinned, we instead try victimize the other side (mfu_ / mru_),
 * and move it to its corresponding ghost list (mfu_ghost_ / mru_ghost_).
 *
 * @return frame id of the evicted frame, or std::nullopt if cannot evict
 */
auto ArcReplacer::Evict() -> std::optional<frame_id_t> {
  if (curr_size_ == 0) {
    return std::nullopt;
  }

  if (mru_.size() >= mru_target_size_) {
    for (auto it = mru_.end(); it != mru_.begin();) {
      --it;
      auto alive_map_it = alive_map_.find(*it);
      if (alive_map_it->second->evictable_) {
        FrameStatus fs = *alive_map_it->second;

        alive_map_.erase(*it);
        it = mru_.erase(it);
        fs.arc_status_ = ArcStatus::MRU_GHOST;
        mru_ghost_.push_front(fs.page_id_);
        ghost_map_[fs.page_id_] = std::make_shared<FrameStatus>(fs);

        if (mru_ghost_.size() > replacer_size_) {
          page_id_t last_entry_id = mru_ghost_.back();
          ghost_map_.erase(last_entry_id);
          mru_ghost_.remove(last_entry_id);
        }

        curr_size_ -= 1;

        // std::cout << "Evict()" << std::endl;
        // PrintState();

        return fs.frame_id_;
      }
    }

    for (auto it = mfu_.end(); it != mfu_.begin();) {
      --it;
      auto alive_map_it = alive_map_.find(*it);
      if (alive_map_it->second->evictable_) {
        FrameStatus fs = *alive_map_it->second;

        alive_map_.erase(*it);
        it = mfu_.erase(it);
        fs.arc_status_ = ArcStatus::MFU_GHOST;
        mfu_ghost_.push_front(fs.page_id_);
        ghost_map_[fs.page_id_] = std::make_shared<FrameStatus>(fs);

        if (mfu_ghost_.size() > replacer_size_) {
          page_id_t last_entry_id = mfu_ghost_.back();
          ghost_map_.erase(last_entry_id);
          mfu_ghost_.remove(last_entry_id);
        }

        curr_size_ -= 1;
        // std::cout << "Evict()" << std::endl;
        // PrintState();

        return fs.frame_id_;
      }
    }
  } else {
    for (auto it = mfu_.end(); it != mfu_.begin();) {
      --it;
      auto alive_map_it = alive_map_.find(*it);
      if (alive_map_it->second->evictable_) {
        FrameStatus fs = *alive_map_it->second;

        alive_map_.erase(*it);
        it = mfu_.erase(it);
        fs.arc_status_ = ArcStatus::MFU_GHOST;
        mfu_ghost_.push_front(fs.page_id_);
        ghost_map_[fs.page_id_] = std::make_shared<FrameStatus>(fs);

        if (mfu_ghost_.size() > replacer_size_) {
          page_id_t last_entry_id = mfu_ghost_.back();
          ghost_map_.erase(last_entry_id);
          mfu_ghost_.remove(last_entry_id);
        }

        curr_size_ -= 1;
        // std::cout << "Evict()" << std::endl;
        // PrintState();

        return fs.frame_id_;
      }
    }
    for (auto it = mru_.end(); it != mru_.begin();) {
      --it;
      auto alive_map_it = alive_map_.find(*it);
      if (alive_map_it->second->evictable_) {
        FrameStatus fs = *alive_map_it->second;

        alive_map_.erase(*it);
        it = mru_.erase(it);
        fs.arc_status_ = ArcStatus::MRU_GHOST;
        mru_ghost_.push_front(fs.page_id_);
        ghost_map_[fs.page_id_] = std::make_shared<FrameStatus>(fs);

        if (mru_ghost_.size() > replacer_size_) {
          page_id_t last_entry_id = mru_ghost_.back();
          ghost_map_.erase(last_entry_id);
          mru_ghost_.remove(last_entry_id);
        }

        curr_size_ -= 1;

        // std::cout << "Evict()" << std::endl;
        // PrintState();

        return fs.frame_id_;
      }
    }
  }

  //   std::cout << "Evict()" << std::endl;
  //   PrintState();

  return std::nullopt;
}

void ArcReplacer::RecordAccess(frame_id_t frame_id, page_id_t page_id, [[maybe_unused]] AccessType access_type) {
  //   std::cout << "RecordAccess()" << std::endl;
  auto it_alive = alive_map_.find(frame_id);
  auto it_ghost = ghost_map_.find(page_id);
  if (it_alive != alive_map_.end()) {
    // Make sure no duplicate inserts
    mru_.remove(frame_id);
    mfu_.remove(frame_id);
    // Page already exists in MRU/MFU: This is the case where the actual cache hits. Move the page to the front of
    // MFU.
    ArcStatus entry_status = it_alive->second->arc_status_;
    if (entry_status == ArcStatus::MRU) {
      it_alive->second->arc_status_ = ArcStatus::MFU;
      mru_.remove(frame_id);
      mfu_.push_front(frame_id);
    } else {
      mfu_.remove(frame_id);
      mfu_.push_front(frame_id);
    }
  } else if (it_ghost != ghost_map_.end()) {
    ArcStatus entry_status = it_ghost->second->arc_status_;
    if (entry_status == ArcStatus::MRU_GHOST) {
      // Page already exists in MRU ghost: This is the case where the actual cache misses but we hit on the ghost
      // list. In this case we treat it as a pseudo-hit and adapt the target size. If the size of the MRU ghost list
      // is greater than or equal to the size of the MFU ghost list, increase the MRU target size by one. Else
      // increase it by MFU ghost size / MRU ghost size (rounded down). Do not increase the target size above
      // replacer_size. Then move the page to the front of MFU. The rational of this is if the MRU list is a little
      // larger, then the DBMS could have had a cache hit.
      if (mru_ghost_.size() >= mfu_ghost_.size()) {
        mru_target_size_ += 1;
      } else {
        mru_target_size_ = mru_target_size_ + (size_t)floor(mfu_ghost_.size() / mru_ghost_.size());
      }

      if (mru_target_size_ > replacer_size_) {
        mru_target_size_ = replacer_size_;
      }
      ghost_map_.erase(page_id);
      mru_ghost_.remove(page_id);
      FrameStatus fs(page_id, frame_id, false, ArcStatus::MFU);
      alive_map_[frame_id] = std::make_shared<FrameStatus>(fs);
      mfu_.push_front(frame_id);
    } else {
      // Page already exists in MFU ghost: Similar to the previous case, this is when the actual cache misses but we
      // hit on the ghost list. If the size of the MFU ghost list is greater than or equal to the size of the MRU
      // ghost list, decrease the MRU target size by 1. Else decrease the MRU target size by MRU ghost size / MFU
      // ghost size (rounded down). Do not decrease the target size below 0. Then move the page to the front of MFU.
      // The rational of this is if the MFU list is a little larger, the DBMS could have had a cache hit.
      if (mfu_ghost_.size() >= mru_ghost_.size()) {
        mru_target_size_ -= 1;
      } else {
        mru_target_size_ = mru_target_size_ - (size_t)floor(mru_ghost_.size() / mfu_ghost_.size());
      }

      if (mru_target_size_ < 0) {
        mru_target_size_ = 0;
      }
      ghost_map_.erase(page_id);
      mfu_ghost_.remove(page_id);
      FrameStatus fs(page_id, frame_id, false, ArcStatus::MFU);
      alive_map_[frame_id] = std::make_shared<FrameStatus>(fs);
      mfu_.push_front(frame_id);
    }

  } else {
    if (mru_.size() + mru_ghost_.size() == replacer_size_) {
      // If MRU size + MRU ghost size = replacer size: Kill the last element in the MRU ghost list, then add the page
      // to the front of MRU.
      page_id_t last_page_id = mru_ghost_.back();
      ghost_map_.erase(last_page_id);
      mru_ghost_.remove(last_page_id);
    } else if (mru_.size() + mru_ghost_.size() + mfu_.size() + mfu_ghost_.size() == 2 * replacer_size_) {
      // If MRU size + MRU ghost size + MFU size + MFU ghost size = 2 * replacer size: Kill the last element in the
      // MFU ghost list, then add the page to the front of MRU.
      page_id_t last_page_id = mfu_ghost_.back();
      ghost_map_.erase(last_page_id);
      mfu_ghost_.remove(last_page_id);
    }
    FrameStatus fs(page_id, frame_id, false, ArcStatus::MRU);
    alive_map_[frame_id] = std::make_shared<FrameStatus>(fs);
    mru_.push_front(frame_id);
  }

  //   PrintState();
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Toggle whether a frame is evictable or non-evictable. This function also
 * controls replacer's size. Note that size is equal to number of evictable entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then size should
 * decrement. If a frame was previously non-evictable and is to be set to evictable,
 * then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void ArcReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  auto it = alive_map_.find(frame_id);
  if (it == alive_map_.end()) {
    throw Exception("frame_id not found in SetEvictable");
  }
  if (it->second->evictable_ == set_evictable) {
    return;
  }
  if (it->second->evictable_ == false and set_evictable == true) {
    curr_size_ += 1;
    if (curr_size_ > replacer_size_) {
      curr_size_ = replacer_size_;
    }
  } else if (it->second->evictable_ == true and set_evictable == false) {
    curr_size_ -= 1;
    if (curr_size_ < 0) {
      curr_size_ = 0;
    }
  }
  it->second->evictable_ = set_evictable;

  //   std::cout << "SetEvictable" << std::endl;
  //   PrintState();
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Remove an evictable frame from replacer.
 * This function should also decrement replacer's size if removal is successful.
 *
 * Note that this is different from evicting a frame, which always remove the frame
 * decided by the ARC algorithm.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 * If specified frame is not found, directly return from this function.
 *
 * @param frame_id id of frame to be removed
 */
void ArcReplacer::Remove(frame_id_t frame_id) {}

/**
 * TODO(P1): Add implementation
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto ArcReplacer::Size() -> size_t { return curr_size_; }

void ArcReplacer::PrintState() {
  //   std::cout << "[<-mru_ghost-]";

  std::cout << "[";
  // mru_ghost_
  for (auto it = mru_ghost_.rbegin(); it != mru_ghost_.rend(); ++it) {
    std::cout << "(" << *it << ", _) ";
  }
  std::cout << "]";

  //   std::cout << "[<-mru-]";

  std::cout << "[";
  // mru_
  for (auto it = mru_.rbegin(); it != mru_.rend(); ++it) {
    auto fs = alive_map_.find(*it)->second;
    if (fs->evictable_) {
      std::cout << "(" << fs->page_id_ << ", f" << fs->frame_id_ << ") ";
    } else {
      std::cout << "p(" << fs->page_id_ << ", f" << fs->frame_id_ << ") ";
    }
  }
  std::cout << "]";

  std::cout << "!";

  // mfu_
  //   std::cout << "[-mfu->]";
  std::cout << "[";
  for (auto it = mfu_.begin(); it != mfu_.end(); ++it) {
    auto fs = alive_map_.find(*it)->second;
    if (fs->evictable_) {
      std::cout << "(" << fs->page_id_ << ", f" << fs->frame_id_ << ") ";
    } else {
      std::cout << "p(" << fs->page_id_ << ", f" << fs->frame_id_ << ") ";
    }
  }
  std::cout << "]";

  //   std::cout << "[->mfu_ghost->]";

  // mfu_ghost_
  std::cout << "[";
  for (auto it = mfu_ghost_.begin(); it != mfu_ghost_.end(); ++it) {
    std::cout << "(" << *it << ", _) ";
  }
  std::cout << "]";

  std::cout << " p=" << mru_target_size_ << " " << "mru_target_size_: " << mru_target_size_ << " "
            << "replacer_size_: " << replacer_size_ << std::endl;
  std::cout << std::endl;
}
}  // namespace bustub
