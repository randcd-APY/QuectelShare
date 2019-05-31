/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#define INFINITE_QUEUE -1

template<typename T>
class queue_mt {
private:
   std::mutex mut;
   std::queue<T> data_queue;
   std::condition_variable data_cond;
   int maxsize;
public:
   std::queue<T> snap_queue;
   queue_mt(int size = 10) { this->maxsize = size; }

   bool empty()
   {
      std::lock_guard<std::mutex> lk(mut);
      return data_queue.empty();
   }

   void clear()
   {
      std::lock_guard<std::mutex> lk(mut);
      while (!data_queue.empty())
         data_queue.pop();
   }

   void check_push(T new_value) {
      std::lock_guard<std::mutex> lk(mut);
      if (data_queue.size() == maxsize ) {
         data_queue.pop();
      }
      data_queue.push(new_value);
      data_cond.notify_all();
   }

   void wait_and_pop(T& value) {
      std::unique_lock<std::mutex> lk(mut);
      data_cond.wait(lk, [this] {return !data_queue.empty(); });
      value = data_queue.front();
      data_queue.pop();
   }

   bool try_pop(T& value){
      std::lock_guard<std::mutex> lk(mut);
      if (data_queue.empty()) return false;
      value = data_queue.front();
      data_queue.pop();
      return true;
    }
};
