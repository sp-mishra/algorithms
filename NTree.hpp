#ifndef BLIB_NTREE_HPP
#define BLIB_NTREE_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Author: BrainlessLibraries

#include <vector>
#include <boost/iterator/iterator_facade.hpp>

namespace blib {
  namespace container {
    namespace tree {
      //=====================================================================
      // Node Iterators implementations
      namespace _private {
        // Left to right iterator for child nodes
        template<typename NodeType>
        class child_node_ltor_iterator :
          public boost::iterator_facade < child_node_ltor_iterator<NodeType>, NodeType, boost::forward_traversal_tag > {
        public:
          typedef NodeType Node;
          typedef Node& NodeRef;
          typedef Node const& ConstNodeRef;
          typedef child_node_ltor_iterator<NodeType> SelfType;

        private:
          typedef typename Node::ChildrenContainerType ChildrenContainerType;
          typedef typename ChildrenContainerType::iterator ItrType;

          friend class IteratorUtility;
        private:
          ItrType _it;
          ItrType _end;

        public:
          child_node_ltor_iterator( ) {
            _it = _end;
          }

          explicit child_node_ltor_iterator( ItrType& aBegin, ItrType& aEnd ) {
            _it = aBegin;
            _end = aEnd;
          }

        private:
          friend class boost::iterator_core_access;

          bool equal( SelfType const& aOther ) const {
            return aOther._it == _it;
          }

          NodeRef dereference( ) const {
            return *_it;
          }

          void increment( ) {
            ++_it;
          }

          ItrType itr( ) {
            return _it;
          }
        };

        // Right to left iterator for child nodes
        template<typename NodeType>
        class child_node_rtol_iterator :
          public boost::iterator_facade < child_node_rtol_iterator<NodeType>, NodeType, boost::forward_traversal_tag > {
        public:
          typedef NodeType Node;
          typedef Node& NodeRef;
          typedef Node const& ConstNodeRef;
          typedef child_node_rtol_iterator<NodeType> SelfType;

        private:
          typedef typename Node::ChildrenContainerType ChildrenContainerType;
          typedef typename ChildrenContainerType::reverse_iterator ItrType;

          friend class IteratorUtility;
        private:
          ItrType _it;
          ItrType _end;

        public:
          child_node_rtol_iterator( ) {
            _it = _end;
          }

          explicit child_node_rtol_iterator( ItrType& aBegin, ItrType& aEnd ) {
            _it = aBegin;
            _end = aEnd;
          }

        private:
          friend class boost::iterator_core_access;

          bool equal( SelfType const& aOther ) const {
            return aOther._it == _it;
          }

          NodeRef dereference( ) const {
            return *_it;
          }

          void increment( ) {
            ++_it;
          }

          ItrType itr( ) {
            return _it;
          }
        };

        class IteratorUtility {
        public:
          template<typename Iterator>
          static typename Iterator::ItrType itr( Iterator aItr ) {
            return aItr.itr( );
          }
        };
      } // _private

      //=====================================================================
      // Tree Node
      template<typename NodeDataType>
      class Node {
      public:
        typedef NodeDataType ValueType;
        typedef ValueType& ValueRef;
        typedef ValueType const& ConstValueRef;
        typedef Node<NodeDataType> NodeType;
        typedef NodeType SelfType;
        typedef NodeType& NodeRef;
        typedef NodeType const& ConstNodeRef;
        typedef std::vector<NodeType> ChildrenContainerType;
        typedef _private::child_node_ltor_iterator<Node<NodeDataType>> child_node_ltor_iterator;
        typedef _private::child_node_rtol_iterator<Node<NodeDataType>> child_node_rtol_iterator;

      private:
        std::shared_ptr<ValueType> _data;
        NodeType* _parent;
        ChildrenContainerType _children;

      private:
        ValueRef get( ) {
          if ( _data ) {
            return *_data;
          }
          else {
            // Throw exception here
          }
        }

      public:
        Node( NodeType* aParent = nullptr ) :
          _parent( aParent ) {}

        Node( ConstValueRef aData, NodeType* aParent = nullptr ) :
          _parent( aParent ) {
          _data = std::make_shared<ValueType>( aData );
        }

        ~Node( ) {
          clear( );
        }

        NodeType* parent( ) {
          return _parent;
        }

        void parent( NodeType* aParent ) {
          _parent = aParent;
        }

        ValueRef data( ) {
          return get( );
        }

        ConstValueRef data( ) const {
          return get( );
        }

        void data( ConstValueRef aData ) {
          _data = std::make_shared<ValueType>( aData );
        }

        // Access the children by index.
        ValueRef operator[]( const std::size_t aIndex ) {
          return _children.at( aIndex );
        }

        std::size_t numberOfChildren( ) const {
          return _children.size( );
        }

        void addChild( ConstNodeRef aNode ) {
          _children.push_back( aNode );
          _children.back( ).parent( this );
        }

        void addChild( NodeType&& aNode ) {
          _children.push_back( aNode );
          _children.back( ).parent( this );
        }

        void addChild( ConstValueRef aValue ) {
          const NodeType n( aValue, this );
          _children.push_back( n );
        }

        //The iterator pos must be valid and dereferenceable. 
        //Thus the end() iterator (which is valid, but is not dereferencable) cannot be used as a value for pos.
        void removeChild( child_node_ltor_iterator const& aItr ) {
          _children.erase( _private::IteratorUtility::itr( aItr ) );
        }

        std::size_t size( ) const {
          return _children.size( );
        }

        bool empty( ) const {
          return !_data && _children.empty( );
        }

        operator bool( ) const {
          bool ret = false;
          if ( _data ) {
            ret = true;
          }
          return ret;
        }

        void clear( ) {
          _data.reset( );
          _children.clear( );
          _parent = nullptr;
        }

        bool operator==( ConstNodeRef aOther ) const {
          bool ret = false;
          if ( aOther._parent == _parent ) {
            if ( aOther._data == _data ) {
              if ( aOther._children.size( ) == _children.size( ) ) {
                ret = true;
              }
            }
          }

          return ret;
        }

        NodeRef operator=( ConstNodeRef aOther ) {
          _parent = aOther._parent;
          _data = aOther._data;
          _children = aOther._children;
          return *this;
        }

        bool isLeaf( ) const {
          return _children.empty( );
        }

        // To support range based for loop
        // Default iteration is left to right
        child_node_ltor_iterator begin( ) {
          child_node_ltor_iterator it( _children.begin( ), _children.end( ) );
          return it;
        }

        child_node_ltor_iterator end( ) {
          child_node_ltor_iterator it( _children.end( ), _children.end( ) );
          return it;
        }

        child_node_ltor_iterator child_node_ltor_begin( ) {
          child_node_ltor_iterator it( _children.begin( ), _children.end( ) );
          return it;
        }

        child_node_ltor_iterator child_node_ltor_end( ) {
          child_node_ltor_iterator it( _children.end( ), _children.end( ) );
          return it;
        }

        child_node_rtol_iterator child_node_rtol_begin( ) {
          child_node_rtol_iterator it( _children.rbegin( ), _children.rend( ) );
          return it;
        }

        child_node_rtol_iterator child_node_rtol_end( ) {
          child_node_rtol_iterator it( _children.rend( ), _children.rend( ) );
          return it;
        }
      };

      //=====================================================================
      // Tree Declaration
      template<typename NodeType>
      class Tree;
      //=====================================================================

      //=====================================================================
      // Tree Definition
      template<typename NodeType>
      class Tree {
      public:
        typedef NodeType Node;
        typedef typename Node::ValueType ValueType;
        typedef typename Node::ValueRef ValueRef;
        typedef typename Node::ConstValueRef ConstValueRef;
        typedef typename Node::NodeRef NodeRef;
        typedef typename Node::ConstNodeRef ConstNodeRef;
        typedef typename Node::ChildrenContainerType ChildrenContainerType;
        typedef typename Node::child_node_ltor_iterator child_node_ltor_iterator;
        typedef typename Node::child_node_rtol_iterator child_node_rtol_iterator;
        typedef Tree<Node> SelfType;
        typedef std::shared_ptr<Node> NodeSharedPtr;

      private:
        NodeSharedPtr _root;

      public:
        Tree( ) {}

        Tree( ConstNodeRef aNode ) {
          root( aNode );
        }

        ~Tree( ) {
          clear( );
        }

        void root( ConstNodeRef aNode ) {
          _root = std::make_shared<Node>( aNode );
        }

        NodeRef root( ) {
          return *_root;
        }

        bool empty( ) const {
          return !_root;
        }

        operator bool( ) const {
          bool ret = false;
          if ( _root ) {
            ret = true;
          }
          return ret;
        }

        void clear( ) {
          if ( _root ) {
            _root->clear( );
          }
          _root.reset( );
        }

        SelfType& operator=( SelfType const& aOther ) {
          _root = aOther;
          return *this;
        }

        bool operator==( SelfType const& aOther ) const {
          return aOther._root == _root;
        }
      };
      //=====================================================================
    }
  }
}

#endif // BLIB_NTREE_HPP
