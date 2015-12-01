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
   if (itor != that.cend() and *itor == '_') {
      isnegative = true;
      negative = true;//
      ++itor;
   }
   int newval = 0;
   while (itor != that.end()){
     digit = (*itor - '0');// 
     newval = newval * 10 + *itor++ - '0';
     big_value.push_back(digit);//
   }
   long_value = isnegative ? - newval : + newval;
   DEBUGF ('~', this << " -> " << long_value)
}

bigint operator+ (const bigint& left, const bigint& right) {
   bigint sum = bigint();
   int neg = bigint::do_bigless(left.big_value, right.big_value);
   if(left.negative == false and right.negative == false){
     sum.big_value = bigint::do_bigadd(left.big_value,right.big_value);
   }
   if(left.negative == true and right.negative == false){
     //_2 1 + = -1
     //cout << "made it";
     if(neg == -1){
       sum.big_value = bigint::do_bigsub(left.big_value, right.big_value);
       sum.negative = true;
     }
     else{
       sum.big_value = bigint::do_bigsub(right.big_value, left.big_value); 
     }
   }
   if(left.negative == false and right.negative == true){
     // 4 _6 + = -2
     if(neg == 1){
       sum.big_value = bigint::do_bigsub(right.big_value, left.big_value);
       sum.negative = true;
     }
     else{
       sum.big_value = bigint::do_bigsub(left.big_value, right.big_value);
     }
   }
   if(left.negative == true and right.negative == true){
     sum.big_value = bigint::do_bigadd(left.big_value, right.big_value); 
     sum.negative = true;
   }
   for(int i: sum.big_value){
     cout << i;
   }
   cout << endl;
   return sum;
   //return left.long_value + right.long_value;
}

// false == left > right true == left < right    
bigint operator- (const bigint& left, const bigint& right) {
   bigint sum = bigint();
   int neg = bigint::do_bigless(left.big_value, right.big_value);
   if(left.negative == false and left.negative == false){
     //5 9 - = 5-9 = -4, 9 5 - = 9-5 = 4
     if(neg == 1){
       sum.big_value = bigint::do_bigsub(right.big_value, left.big_value);
       sum.negative = true;
     }
     else{
       sum.big_value = bigint::do_bigsub(left.big_value, right.big_value);
     }
   }
   //left is negative
   if(left.negative == true and right.negative == false){
     sum.big_value = bigint::do_bigsub(left.big_value, right.big_value);
     //_5 6 - = -11, _6 5 - = -11
     if(neg == 1){
       sum.big_value = bigint::do_bigsub(right.big_value, left.big_value);
       sum.negative = true;
     }
     else{
       sum.big_value = bigint::do_bigsub(left.big_value, right.big_value);
     }
   }
   if(left.negative == false and right.negative == true){
     sum.big_value = bigint::do_bigadd(left.big_value, right.big_value);
     //5 _6 - = 11, 6 _5 - = 11    
   }
   //left is negative
   if(left.negative == true and right.negative == true){
     sum.big_value = bigint::do_bigsub(right.big_value, left.big_value);
     sum.negative = true;
   }
   for(int i: sum.big_value){
     cout << i;
   }
   cout << endl;
   return sum;
   //return left.long_value - right.long_value;
}

bigint operator+ (const bigint& right) {
   bigint sum{};
   bigint::bigvalue_t zero;
   sum.big_value = bigint::do_bigadd(zero, right.big_value);
   return sum;
   //return +right.long_value;
}

bigint operator- (const bigint& right) {
   bigint sum{};
   bigint::bigvalue_t zero;
   sum.big_value = bigint::do_bigsub(zero, right.big_value);
   sum.negative = !sum.negative;
   return sum;
   //return -right.long_value;
}

long bigint::to_long() const {
   if (*this <= bigint (numeric_limits<long>::min())
    or *this > bigint (numeric_limits<long>::max()))
               throw range_error ("bigint__to_long: out of range");
   return long_value;
}

bool abs_less (const long& left, const long& right) {
   return left < right;
}

bool bigint::do_bigabs_less (const bigvalue_t& left, const bigvalue_t& right) {
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

// false == left > right true == left < right
int bigint::do_bigless(const bigvalue_t& left, const bigvalue_t& right){
  bigvalue_t::const_reverse_iterator left_it = left.rbegin();
  bigvalue_t::const_reverse_iterator right_it = right.rbegin();
  bigvalue_t::const_reverse_iterator left_end = left.rend();
  bigvalue_t::const_reverse_iterator right_end = right.rend();
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

bigint::bigvalue_t bigint::trim(bigvalue_t result){
  bigvalue_t::const_iterator result_it = result.begin();
  bigvalue_t::const_iterator result_end = result.end();
  while(result_it != result_end and *result_it == 0){
    ++result_it;
    result.pop_back();
  }
  return result;
}

bigint::bigvalue_t bigint::do_bigadd(const bigvalue_t& left, const bigvalue_t& right){
   bigvalue_t sum;
   int carry = 0;
   bool left_empty = false;
   bool right_empty = false;
   bigvalue_t::const_reverse_iterator left_it = left.rbegin();
   bigvalue_t::const_reverse_iterator right_it = right.rbegin();
   bigvalue_t::const_reverse_iterator left_end = left.rend();
   bigvalue_t::const_reverse_iterator right_end = right.rend();
   if(left.size() == 0){
     left_empty = true;
   }
   if(right.size() == 0){
     right_empty = true;
   }
   while(true){
     int curr_value = (left_empty?0:*left_it) + (right_empty?0:*right_it) + carry;
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

bigint::bigvalue_t bigint::do_bigsub(const bigvalue_t& left, const bigvalue_t& right){
   bigvalue_t sum;
   int carry = 0;
   bool left_empty = false;
   bool right_empty = false;
   bigvalue_t::const_reverse_iterator left_it = left.rbegin();
   bigvalue_t::const_reverse_iterator right_it = right.rbegin();
   bigvalue_t::const_reverse_iterator left_end = left.rend();
   bigvalue_t::const_reverse_iterator right_end = right.rend();
   if(left.size() == 0){
     left_empty = true;
   }
   if(right.size() == 0){
     right_empty = true;
   }
   while(true){
     int curr_value = (left_empty?0:*left_it) - (right_empty?0:*right_it) + carry;
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
   //sum = trim(sum);
   return sum;
}

//
// Multiplication algorithm.
//

bigint operator* (const bigint& left, const bigint& right) {
   bigint prod = bigint();
   prod.big_value = bigint::do_bigmult(left.big_value, right.big_value);
   if(left.negative != right.negative){
     prod.negative = true;
   }
   if(left.negative == right.negative){
     prod.negative = false;
   }
   for(int i: prod.big_value){
     cout << i;
   }
   cout << endl;
   return prod;
   //return left.long_value * right.long_value;
}

bigint::bigvalue_t bigint::do_bigmult(const bigvalue_t& left, const bigvalue_t& right){
   bigvalue_t prod;
   prod.resize(left.size() + right.size());
   int carry = 0;
   int d = 0;
   for(int i = left.size()-1; i != -1; --i) {
     carry = 0;
     for(int j = right.size()-1; j != -1; --j) {     
       d = (prod.at((left.size()-1 -i) + (right.size()-1 - j))) + (left.at(i) * right.at(j)) + carry;
       //cout << (prod.at((left.size()-1 - i) + (right.size()-1 - j))) << " new ";
       //cout << (left.at(i) * right.at(j)) << " old ";
       //cout << carry << "carry" << endl;
       //cout << d << endl;
       //cout << d%10 << "<--insert"<<endl;
       //cout << d/10 << " carry" << endl;
       prod.at((left.size()-1 - i) + (right.size()-1 - j)) = d % 10;
       carry = d/10;
     }
     if(carry != 0){
       //cout << carry << "<- insert c"<<endl;
       prod.at((right.size()-1) - -1) = carry;
     }						
   }
   //trim
   return prod;
}

//
// Division algorithm.
//

void multiply_by_2 (bigint& mult_by) {
   bigint two = bigint();
   two.big_value.push_back(2);
   mult_by.big_value = bigint::do_bigmult(mult_by.big_value, two.big_value);
}

void divide_by_2 (bigint& div_by) {
   //int data = 0;
   for(unsigned it = 0 ; it != div_by.big_value.size(); ++it) {
      // Divide the value at index it 0 by 2
      div_by.big_value.at(it) /= 2;
	  // If the next higher digit is odd, add 5 to current digit
	  if(div_by.big_value.at(it + 1) % 2) { 
	     div_by.big_value.at(it) += 5;
	  }
   }
}

bigint::quot_rem divide (const bigint& left, const bigint& right) {
   if (right.big_value.size() == 1) {
      if(right.big_value.at(0) == 0) throw domain_error ("divide by 0");
   }
   //using unumber = unsigned long;
   //static unumber zero = 0;
   bigint zero = bigint();
   //if (right == 0) throw domain_error ("bigint::divide");
   bigint divisor = bigint();
   bigint quotient = bigint();
   bigint remainder = bigint();
   bigint power_of_2 = bigint();
   
   zero.big_value.push_back(0);
   divisor.big_value = right.big_value;
   quotient.big_value.push_back(0);
   remainder.big_value = left.big_value;
   power_of_2.big_value.push_back(1);
   /*unumber divisor = right.long_value;
   unumber quotient = 0;
   unumber remainder = left.long_value;
   unumber power_of_2 = 1;
   */
   
   // Check to see if remainder is > divisor
   // Remainder is left, Divisor is right as shown above
   while (bigint::do_bigabs_less (divisor.big_value, remainder.big_value)) {

      // cout << (left.at(i) * right.at(j)) << " old " << endl;
      multiply_by_2 (divisor);
      multiply_by_2 (power_of_2);
   }
   // Check to see if Power of 2 is > zero
   while (bigint::do_bigabs_less (zero.big_value, power_of_2.big_value)) {
      // Check to see if divisor is not > remainder
      if (not bigint::do_bigabs_less (remainder.big_value, divisor.big_value)) {
	     remainder.big_value = bigint::do_bigsub(remainder.big_value, divisor.big_value);
		 quotient.big_value = bigint::do_bigadd(quotient.big_value, power_of_2.big_value);
         //remainder = remainder - divisor;
         //quotient = quotient + power_of_2;
      }
      divide_by_2 (divisor);
      divide_by_2 (power_of_2);
   }
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
   for(int i: div.big_value){
     cout << i;
   }
   cout << endl;
   //return div(left, right).first;
   return div;
   //return div (left, right).first;
}

bigint operator% (const bigint& left, const bigint& right) {
   return divide (left, right).second;
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

ostream& bigint::get_stream (ostream& out,const bigvalue_t& that){
   bigvalue_t::const_iterator that_it = that.begin(); 
   bigvalue_t::const_iterator that_end = that.end();
   for(; that_it != that_end; ++that_it){
     out << *that_it;
   } 
   return out;
}

ostream& operator<< (ostream& out, const bigint& that) {
   if(that.negative == true){
     out << "-";
   }
   for(auto it = that.big_value.crbegin(); it != that.big_value.crend(); ++it) {
      int i = *it;
	  out << i;
   }
	  return out;
   //return bigint::get_stream(out, that.big_value);
   //out << that.long_value;
   //return out;
}

bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
   bigint base_copy = base;
   long expt = exponent.to_long();
   bigint result = 1;
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
