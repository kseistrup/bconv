# bconv

Convert integers between bases 2‥36

## Usage

```
Usage: bconv [OPTIONS] [INTEGER [INTEGER …]]

positional arguments:
  INTEGER               integer to convert

optional arguments:
  -h, --help            show this help message and exit
  -v, --version         show version information and exit
  -c, --copyright       show copying policy and exit
  -i INPUT_BASE, --input-base INPUT_BASE
                        input is in INPUT_BASE (2‥36)
  -o OUTPUT_BASE, --output-base OUTPUT_BASE
                        output is in OUTPUT_BASE (2‥36)
  -w WIDTH, --width WIDTH
                        pad on the left with zeroes until WIDTH
  -b, --bits            show number of bits in INTEGER
  -l, --lower           show result in lowercase (default)
  -u, --upper           show result in uppercase

Default bases are 10 (decimal).

For convenience, input and output bases can be specified either as an
integer or as BINary (2), OCTal (8), DECimal (10) or HEXadecimal (16).

The characters A‥Z corresponds to bases 11‥36.

Modelled after a similar program written in C by yours truly in 1996.
```

## Requirements

Python 3.4+ (Python 2 compatible version available as tag `py2`).

## Repositories

* https://github.com/kseistrup/bconv
* ssb://%XrpBNTl7u611sA/AIgjB10AozrZPoCYDkxWWJD86RdA=.sha256

:smile:
