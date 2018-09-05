
/*
 *  Copyright 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_PEERCONNECTIONFACTORY_H_
#define PC_PEERCONNECTIONFACTORY_H_

#include <memory>
#include <string>

#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"
#include "media/sctp/sctptransportinternal.h"
#include "pc/channelmanager.h"
#include "rtc_base/rtccertificategenerator.h"
#include "rtc_base/scoped_ref_ptr.h"
#include "rtc_base/thread.h"

namespace rtc {
class BasicNetworkManager;
class BasicPacketSocketFactory;
}

namespace webrtc {

class RtcEventLog;

// PeerConnection工厂对象
class PeerConnectionFactory : public PeerConnectionFactoryInterface {
 public:
  // Use the overloads of CreateVideoSource that take raw VideoCapturer
  // pointers from PeerConnectionFactoryInterface.
  // TODO(deadbeef): Remove this using statement once those overloads are
  // removed.
  using PeerConnectionFactoryInterface::CreateVideoSource;

  // PeerConnectionFactoryInterface::Options 传输方面的能力设置
  void SetOptions(const Options& options) override;

  // Deprecated, use version without constraints.
  rtc::scoped_refptr<PeerConnectionInterface> CreatePeerConnection(
      const PeerConnectionInterface::RTCConfiguration& configuration,
      const MediaConstraintsInterface* constraints,
      std::unique_ptr<cricket::PortAllocator> allocator,
      std::unique_ptr<rtc::RTCCertificateGeneratorInterface> cert_generator,
      PeerConnectionObserver* observer) override;

  // 创建PeerConnection对象实例
  rtc::scoped_refptr<PeerConnectionInterface> CreatePeerConnection(
      const PeerConnectionInterface::RTCConfiguration& configuration,           // RTC配置信息，ice等
      std::unique_ptr<cricket::PortAllocator> allocator,                        // ???
      std::unique_ptr<rtc::RTCCertificateGeneratorInterface> cert_generator,    // 认证证书生成
      PeerConnectionObserver* observer) override;                               // 事件观察者对象

  bool Initialize();

  // 创建本地媒体流
  rtc::scoped_refptr<MediaStreamInterface> CreateLocalMediaStream(
      const std::string& stream_id) override;

  // 创建音频数据源
  rtc::scoped_refptr<AudioSourceInterface> CreateAudioSource(
      const cricket::AudioOptions& options) override;
  // Deprecated, use version without constraints.
  rtc::scoped_refptr<AudioSourceInterface> CreateAudioSource(
      const MediaConstraintsInterface* constraints) override;

  // 创建视频数据源
  rtc::scoped_refptr<VideoTrackSourceInterface> CreateVideoSource(
      std::unique_ptr<cricket::VideoCapturer> capturer) override;
  // This version supports filtering on width, height and frame rate.
  // For the "constraints=null" case, use the version without constraints.
  // TODO(hta): Design a version without MediaConstraintsInterface.
  // https://bugs.chromium.org/p/webrtc/issues/detail?id=5617
  rtc::scoped_refptr<VideoTrackSourceInterface> CreateVideoSource(
      std::unique_ptr<cricket::VideoCapturer> capturer,
      const MediaConstraintsInterface* constraints) override;

  rtc::scoped_refptr<VideoTrackInterface> CreateVideoTrack(
      const std::string& id,
      VideoTrackSourceInterface* video_source) override;

  rtc::scoped_refptr<AudioTrackInterface>
      CreateAudioTrack(const std::string& id,
                       AudioSourceInterface* audio_source) override;

  bool StartAecDump(rtc::PlatformFile file, int64_t max_size_bytes) override;
  void StopAecDump() override;

  virtual std::unique_ptr<cricket::SctpTransportInternalFactory>
  CreateSctpTransportInternalFactory();

  virtual cricket::ChannelManager* channel_manager();
  virtual rtc::Thread* signaling_thread();
  virtual rtc::Thread* worker_thread();
  virtual rtc::Thread* network_thread();
  const Options& options() const { return options_; }

 protected:
  // 两个构造函数区别在于是否带FecControllerFactoryInterface
  PeerConnectionFactory(
      rtc::Thread* network_thread,                      // 可以传入nullptr
      rtc::Thread* worker_thread,                       // 可以传入nullptr
      rtc::Thread* signaling_thread,                    // 可以传入nullptr
      std::unique_ptr<cricket::MediaEngineInterface> media_engine,
      std::unique_ptr<webrtc::CallFactoryInterface> call_factory,
      std::unique_ptr<RtcEventLogFactoryInterface> event_log_factory);
  PeerConnectionFactory(
      rtc::Thread* network_thread,
      rtc::Thread* worker_thread,
      rtc::Thread* signaling_thread,
      std::unique_ptr<cricket::MediaEngineInterface> media_engine,
      std::unique_ptr<webrtc::CallFactoryInterface> call_factory,
      std::unique_ptr<RtcEventLogFactoryInterface> event_log_factory,
      std::unique_ptr<FecControllerFactoryInterface> fec_controller_factory);
  virtual ~PeerConnectionFactory();

 private:
  std::unique_ptr<RtcEventLog> CreateRtcEventLog_w();
  std::unique_ptr<Call> CreateCall_w(RtcEventLog* event_log);

  bool wraps_current_thread_;
  rtc::Thread* network_thread_;
  rtc::Thread* worker_thread_;
  rtc::Thread* signaling_thread_;
  std::unique_ptr<rtc::Thread> owned_network_thread_;                        // 外部没有传入线程的时候使用
  std::unique_ptr<rtc::Thread> owned_worker_thread_;                         // 外部没有传入线程的时候使用
  Options options_;
  std::unique_ptr<cricket::ChannelManager> channel_manager_;                 // 引擎的操作对象
  std::unique_ptr<rtc::BasicNetworkManager> default_network_manager_;
  std::unique_ptr<rtc::BasicPacketSocketFactory> default_socket_factory_;
  std::unique_ptr<cricket::MediaEngineInterface> media_engine_;               // 音视频引擎
  std::unique_ptr<webrtc::CallFactoryInterface> call_factory_;
  std::unique_ptr<RtcEventLogFactoryInterface> event_log_factory_;
  std::unique_ptr<FecControllerFactoryInterface> fec_controller_factory_;
};

}  // namespace webrtc

#endif  // PC_PEERCONNECTIONFACTORY_H_
