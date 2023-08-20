/*
    
Vec2 a;
Vec2 b;
// a += b
a = (Vec2){add2(a, b)};

*/

#if !defined(LA_H_)
#define LA_H_

typedef struct Vec2 {
	int x, y;
} Vec2;

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define COMPARE2(a, tok, b) ((a).x tok (b).x && (a).y tok (b).y)
#define ARITH2(r, a, tok, b) ((r).x = (a).x tok (b).x, (r).y = (a).y tok (b).y)
#define SCALAR2(r, a, tok, s) ((r).x = (a).x tok s, (r).y = (a).y tok s)

#define SIGN(T, x)    ((T)((x) > 0) - (T)((x) < 0))
#define ABS(T, x)     (SIGN(T, x)*(x))
#define MAX(T, a, b) ((a > b) ? a : b)
#define MIN(T, a, b) ((a > b) ? b : a)

// Print formatting
#define v2_arg(v) (v).x, (v).y
#define v2_fmt "%d,%d"

//----------------------------------------------------------------------------------
// Comparison Operators
//----------------------------------------------------------------------------------
#define v2_eql(a, b) COMPARE2(a, ==, b)
#define v2_neq(a, b) COMPARE2(a, !=, b)
#define v2_gtr(a, b) COMPARE2(a, >,  b)
#define v2_gte(a, b) COMPARE2(a, >=, b)
#define v2_lss(a, b) COMPARE2(a, <,  b)
#define v2_lte(a, b) COMPARE2(a, <=, b)

//----------------------------------------------------------------------------------
// Arithmetic Operators
//----------------------------------------------------------------------------------
#define v2_add(r, a, b) ARITH2(r, a, +, b)
#define v2_sub(r, a, b) ARITH2(r, a, -, b)
#define v2_mul(r, a, b) ARITH2(r, a, *, b)
#define v2_div(r, a, b) ARITH2(r, a, /, b)
#define v2_rem(r, a, b) ARITH2(r, a, %, b)

//----------------------------------------------------------------------------------
// Scalar Operators
//----------------------------------------------------------------------------------
#define v2_adds(r, a, s) SCALAR2(r, a, +, s)
#define v2_subs(r, a, s) SCALAR2(r, a, -, s)
#define v2_muls(r, a, s) SCALAR2(r, a, *, s)
#define v2_divs(r, a, s) SCALAR2(r, a, /, s)
#define v2_rems(r, a, s) SCALAR2(r, a, %, s)

//----------------------------------------------------------------------------------
// Algorithmic Operators
//----------------------------------------------------------------------------------
#define v2_sqr(r, a) ARITH2(r, a, *, a)
#define v2_dst(r, a, b) (v2_sub(r, a, b), v2_sqr(r, r), (r).x + (r).y)
#define v2_sgn(r, a) ((r).x = SIGN(int, (a).x), (r).y = SIGN(int, (a).y))

#endif // LA_H_
