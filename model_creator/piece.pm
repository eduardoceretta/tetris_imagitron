package Piece;

use strict;
use warnings;
use Data::Dumper;

use Vec2;

my $counter = 0;
sub new {
  my ($class, $param) = @_;
  my $obj = {};
  if(defined $param) {
    if (ref($param) eq "ARRAY") {
      $obj->{array} = $param;
    }elsif (ref($param) eq "HASH" || ref($param) eq $class) {
      $obj->{id}    = $param->{id};
      $obj->{array} = $param->{array};
      $obj->{color} = $param->{color};
    }
  }

  $obj->{id}    = ++$counter if !defined $obj->{id};
  $obj->{array} = $obj->{array} // [];
  $obj->{color} = $obj->{color} // [rand(), rand(), rand()];

  return bless $obj, $class;
}

sub id {
  my $self = shift;
  return $self->{id};
}

sub char {
  my $self = shift;
  return chr(65 + ( $self->{id} % 25));
}

sub color {
  my ($self, $color) = @_;
  $self->{color} = $color if defined $color && scalar @{$color} == 3;
  return $self->{color};
}

sub size {
  my ($self) = @_;
  return scalar @{$self->{array}};
}

sub push {
  my ($self, $pos) = @_;
  die "Max 4 elements in a piece" if scalar @{$self->{array}} >= 4;
  return push @{$self->{array}}, $pos;
}

sub array {
  my ($self) = @_;
  return $self->{array};
}


sub at {
  my ($self, $i) = @_;
  return $self->{array}->[$i];
}

sub str {
  my $self = shift;
  return 'id: ' . $self->id . '(' . $self->char . ')'
            . '[' . (join ", ", map{$_->str} @{$self->{array}}) . ']';
}

1
