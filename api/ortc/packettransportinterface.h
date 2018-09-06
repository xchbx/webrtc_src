/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_ORTC_PACKETTRANSPORTINTERFACE_H_
#define API_ORTC_PACKETTRANSPORTINTERFACE_H_

namespace rtc {

class PacketTransportInternal;

}  // namespace rtc

namespace webrtc {

// Base class for different packet-based transports.
// 基于包的传输的类接口
class PacketTransportInterface {
 public:
  virtual ~PacketTransportInterface() {}

 protected:
  // Only for internal use. Returns a pointer to an internal interface, for use
  // by the implementation.
  virtual rtc::PacketTransportInternal* GetInternal() = 0;

  // Classes that can use this internal interface.
  friend class RtpTransportControllerAdapter;
  friend class RtpTransportAdapter;
};

}  // namespace webrtc

#endif  // API_ORTC_PACKETTRANSPORTINTERFACE_H_
