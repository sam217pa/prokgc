[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)

# prokgc

This program computes GC contents in the genome and CDS sequence if
presented as a multifasta file.

```
prokgc \
    --cds examples/example_cds.fasta \
    --genome examples/example_genome.fasta \
    --header \
    --taxid 62977
```

produces the following output :

```
taxid,n_chr,genome_len,genome_gc,genome_n,n_seq,n_good_seq,cds_cumlen,gc_cds,gc1,gc2,gc3
62977,1,4809037,2505158,0,4600,4473,4073316,2167988,804625,559570,803793
```

which can be piped to the
wonderful [`csvtk pretty`](http://bioinf.shenwei.me/csvtk/) command to
get :

```
taxid   n_chr   genome_len   genome_gc   genome_n   n_seq   n_good_seq   cds_cumlen   gc_cds    gc1      gc2      gc3
62977   1       4809037      2505158     0          4600    4473         4073316      2167988   804625   559570   803793
```

## Synopsis

This program is designed to computes the number of GC bases in a
multi-fasta file containing the whole genome sequence and the number of
GC bases in a multi-fasta file containing the coding sequences only.

- *taxid*: the taxid provided via `--taxid`
- *n_chr*: the number of chromosome in the genome file
- *genome_len*: cumulated chromosome length
- *genome_gc*: number of GC bases in the genome file
- *genome_n*: number of N bases in the genome file.
- *n_seq*: number of sequences in the CDS file
- *n_good_seq*: number of sequences with length multiple of 3 and a
  canonical stop codon.
- *cds_cumlen*: cumulated CDS length
- *gc_cds*: number of GC bases in the CDS file
- *gcX*: number of GC bases at codon position X

This program uses the `kseq.h` header from
the [klib](http://attractivechaos.github.io/klib/#About) library and is
really fast :

```sh
time (prokgc -c example_cds.fasta -g example_genome.fasta -t 1234 --header)
# => 0.05s user 0.01s system 81% cpu 0.083 total
```

This speed is attained by being somewhat laxist on the type of data
provided, and is not a strict verification of the fasta specification
agreement like can be done in BioPerl or BioPython.

It also works on gzipped compressed multi-fasta file without prior
decompression.

## Help

Run

```sh
prokgc --help
```

to get :

```
Usage:
  prokgc [OPTION?] - computes gc contents

Synopsis:
  Computes number of GC bases in the genome and coding sequences in three
  codons positions.

Help Options:
  -h, --help       Show help options

Application Options:
  -H, --header     Print csv header
  -c, --cds        CDS sequence file
  -g, --genome     genome sequence file
  -t, --taxid      Taxon ID of corresponding organism

Description:
  This script computes cds_gc contents in a multi-fasta file using kseq.h
  included in the htseq library. It prints out a summary of the number of
  chromosomes, the genome length, the number of GC and N bases, the number
  of sequences in the CDS file, the number of sequence with canonical stop
  codon, the cumulated length of coding sequences, the number of GC bases
  in CDS, and the number of GC at positions 1, 2 and 3 of codons.

  Return a csv file to STDOUT without header by default, with a header if
  --header is specified.
```

`## Install

### Download

Download a copy of the release [here](https://github.com/sam217pa/prokgc/files/1004508/prokgc-0.0.1.tar.gz) and install using

```sh
tar xvzf prokgc-0.0.1.tar.gz

cd prokgc-0.0.1
./configure
make
make install
```

If last step is unsuccessful, try running it as `sudo`.

### Prerequisites

This software requires `glib-2.0 >= 2.50`. MacOS users can download a
recent glib version with

```sh
brew install glib
```

Linux users either have a recent glib version or can follow the
instruction
[here](https://developer.gnome.org/glib/2.52/glib-building.html). `glib`
is made for portability and should be easy to install on most platforms.

## Disclaimer

The `kseq.h` header is included in
the [klib](http://attractivechaos.github.io/klib/#About) library under
the MIT/X11 licence and was not written by me. The MIT licence is
permissive and GPL3 compatible.

This software is distributed without any warranty under the term of the
GNU Public Licence 3.0, as mentioned in the enclosed LICENCE file.
