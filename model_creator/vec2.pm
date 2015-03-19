package Vec2;

use strict;
use warnings;
use Data::Dumper;

sub new {
  my ($class, $param, $y) = @_;
  my $obj = [];
  if(defined $param) {
    if (ref($param) eq "HASH") {
      push @$obj, $param->{x};
      push @$obj, $param->{y};
    } elsif (ref($param) eq "ARRAY") {
      $obj = $param;
    } elsif (!ref($param) && defined $y && !ref($y)) {
      push @$obj, $param;
      push @$obj, $y;
    }
  }

  return bless $obj, $class;
}

sub x {
  return $_[0]->[0];
}

sub y {
  return $_[0]->[1];
}

sub add {
  my ($self, $b) = @_;

  return Vec2->new({
    x => $self->x + $b->x,
    y => $self->y + $b->y,
  });
}



1
