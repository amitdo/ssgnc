# Introduction #

SSGNC (Search System for Giga-scale N-gram Corpus) is a search system desgiend for large-scale N-gram corpus, such as Japanese/English Google N-gram Corpus.

## Overview ##

SSGNC builds a database from N-gram corpus, and the database enables searches with the following 4 kinds of queries.

  * Query types
    * **Unordered**: Unordered boolean AND query
      * Search for N-grams which contain "apple" and "orange".
    * **Ordered**: Ordered boolean AND query
      * Search for N-grams which contain "apple" and "orange" in order.
    * **Phrase**: Phrase query
      * Search for N-grams which contain "apple orange".
    * **Fixed**: Fixed length query
      * Search for "apple orange".

Users can test the search features through the search command **ssgnc-search** or the CGI program **ssgnc-cgi**. Also, due to the SSGNC core C++ library, the search features can be embedded into other applications.


---


## System Requirements ##

SSGNC is designed for Linux and tested on 32/64-bit Ubuntu. SSGNC-0.4.0 or later works on both 32-bit Linux and 64-bit Linux, but SSGNC-0.3.0 or earlier works only on 64-bit Linux.

Note: The database format of SSGNC-0.4.0 or later is different from that of SSGNC-0.3.0 or earlier. And because of the difference, databases for SSGNC-0.3.0 or earlier don't work well with SSGNC-0.4.0 or later.


---


## Installation ##

The following tools are required to build SSGNC.

  * g++
  * make

Then, what you have to do is just the well-known steps as follows:

```
 ./configure
 make
 make check
 make install
```

Note: SSGNC-0.3.0 or earlier requires the Boost C++ library and dawgdic.


---


# How to Use #

## Database construction ##

**ssgnc-build.sh** is the script to build database files from N-gram corpus. The input directory must have N-gram files named Ngms/Ngm-KKKK, same as Google N-gram Corpus. And if the N-gram files are compressed by gzip or xz, **ssgnc-build.sh** automatically decompress them.

```
$ ssgnc-build.sh DATA_DIR INDEX_DIR [TEMP_DIR] [CHECKER]
```

  * DATA\_DIR: Input directory
    * Builds database files from DATA\_DIR/Ngms/Ngm-KKKK.
    * If the N-gram files are compressed, in this case the name of N-gram files is DATA\_DIR/Ngms/Ngm-KKKK.gz or DATA\_DIR/Ngms/Ngm-KKKK.xz, those files are automatically decompressed.
  * INDEX\_DIR: Output directory
    * Outputs database files as INDEX\_DIR/vocab.dic, INDEX\_DIR/ngms.idx, INDEX\_DIR/Ngms-KKKK.db.
  * TEMP\_DIR: Temporary directory
    * Puts temporary files in TEMP\_DIR.
    * If omitted, INDEX\_DIR is used.
  * CHECKER: Options for debug
    * Options for time measurement and memory leak checks.

The database size is as large as the size of the input N-gram corpus. In addition, **ssgnc-build.sh** makes temporary files. Please confirm that there is an enough disk space. If the disk space is twice as large as the input N-gram corpus, the database construction will be completed successfully.

```
$ ssgnc-build.sh
Usage: DATA_DIR INDEX_DIR [TEMP_DIR] [CHECKER]

DATA_DIR: DATA_DIR/Ngms/Ngm-KKKK [.gz, .xz]
INDEX_DIR: INDEX_DIR/vocab.dic, ngms.idx, Ngm-KKKK.db
TEMP_DIR: TEMP_DIR/Ngm-KKKK.bin, Ngm-KKKK.part, Ngms.idx
CHECKER: time, valgrind
  time: time -f 'real %E, user %U, sys %S'
  valgrind: valgrind --leak-check=full
```


---


## Search command ##

The search command of SSGNC is **ssgnc-search**.

```
$ ssgnc-search [OPTION]... INDEX_DIR [FILE]...
```

  * OPTION: Search options
    * Shows the list of options if INDEX\_DIR is not given.
  * INDEX\_DIR: Directory path where database files exist
    * Opens INDEX\_DIR/vocab.dic, INDEX\_DIR/ngms.idx and INDEX\_DIR/Ngms-KKKK.db as database files.
  * FILE: List of queries
    * Handles each line of these files as a query.
    * If omitted, the standard input is used.

```
$ ssgnc-search
Usage: ssgnc-search [OPTION]... INDEX_DIR [FILE]...

Options:
  --ssgnc-min-freq=[1-999000000000000000]
  --ssgnc-num-tokens=[0-30][-][0-30]
  --ssgnc-min-num-tokens=[0-30]
  --ssgnc-max-num-tokens=[0-30]
  --ssgnc-max-num-results=[0-1099511627775]
  --ssgnc-io-limit=[0-1099511627775]
  --ssgnc-order=[UNORDERED, ORDERED, PHRASE, FIXED]
```


---


## CGI program ##

The CGI program **ssgnc-cgi** is a web-based search interface. You can find this command in ssgnc-x.y.z/cgi.

In default, the database directory path is /usr/share/ssgnc. Please modify the setting in ssgnc-x.y.z/cgi/config.h. Also, you can change the default values and the limitations of max\_num\_results and io\_limit.

```
enum TokenType { WORD_TOKEN, CHAR_TOKEN };

// The path of the directory where the database files exist.
static const Int8 *INDEX_DIR() { return "/usr/share/ssgnc"; }

// If the token type is CHAR_TOKEN, the CGI program splits the given query
// parameter into character tokens.
static TokenType TOKEN_TYPE() { return WORD_TOKEN; }

// The default settings. 0 means unlimited.
static Int64 DEFAULT_MAX_NUM_RESULTS() { return 100LL; }
static Int64 DEFAULT_IO_LIMIT() { return 256LL << 10; }

// The limitations. 0 means unlimited.
static Int64 MAX_MAX_NUM_RESULTS() { return 0LL; }
static Int64 MAX_IO_LIMIT() { return 1LL << 20; }
```

Then, please copy the customized **ssgnc-cgi** into the directory for CGI programs. For example, Apache2 on Ubuntu 10.4 uses /usr/lib/cgi-bin/ for CGI programs. After the copy, please test the CGI program from your favorite browser.