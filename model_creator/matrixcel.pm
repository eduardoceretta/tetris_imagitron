package MatrixCel;

use strict;
use warnings;
use Data::Dumper;
use Vec2;
use Piece;

my %type_class_map = (
  free  => 'free',
  piece => 'block',
  block => 'block',
  softblock => 'block',
);


my %type_char_map = (
  free  => '.',
  block     => '#',
  softblock => '|',
  piece     => '?',
);


sub new {
  my ($class, $param) = @_;
  my $obj = {};

  $obj->{type} = 'free';
  if(defined $param) {
    if (ref($param) eq "HASH" || ref($param) eq $class) {
      $obj->{pos}  = $param->{pos};
      $obj->{type} = lc($param->{type});
      $obj->{content} = $param->{content};
    }
  }

  return bless $obj, $class;
}

sub type {
  my ($self, $ntype) = @_;
  $self->{type} = lc($ntype) if defined $ntype && $ntype;
  return $self->{type};
}

sub pos {
  my ($self, $npos) = @_;
  $self->{pos} = $npos if defined $npos;
  return $self->{pos};
}

sub content {
  my ($self, $ncontent) = @_;
  $self->{content} = $ncontent if defined $ncontent;
  return $self->{content};
}

sub char {
  my ($self) = @_;
  return defined $self->{content} && $self->{content}->char()
                    ? $self->{content}->char()
                    : $type_char_map{$self->{type}};
}

sub class {
  my ($self) = @_;
  return $type_class_map{$self->{type}};
}

sub isFreeType {
  my ($self) = @_;
  return $self->{type} eq 'free';
}

sub isBlockType {
  my ($self) = @_;
  return $self->{type} eq 'block';
}

sub isSoftBlockType {
  my ($self) = @_;
  return $self->{type} eq 'softblock';
}

sub isPieceType {
  my ($self) = @_;
  return $self->{type} eq 'piece';
}

sub str {
  my ($self) = @_;
  return $self->{pos}->str()
          . ' - type:' . $self->{type}
          . '(class:' .  $self->class() . ')'
          . (defined $self->{content} ? ' ' . $self->{content}->str() : '');
}

1
