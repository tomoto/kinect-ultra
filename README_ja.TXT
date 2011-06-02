kinect-ultra V0.1f (rc)
Author: Tomoto S. Washio
Release Date: yyyy/mm/dd
Project URL: http://code.google.com/p/kinect-ultra/

1. 前提条件

(1) Kinect持ってますね？
(2) Windowsで動きます。テストしたのはWindows XP Pro SP3だけです。
(3) それなりに早いCPUとGPUが必要だと思います。
　　- どこが最低ラインかはわかりません。
　　- 少なくともGPUはOpenGL 2.x以上、プログラマブルシェーダ、
　　　VAOをサポートしている必要があります。
　　- Core i7 920とGeForce GTX 285で快適に動いています。
    動作報告のあった構成が下記のページにあるので参考にしてください。
    http://code.google.com/p/kinect-ultra/wiki/WorkingConfigurations

2. インストール

(1) もしまだであれば、OpenNIをダウンロード、インストールしてください。
    http://www.openni.org/downloadfiles/2-openni-binaries

(2) もしまだであれば、NITEをダウンロード、インストールしてください。
    http://www.openni.org/downloadfiles/12-openni-compliant-middleware-binaries

    ライセンスキーはこれを使ってください: 0KOIk2JeIBYClPWVnMoRKn5cdY4=

(3) もしまだであれば、OpenNI互換Kinectドライバをダウンロード、
    インストールしてください。
    https://github.com/avin2/SensorKinect/tree/master/Bin

    OpenKinectなど、他のドライバを既にインストールしている場合、
    ドライバを切り替える必要があるでしょう。

    これらのソフトウェアの推奨バージョンは、現在知る限り、以下の通りです。
    推奨バージョンは変わる可能性があるので、プロジェクトサイトを見て
    最新情報をチェックしてください。
    - OpenNI unstable v1.1.0.41: http://www.openni.org/downloadfiles/openni-binaries/latest-unstable/108-openni-unstable-build-for-windows-x86-32-bit-v1-1-0/download
    - NITE unstable v1.3.1.5: http://www.openni.org/downloadfiles/openni-compliant-middleware-binaries/latest-unstable/115-primesense-nite-unstable-build-for-windows-x86-32-bit-v1-3-1/download
    - SensorKinect driver for unstable OpenNI 5.0.1: https://github.com/avin2/SensorKinect/blob/unstable/Bin/SensorKinect-Win-OpenSource32-5.0.1.msi

(4) このアーカイブを好きなところに展開してください。

3. 使い方

(1) kinect-ultra.exeを実行するだけです。作業ディレクトリは実行ファイルと
    同じ場所にしてください。

(2) 下記でデモが見られます。
    - 日本語
      http://www.youtube.com/watch?v=eCbURRDUUdI
      http://www.youtube.com/watch?v=kxvn98lqr5Y
    - 英語
      http://www.youtube.com/watch?v=RUG-Uvq-J-w
  　　http://www.youtube.com/watch?v=Uuq9SCL_LXY

(3) 問題があった場合...
    - 次のページのFAQを参照ください: http://code.google.com/p/kinect-ultra/wiki/FAQ_ja
    - 次のページに報告お願いします: http://code.google.com/p/kinect-ultra/issues

4. ライセンス

LICENSE.TXTを見てください。

5. ビルド方法

自分でソースからビルドしたり変更したいという方は、プロジェクトページから
ソースコードをダウンロードし、build/kinect-ultra.slnをVisual C++ 2008で
開いてください。デバッガを使うには、プロジェクトの設定で作業ディレクトリを
".." に設定してください。

Enjoy!
Tomoto
