/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_PHYSICALSOCKETSERVER_H_
#define RTC_BASE_PHYSICALSOCKETSERVER_H_

#if defined(WEBRTC_POSIX) && defined(WEBRTC_LINUX)
#include <sys/epoll.h>
#define WEBRTC_USE_EPOLL 1
#endif

#include <memory>
#include <set>
#include <vector>

#include "rtc_base/criticalsection.h"
#include "rtc_base/nethelpers.h"
#include "rtc_base/socketserver.h"

#if defined(WEBRTC_POSIX)
typedef int SOCKET;
#endif // WEBRTC_POSIX

namespace rtc {

// Event constants for the Dispatcher class.
enum DispatcherEvent {
  DE_READ    = 0x0001,
  DE_WRITE   = 0x0002,
  DE_CONNECT = 0x0004,
  DE_CLOSE   = 0x0008,
  DE_ACCEPT  = 0x0010,
};

class Signaler;
#if defined(WEBRTC_POSIX)
class PosixSignalDispatcher;
#endif

// 定义了事件分发器的纯虚基类。事件分发器主要是将IO或Event信号对应到一定的处理函数上去。
class Dispatcher {
 public:
  virtual ~Dispatcher() {}
  virtual uint32_t GetRequestedEvents() = 0;
  virtual void OnPreEvent(uint32_t ff) = 0;
  virtual void OnEvent(uint32_t ff, int err) = 0;
#if defined(WEBRTC_WIN)
  virtual WSAEVENT GetWSAEvent() = 0;
  virtual SOCKET GetSocket() = 0;
  virtual bool CheckSignalClose() = 0;
#elif defined(WEBRTC_POSIX)
  virtual int GetDescriptor() = 0;
  virtual bool IsDescriptorClosed() = 0;
#endif
};

// A socket server that provides the real sockets of the underlying OS.
// 实现了消息和IO的多路分发功能, 类似于Windows平台上的WSAWaitForMultipleEvents的功能。
class PhysicalSocketServer : public SocketServer {
 public:
  PhysicalSocketServer();
  ~PhysicalSocketServer() override;

  // SocketFactory:
  // 创建一个talk_base::Socket实例，实质为rtc::PhysicalSocket
  Socket* CreateSocket(int type) override;
  Socket* CreateSocket(int family, int type) override;
  // 创建一个talk_base::AsyncSocket实例，实质是rtc::SocketDispatcher。
  AsyncSocket* CreateAsyncSocket(int type) override;
  AsyncSocket* CreateAsyncSocket(int family, int type) override;

  // Internal Factory for Accept (virtual so it can be overwritten in tests).
  // 将一个系统socket句柄/文件描述符封装成rtc::SocketDispatcher并添加入分发器列表（talk_base::PhysicalSocketServer::dispatchers_）
  virtual AsyncSocket* WrapSocket(SOCKET s);

  // SocketServer:
  // 实现了多路信号分离器
  bool Wait(int cms, bool process_io) override;
  void WakeUp() override;

  // 向分发器列表添加/删除一个分发器
  void Add(Dispatcher* dispatcher);
  void Remove(Dispatcher* dispatcher);
  void Update(Dispatcher* dispatcher);

#if defined(WEBRTC_POSIX)
  // Sets the function to be executed in response to the specified POSIX signal.
  // The function is executed from inside Wait() using the "self-pipe trick"--
  // regardless of which thread receives the signal--and hence can safely
  // manipulate user-level data structures.
  // "handler" may be SIG_IGN, SIG_DFL, or a user-specified function, just like
  // with signal(2).
  // Only one PhysicalSocketServer should have user-level signal handlers.
  // Dispatching signals on multiple PhysicalSocketServers is not reliable.
  // The signal mask is not modified. It is the caller's responsibily to
  // maintain it as desired.
  virtual bool SetPosixSignalHandler(int signum, void (*handler)(int));

 protected:
  Dispatcher* signal_dispatcher();
#endif

 private:
  typedef std::set<Dispatcher*> DispatcherSet;

  void AddRemovePendingDispatchers();

#if defined(WEBRTC_POSIX)
  bool WaitSelect(int cms, bool process_io);
  static bool InstallSignal(int signum, void (*handler)(int));

  std::unique_ptr<PosixSignalDispatcher> signal_dispatcher_;
#endif  // WEBRTC_POSIX
#if defined(WEBRTC_USE_EPOLL)
  void AddEpoll(Dispatcher* dispatcher);
  void RemoveEpoll(Dispatcher* dispatcher);
  void UpdateEpoll(Dispatcher* dispatcher);
  bool WaitEpoll(int cms);
  bool WaitPoll(int cms, Dispatcher* dispatcher);

  int epoll_fd_ = INVALID_SOCKET;
  std::vector<struct epoll_event> epoll_events_;
#endif  // WEBRTC_USE_EPOLL
  DispatcherSet dispatchers_;               // 分发器列表
  DispatcherSet pending_add_dispatchers_;
  DispatcherSet pending_remove_dispatchers_;
  bool processing_dispatchers_ = false;
  Signaler* signal_wakeup_;                 // 中止talk_base::PhysicalSocketServer::Wait函数的talk_base::Signaler对象
  CriticalSection crit_;
  bool fWait_;
#if defined(WEBRTC_WIN)
  WSAEVENT socket_ev_;
#endif
};


// 内部私有的类，不对外暴露，它主要是对Socket的跨平台封装。
class PhysicalSocket : public AsyncSocket, public sigslot::has_slots<> {
 public:
  PhysicalSocket(PhysicalSocketServer* ss, SOCKET s = INVALID_SOCKET);
  ~PhysicalSocket() override;

  // Creates the underlying OS socket (same as the "socket" function).
  virtual bool Create(int family, int type);

  SocketAddress GetLocalAddress() const override;
  SocketAddress GetRemoteAddress() const override;

  int Bind(const SocketAddress& bind_addr) override;
  // 连接指定的地址和端口，如果地址尚未解析，调用异步的地址解析器
  int Connect(const SocketAddress& addr) override;

  int GetError() const override;
  void SetError(int error) override;

  ConnState GetState() const override;

  int GetOption(Option opt, int* value) override;
  int SetOption(Option opt, int value) override;

  int Send(const void* pv, size_t cb) override;
  int SendTo(const void* buffer,
             size_t length,
             const SocketAddress& addr) override;

  int Recv(void* buffer, size_t length, int64_t* timestamp) override;
  int RecvFrom(void* buffer,
               size_t length,
               SocketAddress* out_addr,
               int64_t* timestamp) override;

  int Listen(int backlog) override;
  AsyncSocket* Accept(SocketAddress* out_addr) override;

  int Close() override;

  SocketServer* socketserver() { return ss_; }

 protected:
  int DoConnect(const SocketAddress& connect_addr);

  // Make virtual so ::accept can be overwritten in tests.
  virtual SOCKET DoAccept(SOCKET socket, sockaddr* addr, socklen_t* addrlen);

  // Make virtual so ::send can be overwritten in tests.
  virtual int DoSend(SOCKET socket, const char* buf, int len, int flags);

  // Make virtual so ::sendto can be overwritten in tests.
  virtual int DoSendTo(SOCKET socket, const char* buf, int len, int flags,
                       const struct sockaddr* dest_addr, socklen_t addrlen);

  void OnResolveResult(AsyncResolverInterface* resolver);

  void UpdateLastError();
  void MaybeRemapSendError();

  uint8_t enabled_events() const { return enabled_events_; }
  virtual void SetEnabledEvents(uint8_t events);
  virtual void EnableEvents(uint8_t events);
  virtual void DisableEvents(uint8_t events);

  static int TranslateOption(Option opt, int* slevel, int* sopt);

  PhysicalSocketServer* ss_;  // 管理的talk_base::SocketServer
  SOCKET s_;                  // socket句柄/文件描述符
  bool udp_;                  // 通信方式是否为UDP
  CriticalSection crit_;
  int error_ RTC_GUARDED_BY(crit_);   // 最后出错码（Last Error）
  ConnState state_;                   // 连接状态
  AsyncResolver* resolver_;           // 异步的网址解析器

#if !defined(NDEBUG)
  std::string dbg_addr_;
#endif

 private:
  uint8_t enabled_events_ = 0;  // 需要监听的IO事件
};

class SocketDispatcher : public Dispatcher, public PhysicalSocket {
 public:
  explicit SocketDispatcher(PhysicalSocketServer *ss);
  SocketDispatcher(SOCKET s, PhysicalSocketServer *ss);
  ~SocketDispatcher() override;

  bool Initialize();

  virtual bool Create(int type);
  bool Create(int family, int type) override;

#if defined(WEBRTC_WIN)
  WSAEVENT GetWSAEvent() override;
  SOCKET GetSocket() override;
  bool CheckSignalClose() override;
#elif defined(WEBRTC_POSIX)
  int GetDescriptor() override;
  bool IsDescriptorClosed() override;
#endif

  uint32_t GetRequestedEvents() override;
  void OnPreEvent(uint32_t ff) override;
  void OnEvent(uint32_t ff, int err) override;

  int Close() override;

#if defined(WEBRTC_USE_EPOLL)
 protected:
  void StartBatchedEventUpdates();
  void FinishBatchedEventUpdates();

  void SetEnabledEvents(uint8_t events) override;
  void EnableEvents(uint8_t events) override;
  void DisableEvents(uint8_t events) override;
#endif

 private:
#if defined(WEBRTC_WIN)
  static int next_id_;
  int id_;
  bool signal_close_;
  int signal_err_;
#endif // WEBRTC_WIN
#if defined(WEBRTC_USE_EPOLL)
  void MaybeUpdateDispatcher(uint8_t old_events);

  int saved_enabled_events_ = -1;
#endif
};

} // namespace rtc

#endif // RTC_BASE_PHYSICALSOCKETSERVER_H_
