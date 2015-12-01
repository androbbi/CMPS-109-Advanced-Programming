// Antony Robbins androbbi@ucsc.edu
// Brandon Gomez brlgomez@ucsc.edu
// $Id: bigint.cpp,v 1.61 2014-06-26 17:06:06-07 - - $

#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>

using namespace std;    

#include "bigint.h"
#include "debug.h"

bigint::bigint() :negative(false){
  digit_t digit;
  digit = '0';
  big_value.push_back(digit);
}

bigint::bigint (long that): long_value (that) {
  DEBUGF ('~', this << " -> " << long_value)
}

bigint::bigint (const string& that) { 
   auto itor = that.cbegin();
   bool isnegative = false;
   negative = false;
   digit_t digit;//
   if (*itor == '_') {
      isnegative = true;
      negative = true;//
      //++itor;
   }
   int newval = 0;
   auto ritor = that.crbegin();
   while (ritor != that.crend() and *ritor != '_' and *ritor != '+'){
     digit = (*ritor - '0');// 
     newval = newval * 10 + *ritor++ - '0';
     big_value.push_back(digit);//
    //cout << digit + '0' << endl;
   }
   long_value = isnegative ? - newval : + newval;
   //DEBUGF ('~', this << " -> " << long_value)
}

bigint::bigvalue_t trim(bigint::bigvalue_t result){
  for(auto it = result.rbegin(); it != result.rend()
                                 and *it == 0; ++it){
    result.pop_back();
  }
  if(result.empty()) result.push_back(0);
  return result;
}

bigint::bigvalue_t do_bigadd(const bigint::bigvalue_t& left,
                             const bigint::bigvalue_t& right){
  bigint::bigvalue_t sum;
  int carry = 0;
  bool left_empty = false;
  bool right_empty = false;
  auto left_it = left.begin(); 
  //bigint::bigvalue_t::const_reverse_iterator left_it = left.rbegin();
  auto right_it = right.begin();
  auto left_end = left.end();
  auto right_end = right.end();
  if(left.size() == 0){
    left_empty = true;
  }
  if(right.size() == 0){
    right_empty = true;
  }
  while(true){
    int curr_value = (left_empty?0:*left_it) +
                 (right_empty?0:*right_it) + carry;
    carry = 0;
    if(curr_value >= 10){
      carry = 1;
      curr_value -= 10;
    }
    sum.push_back(curr_value);
    if(++left_it == left_end){
      left_empty = true;
    }
    if(++right_it == right_end){
      right_empty = true; 
    }
    if(left_empty and right_empty){
      if(carry == 1){
        sum.push_back(carry);
      }
      break;
    }
  }
  return sum;
}

bigint::bigvalue_t do_bigsub(const bigint::bigvalue_t& left,
                             const bigint::bigvalue_t& right){
  bigint::bigvalue_t sum;
  int carry = 0;
  bool left_empty = false;
  bool right_empty = false;
  auto left_it = left.begin(); 
  //bigint::bigvalue_t::const_reverse_iterator left_it = left.rbegin();
  auto right_it = right.begin();
  auto left_end = left.end();
  auto right_end = right.end();
  if(left.size() == 0){
    left_empty = true;
  }
  if(right.size() == 0){
    right_empty = true;
  }
  while(true){
    int curr_value = (left_empty?0:*left_it) -
                    (right_empty?0:*right_it) + carry;
    carry = 0;
    if(curr_value < 0 and !left_empty){
      carry = -1;
      curr_value += 10;
    }
    sum.push_back(curr_value);
    if(++left_it == left_end){
      left_empty = true;
    }
    if(++right_it == right_end){
      right_empty = true;
    }
    if(left_empty and right_empty){
      break;
    }
  }
  //remove leading 0s
  sum = trim(sum);
  return sum;
}

bigint operator+ (const bigint& left, const bigint& right) {
   bigint sum = bigint();
   int neg = bigint::do_bigless(left.big_value, right.big_value);
   if(left.negative == false and right.negative == false){
     sum.big_value = do_bigadd(left.big_value, right.big_value);
   }
   if(left.negative == true and right.negative == false){
     //_2 1 + = -1
     if(neg == -1){
       sum.big_value = do_bigsub(left.big_value, right.big_value);
       sum.negative = true;
     }
     else{
       sum.big_value = do_bigsub(right.big_value, left.big_value); 
     }
   }
   if(left.negative == false and right.negative == true){
     // 4 _6 + = -2
     if(neg == 1){
       sum.big_value = do_bigsub(right.big_value, left.big_value);
       sum.negative = true;
     }
     else{
       sum.big_value = do_bigsub(left.big_value, right.big_value);
     }
   }
   if(left.negative == true and right.negative == true){
     sum.big_value = do_bigadd(left.big_value, right.big_value); 
     sum.negative = true;
   }
   return sum;
}

bigint operator- (const bigint& left, const bigint& right) {
   bigint sum = bigint();
   int neg = bigint::do_bigless(left.big_value, right.big_value);
   if(left.negative == false and left.negative == false){
     //5 9 - = 5-9 = -4, 9 5 - = 9-5 = 4
     if(neg == 1){
       sum.big_value = do_bigsub(right.big_value, left.big_value);
       sum.negative = true;
     }
     else{
       sum.big_value = do_bigsub(left.big_value, right.big_value);
     }
   }
   if(left.negative == true and right.negative == false){
     //_5 6 - = -11, _6 5 - = -11
     if(neg == 1){
       sum.big_value = do_bigadd(right.big_value, left.big_value);
       sum.negative = true;
     }
     else{
      //if(left.big_value.size() > right.big_value.size()) {
          sum.big_value = do_bigadd(left.big_value, right.big_value);
        sum.negative = true;
      //}
      //else
      //sum.big_value = do_bigsub(left.big_value, right.big_value);
     }
   }
   if(left.negative == false and right.negative == true){
     sum.big_value = do_bigadd(left.big_value, right.big_value);
     //5 _6 - = 11, 6 _5 - = 11    
   }
   if(left.negative == true and right.negative == true){
     sum.big_value = do_bigsub(right.big_value, left.big_value);
     sum.negative = true;
   }
   return sum;
}

bigint operator+ (const bigint& right) {
   bigint sum = bigint();
   bigint::bigvalue_t zero;
   sum.big_value = do_bigadd(zero, right.big_value);
   return sum;
   //return +right.long_value;
}

bigint operator- (const bigint& right) {
   bigint sum = bigint();
   bigint::bigvalue_t zero;
   sum.big_value = do_bigsub(zero, right.big_value);
   sum.negative = !sum.negative;
   return sum;
   //return -right.long_value;
}

long bigint::to_long() const {
  //if (*this <= bigint (numeric_limits<long>::min())
  //or *this > bigint (numeric_limits<long>::max()))
  //           throw range_error ("bigint__to_long: out of range");
   long result = 0;
   long mul = 1;
   for(auto it = this->big_value.rbegin();
            it != this->big_value.rend(); ++it){
     int itor = *it;
     result += (itor * mul);
     mul *= 10;
   }
   return result;
}

bool abs_less (const long& left, const long& right) {
   return left < right;
}

// false == left > right, true == left < right
int bigint::do_bigless(const bigvalue_t& left, const bigvalue_t& right){
  auto left_it = left.rbegin();
  auto right_it = right.rbegin();
  auto left_end = left.rend();
  auto right_end = right.rend();
  //cout << "left size" << left.size() << endl;
  //cout << "right size" << right.size() << endl;
  if(left.size() > right.size()){
    return -1;
  }
  if(left.size() < right.size()){
    return 1;
  }
  while(left_it != left_end and right_it != right_end){
    if(*left_it > *right_it){
      return -1;
    }
    else if (*left_it < *right_it){
      return 1;
    }
    ++left_it;
    ++right_it;
  }
  return 0;
}

bool bigint::do_bigabs_less (const bigvalue_t& left,
                             const bigvalue_t& right) {
   if(right.size() > left.size()) {
      return true;
   }
   bigvalue_t::const_reverse_iterator left_it = left.rbegin();
   bigvalue_t::const_reverse_iterator right_it = right.rbegin();
   bigvalue_t::const_reverse_iterator left_end = left.rend();
   bigvalue_t::const_reverse_iterator right_end = right.rend();
   
   while(left_it != left_end and right_it != right_end){
      if(*left_it > *right_it){
         return false;
      }
      else if (*left_it < *right_it){
         return true;
      }
      ++left_it;
      ++right_it;
   }
   return false;
}
/*
bigint::bigvalue_t reverse_bigint(const bigint::bigvalue_t curr) {
   bigint::bigvalue_t rev;
   for(auto it = curr.crbegin(); it != curr.crend(); ++it){
      int i = *it;
     rev.push_back(i);
   }
   return rev;
}
*/
//
// Multiplication algorithm.
//

bigint::bigvalue_t do_bigmult(const bigint::bigvalue_t& left,
                              const bigint::bigvalue_t& right){
  bigint::bigvalue_t prod;
  prod.resize(left.size() + right.size());
  int carry = 0;
  int d = 0;
  int uv = 0;
  
  for(unsigned i = 0; i != left.size(); ++i) {
    carry = 0;
    for(unsigned j = 0; j != right.size(); ++j) { 
      uv = left.at(i) * right.at(j);    
      d = prod.at(i + j) + uv + carry;
      prod.at(i + j) = d % 10;
      carry = d/10;
    }
    if(carry != 0){
      prod.at(right.size() + i) = carry;
    }
  }
  prod = trim(prod);
  return prod;
}

bigint operator* (const bigint& left, const bigint& right) {
   bigint prod = bigint();
   prod.big_value = do_bigmult(left.big_value, right.big_value);
   if(left.negative != right.negative){
     prod.negative = true;
   }
   if(left.negative == right.negative){
     prod.negative = false;
   }
   //for(int i: prod.big_value){
   //cout << i;
   //}
   //cout << endl;
   return prod;
   //return left.long_value * right.long_value;
}

//
// Division algorithm.
//

void multiply_by_2 (bigint::bigvalue_t& mult_by) {
   mult_by = do_bigadd(mult_by, mult_by);
}

void divide_by_2 (bigint::bigvalue_t& div_by) {
   //int data = 0;      begin/rend 000001 end/rbegin == 100000
   for(unsigned it = 0; it < div_by.size(); ++it) {
      
      // Divide the value at index it by 2
      div_by.at(it) /= 2;
     // If the next higher digit is odd, add 5 to current digit
    if(it < div_by.size() - 1) {
         if(div_by.at(it + 1) % 2) { 
            div_by.at(it) += 5;
         }
      }      
   }
   div_by = trim(div_by);
}

bigint::quot_rem divide (const bigint& left, const bigint& right) {
   if (right.big_value.size() == 1) {
      if(right.big_value.at(0) == 0) throw domain_error ("divide by 0");
   }

   bigint zero = bigint(0);
   zero.big_value.push_back(0);
   
   bigint quotient = bigint(0);
   quotient.big_value.push_back(0);
     
   bigint divisor = bigint();
   divisor.big_value = right.big_value;
   
   bigint remainder = bigint();
   remainder.big_value = left.big_value;
   
   bigint power_of_2 = bigint(0);
   power_of_2.big_value.push_back(1);
   
   // Check to see if remainder is > divisor
   // Remainder is left, Divisor is right as shown above
   while (bigint::do_bigabs_less (divisor.big_value,
          remainder.big_value)) {
      multiply_by_2 (divisor.big_value);
      multiply_by_2 (power_of_2.big_value);  
   }

   // Check to see if Power of 2 is > zero
   while (bigint::do_bigabs_less (zero.big_value,
   power_of_2.big_value)) {
      // Check to see if divisor is not > remainder
      if (not (bigint::do_bigabs_less(remainder.big_value,
     divisor.big_value))) {
        /*for(int i: quotient.big_value){
            cout << i;
         }
        cout<< "\n" << endl;*/
         remainder.big_value = do_bigsub(remainder.big_value,
         divisor.big_value);
       
         quotient.big_value = do_bigadd(quotient.big_value,
         power_of_2.big_value);
       
      }

     //std::reverse(divisor.big_value.begin(), divisor.big_value.end());
      divide_by_2(divisor.big_value);
      divide_by_2(power_of_2.big_value);
   }
   //quotient.big_value = trim(quotient.big_value);
   //remainder.big_value = trim(remainder.big_value);
   return {quotient, remainder};
}

bigint operator/ (const bigint& left, const bigint& right) {
   bigint div = bigint();
   div = divide(left, right).first;
   if(left.negative != right.negative){
     div.negative = true;
   }
   if(left.negative == right.negative){
     div.negative = false;
   }
   //return div(left, right).first;
   return div;
   //return div (left, right).first;
}

bigint operator% (const bigint& left, const bigint& right) {
   //return divide (left, right).second;
   bigint rem = bigint();
   rem = divide(left, right).second;
   if(left.negative != right.negative){
     rem.negative = true;
   }
   if(left.negative == right.negative){
     rem.negative = false;
   }
   //return div(left, right).first;
   return rem;
}

bool operator== (const bigint& left, const bigint& right) {
   bool result;
   result = bigint::do_bigless(left.big_value, right.big_value);
   if(result == 0){
     return true;
   }
   return false;
   //return left.long_value == right.long_value;
}

bool operator< (const bigint& left, const bigint& right) {
   bool result;
   result = bigint::do_bigless(left.big_value, right.big_value);
   if(result == 1){
     return true;
   }
   return false;
   //return left.long_value < right.long_value;
}

ostream& operator<< (ostream& out, const bigint& that) {
   if(that.negative == true){
     out << "-";
   }
   for(auto it = that.big_value.crbegin();
        it != that.big_value.crend(); ++it){
     int i = *it;
     out << i;
   }
   return out;
   //out << that.long_value;
}

bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
   bigint base_copy = base;
   long expt = exponent.to_long();
   //long_max
   if(expt > 2147483647){
     throw range_error ("Exponent cannot fit");
   }
   bigint result = bigint("1");
   if (expt < 0) {
      base_copy = 1 / base_copy;
      expt = - expt;
   }
   while (expt > 0) {
      if (expt & 1) { //odd
         result = result * base_copy;
         --expt;
      }else { //even
         base_copy = base_copy * base_copy;
         expt /= 2;
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}
