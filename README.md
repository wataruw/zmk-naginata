# ZMK 薙刀式 フォーク版

**eswaiさんの「ZMK 薙刀式」をフォークしてCornix専用にカスタマイズしました。**

---

ZMK Firmwareで薙刀式かな入力を実現します。
薙刀式は大岡俊彦氏が考案されたかな入力方式です。

http://oookaworks.seesaa.net/article/456099128.html

## GitHub Actionsでbuildはしますが……

[wataruw/zmk-keyboard-cornix](https://github.com/wataruw/zmk-keyboard-cornix.git) のGitHub Actionsから、本リポジトリが呼び出され、リンクされたうえで、オブジェクトが生成されます。
本リポジトリ単独ではオブジェクトは生成できません。

## キーマップも本リポジトリのは使いません……

Cornix＋薙刀式のキーマップは、[wataruw/zmk-keyboard-cornix](https://github.com/wataruw/zmk-keyboard-cornix.git)のconfig/cornix.keymapです。レイヤー3に薙刀式のキーマップが定義されています。
**&ng** xx を本リポジトリで読み込み動作します。

## つまり本リポジトリは……

薙刀式の動作のみを定義しています。
