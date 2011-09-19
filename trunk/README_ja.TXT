kinect-ultra V0.1f
Author: Tomoto S. Washio
Release Date: 2011/9/18
Project URL: http://code.google.com/p/kinect-ultra/

0. これは何？

ウルトラセブンのようなものに変身できるKinect Hackです。

1. 前提条件

(1) Kinect持ってますね？
(2) Windowsで動きます。テストしたのはWindows XP Pro SP3 32bitと
    Windows 7 Pro 64bitだけです。
(3) それなりに早いCPUとGPUが必要だと思います。
　　- どこが最低ラインかはわかりません。
　　- 少なくともGPUはOpenGL 2.x以上、プログラマブルシェーダ、
　　　VAOをサポートしている必要があります。
　　- Core i7 920とGeForce GTX 285で快適に動いています。
    動作報告のあった構成が下記のページにあるので参考にしてください。
    http://code.google.com/p/kinect-ultra/wiki/WorkingConfigurations

2. インストール

(1) もしまだであれば、OpenNIをダウンロード、インストールしてください。
    http://www.openni.org/downloadfiles/opennimodules/2-openni-binaries

(2) もしまだであれば、NITEをダウンロード、インストールしてください。
    http://www.openni.org/downloadfiles/opennimodules/12-openni-compliant-middleware-binaries

    ライセンスキーはこれを使ってください: 0KOIk2JeIBYClPWVnMoRKn5cdY4=

(3) もしまだであれば、OpenNI互換Kinectドライバをダウンロード、
    インストールしてください。
    https://github.com/avin2/SensorKinect/tree/master/Bin

    OpenKinectなど、他のドライバを既にインストールしている場合、
    ドライバを切り替える必要があるでしょう。

    上記ソフトウェアの推奨バージョンを、現在知る限りにおいて、
    下記ページに掲載しています。推奨バージョンは変わる可能性が
    あるので、ときどきサイトを見て最新情報をチェックしてください。
    http://code.google.com/p/kinect-ultra/wiki/OpenNI_Downloads

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
ソースコードをダウンロードし、build/kinect-kamehameha.slnをVisual C++ 2008
または2010で開いてください。デバッガを使うには、プロジェクトの設定で
作業ディレクトリを ".." に設定してください。

Enjoy!
Tomoto
