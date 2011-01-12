kinect-ultra V0.1
Author: Tomoto S. Washio
Release Date: 1/9/2011
Project URL: http://code.google.com/p/kinect-ultra/

1. 前提条件

(1) Kinect持ってますね？
(2) Windowsで動きます。テストしたのはWindows XP Pro SP3だけです。
(3) それなりに早いCPUとグラフィックカードが必要だと思います。
    どこが最低ラインかはわかりません。少なくとも、Core i7 920と
    GeForce GTX 285で快適に動いています。

2. インストール

(1) もしまだであれば、OpenNIをダウンロード、インストールしてください。
    http://www.openni.org/downloadfiles/openni-binaries/stable/26-openni-alpha-build-for-windows-v1-0-0/download

(2) もしまだであれば、OpenNI互換Kinectドライバをダウンロード、
　　インストールしてください。
    https://github.com/avin2/SensorKinect 

    OpenKinectなど、他のドライバを既にインストールしている場合、
    ドライバを切り替える必要があるでしょう。

(3) このアーカイブを好きなところに展開してください。

3. 使い方

(1) kinect-ultra.exeを実行するだけです。作業ディレクトリは実行ファイルと
    同じ場所にしてください。
(2) 下記でデモが見られます。
    http://www.youtube.com/watch?v=RUG-Uvq-J-w (English)
    http://www.youtube.com/watch?v=eCbURRDUUdI (Japanese).

4. ライセンス

LICENSE.TXTを見てください。

5. ビルド方法

自分でソースからビルドしたり変更したいという方は、プロジェクトページから
ソースコードをダウンロードし、build/kinect-ultra.slnをVisual C++ 2008で
開いてください。デバッガを使うには、プロジェクトの設定で作業ディレクトリを
".." に設定してください。

Enjoy!
Tomoto
