// $Id: listmap.tcc,v 1.5 2014-07-09 11:50:34-07 - - $

/*
  This Program was edited by
  Partner: Brandon Gomez (brlgomez@ucsc.edu)
  Partner: Antony Robbins (androbbi@ucsc.edu)
*/

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   TRACE ('l', (void*) this);
}

//
// listmap::empty()
// 
template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::empty() const {
   return anchor_.next == anchor_.prev;
}

//
// listmap::iterator listmap::begin()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin() {
   return iterator (anchor_.next);
}

//
// listmap::iterator listmap::end()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end() {
   return iterator (anchor());
}


//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   //cout << "inside insert" << endl;
   // if empty insert in front
   // if else look in using itor to find a the less for lexico
   // else insert at back
   auto it = begin();
   if(begin() == end()){
      node* in = new node(begin().return_node(), 
           begin().return_node()->next, pair);
      //node* in = new node(begin(), begin(), pair);
      begin().return_node()->next = in;
      begin().return_node()->next->prev = in;
      return it;
   }
   else{
      for(; it != end(); ++it){
        if(less((pair).first, (*it).first)){
            node* in = new node(it.return_node(),
                  (--it).return_node(), pair);
            (++it).return_node()->prev->next = in;
            (it).return_node()->prev = in;
            return it;
         }
      }
      node* in = new node(it.return_node(),(--it).return_node(), pair);
      (++it).return_node()->prev->next = in;
      (it).return_node()->prev = in;
      return it;
   }
   TRACE ('l', &pair << "->" << pair);
   //return iterator();
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that);
   auto it = begin();
   for(; it != end(); ++it){
      if(it->first == that){
         return it;
      }
   }
   return it;
   //return iterator();
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   TRACE ('l', &*position);
   auto pos = position.return_node();
   ++position;
   pos->prev->next = pos->next;
   pos->next->prev = pos->prev;
   delete pos;
   return position;
   //return iterator();
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

template <typename Key, typename Value, class Less>
typename  listmap<Key,Value,Less>::node*
listmap<Key,Value,Less>::iterator::return_node(){
      return  where;
}
