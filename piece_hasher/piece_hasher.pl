use strict;
use warnings;
use Carp::Always;

use POSIX qw/floor strftime/;
use List::Util qw(shuffle);
use Getopt::Long;
use Data::Dumper;

my $VERSION = 1.0;

my $VERBOSE      = 0;
my $INCLUDE_IMG  = 0;
my $SEED         = undef;

# RESULT
# I, 0, 4369
# I, 90, 15
# L, 0, 275
# L, 90, 71
# L, 180, 802
# L, -90, 113
# J, 0, 547
# J, 90, 23
# J, 180, 785
# J, -90, 116
# Z, 0, 54
# Z, 90, 561
# S, 0, 99
# S, 90, 306
# T, 0, 114
# T, 90, 562
# T, 180, 39
# T, -90, 305
# O, 0, 51


my $piece_data = [
# IIIIIIIIIIIII
  {
    type => "I",
    angle => 0,
    matrix =>   "#...\n"
              . "#...\n"
              . "#...\n"
              . "#...\n"
  },
  {
    type => "I",
    angle => 90,
    matrix =>   "....\n"
              . "....\n"
              . "....\n"
              . "####\n"
  },
# LLLLLLLLLLLLL
  {
    type => "L",
    angle => 0,
    matrix =>   "....\n"
              . "#...\n"
              . "#...\n"
              . "##..\n"
  },
  {
    type => "L",
    angle => -90,
    matrix =>   "....\n"
              . "....\n"
              . "..#.\n"
              . "###.\n"
  },
  {
    type => "L",
    angle => 180,
    matrix =>   "....\n"
              . "##..\n"
              . ".#..\n"
              . ".#..\n"
  },
  {
    type => "L",
    angle => 90,
    matrix =>   "....\n"
              . "....\n"
              . "###.\n"
              . "#...\n"
  },
# JJJJJJJJJJJJ
  {
    type => "J",
    angle => 0,
    matrix =>   "....\n"
              . ".#..\n"
              . ".#..\n"
              . "##..\n"
  },
  {
    type => "J",
    angle => 90,
    matrix =>   "....\n"
              . "....\n"
              . "#...\n"
              . "###.\n"
  },
  {
    type => "J",
    angle => 180,
    matrix =>   "....\n"
              . "##..\n"
              . "#...\n"
              . "#...\n"
  },
  {
    type => "J",
    angle => -90,
    matrix =>   "....\n"
              . "....\n"
              . "###.\n"
              . "..#.\n"
  },
# ZZZZZZZZZZZZ
  {
    type => "Z",
    angle => 0,
    matrix =>   "....\n"
              . "....\n"
              . "##..\n"
              . ".##.\n"
  },
  {
    type => "Z",
    angle => 90,
    matrix =>   "....\n"
              . ".#..\n"
              . "##..\n"
              . "#...\n"
  },
# SSSSSSSSSSSSSS
  {
    type => "S",
    angle => 0,
    matrix =>   "....\n"
              . "....\n"
              . ".##.\n"
              . "##..\n"
  },
  {
    type => "S",
    angle => 90,
    matrix =>   "....\n"
              . "#...\n"
              . "##..\n"
              . ".#..\n"
  },
# TTTTTTTTTTTTT
  {
    type => "T",
    angle => 0,
    matrix =>   "....\n"
              . "....\n"
              . "###.\n"
              . ".#..\n"
  },
  {
    type => "T",
    angle => 90,
    matrix =>   "....\n"
              . ".#..\n"
              . "##..\n"
              . ".#..\n"
  },
  {
    type => "T",
    angle => 180,
    matrix =>   "....\n"
              . "....\n"
              . ".#..\n"
              . "###.\n"
  },
  {
    type => "T",
    angle => -90,
    matrix =>   "....\n"
              . "#...\n"
              . "##..\n"
              . "#...\n"
  },
# OOOOOOOOOOOOOO
  {
    type => "O",
    angle => 0,
    matrix =>   "....\n"
              . "....\n"
              . "##..\n"
              . "##..\n"
  },
];


main();

sub main {
  return unless GetOptions (
    'verbose' => \$VERBOSE,
  );
  for my $data (@{$piece_data}) {
    my $hash = getHash(getMatrix($data->{matrix}));
    print STDERR $data->{matrix},"\n" if $VERBOSE;
    print STDERR "hash:", $hash,"\n" if $VERBOSE;
    print join(", ", ($data->{type}, $data->{angle}, $hash)), "\n";
  }
}


sub getMatrix {
  my $str_image = shift;

  my @matrix = ();
  my $y = 0;
  for my $line (split /^/, $str_image) {
    my @row = ();
    chomp $line;

    my $x = 0;
    for my $c (split //, $line) {
      # . free, # block
      push @row, $c eq '#' ? 1 : 0;
      ++$x;
    }
    # My head works in a different way so let`s go from bottom up
    unshift @matrix, \@row;
    ++$y;
  }

  return \@matrix;
}

sub getHash {
  # unsigned int hash = 0;
  # for (int i = 0; i < 4; ++i) {
  #   for (int j = 0; j < 4; ++j) {
  #     if (m[i][j]) {
  #       hash += (1 << (i*4+j));
  #     }
  #   }
  # }
  # return hash;
  my $hash = 0;
  my ($m) = @_;

  for (my $i = 0; $i < 4; ++$i) {
    for (my $j = 0; $j < 4; ++$j) {
      if($m->[$i]->[$j]) {
        print 1 << ($i*4+$j) , "\n" if $VERBOSE;
        $hash += (1 << ($i*4+$j));
      }
    }
  }
  return $hash;
}




