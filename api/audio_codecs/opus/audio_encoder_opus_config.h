/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_AUDIO_CODECS_OPUS_AUDIO_ENCODER_OPUS_CONFIG_H_
#define API_AUDIO_CODECS_OPUS_AUDIO_ENCODER_OPUS_CONFIG_H_

#include <stddef.h>

#include <vector>

#include "api/optional.h"

namespace webrtc {

// NOTE: This struct is still under development and may change without notice.
// Opus编码配置参数
struct AudioEncoderOpusConfig {
  //  C++11允许声明constexpr类型来由编译器检验变量的值是否是一个常量表达式。
  //  声明为constexpr的必须是一个常量，并且只能用常量或者常量表达式来初始化。
  // 默认20毫米一帧
  static constexpr int kDefaultFrameSizeMs = 20;

  // Opus API allows a min bitrate of 500bps, but Opus documentation suggests
  // bitrate should be in the range of 6000 to 510000, inclusive.
  static constexpr int kMinBitrateBps = 6000;
  static constexpr int kMaxBitrateBps = 510000;

  AudioEncoderOpusConfig();
  AudioEncoderOpusConfig(const AudioEncoderOpusConfig&);
  ~AudioEncoderOpusConfig();
  AudioEncoderOpusConfig& operator=(const AudioEncoderOpusConfig&);

  bool IsOk() const;  // Checks if the values are currently OK.

  int frame_size_ms;
  size_t num_channels;   // 通道数
  enum class ApplicationMode { kVoip, kAudio };
  ApplicationMode application;

  // NOTE: This member must always be set.
  // TODO(kwiberg): Turn it into just an int.
  rtc::Optional<int> bitrate_bps;

  bool fec_enabled;         // 是否开启FEC(向前纠错)
  bool cbr_enabled;         // 恒定比特率（CBR）和可变比特率（VBR）
  int max_playback_rate_hz; // 音频采样率

  // |complexity| is used when the bitrate goes above
  // |complexity_threshold_bps| + |complexity_threshold_window_bps|;
  // |low_rate_complexity| is used when the bitrate falls below
  // |complexity_threshold_bps| - |complexity_threshold_window_bps|. In the
  // interval in the middle, we keep using the most recent of the two
  // complexity settings.
  int complexity;  // complexity（复杂性）是一个值从1到10,1最低，10最高，值越大越复杂, 作用？
  int low_rate_complexity;
  int complexity_threshold_bps;
  int complexity_threshold_window_bps;

  bool dtx_enabled;   // Discontinuous Transmission不连续,发送在没有语音信号传输时停止发射无线信号，使干扰降低，来提高系统的效率。
  std::vector<int> supported_frame_lengths_ms;
  int uplink_bandwidth_update_interval_ms;

  // NOTE: This member isn't necessary, and will soon go away. See
  // https://bugs.chromium.org/p/webrtc/issues/detail?id=7847
  int payload_type;
};

}  // namespace webrtc

#endif  // API_AUDIO_CODECS_OPUS_AUDIO_ENCODER_OPUS_CONFIG_H_
