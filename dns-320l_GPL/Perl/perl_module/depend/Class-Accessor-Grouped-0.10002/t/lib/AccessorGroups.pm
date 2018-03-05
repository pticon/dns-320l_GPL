{
  package AccessorGroups::BeenThereDoneThat;

  use strict;
  use warnings;
  use base 'Class::Accessor::Grouped';

  __PACKAGE__->mk_group_accessors('simple', 'singlefield');
  __PACKAGE__->mk_group_accessors('multiple', qw/multiple1 multiple2/);
}


package AccessorGroups;
use strict;
use warnings;
use base 'Class::Accessor::Grouped';
__PACKAGE__->mk_group_accessors('simple', 'singlefield');
__PACKAGE__->mk_group_accessors('multiple', qw/multiple1 multiple2/);
__PACKAGE__->mk_group_accessors('listref', [qw/lr1name lr1;field/], [qw/lr2name lr2'field/]);
__PACKAGE__->mk_group_accessors('simple', 'runtime_around');

sub get_simple {
  my $v = shift->SUPER::get_simple (@_);
  $v =~ s/ Extra tackled on$// if $v;
  $v;
}

sub set_simple {
  my ($self, $f, $v) = @_;
  $v .= ' Extra tackled on' if $f eq 'singlefield';
  $self->SUPER::set_simple ($f, $v);
  $_[2];
}

# a runtime Class::Method::Modifiers style around
# the eval/our combo is so that we do not need to rely on Sub::Name being available
my $orig_ra_cref = __PACKAGE__->can('runtime_around');
our $around_cref = sub {
  my $self = shift;
  if (@_) {
    my $val = shift;
    $self->$orig_ra_cref($val . ' Extra tackled on');
    $val;
  }
  else {
    my $val = $self->$orig_ra_cref;
    $val =~ s/ Extra tackled on$// if defined $val;
    $val;
  }
};
{
  no warnings qw/redefine/;
  eval <<'EOE';
    sub AccessorGroups::runtime_around { goto $AccessorGroups::around_cref };
    sub AccessorGroups::_runtime_around_accessor { goto $AccessorGroups::around_cref };
EOE
}

sub new {
    return bless {}, shift;
};

foreach (qw/multiple listref/) {
    no strict 'refs';
    *{"get_$_"} = __PACKAGE__->can('get_simple');
    *{"set_$_"} = __PACKAGE__->can('set_simple');
};

1;
