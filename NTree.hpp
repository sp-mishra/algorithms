#ifndef BLIB_NTREE_HPP
#define BLIB_NTREE_HPP

#include <vector>
#include <functional>
#include <boost/iterator/iterator_facade.hpp>
#include <stack>

namespace blib {
  namespace container {
    namespace tree {
      //=====================================================================
      // Tree Node
      template<typename NodeDataType>
      class Node {
      public:
        typedef NodeDataType ValueType;
        typedef ValueType& ValueRef;
        typedef ValueType const& ConstValueRef;
        typedef Node<NodeDataType> NodeType;
        typedef NodeType& NodeRef;
        typedef NodeType const& ConstNodeRef;
        typedef std::vector<NodeType> ChildrenContainerType;

      private:
        ValueType _data;
        ChildrenContainerType _children;

      public:
        Node( ) {}

        Node( ConstValueRef aData ) :
          _data( aData ) {}

        ValueType data( ) {
          return _data;
        }

        ConstValueRef data( ) const {
          return _data;
        }

        void data( ConstNodeRef aData ) {
          _data = aData;
        }

        ChildrenContainerType& children( ) {
          return _children;
        }

        void addChild( ConstNodeRef aNode ) {
          _children.push_back( aNode );
        }

        void addChild( NodeType&& aNode ) {
          _children.push_back( aNode );
        }

        std::size_t size( ) const {
          return _children.size( );
        }

        bool empty( ) const {
          return _children.empty( );
        }

        void clear( ) {
          for ( auto child : _children ) {
            child.clear( );
          }
        }

        bool operator==( ConstNodeRef aOther ) const {
          bool ret = false;
          if ( aOther.data == _data ) {
            if ( aOther._children.size( ) == _children.size( ) ) {
              ret = true;
            }
          }
          return ret;
        }

        NodeRef operator=( ConstNodeRef aOther ) {
          _data = aOther._data;
          _children = aOther._children;
        }
      };

      //=====================================================================
      // Tree Declaration
      template<typename NodeDataType>
      class Tree;
      //=====================================================================

      //=====================================================================
      // Iterators implementations
      namespace _private {
        //=====================================================================
        // PostOrder Iterators
        // Post Order with out recursion and with 2 stacks
        //Post - order
        //  Traverse the left subtree by recursively calling the post - order function.
        //  Traverse the right subtree by recursively calling the post - order function.
        //  Display the data part of root element( or current element ).
        template<typename NodeType>
        class df_post_order_iterator :
          public boost::iterator_facade < df_post_order_iterator<NodeType>, NodeType, boost::forward_traversal_tag > {
        private:
          friend class boost::iterator_core_access;
          typedef NodeType Node;
          typedef typename Node::ValueType ValueType;
          typedef typename Tree<ValueType>::TreeType Tree;

          Tree& _tree;
          std::stack<std::reference_wrapper<Node>> _stack1;
          std::stack<std::reference_wrapper<Node>> _stack2;

        public:
          df_post_order_iterator( ) {}

          df_post_order_iterator( Tree& aTree ) :
            _tree( aTree ) {
            if ( !_tree.empty( ) ) {
              _stack1.push( _tree.root( ) );
            }
          }

        private:
          bool equal1( df_post_order_iterator const& aOther ) const {
            bool ret = false;
            if ( _stack1.size( ) == aOther._stack1.size( ) ) {
              if ( _stack1.top( ) == aOther._stack1.top( ) ) {
                ret = true;
              }
            }
            return ret;
          }

          bool equal2( df_post_order_iterator const& aOther ) const {
            bool ret = false;
            if ( _stack2.size( ) == aOther._stack2.size( ) ) {
              if ( _stack2.top( ) == aOther._stack2.top( ) ) {
                ret = true;
              }
            }
            return ret;
          }

          bool equal( df_post_order_iterator const& aOther ) const {
            return equal1( aOther ) && equal2( aOther );
          }

          Node& dereference( ) const {
            return _stack1.top( );
          }

          // http://www.geeksforgeeks.org/iterative-postorder-traversal/
          //1. Push root to first stack.
          //2. Loop while first stack is not empty
          //  2.1 Pop a node from first stack and push it to second stack
          //  2.2 Push left and right children of the popped node to first stack
          //3. Print contents of second stack
          void increment( ) {
            if ( _stack1.empty( ) ) {
              return;
            }

            auto node = _stack1.top( );
            _stack2.push( node );
            for ( auto& n : node.children( ) ) {
              _stack1.push( n );
            }
          }
        };

        //=====================================================================
        // Preorder Iterators
        //Algorithm Preorder( tree )
        //  1. Visit the root.
        //  2. Traverse the left subtree, i.e., call Preorder( left - subtree )
        //  3. Traverse the right subtree, i.e., call Preorder( right - subtree )
        template<typename NodeType>
        class df_pre_order_iterator :
          public boost::iterator_facade < df_pre_order_iterator<NodeType>, NodeType, boost::forward_traversal_tag > {
        private:
          friend class boost::iterator_core_access;
          typedef NodeType Node;
          typedef typename Node::ValueType ValueType;
          typedef typename Tree<ValueType>::TreeType Tree;

          Tree& _tree;
          std::vector<std::reference_wrapper<Node>> _stack;

        public:
          df_pre_order_iterator( ) {}

          df_pre_order_iterator( Tree& aTree ) :
            _tree( aTree ) {
            if ( !_tree.empty( ) ) {
              _stack.push_back( _tree.root( ) );
            }
          }

        private:
          bool equal( df_pre_order_iterator const& aOther ) const {
            bool ret = false;
            if ( _stack.size( ) == aOther._stack.size( ) ) {
              if ( _stack.back( ) == aOther._stack.back( ) ) {
                ret = true;
              }
            }
            return ret;
          }

          Node& dereference( ) const {
            return _stack.back( );
          }

          //iterativePreorder( node )
          //  parentStack = empty stack
          //  while ( not parentStack.isEmpty( ) or node != null )
          //    if ( node != null )
          //      visit( node )
          //      if ( node.right != null ) parentStack.push( node.right )
          //        node = node.left
          //      else
          //      node = parentStack.pop( )
          void increment( ) {
            if ( _stack.empty( ) ) {
              return;
            }

            auto& node = _stack.back( );
            _stack.pop_back( );
            for ( auto& n : node.children( ) ) {
              _stack.push_back( n );
            }
          }
        };

        //=====================================================================
        // Preorder Const Iterators
        //Algorithm Preorder( tree )
        //  1. Visit the root.
        //  2. Traverse the left subtree, i.e., call Preorder( left - subtree )
        //  3. Traverse the right subtree, i.e., call Preorder( right - subtree )
        template<typename NodeType>
        class df_const_pre_order_iterator :
          public boost::iterator_facade < df_pre_order_iterator<NodeType>, NodeType const, boost::forward_traversal_tag > {
        private:
          friend class boost::iterator_core_access;
          typedef NodeType Node;
          typedef typename Node::ValueType ValueType;
          typedef typename Tree<ValueType>::TreeType Tree;

          Tree& _tree;
          std::vector<std::reference_wrapper<const Node>> _stack;

        public:
          df_const_pre_order_iterator( Tree& aTree ) :
            _tree( aTree ) {
            if ( !_tree.empty( ) ) {
              _stack.push_back( _tree.root( ) );
            }
          }

        private:
          bool equal( df_const_pre_order_iterator const& aOther ) const {
            bool ret = false;
            if ( _stack.size( ) == aOther._stack.size( ) ) {
              if ( _stack.back( ) == aOther._stack.back( ) ) {
                ret = true;
              }
            }
            return ret;
          }

          Node const& dereference( ) const {
            return _stack.back( );
          }

          //iterativePreorder( node )
          //  parentStack = empty stack
          //  while ( not parentStack.isEmpty( ) or node != null )
          //    if ( node != null )
          //      visit( node )
          //      if ( node.right != null ) parentStack.push( node.right )
          //        node = node.left
          //      else
          //      node = parentStack.pop( )
          void increment( ) {
            if ( _stack.empty( ) ) {
              return;
            }

            auto& node = _stack.back( );
            _stack.pop_back( );
            for ( auto& n : node.children( ) ) {
              _stack.push_back( n );
            }
          }
        };
      } // namespace _private

      //=====================================================================
      // Tree Definition
      template<typename NodeDataType>
      class Tree {
      public:
        typedef NodeDataType ValueType;
        typedef Tree<ValueType> TreeType;
        typedef Node<ValueType> Node;
        typedef typename Node::ChildrenContainerType ChildrenContainerType;
        typedef _private::df_pre_order_iterator<Node> df_pre_order_iterator;
        typedef _private::df_const_pre_order_iterator<Node> df_const_pre_order_iterator;
        typedef _private::df_post_order_iterator<Node> df_post_order_iterator;
        class sibling_iterator;
        class leaf_iterator;
      private:
        Node _root;

      public:
        Node& root( ) {
          return _root;
        }

        bool empty( ) {
          bool ret = false;
          if ( _root.empty( ) ) {
            ret = true;
          }
          return ret;
        }

        df_pre_order_iterator df_pre_order_begin( ) {
          df_pre_order_iterator ret( *this );
          return ret;
        }

        df_const_pre_order_iterator df_const_pre_order_begin( ) {
          df_const_pre_order_iterator ret( *this );
          return ret;
        }
      };
    }
  }
}

#endif // BLIB_NTREE_HPP
