package Matrix;

use strict;
use warnings;
use Data::Dumper;
use Vec2;
use Piece;
use MatrixCel;

sub _vec2 { return Vec2->new(@_);}

sub new {
  my ($class, $param) = @_;
  my $obj = {};

  if(defined $param) {
    if (ref($param) eq "ARRAY") {
      $obj->{matrix} = $param;
    } elsif (ref($param) eq "HASH" || ref($param) eq $class) {
      $obj->{matrix} = $param->{matrix};
    }
  }

  $obj->{matrix} = $obj->{matrix} // [];

  return bless $obj, $class;
}

sub matrix {
  my ($self) = @_;
  return $self->{matrix};
}

sub size {
  my ($self) = @_;

  my $sum = 0;
  for my $row (@{$self->{matrix}}) {
    $sum += scalar @{$row};
  }
  return $sum;
}

sub set {
  my ($self, $pos, $type, $content) = @_;
  my $x = $pos->x;
  my $y = $pos->y;
  die "No negative positions" if $x < 0 || $y < 0;

  $self->{matrix}->[$y]->[$x] = MatrixCel->new({
                                  pos => $pos,
                                  type => $type,
                                  content => $content
                                } );
  return $self->at($pos);
}


sub exist {
  my ($self, $pos) = @_;
  my $x = $pos->x;
  my $y = $pos->y;
  return $x >= 0 && $y >= 0 && $self->{matrix}->[$y]->[$x];
}

sub at {
  my ($self, $pos) = @_;
  my $x = $pos->x;
  my $y = $pos->y;
  die "No negative positions" if $x < 0 || $y < 0;
  return $self->{matrix}->[$y]->[$x];
}

sub getTypeSize {
  my ($self, $type) = @_;
  my $sum = 0;
  for my $row (@{$self->{matrix}}) {
    for my $cel (@{$row}) {
      ++$sum if $cel->type() eq $type;
    }
  }
  return $sum;
}

sub getClassSize {
  my ($self, $class) = @_;
  my $sum = 0;
  for my $row (@{$self->{matrix}}) {
    for my $cel (@{$row}) {
      ++$sum if $cel->class() eq $class;
    }
  }
  return $sum;
}

sub str {
  my ($self) = @_;
  my $str = '';
  for my $row (@{$self->{matrix}}) {
    my $str_row = '';
    for my $cel (@{$row}) {
      $str_row .= $cel->char();
    }
    $str .= "\n" . $str_row;
  }
  return $str;
}

sub copy {
  my ($self, $type_map) = @_;
  my @n_matrix = ();
  for my $row (@{$self->{matrix}}) {
    my @n_row = ();
    for my $cel (@{$row}) {
      my $ncel = MatrixCel->new($cel);
      push @n_row, $ncel;
    }
    push @n_matrix, \@n_row;
  }
  return Matrix->new(\@n_matrix);
}

sub addPiece {
  my ($self, $piece) = @_;
  for my $pos (@{$piece->array}) {
    $self->set($pos, 'piece', $piece);
  }
}

sub removePiece {
  my ($self, $piece, $replace_type) = @_;
  $replace_type //= 'free';
  for my $pos (@{$piece->array}) {
    $self->set($pos, $replace_type, undef);
  }
}

sub isFreeCel {
  my ($self, $pos) = @_;
  return $self->at($pos)->isFreeType();
}

sub getNextFreeCel {
  my ($self, $starting_cell) = @_;

  for (my $y = $starting_cell->y; $y <= $#{$self->{matrix}}; ++$y) {
    my $row = $self->{matrix}->[$y];
    my $initx = $y == $starting_cell->y ? $starting_cell->x : 0;
    for (my $x = $initx; $x <= $#{$row}; ++$x) {
      my $p = _vec2($x,$y);
      if ($self->isFreeCel($p)) {
        return $p;
      }
    }
  }

  return undef;
}

sub getFreeNeighbors {
  my ($self, $pos) = @_;
  my @neighbors_directions = (
    _vec2(-1,  0), #L
    _vec2( 1,  0), #R
    _vec2( 0, -1), #D
    _vec2( 0,  1), #U
  );

  my @free_list = ();

  for my $dir (@neighbors_directions) {
    my $npos = $pos->add($dir);
    push @free_list, $npos if $self->exist($npos) && $self->isFreeCel($npos);
  }
  return \@free_list;
}

sub getUniqueCelsPerLine {
  my ($self, $line) = @_;

  my %cel_map = ();
  for my $cel (@{$self->{matrix}->[$line]}) {
    my $key = defined $cel->content ? $cel->content : $cel;
    $cel_map{$key} = $cel;
  }
  return [values %cel_map];
}

1
