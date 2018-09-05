## peerconnection分析

### 流程分析

#### Client连接服务器

##### 客户端处理

+ MainWnd::OnDefaultAction里面接收用户单击“connect"的事件。

+ 委托给回调Conductor::StartLogin执行连接操作。

+ Conductor::StartLogin里再命令PeerConnectionClient::Connect执行连接操作。
  在该函数里，若服务器地址未解析，需要先解析，然后才开始连接。 当需要解析时，若PeerConnectionClient::OnResolveResult解析成功，才继续执行。

+ 当解析成功，调用PeerConnectionClient::DoConnect执行连接操作, 该函数对socket做一些相关的初始化工作。

+ 当相关的初始化工作完成后，调用PeerConnectionClient::ConnectControlSocket来向server发送连接命令。

+ 若连接成功，则回调PeerConnectionClient::OnConnect被调用, 这里准备sign_in数据包(并没有发送)，发起连接请求,连接完成会在OnConnect中发送登入命令。

+ **服务器执行相关的处理工作。**

  + 保存客户端信息，持续监测连接。

+ peer在PeerConnectionClient::OnRead(从对端获取数据触发Read事件，所以OnRead函数被触发)里接收到服务器端的数据包，并确定自己登录成功，并使用hanging_get_开始连接server。

+ 连接成功后，在PeerConnectionClient::OnHangingGetConnect里面向服务器发送/wait请求。

  ​

#### 当Peer A请求与Peer B通话时

##### Peer A部分

+ 用户在MainWnd::OnDefaultAction里触发通话命令LIST_PEERS。

+ 其在回调Conductor::ConnectToPeer里执行相关连接工作,该回调会调用InitializePeerConnection执行一些初始化工作并用CreateOffer创建发送端，设置Conductor为回调类。

+ 当Conductor创建Offer成功后, 回调Conductor::OnSuccess被调用, 该函数里面构建**一个json的sdp数据**发送给server(类型为offer)。

#####  服务器处理peer的请求和另一端聊天的数据

+ 服务器接收POST /message?peer_id=my_id&to=peer_id 信息
+ 发送SDP信息给对方

#####  Peer B部分

+ PeerConnectionClient::OnHangingGetRead会收到服务器传来的开启视频会话的命令
+ PeerConnectionClient::OnMessageFromPeer里面继续处理，交给其回调Conductor::OnMessageFromPeer来处理
  + 设置对端SDP
+ 创建响应CreateAnswer,Conductor::OnSuccess里面发送自己的sdp信息给对方



### 信令分析

#### 登入

```http
# name表示登入客户的名字
GET /sign_in?name 

# name表示登入客户的名字
GET /sign_out?name 

# my_id表示客户端的id
GET /wait?peer_id=my_id

# my_id 消息的发送方，peer_id 消息的接受方
POST /message?peer_id=my_id&to=peer_id
Body: sdp...
内容类似:
Client fx@fx-w10 sending to frank@DESKTOP-RDP2DU7
 {
   "sdp" : "v=0\r\no=- 5654960704494368350 2 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\na=group:BUNDLE audio video\r\na=msid-semantic: WMS stream_id\r\nm=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 102 0 8 106 105 13 110 112 113 126\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:yF1e\r\na=ice-pwd:KxvwUJ4UDBITOBy2MbZ5pZRx\r\na=ice-options:trickle\r\na=fingerprint:sha-256 4E:65:F4:56:C1:9D:EF:43:CA:81:E4:82:76:49:C9:98:E3:A1:0E:1C:90:07:BF:7F:4C:6B:69:F7:0E:2D:75:86\r\na=setup:actpass\r\na=mid:audio\r\na=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\na=sendrecv\r\na=rtcp-mux\r\na=rtpmap:111 opus/48000/2\r\na=rtcp-fb:111 transport-cc\r\na=fmtp:111 minptime=10;useinbandfec=1\r\na=rtpmap:103 ISAC/16000\r\na=rtpmap:104 ISAC/32000\r\na=rtpmap:9 G722/8000\r\na=rtpmap:102 ILBC/8000\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=rtpmap:106 CN/32000\r\na=rtpmap:105 CN/16000\r\na=rtpmap:13 CN/8000\r\na=rtpmap:110 telephone-event/48000\r\na=rtpmap:112 telephone-event/32000\r\na=rtpmap:113 telephone-event/16000\r\na=rtpmap:126 telephone-event/8000\r\na=ssrc:1837448244 cname:skwRLqIN20RZNAyM\r\na=ssrc:1837448244 msid:stream_id audio_label\r\na=ssrc:1837448244 mslabel:stream_id\r\na=ssrc:1837448244 label:audio_label\r\nm=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 127\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:yF1e\r\na=ice-pwd:KxvwUJ4UDBITOBy2MbZ5pZRx\r\na=ice-options:trickle\r\na=fingerprint:sha-256 4E:65:F4:56:C1:9D:EF:43:CA:81:E4:82:76:49:C9:98:E3:A1:0E:1C:90:07:BF:7F:4C:6B:69:F7:0E:2D:75:86\r\na=setup:actpass\r\na=mid:video\r\na=extmap:2 urn:ietf:params:rtp-hdrext:toffset\r\na=extmap:3 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:4 urn:3gpp:video-orientation\r\na=extmap:5 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\r\na=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type\r\na=extmap:8 http://www.webrtc.org/experiments/rtp-hdrext/video-timing\r\na=sendrecv\r\na=rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:96 VP8/90000\r\na=rtcp-fb:96 goog-remb\r\na=rtcp-fb:96 transport-cc\r\na=rtcp-fb:96 ccm fir\r\na=rtcp-fb:96 nack\r\na=rtcp-fb:96 nack pli\r\na=rtpmap:97 rtx/90000\r\na=fmtp:97 apt=96\r\na=rtpmap:98 VP9/90000\r\na=rtcp-fb:98 goog-remb\r\na=rtcp-fb:98 transport-cc\r\na=rtcp-fb:98 ccm fir\r\na=rtcp-fb:98 nack\r\na=rtcp-fb:98 nack pli\r\na=rtpmap:99 rtx/90000\r\na=fmtp:99 apt=98\r\na=rtpmap:100 red/90000\r\na=rtpmap:101 rtx/90000\r\na=fmtp:101 apt=100\r\na=rtpmap:127 ulpfec/90000\r\na=ssrc-group:FID 877323008 282751725\r\na=ssrc:877323008 cname:skwRLqIN20RZNAyM\r\na=ssrc:877323008 msid:stream_id video_label\r\na=ssrc:877323008 mslabel:stream_id\r\na=ssrc:877323008 label:video_label\r\na=ssrc:282751725 cname:skwRLqIN20RZNAyM\r\na=ssrc:282751725 msid:stream_id video_label\r\na=ssrc:282751725 mslabel:stream_id\r\na=ssrc:282751725 label:video_label\r\n",
   "type" : "offer"
}


{
   "candidate" : "candidate:2999745851 1 udp 2122260223 192.168.56.1 57256 typ host generation 0 ufrag yF1e network-id 3 network-cost 50",
   "sdpMLineIndex" : 0,
   "sdpMid" : "audio"
}.....


```

#### 参考资料

+ [WebRTC中peerconnectclient&server 简析](https://blog.csdn.net/oldmtn/article/details/49778099)
+ [WebRTC手记之初探](http://www.cnblogs.com/fangkm/p/4364553.html)
+ [WebRTC手记之框架与接口](http://www.cnblogs.com/fangkm/p/4370492.html)