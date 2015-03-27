#include "containers/tree/NTree.hpp"
#include <iostream>

int main( int/* argc*/, char ** /*argv[]*/ ) {
  typedef blib::container::tree::Node<int> Node;
  typedef blib::container::tree::Tree<Node> Tree;
  Node r;
  Tree t;
  t.root( r );
  auto& n = t.root( );
  if ( !n ) {
    std::cout << "No data in root node" << std::endl;
  }

  n.data( 0 );
  if ( n ) {
    std::cout << "Node data populated to = " << n.data( ) << std::endl;
  }
  for ( int i = 1; i < 20; ++i ) {
    n.addChild( i );
  }

  for ( auto c : n ) {
    if ( c ) {
      std::cout << c.data( ) << " ";
    }
  }
  std::cout << std::endl;
  for ( auto it = n.begin( ); it != n.end( ); ++it ) {
    if ( *it ) {
      std::cout << it->data( ) << " ";
    }
  }
  std::cout << std::endl;

  auto it = n.begin( );
  n.removeChild( it );

  for ( auto c : n ) {
    if ( c ) {
      std::cout << c.data( ) << " ";
    }
  }
  std::cout << std::endl;

  for ( auto rit = n.child_node_rtol_begin( ); rit != n.child_node_rtol_end( ); ++rit ) {
    if ( *rit ) {
      std::cout << rit->data( ) << " ";
    }
  }
  std::cout << std::endl;

  int i;
  std::cin >> i;
  return 1;
}
