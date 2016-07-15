#include "Utilities/utilitiesMath.h"

#include <limits>

typedef union
{
	unsigned __int64 ll;
	struct
	{
		unsigned __int32 low, high;
	} l;
} UInt64;

static inline void utilities_uint64_mul_uint64(UInt64 *c1, UInt64 *c0, unsigned __int64 arg1, unsigned __int64 arg2)
{
  UInt64 a1, b0;
  UInt64 v, n;

  /* prepare input */
  v.ll = arg1;
  n.ll = arg2;

  /* do 128 bits multiply
   *                   nh   nl
   *                *  vh   vl
   *                ----------
   * a0 =              vl * nl
   * a1 =         vl * nh
   * b0 =         vh * nl
   * b1 =  + vh * nh
   *       -------------------
   *        c1h  c1l  c0h  c0l
   *
   * "a0" is optimized away, result is stored directly in c0.  "b1" is
   * optimized away, result is stored directly in c1.
   */
  c0->ll = (unsigned __int64) v.l.low * n.l.low;
  a1.ll = (unsigned __int64) v.l.low * n.l.high;
  b0.ll = (unsigned __int64) v.l.high * n.l.low;

  /* add the high word of a0 to the low words of a1 and b0 using c1 as
   * scratch space to capture the carry.  the low word of the result becomes
   * the final high word of c0 */
  c1->ll = (unsigned __int64) c0->l.high + a1.l.low + b0.l.low;
  c0->l.high = c1->l.low;

  /* add the carry from the result above (found in the high word of c1) and
   * the high words of a1 and b0 to b1, the result is c1. */
  c1->ll = (unsigned __int64) v.l.high * n.l.high + c1->l.high + a1.l.high + b0.l.high;
}

/* count leading zeros */
static inline unsigned int gst_util_clz (unsigned int val)
{
  unsigned int s;

  s = val | (val >> 1);
  s |= (s >> 2);
  s |= (s >> 4);
  s |= (s >> 8);
  s = ~(s | (s >> 16));
  s = s - ((s >> 1) & 0x55555555);
  s = (s & 0x33333333) + ((s >> 2) & 0x33333333);
  s = (s + (s >> 4)) & 0x0f0f0f0f;
  s += (s >> 8);
  s = (s + (s >> 16)) & 0x3f;

  return s;
}

/* based on Hacker's Delight p152 */
static inline unsigned __int64 utilities_div128_64(UInt64 c1, UInt64 c0, unsigned __int64 denom)
{
  UInt64 q1, q0, rhat;
  UInt64 v, cmp1, cmp2;
  unsigned int s;

  v.ll = denom;

  /* count number of leading zeroes, we know they must be in the high
   * part of denom since denom > G_MAXUINT32. */
  s = gst_util_clz (v.l.high);

  if (s > 0) {
    /* normalize divisor and dividend */
    v.ll <<= s;
    c1.ll = (c1.ll << s) | (c0.l.high >> (32 - s));
    c0.ll <<= s;
  }

  q1.ll = c1.ll / v.l.high;
  rhat.ll = c1.ll - q1.ll * v.l.high;

  cmp1.l.high = rhat.l.low;
  cmp1.l.low = c0.l.high;
  cmp2.ll = q1.ll * v.l.low;

  while (q1.l.high || cmp2.ll > cmp1.ll) {
    q1.ll--;
    rhat.ll += v.l.high;
    if (rhat.l.high)
      break;
    cmp1.l.high = rhat.l.low;
    cmp2.ll -= v.l.low;
  }
  c1.l.high = c1.l.low;
  c1.l.low = c0.l.high;
  c1.ll -= q1.ll * v.ll;
  q0.ll = c1.ll / v.l.high;
  rhat.ll = c1.ll - q0.ll * v.l.high;

  cmp1.l.high = rhat.l.low;
  cmp1.l.low = c0.l.low;
  cmp2.ll = q0.ll * v.l.low;

  while (q0.l.high || cmp2.ll > cmp1.ll) {
    q0.ll--;
    rhat.ll += v.l.high;
    if (rhat.l.high)
      break;
    cmp1.l.high = rhat.l.low;
    cmp2.ll -= v.l.low;
  }
  q0.l.high += q1.l.low;

  return q0.ll;
}

extern "C" UTILITIES_EXPORT unsigned __int64 utilities_uint64_scale(unsigned __int64 val, unsigned __int64 num, unsigned __int64 denom)//, unsigned __int64 correct)
{
	UInt64 c1, c0;

	/* compute 128-bit numerator product */
	utilities_uint64_mul_uint64 (&c1, &c0, val, num);

//	/* perform rounding correction */
//	CORRECT (c0, c1, correct);

	/* high word as big as or bigger than denom --> overflow */
	if(c1.ll >= denom)
		return std::numeric_limits<unsigned __int64>::max();

	/* compute quotient, fits in 64 bits */
	return utilities_div128_64 (c1, c0, denom);
}