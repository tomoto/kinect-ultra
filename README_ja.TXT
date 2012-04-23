kinect-ultra V1.0a
Author: Tomoto S. Washio
Release Date: 2012/4/23
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
    - Core i7 2600とGeForce GTX 285で快適に動いています。
    動作報告のあった構成が下記のページにあるので参考にしてください。
    http://code.google.com/p/kinect-ultra/wiki/WorkingConfigurations

2. インストール

2.1 Kinect SDK版

(1) もしまだであれば、Kinect SDKをダウンロード、インストールしてください。
    http://www.kinectforwindows.org

(2) このアーカイブを好きなところに展開してください。

2.2 OpenNI版

(1) もしまだであれば、OpenNIと関連ソフトウェア一式をダウンロード、
    インストールしてください。やり方がよく変わるので次のページを
    参考にしてください。
    http://code.google.com/p/kinect-ultra/wiki/OpenNI_Downloads

    OpenKinectなど、他のドライバを既にインストールしている場合、
    ドライバを切り替える必要があるでしょう。

(2) このアーカイブを好きなところに展開してください。

3. 使い方

(1) kinect-ultra.exeを実行するだけです。
    - 作業ディレクトリは実行ファイルと同じ場所にしてください。
    - [ESC]キーで終了します。
    - [Enter]キーで画面を最大化します。

(2) 下記でデモが見られます。
    - 日本語
      http://www.youtube.com/watch?v=eCbURRDUUdI
      http://www.youtube.com/watch?v=kxvn98lqr5Y
    - 英語
      http://www.youtube.com/watch?v=RUG-Uvq-J-w
      http://www.youtube.com/watch?v=Uuq9SCL_LXY
    
    変身方法はデモと変わっているのでご注意ください。

(3) 問題があった場合...
    - 次のページのFAQを参照ください: http://code.google.com/p/kinect-ultra/wiki/FAQ_ja
    - 次のページに報告お願いします: http://code.google.com/p/kinect-ultra/issues

4. ライセンス

LICENSE.TXTを見てください。

5. ビルド方法

自分でソースからビルドしたり変更したいという方は、プロジェクトページから
ソースコードをダウンロードし、build2010/kinect-ultra.slnをVisual C++ 2010で
開いてください。デバッガを使うには、プロジェクトの設定で作業ディレクトリを
".." に設定してください。

Enjoy!
Tomoto
