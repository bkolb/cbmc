/*******************************************************************\

Module: C++ Language Conversion

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <cassert>

#include <util/arith_tools.h>
#include <util/config.h>
#include <util/ieee_float.h>
#include <util/std_types.h>
#include <util/string2int.h>
#include <util/expr_util.h>

#include "../c_types.h"
#include "parse_float.h"
#include "convert_float_literal.h"

/*******************************************************************\

Function: convert_float_literal

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

exprt convert_float_literal(const std::string &src)
{
  mp_integer significand;
  mp_integer exponent;
  bool is_float, is_long, is_imaginary;
  bool is_decimal, is_float80, is_float128; // GCC extensions
  unsigned base;
  
  parse_float(src, significand, exponent, base,
              is_float, is_long, is_imaginary,
              is_decimal, is_float80, is_float128);

  exprt result=exprt(ID_constant);
  
  result.set(ID_C_cformat, src);
  
  // In ANSI-C, float literals are double by default,
  // unless marked with 'f'.
  // All of these can be complex as well.
  // This can be overriden with
  // config.ansi_c.single_precision_constant.

  if(is_float)
    result.type()=float_type();
  else if(is_long)
    result.type()=long_double_type();
  else if(is_float80)
  {
    result.type()=ieee_float_spect(64, 15).to_type();
    result.type().set(ID_C_c_type, ID_long_double);
  }
  else if(is_float128)
  {
    result.type()=ieee_float_spect::quadruple_precision().to_type();
    result.type().set(ID_C_c_type, ID_gcc_float128);
  }
  else
  {
    // default
    if(config.ansi_c.single_precision_constant)
      result.type()=float_type(); // default
    else
      result.type()=double_type(); // default
  }

  if(is_decimal)
  {
    // TODO - should set ID_gcc_decimal32/ID_gcc_decimal64/ID_gcc_decimal128,
    // but these aren't handled anywhere
  }
  
  if(config.ansi_c.use_fixed_for_float)
  {
    unsigned width=result.type().get_int(ID_width);
    unsigned fraction_bits;
    const irep_idt integer_bits=result.type().get(ID_integer_bits);
    
    assert(width!=0);

    if(integer_bits==irep_idt())
      fraction_bits=width/2; // default
    else
      fraction_bits=width-safe_string2int(id2string(integer_bits));

    mp_integer factor=mp_integer(1)<<fraction_bits;
    mp_integer value=significand*factor;
    
    if(value!=0)
    {
      if(exponent<0)
        value/=power(base, -exponent);
      else
      {
        value*=power(base, exponent);    

        if(value>=power(2, width-1))
        {
          // saturate: use "biggest value"
          value=power(2, width-1)-1;
        }
        else if(value<=-power(2, width-1)-1)
        {
          // saturate: use "smallest value"
          value=-power(2, width-1);
        }
      }
    }

    result.set(ID_value, integer2binary(value, width));  
  }
  else
  {
    ieee_floatt a;

    a.spec=to_floatbv_type(result.type());
    
    if(base==10)
      a.from_base10(significand, exponent);
    else if(base==2) // hex
      a.build(significand, exponent);
    else
      assert(false);

    result.set(ID_value,
      integer2binary(a.pack(), a.spec.width()));  
  }

  if(is_imaginary)
  {
    complex_typet complex_type;
    complex_type.subtype()=result.type();
    exprt complex_expr(ID_complex, complex_type);
    complex_expr.operands().resize(2);
    complex_expr.op0()=gen_zero(result.type());
    complex_expr.op1()=result;
    return complex_expr;
  }
  
  return result;
}
