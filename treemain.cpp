#include "containers/tree/NTree.hpp"
#include <iostream>

typedef blib::container::tree::Node<int> Node;
typedef blib::container::tree::NTree<Node> Tree;

void nodeOperationsTest( ) {
  std::cout << "Node operations start" << std::endl;
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
  std::cout << "\nNode operations end" << std::endl;
}

void createTree( Tree& t ) {
  Node l11, l12, l21, l22, l31, l32;
  l11.data( 11 );
  l12.data( 12 );
  l21.data( 21 );
  l22.data( 22 );
  l31.data( 31 );
  l32.data( 32 );

  Node l1, l2, l3;
  l1.data( 1 );
  l2.data( 2 );
  l3.data( 3 );

  l1.addChild( l11 );
  l1.addChild( l12 );

  l2.addChild( l21 );
  l2.addChild( l22 );

  l3.addChild( l31 );
  l3.addChild( l32 );

  Node r;
  r.data( 0 );
  r.addChild( l1 );
  r.addChild( l2 );
  r.addChild( l3 );

  t.root( r );
}

void preOrderTest( ) {
  std::cout << "preOrderTest start" << std::endl;
  Tree t;
  createTree( t );
  for ( auto it = t.pre_order_begin( ); it != t.pre_order_end( ); ++it ) {
    auto n = *it;
    if ( n ) {
      std::cout << n.data( ) << " ";
    }
  }
  std::cout << "\npreOrderTest end" << std::endl;
}

int main( int/* argc*/, char ** /*argv[]*/ ) {
  try {
    nodeOperationsTest( );
    preOrderTest( );
  }
  catch ( std::exception& e ) {
    std::cout << "exception = " << e.what( ) << std::endl;
  }
  catch ( ... ) {
    std::cout << "unknown exception" << std::endl;
  }


  int i;
  std::cin >> i;
  return 1;
}
