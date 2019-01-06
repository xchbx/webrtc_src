/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

package org.webrtc;

import org.webrtc.CameraEnumerationAndroid.CaptureFormat;

import java.util.List;

/*
 * CameraEnumerator 摄像头枚举器接口,用于获取全部摄像头的名字、判断前后置
 * 创建CameraVideoCapturer、获取支持的CaptureFormat
 * */
public interface CameraEnumerator {
  public String[] getDeviceNames();
  public boolean isFrontFacing(String deviceName);
  public boolean isBackFacing(String deviceName);
  public List<CaptureFormat> getSupportedFormats(String deviceName);

  public CameraVideoCapturer createCapturer(
      String deviceName, CameraVideoCapturer.CameraEventsHandler eventsHandler);
}
