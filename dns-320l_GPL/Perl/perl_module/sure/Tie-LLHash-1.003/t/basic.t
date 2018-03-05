$^W = 1;

use strict;
use Test;
BEGIN { plan tests => 22 }
use Tie::LLHash;
ok 1;

{
  my (%hash, %hash2);

  # 2: Test the tie interface
  tie (%hash, "Tie::LLHash");
  ok( tied %hash );

  # 3: Add first element
  (tied %hash)->first('firstkey', 'firstval');
  ok( $hash{firstkey} eq 'firstval' );

  # 4: Add more elements
  (tied %hash)->insert( red => 'rudolph', 'firstkey');
  (tied %hash)->insert( orange => 'julius', 'red');
  ok( $hash{red} eq 'rudolph' 
		  and $hash{orange} eq 'julius'
		  and (keys(%hash))[0] eq 'firstkey'
		  and (keys(%hash))[1] eq 'red'
		  and (keys(%hash))[2] eq 'orange');

  # 5: Delete first element
  delete $hash{firstkey};
  ok( keys %hash  == 2
		  and not exists $hash{firstkey} );

  # 6: Delete all elements
  {
    my $o = delete $hash{orange};
    ok $o, 'julius';
    ok !exists $hash{orange};

    my $r = delete $hash{red};
    ok $r, 'rudolph';
    ok !exists $hash{red};

    ok( keys %hash, 0 );
  }

  # 7: Exercise the ->last method
  {
    my ($i, $bad);
    for ($i=0; $i<10; $i++) {
      (tied %hash)->last($i, $i**2);
    }

    $i=0;
    foreach (keys %hash) {
      $bad++ if ($i++ ne $_);
    }
    ok(!$bad);
  }

  # 8: delete all contents
  %hash = ();
  ok( !%hash );

  # 9: Combine some ->first and ->last action
  {
    my @result = qw(1 6 4 5 7 9 n r);
    (tied %hash)->first(5=>1);
    (tied %hash)->last (7=>1);
    (tied %hash)->last (9=>1);
    (tied %hash)->first(4=>1);
    (tied %hash)->last (n=>1);
    (tied %hash)->first(6=>1);
    (tied %hash)->first(1=>1);
    (tied %hash)->last (r=>1);
		
    my ($i, $bad);
    foreach (keys %hash) {
      $bad++ if ($_ ne $result[$i++]);
    }
    ok(!$bad);
  }

  # 10: create a new hash with an initialization hash
  {
    my @keys = qw(zero one two three four five six seven eight);

    tie(%hash2, 'Tie::LLHash', map {$keys[$_], $_} 0..8);
    my ($bad, $i) = (0,0);
		
    foreach (keys %hash2) {
      $bad++ unless ($_ eq $keys[$i]  and  $hash2{$_} eq $i++); 
    }

    ok( !$bad );
  }

  # 11: use insert() to add an item at the beginning
  untie %hash2;
  {
    my $t = tie(%hash2, 'Tie::LLHash', one=>1);
    $t->insert(zero=>0);
    ok($t->first eq 'zero' and $t->last eq 'one')
  }

  # 12: lazy mode
  untie %hash2;
  {
    tie(%hash2, 'Tie::LLHash', {lazy=>1}, zero=>0);
    $hash2{one}=1;
    my @k = keys %hash2;
    ok($k[0] eq 'zero' and $k[1] eq 'one')
  }
}

{
  # Test deletes in a loop
  tie my(%hash), "Tie::LLHash", {lazy => 1};
  ok tied(%hash);
  
  $hash{one} = 1;
  $hash{two} = 2;
  $hash{three} = 3;
  ok keys(%hash), 3;
  
  my ($k, $v) = each %hash;
  ok $k, 'one';
  delete $hash{$k};
  
  ($k, $v) = each %hash;
  ok $k, 'two';
  delete $hash{$k};
  
  ($k, $v) = each %hash;
  ok $k, 'three';
  delete $hash{$k};
  
  ok keys(%hash), 0;
}
