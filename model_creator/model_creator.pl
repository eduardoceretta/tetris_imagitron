use strict;
use warnings;
use Data::Dumper;
use List::Util qw(shuffle);
use POSIX qw/floor/;
use Vec2;

my $lastCell = Vec2->new({x=>0, y=>0});
# . free | # soft block | X hard block
my $str_img = <<"TXT";
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

main();

sub main {
  my $matrix = getMatrix($str_img);
  my $msize = getMatrixSize($matrix);
  my $mfreeSize = getMatrixFreeSize($matrix);
  my $optimal = floor(getMatrixFreeSize($matrix)/4);
  printMatrix($matrix);
  print STDERR Data::Dumper::Dumper('MatrixSize: ' . $msize);
  print STDERR Data::Dumper::Dumper('MatrixFreeSize: ' . $mfreeSize);
  print STDERR Data::Dumper::Dumper('OptmalNumberOfPieces: ' . $optimal);


  my $i = 3;
  my $cel = getNextFreeCel($matrix);
  traverse($matrix, [], $cel, $i);
  while ($cel = getNextFreeCel($matrix)) {
    $i = ($i + 1) >= 10 ? 3 : $i + 1;
    traverse($matrix, [], $cel, $i);
  }
  printMatrix($matrix);
  my $numPieces = getMatrixOccupiedSize($matrix) / 4;
  print STDERR Data::Dumper::Dumper('NumPieces: ' . $numPieces);
  print STDERR Data::Dumper::Dumper('Percentage: ' . 100*$numPieces/$optimal);
}

sub traverse {
  my ($matrix, $piece, $p, $mark) = @_;
  push @{$piece}, $p if blockCel($matrix, $p, $mark);
  if (scalar @$piece >= 4) {
    return 1;
  }

  my @shuffledParts = shuffle @{$piece};
  for my $piecePart (@shuffledParts) {
    my @neighbors = shuffle @{getFreeNeighbors($matrix, $piecePart)};
    if(scalar @neighbors) {
      for my $n (@neighbors) {
        return 1 if traverse($matrix, $piece, $n, $mark) == 1;
      }
    }
  }

  softBlockCel($matrix, $p);
  return 0;
}

sub getFreeNeighbors {
  my ($matrix, $pos) = @_;
  my @neighborsDirections = (
    Vec2->new(-1,  0), #L
    Vec2->new( 1,  0), #R
    Vec2->new( 0, -1), #D
    Vec2->new( 0,  1), #U
  );

  my @freeList = ();

  for my $dir (@neighborsDirections) {
    my $npos = $pos->add($dir);
    push @freeList, $npos if isFreeCel($matrix, $npos);
  }
  return \@freeList;
}

sub getNextFreeCel {
  my ($matrix) = @_;

  for (my $y = $lastCell->y; $y <= $#{$matrix}; ++$y) {
    my $row = $matrix->[$y];
    my $initx = $y == $lastCell->y ? $lastCell->x : 0;
    for (my $x = $initx; $x <= $#{$row}; ++$x) {
      if ($row->[$x] == 0) {
        $lastCell = Vec2->new($x,$y);
        return $lastCell;
      }
    }
  }

  return undef;
}


############# MATRIX
############# MATRIX
############# MATRIX
############# MATRIX

sub getMatrix {
  my $str_image = shift;

  my @matrix = ();
  for my $line (split /^/, $str_image) {
    my @row = ();
    chomp $line;
    for my $c (split //, $line) {
      # . free | # soft block | X hard block
      push @row, $c eq'.' ? 0 : $c eq '|' ? 1 : 2;
    }
    push @matrix, \@row;
  }

  return \@matrix;
}

sub getMatrixSize {
  my ($matrix) = @_;
  my $sum = 0;
  for my $row (@{$matrix}) {
    $sum += scalar @{$row};
  }
  return $sum;
}

sub getMatrixFreeSize {
  my ($matrix) = @_;
  my $sum = 0;
  for my $row (@{$matrix}) {
    for my $cel (@{$row}) {
      ++$sum if $cel == 0;
    }
  }
  return $sum;
}

sub getMatrixOccupiedSize {
  my ($matrix) = @_;
  my $sum = 0;
  for my $row (@{$matrix}) {
    for my $cel (@{$row}) {
      ++$sum if $cel > 2;
    }
  }
  return $sum;
}

sub printMatrix {
  my ($matrix) = @_;
  my $str = '';
  for my $row (@{$matrix}) {
    my $str_row = '';
    for my $cel (@{$row}) {
      $str_row .= $cel == 0 ? '.' : $cel == 1 ? '|' : $cel == 2 ? '.' : $cel;
    }
    $str .= "\n" . $str_row;
  }
  print $str, "\n";
}

sub isFreeCel {
  my ($matrix, $pos) = @_;
  my $x = $pos->x;
  my $y = $pos->y;

  return $x >= 0 && $y >= 0 && defined $matrix->[$y] && defined $matrix->[$y]->[$x] && $matrix->[$y]->[$x] == 0;
}

sub blockCel {
  my ($matrix, $pos, $mark) = @_;
  my $x = $pos->x;
  my $y = $pos->y;

  if ($x >= 0 && $y >= 0 && defined $matrix->[$y] && defined $matrix->[$y]->[$x] && $matrix->[$y]->[$x] == 0) {
    $matrix->[$y]->[$x] = $mark // 9;
    return $pos;
  }
  return undef;
}

sub softBlockCel {
  my ($matrix, $pos) = @_;
  my $x = $pos->x;
  my $y = $pos->y;

  if ($x >= 0 && $y >= 0 && defined $matrix->[$y] && defined $matrix->[$y]->[$x]) {
    $matrix->[$y]->[$x] = 2;
    return $pos;
  }
  return undef;
}

sub freeCel {
  my ($matrix, $pos) = @_;
  my $x = $pos->x;
  my $y = $pos->y;

  if ($x >= 0 && $y >= 0 && defined $matrix->[$y] && defined $matrix->[$y]->[$x] && $matrix->[$y]->[$x] != 0) {
    $matrix->[$y]->[$x] = 0;
    return $pos;
  }
  return undef;
}

