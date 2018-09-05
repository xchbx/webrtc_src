/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_MESSAGEHANDLER_H_
#define RTC_BASE_MESSAGEHANDLER_H_

#include <memory>
#include <utility>

#include "rtc_base/constructormagic.h"

// https://blog.csdn.net/kenny_zh/article/details/38587733
// https://blog.csdn.net/kenny_zh/article/details/38587835
// 
namespace rtc {

struct Message;

// Messages get dispatched to a MessageHandler
// 定义了消息处理器的基本数据结构，子类在继承了该类之后要重载OnMessage函数，并实现消息响应的逻辑。
class MessageHandler {
 public:
  virtual ~MessageHandler();
  virtual void OnMessage(Message* msg) = 0;

 protected:
  MessageHandler() {}

 private:
  RTC_DISALLOW_COPY_AND_ASSIGN(MessageHandler);
};

// Helper class to facilitate executing a functor on a thread.
// 主要功能是将一个函数投递到目标线程执行。
template <class ReturnT, class FunctorT>
class FunctorMessageHandler : public MessageHandler {
 public:
  explicit FunctorMessageHandler(FunctorT&& functor)
      : functor_(std::forward<FunctorT>(functor)) {}
  virtual void OnMessage(Message* msg) {
    result_ = functor_();
  }
  const ReturnT& result() const { return result_; }

  // Returns moved result. Should not call result() or MoveResult() again
  // after this.
  ReturnT MoveResult() { return std::move(result_); }

 private:
  FunctorT functor_;
  ReturnT result_;
};

// Specialization for ReturnT of void.
template <class FunctorT>
class FunctorMessageHandler<void, FunctorT> : public MessageHandler {
 public:
  explicit FunctorMessageHandler(const FunctorT& functor)
      : functor_(functor) {}
  virtual void OnMessage(Message* msg) {
    functor_();
  }
  void result() const {}
  void MoveResult() {}

 private:
  FunctorT functor_;
};

} // namespace rtc

#endif // RTC_BASE_MESSAGEHANDLER_H_
