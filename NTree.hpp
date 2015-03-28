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


        //=====================================================================
        // Node Handle
        template<class NodeType>
        class NodeHandleImpl {
        public:
          typedef NodeType Node;
          typedef NodeHandleImpl<Node> SelfType;

        private:
          Node const* _handle;
          friend class NodeUtility;

        private:
          Node const* const pointer( ) const {
            return const_cast< Node const* const >( _handle );
          }

        public:
          NodeHandleImpl( Node const* const aPtr = nullptr ) :
            _handle( const_cast< Node const* >( aPtr ) ) {}

          NodeHandleImpl( SelfType const& aNode ) :
            _handle( aNode._handle ) {}

          NodeHandleImpl( Node const& aNode ) :
            _handle( aNode.handle( )._handle ) {}

          ~NodeHandleImpl( ) {
            _handle = nullptr;
          }

          bool operator==( SelfType const& aNode ) const {
            return aNode._handle == _handle;
          }

          bool operator==( Node const& aNode ) const {
            return aNode.handle( )._handle == _handle;
          }

          SelfType& operator=( SelfType const& aNode ) {
            _handle = aNode._handle;
            return *this;
          }

          operator bool( ) const {
            bool ret = false;
            if ( _handle ) {
              ret = true;
            }
            return ret;
          }
        };

        class NodeUtility {
        public:
          template<typename NodeType>
          NodeType const * const getNodeInternal( NodeHandleImpl<NodeType> const& aNodeHandle ) {
            return const_cast< NodeType const * const >( aNodeHandle._handle );
          }
        };
      } // _private

      //=====================================================================
      // Node Handle
      template<class NodeType>
      class NodeHandle :
        public _private::NodeHandleImpl < NodeType > {
      private:
        typedef NodeHandleImpl<NodeType> BaseType;

      public:
        NodeHandle( Node const* const aPtr = nullptr ) :
          BaseType( aPtr ) {}

        NodeHandle( SelfType const& aNode ) :
          BaseType( aNode ) {}

        NodeHandle( Node const& aNode ) :
          BaseType( aNode ) {}
      };

      //=====================================================================
      // Tree Node
      template<
        typename NodeDataType,
        typename DataAlloc = std::allocator<NodeDataType>,
        template<typename>class NodeAlloc = std::allocator>
      class Node {
      public:
        typedef NodeDataType ValueType;
        typedef ValueType& ValueRef;
        typedef ValueType const& ConstValueRef;
        typedef Node<NodeDataType, DataAlloc, NodeAlloc> NodeType;
        typedef NodeType SelfType;
        typedef NodeType& NodeRef;
        typedef NodeType const& ConstNodeRef;
        typedef _private::child_node_ltor_iterator<Node<NodeDataType>> child_node_ltor_iterator;
        typedef _private::child_node_rtol_iterator<Node<NodeDataType>> child_node_rtol_iterator;
        typedef NodeHandle<SelfType> NodeHandle;
        typedef NodeAlloc<SelfType> NodeAllocator;
        typedef DataAlloc DataAllocator;

      private:
        friend class child_node_ltor_iterator;
        friend class child_node_rtol_iterator;
        typedef std::vector<NodeType, NodeAllocator> ChildrenContainerType;

      private:
        std::shared_ptr<ValueType> _data;
        NodeHandle _parent;
        ChildrenContainerType _children;

      public:
        Node( NodeHandle const& aParent = NodeHandle( ) ) :
          _parent( aParent ) {}

        Node( ConstValueRef aData, NodeHandle const& aParent ) :
          _parent( aParent ) {
          _data = std::allocate_shared<ValueType>( DataAllocator( ), aData );
        }

        ~Node( ) {
          clear( );
        }

        NodeHandle& parent( ) const {
          return _parent;
        }

        void parent( NodeHandle const& aParent ) {
          _parent = aParent;
        }

        NodeHandle handle( ) const {
          NodeHandle ret( this );
          return ret;
        }

        ValueRef data( ) {
          return *_data;;
        }

        ConstValueRef data( ) const {
          return *_data;;
        }

        void data( ConstValueRef aData ) {
          _data = std::allocate_shared<ValueType>( DataAllocator( ), aData );
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
          _children.back( ).parent( handle( ) );
        }

        void addChild( NodeType&& aNode ) {
          _children.push_back( aNode );
          _children.back( ).parent( handle( ) );
        }

        void addChild( ConstValueRef aValue ) {
          const NodeType n( aValue, handle( ) );
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
        typedef typename Node::NodeAllocator NodeAllocator;
        typedef typename Node::DataAllocator DataAllocator;
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

        void root( ConstValueRef aVal ) {
          _root = std::allocate_shared<Node>( NodeAllocator( ), aVal );
        }

        void root( ConstNodeRef aNode ) {
          _root = std::allocate_shared<Node>( NodeAllocator( ), aNode );
        }

        NodeRef root( ) {
          return *_root;
        }

        ConstNodeRef root( ) const {
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
