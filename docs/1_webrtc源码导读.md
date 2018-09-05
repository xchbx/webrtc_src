## Webrtc源码导读

#### 一：源码结构

```json
├── ./api   // WebRTC 接口层,各浏览器都是通过该接口层调用的 WebRTC。
│   ├── audio_codecs  
├── ./audio // 存放音频网络逻辑层相关的代码。音频数据逻辑上的发送，接收等代码。
├── ./build // 编译脚本，gyp
├── ./buildtools build_overrides
├── ./build_overrides
├── ./call  // 存放的是 WebRTC "呼叫（Call）" 相关逻辑层的代码。
├── ./common_audio  // 存放一些音频的基本算法。包括环形队列，博利叶算法，滤波器等。
│   ├── ./common_audio/include  // 就一个类型转换头文件
│   ├── ./common_audio/resampler    // 音频重采样代码
│   ├── ./common_audio/signal_processing    // 音频信号处理代码，和硬件平台有关，有汇编代码
│   └── ./common_audio/vad  // vad代码
├── ./common_video  // 存放了视频算法相关的常用工具，如libyuv, sps/pps分析器，I420缓冲器等。
├── ./examples // 例子
├── ./modules
│   ├── ./modules/audio_coding  // 音频编解码相关代码
│   ├── ./modules/audio_mixer    // 音频合成代码
│   ├── ./modules/audio_device  // audio设备处理代码，采集和放音，android，ios，linux，mac，win
│   ├── ./modules/audio_processing // 音频前后端处理，aec，aecm,agc,beamformer,ns,transient
│   ├── ./modules/bitrate_controller    // 码率模块控制代码
│   ├── ./modules/desktop_capture // 桌面抓拍处理代码和各平台处理代码,mac,win,x11
│   ├── ./modules/pacing    // 码率探测代码
│   ├── ./modules/remote_bitrate_estimator  // 远端码率计算
│   ├── ./modules/rtp_rtcp // rtp、rtcp的处理代码，封装解封装，各种codec的不同处理、fec
│   ├── ./modules/utility
│   ├── ./modules/video_capture // 视频摄像头采集代码，android、ios、linux、mac、win
│   ├── ./modules/video_coding // 视频codec处理代码，i420、vp8、vp9
│   ├── ./modules/video_processing // 视频前后处理，brighten，color enhancement，deflickering，spatial resampler等
│   └── ./modules/video_render  // 视频渲染代码，android，ios、linux、mac、windows、opengles
├── ./p2p // nat穿越代码，turn/stun等，服务器和客户端
├── ./pc  // 未知
├── ./sound // 未知
├── ./system_wrappers // 系统api封装
├── ./test
├── ./tools // 音视频测试工具代码
├── ./video // 未知
├── ./media // 存放媒体相关的代码
```

#### 二: 源码使用
+ 编译

  ```sh
  gn gen out/Default --ide=vs2017
  ninja -C out/Default         # 编译全部
  ninja -C out/Default webrtc  # 全部编译在一起(src目录下面的BUILD.gn)
  ninja -C out/Default rtc_base_generic # 只编译rtc_base_generic库
  ```

  
#### 参考资料

+ [《最新 WebRTC 源码目录结构分析》](https://blog.csdn.net/garrylea/article/details/77899109)