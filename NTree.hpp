#ifndef BLIB_NTREE_HPP
#define BLIB_NTREE_HPP

#include <vector>
#include <functional>
#include <boost/iterator/iterator_facade.hpp>

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
      // Iterators
      namespace _private {
        //=====================================================================
        // PostOrder Iterators
        template<typename NodeType>
        class df_post_order_iterator :
          public boost::iterator_facade < df_post_order_iterator<NodeType>, NodeType, boost::forward_traversal_tag > {
        private:
          friend class boost::iterator_core_access;
          typedef NodeType Node;
          typedef typename Node::ValueType ValueType;
          typedef typename Tree<ValueType>::TreeType Tree;

          Tree& _tree;
          std::vector<std::reference_wrapper<Node>> _stack;

        public:
          df_post_order_iterator( ) {}

          df_post_order_iterator( Tree& aTree ) :
            _tree( aTree ) {
            if ( !_tree.empty( ) ) {
              _stack.push_back( _tree.root( ) );
            }
          }

        private:
          bool equal( df_post_order_iterator const& aOther ) const {
            bool ret = false;
            if ( !_stack.empty( ) ) {
              if ( _stack.back( ) == aOther._stack.back( ) ) {
                ret = true;
              }
            }
            return ret;
          }

          Node& dereference( ) const {
            return _stack.back( );
          }

          void increment( ) {
          }
        };

        //=====================================================================
        // Preorder Iterators
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
            if ( !_stack.empty( ) ) {
              if ( _stack.back( ) == aOther._stack.back( ) ) {
                ret = true;
              }
            }
            return ret;
          }

          Node& dereference( ) const {
            return _stack.back( );
          }

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
            if ( !_stack.empty( ) ) {
              if ( _stack.back( ) == aOther._stack.back( ) ) {
                ret = true;
              }
            }
            return ret;
          }

          Node const& dereference( ) const {
            return _stack.back( );
          }

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
