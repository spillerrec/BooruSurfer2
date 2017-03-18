BooruSurfer2
============
[![GitHub license](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://www.gnu.org/licenses/gpl-3.0.txt)

BooruSurfer2 is a remake of BooruSurfer, however ported from PHP to C++. The goal of BooruSurfer is to provide a consistent and space-efficient interface to Booru-like image boards, using a web browser as front end. BooruSurfer2 attempts to improve performance in parallel situations, where PHP showed to be impractical. Furthermore, it attempts to integrate a personal image collection into the experience.

NOTE: Still very much work in progress

### Dependencies

- C++14 compiler
- Poco with Crypto/NetSSL
- jansson
- tidy-html5
- sqlite3
- libpng
- sass (and therefore Ruby)