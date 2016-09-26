#include "boolector.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define BV1_EXAMPLE_NUM_BITS 8

/* We verify the XOR swap algorithm. The XOR bitwise operation can
 * be used to swap variables without using a temporary variable:
 * int x, y;
 * ...
 * x = x ^ y
 * y = x ^ y
 * x = x ^ y
 */

int
main (void)
{
  Btor *btor;
  BoolectorNode *x, *y, *temp, *old_x, *old_y, *eq1, *eq2, *and, *formula;
  int result;

  btor = boolector_new ();
  x = boolector_var (btor, BV1_EXAMPLE_NUM_BITS, NULL);
  y = boolector_var (btor, BV1_EXAMPLE_NUM_BITS, NULL);
  /* remember initial values of x and y */
  old_x = boolector_copy (btor, x);
  old_y = boolector_copy (btor, y);

  /* x = x ^ y */
  temp = boolector_xor (btor, x, y);
  boolector_release (btor, x);
  x = temp;

  /* y = x ^ y */
  temp = boolector_xor (btor, x, y);
  boolector_release (btor, y);
  y = temp;

  /* x = x ^ y */
  temp = boolector_xor (btor, x, y);
  boolector_release (btor, x);
  x = temp;

  /* Now, we have to show that old_x = y and old_y = x */
  eq1 = boolector_eq (btor, old_x, y);
  eq2 = boolector_eq (btor, old_y, x);
  and = boolector_and (btor, eq1, eq2);

  /* In order to prove that this is a theorem, we negate the whole
   * formula and show that the negation is unsatisfiable */
  formula = boolector_not (btor, and);

  /* We assert the formula and call Boolector */
  boolector_assert (btor, formula);
  result = boolector_sat (btor);
  if (result == BOOLECTOR_UNSAT)
    printf ("Formula is unsatisfiable\n");
  else
    abort ();

  /* cleanup */
  boolector_release (btor, x);
  boolector_release (btor, old_x);
  boolector_release (btor, y);
  boolector_release (btor, old_y);
  boolector_release (btor, eq1);
  boolector_release (btor, eq2);
  boolector_release (btor, and);
  boolector_release (btor, formula);
  assert (boolector_get_refs (btor) == 0);
  boolector_delete (btor);
  return 0;
}
