use strict;
use warnings;

use POSIX qw/floor strftime/;
use List::Util qw(shuffle);
use Getopt::Long;
use Data::Dumper;

use Image::Magick;

use Vec2;
use Piece;
use Matrix;
use MatrixCel;


my $VERSION = 1.1;

# . free , | soft block , # hard block

my $str_img = <<"TXT";
GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGGGGGGGGGRGGGGGGGGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGGGGGGGGRRRGGGGGGGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGGGGGGGRRRRRGGGGGGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGGGGGRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGGGGRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGGRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGGGRRRRRRBBBBBRRRRRRRGGGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGGRRRRRRBBRRRRRBBRRRRRRRGGGGGGGGGGGGGGGG
GGGGGGGGGGGGGRRRRRRBRRRRRRRRRBRRRRRRRRGGGGGGGGGGGGGG
GGGGGGGGGGGRRRRRRRBRRRRRRRRRRRBRRRRRRRRRGGGGGGGGGGGG
GGGGGGGGGRRRRRRRRBRRRRRRRRRRRRRBRRRRRRRRRGGGGRGGGGGG
GGGGGGGGRRRRRRRRBRRRRRRRRRRRRRRRBRRRRRRRRRGGGGGGGGGG
GGGGGGGRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGG
GGGGGGRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGG
GGGGGRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGG
GGGGGRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGG
GGGGRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGG
GGGGRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGG
GGGGRRRRRBRRRRRRRRRRRRRRRRRRRBBRRRRRRRRRRRRRRRRGGGGG
GGGGRRRRRBRRRRRRRRRRRRRRRRRRRRBBRRRRRRRRRRRRRRGGGGGG
GGGGGRRRRBBRRRRRRRRRRRRRRRRRRRRBBRRRRRRRRRRRRGGGGGGG
GGGGGGRRRRBBBRRRRRRRRRRGGRRRRRRRBBRRRRRRRRRRGGGGGGGG
GGGGGGGGRRRRRBRRRRRRRRGGGGRRRRRRRRRRRRRRRRGGGGGGGGGG
GGGGGGGGGGRRRRRRRRGGGGGGGGGGGGRRRRRRRRRRGGGGGGGGGGGG
GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
TXT






my $str_imgT = <<"TXT";
#.###.##...##.###
...#..###.###..##
#####.#######.###
#################
TXT

my $str_imgS = <<"TXT";
..###.###
#..#..###
####.####
#########
TXT



my $str_imgJ = <<"TXT";
..#...#.#
#.#.###.#
#.###.#..
###...###
#########
TXT

my $str_imgL = <<"TXT";
..#.###.
.##...#.
.#####..
#...####
###.####
TXT


my $str_img0 = <<"TXT";
####.....
###......
##.......
##.......
###......
TXT

my $str_imgK = <<"TXT";
.................................
.................................
....|||||......|||||....|||||....
....|||||.....|||||.....|||||....
....|||||....|||||......|||||....
....|||||...|||||.......|||||....
....|||||..|||||........|||||....
....|||||||||||.........|||||....
....|||||||||||.........|||||....
....|||||..|||||........|||||....
....|||||...|||||.......|||||....
....|||||....|||||......|||||....
....|||||.....|||||.....|||||....
.................................
.................................
TXT



#helper
sub vec2 { return Vec2->new(@_);}
sub matrixcel { return MatrixCel->new({pos => $_[0], type => $_[1], content => $_[2]});}

my $VERBOSE      = 0;
my $INCLUDE_IMG  = 0;
my $SEED         = undef;

main();

sub main {
  return unless GetOptions (
    'verbose' => \$VERBOSE,
    'seed=s' => \$SEED,
    'include_img' => \$INCLUDE_IMG,
  );

  my $seed = defined $SEED ? $SEED : rand(2**32);
  srand($seed);
  print STDERR Data::Dumper::Dumper("Seed: " . $seed) if $VERBOSE;

  my $file_name = $ARGV[0];
  if(!defined $file_name) {
    print "No input image received.\n";
    exit();
  }

  my $matrix = getOccupancyMatrixFromImage($file_name);
  my $color_data = getColorDataFromImage($file_name);

  # my $matrix = getOccupancyMatrix($str_img);
  # my $color_data = getColorData($str_img);
  my $msize = $matrix->size();
  my $mfree_size = $matrix->getClassSize('free');
  my $optimal = floor($mfree_size/4);
  printMatrix($matrix);

  print STDERR Data::Dumper::Dumper('MatrixSize: ' . $msize) if $VERBOSE;
  print STDERR Data::Dumper::Dumper('MatrixFreeSize: ' . $mfree_size) if $VERBOSE;
  print STDERR Data::Dumper::Dumper('OptmalNumberOfPieces: ' . $optimal) if $VERBOSE;

  my @piece_list = ();
  my $free_cel = vec2(0,0);
  while ($free_cel = $matrix->getNextFreeCel($free_cel)) {
    my $piece = traverse($matrix, Piece->new(), $free_cel);
    push @piece_list, $piece if defined $piece;
  }

  printMatrix($matrix);
  my $numPieces = $matrix->getTypeSize('piece') / 4;
  print STDERR Data::Dumper::Dumper('NumPieces: ' . $numPieces) if $VERBOSE;
  print STDERR Data::Dumper::Dumper('Percentage: ' . 100*$numPieces/$optimal) if $VERBOSE;

  my $sorted_list = sortForAnimation($matrix, \@piece_list);
  die "We lost pieces during sorting..." if scalar @piece_list != scalar @{$sorted_list};

  applyColor($color_data, $sorted_list);

  print STDERR Data::Dumper::Dumper("Seed: " . $seed) if $VERBOSE;

  writeResult($matrix, $sorted_list, $seed);
}

sub traverse {
  my ($matrix, $piece, $p) = @_;
  # Add position to piece and block the space in the matrix
  $piece->push($p) if $matrix->set($p, 'block');

  # If the piece is complete add the piece to the matrix and end the traverse
  if ($piece->size() >= 4) {
    $matrix->addPiece($piece);
    return $piece;
  }

  # Iterate through the positions in the piece
  my @shuffled_array = shuffle @{$piece->array};

  for my $pos (@shuffled_array) {
    # Get the free cells around the piece
    my @neighbors = shuffle @{$matrix->getFreeNeighbors($pos)};
    if(scalar @neighbors) {
      for my $n (@neighbors) {
        # Traverse in a free piece
        my $ret = traverse($matrix, $piece, $n);

        if (!defined $ret) {
          # Can't build a 4 piece, soft block the cell to avoid retries
          $matrix->set($p, 'softblock');
        }

        return $ret;
      }
    }
  }

  # Can't build a 4 piece, soft block the cell to avoid retries
  $matrix->set($p, 'softblock');
  return undef;
}

sub sortForAnimation {
  my ($matrix, $piece_list) = @_;

  my $input_matrix = $matrix->copy();
  my $output_matrix = $matrix->copy();
  for my $piece (@{$piece_list}) {
    $output_matrix->removePiece($piece);
  }
  for my $row (@{$output_matrix->matrix}) {
    for my $cel (@{$row}) {
      if(!$cel->isFreeType) {
        $cel->type('free');
      }
    }
  }
  printMatrix($matrix);
  printMatrix($input_matrix);
  printMatrix($output_matrix);

  my @sorted_list = ();

  for (my $j = 0; $j <= $#{$input_matrix->matrix}; ++$j){
    my @cur_list = shuffle grep {!$_->isFreeType} @{$input_matrix->getUniqueCelsPerLine($j)};

    my $has_change = 1;
    while ($has_change) {
      my @shuffled_list = @cur_list;
      $has_change = 0;
      @cur_list = ();

      while (scalar @shuffled_list > 0) {
        my $cel = pop @shuffled_list;
        if(hasFreeNeighbors($output_matrix, $cel, [vec2(0,-1)])) {
          unshift @cur_list, $cel;
        } else {
          if ($cel->isPieceType()) {
            $output_matrix->addPiece($cel->content);
            $input_matrix->removePiece($cel->content);
          } else {
            $output_matrix->set($cel->pos, 'block');
            $input_matrix->set($cel->pos, 'free');
          }
          push @sorted_list, $cel->content if $cel->isPieceType();
          $has_change = 1;
        }
      }
    }
  }

  return \@sorted_list;
}


sub applyColor {
  my ($color_data, $piece_list) = @_;
  for my $piece (@{$piece_list}) {
    my $avg_color = [0,0,0];
    for my $pos (@{$piece->array}) {
      my $color = $color_data->[$pos->y]->[$pos->x];
      $avg_color->[0] += $color->[0];
      $avg_color->[1] += $color->[1];
      $avg_color->[2] += $color->[2];
    }
    $avg_color->[0] = $avg_color->[0]/4;
    $avg_color->[1] = $avg_color->[1]/4;
    $avg_color->[2] = $avg_color->[2]/4;
    $piece->color($avg_color);
  }
}

sub hasFreeNeighbors {
  my ($matrix, $cel, $directions) = @_;

  my $hasFree = 0;
  my @pos_array;
  if($cel->isPieceType) {
    my $piece = $cel->content;
    @pos_array = @{$piece->array};
    $matrix->addPiece($piece);
  } else {
    @pos_array = ($cel->pos);
  }

  OUT: for my $pos (@pos_array) {
    for my $dir (@{$directions}) {
      my $p = $pos->add($dir);
      if($matrix->exist($p) && $matrix->isFreeCel($p)) {
        $hasFree = 1;
        last OUT;
      }
    }
  }

  if($cel->isPieceType) {
    $matrix->removePiece($cel->content);
  }

  return $hasFree;
}


sub getOccupancyMatrix {
  my $str_image = shift;

  my @matrix = ();
  my $y = 0;
  for my $line (split /^/, $str_image) {
    my @row = ();
    chomp $line;

    my $x = 0;
    for my $c (split //, $line) {
      # . free, | soft block, # hard block
      push @row, $c =~ /[.RGB]/
              ? matrixcel(vec2($x, $y), 'free')
              : $c eq '|'
                  ? matrixcel(vec2($x, $y), 'softblock')
                  : matrixcel(vec2($x, $y), 'block');
      ++$x;
    }
    push @matrix, \@row;
    ++$y;
  }

  return Matrix->new(\@matrix);
}



sub getOccupancyMatrixFromImage {
  my ($file_name) = @_;

  my $image = Image::Magick->new;
  my $x = $image->Read($file_name);
  die "Can't open image file $file_name: $x" if $x;

  my ($w, $h) = $image->Get('columns', 'rows');

  my @matrix = ();
  for (my $l = $h - 1; $l >= 0; --$l) {
    my @row = ();
    for (my $c = 0; $c < $w; ++$c) {
      my @color = $image->GetPixel(
        x         => $c,
        y         => $l,
      );

      push @row, scalar @color == 3
                      ? matrixcel(vec2($c, $l), 'free')
                      : matrixcel(vec2($c, $l), 'block');
    }
    push @matrix, \@row;
  }

  return Matrix->new(\@matrix);
}


sub getColorDataFromImage {
  my ($file_name) = @_;

  my $image = Image::Magick->new;
  my $x = $image->Read($file_name);
  die "Can't open image file $file_name: $x" if $x;

  my ($w, $h) = $image->Get('columns', 'rows');

  my @matrix = ();
  for (my $l = $h - 1; $l >= 0; --$l) {
    my @row = ();
    for (my $c = 0; $c < $w; ++$c) {
      my @color = $image->GetPixel(
        x         => $c,
        y         => $l,
      );

      push @row, scalar @color == 3
                      ? \@color
                      : [0,0,0];
    }
    push @matrix, \@row;
  }

  return \@matrix;
}





sub getColorData {
  my $str_image = shift;

  my @matrix = ();
  my $y = 0;
  for my $line (split /^/, $str_image) {
    my @row = ();
    chomp $line;

    my $x = 0;
    for my $c (split //, $line) {
      # . free, | soft block, # hard block
      push @row, $c eq 'R'
              ? [1,0,0]
              : $c eq 'G'
                  ? [0,1,0]
                  : $c eq 'B'
                    ? [0,0,1]
                    : [0,0,0];
      ++$x;
    }
    push @matrix, \@row;
    ++$y;
  }

  return \@matrix;
}


sub printMatrix {
  my $matrix = shift;
  print STDERR $matrix->str(),  "\n" if $VERBOSE;
}

sub writeResult {
  my ($matrix, $sorted_list, $seed) = @_;
  print "TetrisImagitron " . $VERSION, "\n";
  print "MODEL_PATH resources/Models/TetrisImagitron/", "\n";
  print "// Seed: ", $seed, "\n";
  print "// Automatically generated on ", strftime ("%Y-%m-%d %H:%M:%S (%Z)", localtime), "\n";
  print "//", "\n";
  print "\n";

  my $matrix_str = $matrix->str();
  $matrix_str =~ s!\n!\n// !g;
  print "// ", $matrix_str, "\n";
  print "\n";

  print "// Pieces (sorted for animation)", "\n";
  print "//   PIECE PART0_X PART0_Y PART1_X PART1_Y PART2_X PART2_Y PART3_X PART3_Y COLOR_R COLOR_G COLOR_B", "\n";


  for my $piece (@{$sorted_list}) {
    print 'PIECE '
      . (sprintf "     %5d %5d"x4,  map {($_->x,$_->y)} @{$piece->array})
      . (sprintf "     %5.3f %5.3f %5.3f",  @{$piece->color})
      , "\n";
  }

  print "\n";
  print "// Environment", "\n";
  print "//   [BLOCK|SOFTBLOCK|FREE] X Y", "\n";

  for my $row (@{$matrix->matrix}) {
    for my $cel (@{$row}) {
      if(!$cel->isPieceType()) {
        print (sprintf "%-10s %5d %5d\n", uc ($cel->type), $cel->pos->x ,$cel->pos->y);
      }
    }
  }
  print "\n";

  if($INCLUDE_IMG) {
    print "\n";
    for my $line (split /^/, $matrix->str()) {
      print "// $line";
    }

    print "//\n";
  }

  print "// END", "\n";
}



=cut
.................................
.................................
....|||||......|||||....|||||....
....|||||.....|||||.....|||||....
....|||||....|||||......|||||....
....|||||...|||||.......|||||....
....|||||..|||||........|||||....
....|||||||||||.........|||||....
....|||||||||||.........|||||....
....|||||..|||||........|||||....
....|||||...|||||.......|||||....
....|||||....|||||......|||||....
....|||||.....|||||.....|||||....
.................................
.................................
$VAR1 = 'MatrixSize: 495';
$VAR1 = 'MatrixFreeSize: 328';
$VAR1 = 'OptmalNumberOfPieces: 82';

344445566677888993344556677889934
3355.5567777888993344596677889934
3355|||||747588|||||6599|||||7734
338.|||||44355|||||66495|||||7734
3888|||||4335|||||666445|||||7788
9933|||||443|||||5666475|||||7889
9933|||||44|||||45665775|||||7889
6633|||||||||||445565577|||||8889
6633|||||||||||.43345577|||||6889
7788|||||99|||||33445573|||||6684
5778|||||996|||||7748593|||||6344
5548|||||6665|||||778893|||||6334
5444|||||77.59|||||33893|||||6344
556677778775599999333495566676644
55.66.8883333.9.9333.444556777...
$VAR1 = 'NumPieces: 79';
$VAR1 = 'Percentage: 96.3414634146341';
==