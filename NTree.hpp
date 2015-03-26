#ifndef BLIB_NTREE_HPP
#define BLIB_NTREE_HPP

#include <vector>
#include <functional>
#include <boost/iterator/iterator_facade.hpp>
#include <stack>

namespace blib {
  namespace container {
    namespace tree {
      template<typename NodeDataType>
      class Node;
      //=====================================================================
      // Tree Iterators implementations
      namespace _private {
        template<typename NodeType>
        class child_node_ltor_iterator :
          public boost::iterator_facade < child_node_ltor_iterator<NodeType>, NodeType, boost::forward_traversal_tag > {
        public:
          typedef NodeType Node;
          typedef Node& NodeRef;
          typedef Node const& ConstNodeRef;
          typedef typename Node::ChildrenContainerType ChildrenContainerType;
          typedef child_node_ltor_iterator<NodeType> SelfType;
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
        typedef NodeType& NodeRef;
        typedef NodeType const& ConstNodeRef;
        typedef std::vector<NodeType> ChildrenContainerType;
        typedef _private::child_node_ltor_iterator<Node<NodeDataType>> child_node_ltor_iterator;

      private:
        std::shared_ptr<ValueType> _data;
        ChildrenContainerType _children;

      private:
        ValueRef get( ) {
          if ( _data ) {
            return *_data;
          }
        }

        void assignData( ConstValueRef aData ) {
          _data.reset( new ValueType );
          *_data = aData;
        }

      public:
        Node( ) {}

        Node( ConstValueRef aData ) {
          assignData( aData );
        }

        ValueRef data( ) {
          return get( );
        }

        ConstValueRef data( ) const {
          return get( );
        }

        void data( ConstValueRef aData ) {
          assignData( aData );
        }

        //ChildrenContainerType& children( ) {
        // return _children;
        //}

        ValueRef operator[]( const std::size_t aIndex ) {
          return _children.at( aIndex );
        }

        std::size_t numberOfChildren( ) const {
          return _children.size( );
        }

        void addChild( ConstNodeRef aNode ) {
          _children.push_back( aNode );
        }

        void addChild( NodeType&& aNode ) {
          _children.push_back( aNode );
        }

        void addChild( ConstValueRef aValue ) {
          const NodeType n( aValue );
          addChild( n );
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
          return _data && _children.empty( );
        }

        void clear( ) {
          _data.reset( );
          for ( auto child : _children ) {
            child.clear( );
          }
        }

        bool operator==( ConstNodeRef aOther ) const {
          bool ret = false;
          if ( aOther.data( ) == _data ) {
            if ( aOther._children.size( ) == _children.size( ) ) {
              ret = true;
            }
          }
          return ret;
        }

        NodeRef operator=( ConstNodeRef aOther ) {
          _data = aOther._data;
          _children = aOther._children;
          return *this;
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
      };

      //=====================================================================
      // Tree Declaration
      template<typename NodeDataType>
      class Tree;
      //=====================================================================
    }
  }
}

#endif // BLIB_NTREE_HPP
