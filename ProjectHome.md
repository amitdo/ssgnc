# Search System for Giga-scale N-gram Corpus #

The SSGNC is a search system designed for N-gram corpus of around 100GB. The first version was designed for the Google N-gram Corpus and thus the SSGNC was short for Search System for Google N-gram Corpus. But now the system is applicable to other N-gram corpus, so currently the G of the SSGNC means the initial letter of Giga-scale.

This system uses a kind of inverted index for finding specified N-grams but the index structure natively supports only a simple search function to find N-grams containing one of the given tokens. So this system provides filtering functions to find N-grams containing all the given tokens or to handle queries containing wildcards.

## Search Features ##

The latest SSGNC can handle the following kinds of queries.

  * **Unordered**: Unordered boolean AND query
    * A query "A B" matches both "A B" and "B A". N-grams containing tokens other than "A" and "B" are acceptable.
  * **Ordered**: Ordered boolean AND query
    * A query "A B" matches "A B" and does not match "B A". N-grams containing tokens other than "A" and "B" are acceptable.
  * **Phrase**: Phrase query
    * A query "A B" matches "A B" and does not match "B A" and "A C B". N-grams containing tokens before and/or after "A B" are acceptable.
  * **Fixed**: Fixed length query
    * A query "A B" matches only "A B".

If a wildcard (`*`) appears in query, it mathces any token of N-grams. Also, there are 4 customizable parameters: 1. the lower limit of frequency, 2. the number of tokens, 3. the maximum number of search results (N-grams), 4. the upper limit of disk I/O.

## English N-gram Corpus ##

  * All Our N-gram are Belong to You
    * http://googleresearch.blogspot.com/2006/08/all-our-n-gram-are-belong-to-you.html


---


# 大規模 N-gram コーパス検索システム #

SSGNC は，100GB 程度の N-gram コーパスを対象として設計された検索システムです．初期バージョンの開発段階では Google N-gram コーパスくらいしか検索対象がなかったため Search System for Google N-gram Corpus の略でしたが，他のコーパスにも使うようになったことから，G の部分を Giga-scale に置き換えることにしました．

本システムにより構築される検索用のデータ構造は，転置索引を N-gram コーパス用に拡張したもので，特定のトークン（Unigram）を含む N-gram を列挙する程度の基本機能しか持っていません．AND 検索やフレーズ検索，ワイルドカードを用いた検索については，基本機能により取得した N-gram にフィルタをかけることで実現しています．

## 検索機能 ##

最新版のライブラリで提供している検索機能は以下のとおりです．

  * **Unordered**: トークンの出現順序を考慮しない AND 検索
    * "A B" というクエリは，"A B" と "B A" の両方にマッチします．"A" と "B" 以外のトークンを含む N-gram にもマッチします．
  * **Ordered**: トークンの出現順序を考慮する AND 検索
    * "A B" というクエリは，"A B" にマッチする一方，"B A" にはマッチしません．Unordered と同様に，"A" と "B" 以外のトークンを含む N-gram にもマッチします．
  * **Phrase**: フレーズ検索
    * "A B" というクエリは，"A B" にはマッチするものの，"B A" や "A C B" にはマッチしません．"A B" というフレーズの前後に他のトークンが現れる N-gram にもマッチします．
  * **Fixed**: 固定長検索
    * "A B" というクエリは，"A B" のみにマッチします．

クエリには，任意のトークンとマッチするワイルドカード（`*`）を含めることができます．また，検索時に指定できる項目としては，頻度の下限，構成トークン数，検索結果として受け取る N-gram 数の上限，ディスク I/O の上限があります．

## 日本語 N-gram コーパス ##

  * 大規模日本語 n-gram データの公開
    * http://googlejapan.blogspot.com/2007/11/n-gram.html
  * N-gram コーパス - 日本語ウェブコーパス 2010
    * http://s-yata.jp/corpus/nwc2010/ngrams/

## サンプル ##

  * 形態素 N-gram 検索
    * http://s-yata.jp/ssgnc/word
  * 文字 N-gram 検索
    * http://s-yata.jp/ssgnc/char