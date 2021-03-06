//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// multi_thread_context.h
//
// Identification: src/include/codegen/multi_thread_context.h
//
// Copyright (c) 2015-17, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "codegen/barrier.h"
#include "codegen/codegen.h"
#include "codegen/utils/oa_hash_table.h"

#include <atomic>

namespace peloton {
namespace codegen {

//===----------------------------------------------------------------------===//
// Multi thread context is created per thread, and stored all information that
// thread needs.
//===----------------------------------------------------------------------===//
class MultiThreadContext {
 public:
  static void InitInstance(MultiThreadContext *ins, int64_t thread_id,
                           int64_t thread_count, Barrier *bar);

  int64_t GetRangeStart(int64_t tile_group_num);

  int64_t GetRangeEnd(int64_t tile_group_num);

  int64_t GetThreadId();

  bool BarrierWait();

  void NotifyMaster();

  void MergeToGlobalHashTable(utils::OAHashTable *global_ht,
                              utils::OAHashTable *local_ht);

  ~MultiThreadContext() { bar_ = nullptr; }

 private:
  MultiThreadContext(int64_t thread_id, int64_t thread_count, Barrier *bar)
      : thread_id_(thread_id), thread_count_(thread_count), bar_(bar) {}

  inline void SetThreadId(int64_t thread_id) { thread_id_ = thread_id; }

  inline void SetThreadCount(int64_t thread_count) {
    thread_count_ = thread_count;
  }

  inline void SetBarrier(Barrier *bar) { bar_ = bar; }

  int64_t thread_id_;
  int64_t thread_count_;
  Barrier *bar_;
};
}
}
