//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// multi_thread_context.cpp
//
// Identification: src/codegen/multi_thread_context.cpp
//
// Copyright (c) 2015-17, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <iostream>

#include "codegen/barrier.h"
#include "codegen/codegen.h"
#include "codegen/multi_thread_context.h"

namespace peloton {
namespace codegen {

void MultiThreadContext::InitInstance(MultiThreadContext *ins,
                                      int64_t thread_id, int64_t thread_count,
                                      Barrier *bar) {
  assert(thread_id < thread_count);
  ins->SetThreadId(thread_id);
  ins->SetThreadCount(thread_count);
  ins->SetBarrier(bar);
}

int64_t MultiThreadContext::GetRangeStart(int64_t tile_group_num) {
  // thread_count_ must be less than tile_group_num
  int64_t slice_size = tile_group_num / std::min(thread_count_, tile_group_num);
  int64_t start = thread_id_ * slice_size;

  return start;
}

int64_t MultiThreadContext::GetRangeEnd(int64_t tile_group_num) {
  // thread_count_ must be less than tile_group_num
  int64_t slice_size = tile_group_num / std::min(thread_count_, tile_group_num);
  int64_t end;
  if (thread_id_ == thread_count_ - 1) {
    end = tile_group_num;
  } else {
    end = std::min(tile_group_num, (thread_id_ + 1) * slice_size);
  }

  return end;
}

int64_t MultiThreadContext::GetThreadId() { return thread_id_; }

bool MultiThreadContext::BarrierWait() { return bar_->BarrierWait(); }

void MultiThreadContext::NotifyMaster() { bar_->WorkerFinish(); }

void MultiThreadContext::MergeToGlobalHashTable(utils::OAHashTable *global_ht,
                                                utils::OAHashTable *local_ht) {
  bar_->MergeToGlobalHashTable(global_ht, local_ht);
}
}
}
