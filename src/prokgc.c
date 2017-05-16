#include "kseq.h"
#include <glib.h>
#include <glib/gprintf.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
KSEQ_INIT(gzFile, gzread)

// functions declarations
void file_exists();

// Command line parsing
static gboolean header = FALSE;
static gchar *cds;
static gchar *genome;
static gchar *taxid;

/*
  This block defines the command line interface. First argument is the
  long form, second the short one, third is 0 if the documentation
  should appear in the help page, fourth points to a static definition,
  fifth is short help for function, and sixth is a placeholder
  description of argument.

  See help at
  <https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionEntry>.
*/

static GOptionEntry entries[] = {
  {"header", 'H', 0, G_OPTION_ARG_NONE, &header, "Print csv header", NULL},
  {"cds", 'c', 0, G_OPTION_ARG_FILENAME, &cds, "CDS sequence file", NULL},
  {"genome", 'g', 0, G_OPTION_ARG_FILENAME, &genome, "genome sequence file", NULL},
  {"taxid", 't', 0, G_OPTION_ARG_STRING, &taxid, "Taxon ID of corresponding organism", NULL},
  {NULL}};

int main(int argc, char *argv[]) {
  GError *error = NULL;
  GOptionContext *context;

  context = g_option_context_new("- computes gc contents");
  g_option_context_add_main_entries(context, entries, NULL);

  g_option_context_set_summary(context, "\
Synopsis:\n  \
Computes number of GC bases in the genome and coding sequences in three\n  \
codons positions.");

  g_option_context_set_description(context, "\
Description:\n  \
This script computes cds_gc contents in a multi-fasta file using kseq.h\n  \
included in the htseq library. It prints out a summary of the number of\n  \
chromosomes, the genome length, the number of GC and N bases, the number\n  \
of sequences in the CDS file, the number of sequence with canonical stop\n  \
codon, the cumulated length of coding sequences, the number of GC bases\n  \
in CDS, and the number of GC at positions 1, 2 and 3 of codons.\n  \
\n  \
Return a csv file to STDOUT without header by default, with a header if\n  \
--header is specified.");

  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_print("option parsing failed: %s\n", error->message);
    g_print("%s\n", g_option_context_get_help(context, TRUE, NULL));
    exit(1);
  }

  if (taxid == NULL) {
    g_fprintf(stderr, "ERROR: Taxon ID not provided\n");
    exit(1);
  }

  /* FIXME: should return boolean value to parse errors */
  file_exists(cds, context);
  file_exists(genome, context);

  gzFile fp;
  kseq_t *seq;
  /*
    n_seq counts the number of sequence per genome

    n_good_seq counts the number of good sequences in file, ie
    sequence of length multiple of 3, ending with canonical stop codons
    TAA, TAG or TGA.

    cds_cumlen is the accumulated length of coding sequences in file.
  */

  int l, n_chr = 0, n_seq = 0, n_good_seq = 0, genome_n = 0;
  long int genome_len = 0, cds_cumlen = 0;
  long int genome_gc = 0, cds_gc = 0, gc1 = 0, gc2 = 0, gc3 = 0;

  fp = gzopen(genome, "r");
  seq = kseq_init(fp);

  while ((l = kseq_read(seq)) >= 0) {
    n_chr++;

    genome_len += strlen(seq->seq.s);
    for (int i = 0; i < strlen(seq->seq.s); i++) {
      if (seq->seq.s[i] == 'C' || seq->seq.s[i] == 'G') {
        genome_gc++;
      } else if (seq->seq.s[i] == 'N') {
        genome_n++;
      }
    }
  }

  /* This part deals with CDS-related computations. */
  fp = gzopen(cds, "r");
  seq = kseq_init(fp);

  while ((l = kseq_read(seq)) >= 0) {

    n_seq++; // counts sequence in file

    // get sequence length
    int len = strlen(seq->seq.s);
    // get last codon
    const gchar *last_codon = &seq->seq.s[len - 3];

    // check that last codon is a canonical stop codon and that sequence
    // length is a multiple of 3
    GRegex *regex;
    GMatchInfo *match_info;
    regex = g_regex_new ("TAA|TAG|TGA", 0, 0, NULL);
    gboolean good_last_codon = g_regex_match(regex, last_codon, 0, &match_info);

    if (len % 3 == 0 && good_last_codon) {
      n_good_seq++; // counts good sequence in file
      cds_cumlen += len;

      //  parse cds_gc content at positions 1 2 3 and all
      for (int i = 0; i < len; i += 3) {
        if (seq->seq.s[i] == 'C' || seq->seq.s[i] == 'G') {
          gc1++;
        }
        if (seq->seq.s[i + 1] == 'C' || seq->seq.s[i + 1] == 'G') {
          gc2++;
        }
        if (seq->seq.s[i + 2] == 'C' || seq->seq.s[i + 2] == 'G') {
          gc3++;
        }
      }

      cds_gc = gc1 + gc2 + gc3;
    }
  }

  // print header if required by cmdline
  if (header) {
    g_print("taxid,n_chr,genome_len,genome_gc,genome_n,n_seq,n_good_seq,cds_"
            "cumlen,gc_cds,gc1,gc2,gc3\n");
  }

  g_fprintf(stdout, "%s,%d,%ld,%ld,%d,%d,%d,%ld,%ld,%ld,%ld,%ld\n", taxid,
            n_chr, genome_len, genome_gc, genome_n, n_seq, n_good_seq,
            cds_cumlen, cds_gc, gc1, gc2, gc3);

  kseq_destroy(seq);
  gzclose(fp);
  return 0;
}

void file_exists(const gchar *filename, GOptionContext *context) {
  if (filename == NULL) {
    g_fprintf(stderr, "ERROR: missing argument\n");
    g_fprintf(stderr, "%s\n", g_option_context_get_help(context, FALSE, NULL));
    exit(1);
  } else if (!g_file_test(filename, G_FILE_TEST_EXISTS)) {
    g_fprintf(stderr, "ERROR: `%s` does not exists\n", filename);
    exit(1);
  }
}
