# CGI プログラム #

SSGNC には CGI プログラムが同梱されているので，簡単に検索ウェブサービスを提供することが簡単にできます．設定方法については [Introduction](Introduction.md) をご覧ください．本ドキュメントでは，CGI プログラムにより提供される Web API について説明します．


---


## リクエスト ##

本 CGI プログラムは GET リクエストによる検索を想定しているため，URL のクエリ文字列を検索条件として使用します．検索条件として指定できるパラメータは以下のとおりです．

| **パラメータ** | **値** | **説明** |
|:----------|:------|:-------|
| q         | 文字列   | 空白区切りのトークン列（UTF-8） |
| o         | 文字列   | マッチングの条件（UNORDERED, ORDERED, PHRASE, FIXED） |
| f         | 数値     | 頻度の下限  |
| t         | 数値範囲  | 検索対象とする N-gram の構成トークン数 |
| r         | 数値     | 検索結果として受け取る N-gram 数の上限 |
| i         | 数値     | 検索時に読み込むバイト数の上限 |
| c         | 文字列   | 検索結果の書式（HTML, TEXT, XML） |

  * 空白区切りのトークン列
    * 記号 `"*"` は任意のトークンに一致します．

  * マッチングの条件
    * UNORDERED: 指定されたトークンの順序を考慮しません．
    * ORDERED: 指定されたトークンの順序を考慮します．
    * PHRASE: 指定されたトークンが連続して出現する場合にマッチします．
    * FIXED: 指定されたトークンのみが連続して出現する場合にマッチします．他とは異なり，指定していないトークンの出現を許容しません．

  * パラメータの数値（f 以外）
    * 0 を指定すると，範囲なし，あるいは上限なしになります．ただし，CGI プログラム側で設定されている上限を超えることはできません．


---


## HTML 形式のレスポンス ##

トークン列が空であれば検索フォームを返し，空でなければ検索結果を HTML 文書として返します．

  * サンプル
    * 検索フォーム：http://s-yata.jp/ssgnc/word
    * 検索結果：http://s-yata.jp/ssgnc/word?q=%E6%9C%89%E6%A9%9F+*&o=ordered&r=2&c=html

```
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
 "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <title>SSGNC</title>
    <style type="text/css">
    <!--
      table { border: 2px solid; margin: 10px; padding: 5px; }
      table caption { caption-side: top; font-size: 125%;
       padding: 0px 0px 10px 0px; }
      table tr td { padding: 5px 10px; }
      table tr td.last { padding: 5px 10px 10px 10px; }
      table tr td.link { border-top: 1px solid; text-align: right; }
      a { color: inherit; text-decoration: none; }
    -->
    </style>
  </head>
  <body>
    <div class="result">
      <span class="token">有機</span>
      <span class="token">野菜</span>
      <span class="freq">18700</span>
    </div>
    <div class="result">
      <span class="token">有機</span>
      <span class="token">栽培</span>
      <span class="freq">16300</span>
    </div>
  </body>
</html>
```


---


## TEXT 形式のレスポンス ##

検索結果をタグなしのテキストとして返します．トークン同士の間は半角空白，トークンと頻度の間は水平タブにより区切られます．

  * サンプル
    * 検索結果：http://s-yata.jp/ssgnc/word?q=%E6%9C%89%E6%A9%9F+*&o=ordered&r=2&c=text

```
有機 野菜	18700
有機 栽培	16300
```


---


## XML 形式のレスポンス ##

リクエストの内容と検索結果を XML 文書として返します．

  * サンプル
    * 検索結果：http://s-yata.jp/ssgnc/word?q=%E6%9C%89%E6%A9%9F+*&o=ordered&r=2&c=xml

```
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<search>
  <query>
    <str>有機 *</str>
      <min_freq>10</min_freq>
      <min_num_tokens>0</min_num_tokens>
      <max_num_tokens>0</max_num_tokens>
      <max_num_results>2</max_num_results>
      <io_limit>262144</io_limit>
      <order>fixed</order>
    </query>
  <results>
    <result>
      <token>有機</token>
      <token>野菜</token>
      <freq>18700</freq>
    </result>
    <result>
      <token>有機</token>
      <token>栽培</token>
      <freq>16300</freq>
    </result>
  </results>
</search>
```