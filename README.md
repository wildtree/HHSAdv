High High School Adventure for M5Stack Core/Core2 / M5Cardputer

ハイハイスクールアドベンチャーのM5Stack Core/Core2 および M5Cardputer 対応版です。
M5Unified, M5GFX, M5Cardputer 依存です。

データファイル群(*.dat)はSDカードに /HHSAdv を作りその中に入れてください。

ver 1.2 からBLEキーボードをサポートしました。
Faces などのキーボードがつながっていなかったら、BLEキーボードを探すようにしたかったのですが、Facesがつながっているかどうかの判別方法がわからなかったため、BtnAを長押ししていたらBLEキーボードを探すようにしました。

なお、BLEはNimBLEを使っているため、古いBTキーボードは使えません。BLEのものをご使用ください。また、ハイハイスクールアドベンチャーを遊んでいる限りはあまり関係ないですが、キーマップは英字配列を想定しています。

BLEキーボードに関しては、手探りの状態で、古めのBLEキーボードはどうやらうまく動きません。接続できるものの通知が来ないためキーを受け取れないのです。BuffaloのBSKBB335は使えますが、https://github.com/geo-tp/Bluetooth-Keyboard-Mouse-Emulator.git を使って CardpuerをBLEキーボード化したものは使えません。ただ、このアプリのBLEスタックをNimBLEで置き換えると動きます。

現状、BLEキーボードを選ぶ状態であることは間違えありません。

画面のスケーリングをサポートしました。一応、M5.Display.width(), M5.Display.height()が表示装置と一致していれば、自動的にスケーリングしますが、この判別がうまく行かなかったとき、CTRL+Gで1.0->0.75->0.67->1.0と倍率を変更できます。