/*
 * THIS CODE IS SPECIFICALLY EXEMPTED FROM THE NCURSES PACKAGE COPYRIGHT.
 * You may freely copy it for use as a template for your own field types.
 * If you develop a field type that might be of general use, please send
 * it back to the ncurses maintainers for inclusion in the next version.
 */
/***************************************************************************
*                                                                          *
*  Author : Juergen Pfeifer                                                *
*                                                                          *
***************************************************************************/

#include "form.priv.h"

MODULE_ID("$Id: fty_num.c,v 1.1.1.1 2009/10/09 02:45:06 jack Exp $")

#if HAVE_LOCALE_H
#include <locale.h>
#endif

#if HAVE_LOCALE_H
#define isDecimalPoint(c) ((c) == ((L && L->decimal_point) ? *(L->decimal_point) : '.'))
#else
#define isDecimalPoint(c) ((c) == '.')
#endif

#if USE_WIDEC_SUPPORT
#define isDigit(c) (iswdigit((wint_t)(c)) || isdigit(UChar(c)))
#else
#define isDigit(c) isdigit(UChar(c))
#endif

#define thisARG numericARG

typedef struct
  {
    int precision;
    double low;
    double high;
    struct lconv *L;
  }
thisARG;

/*---------------------------------------------------------------------------
|   Facility      :  libnform
|   Function      :  static void *Make_This_Type(va_list * ap)
|
|   Description   :  Allocate structure for numeric type argument.
|
|   Return Values :  Pointer to argument structure or NULL on error
+--------------------------------------------------------------------------*/
static void *
Make_This_Type(va_list *ap)
{
  thisARG *argn = (thisARG *) malloc(sizeof(thisARG));

  if (argn)
    {
      argn->precision = va_arg(*ap, int);
      argn->low = va_arg(*ap, double);
      argn->high = va_arg(*ap, double);

#if HAVE_LOCALE_H
      argn->L = localeconv();
#else
      argn->L = NULL;
#endif
    }
  return (void *)argn;
}

/*---------------------------------------------------------------------------
|   Facility      :  libnform
|   Function      :  static void *Copy_This_Type(const void * argp)
|
|   Description   :  Copy structure for numeric type argument.
|
|   Return Values :  Pointer to argument structure or NULL on error.
+--------------------------------------------------------------------------*/
static void *
Copy_This_Type(const void *argp)
{
  const thisARG *ap = (const thisARG *)argp;
  thisARG *result = (thisARG *) 0;

  if (argp)
    {
      result = (thisARG *) malloc(sizeof(thisARG));
      if (result)
	*result = *ap;
    }
  return (void *)result;
}

/*---------------------------------------------------------------------------
|   Facility      :  libnform
|   Function      :  static void Free_This_Type(void * argp)
|
|   Description   :  Free structure for numeric type argument.
|
|   Return Values :  -
+--------------------------------------------------------------------------*/
static void
Free_This_Type(void *argp)
{
  if (argp)
    free(argp);
}

/*---------------------------------------------------------------------------
|   Facility      :  libnform
|   Function      :  static bool Check_This_Field(FIELD * field,
|                                                 const void * argp)
|
|   Description   :  Validate buffer content to be a valid numeric value
|
|   Return Values :  TRUE  - field is valid
|                    FALSE - field is invalid
+--------------------------------------------------------------------------*/
static bool
Check_This_Field(FIELD *field, const void *argp)
{
  const thisARG *argn = (const thisARG *)argp;
  double low = argn->low;
  double high = argn->high;
  int prec = argn->precision;
  unsigned char *bp = (unsigned char *)field_buffer(field, 0);
  char *s = (char *)bp;
  double val = 0.0;
  struct lconv *L = argn->L;
  char buf[64];
  bool result = FALSE;

  while (*bp && *bp == ' ')
    bp++;
  if (*bp)
    {
      if (*bp == '-' || *bp == '+')
	bp++;
#if USE_WIDEC_SUPPORT
      if (*bp)
	{
	  bool blank = FALSE;
	  int state = 0;
	  int len;
	  int n;
	  wchar_t *list = _nc_Widen_String((char *)bp, &len);

	  if (list != 0)
	    {
	      result = TRUE;
	      for (n = 0; n < len; ++n)
		{
		  if (blank)
		    {
		      if (list[n] != ' ')
			{
			  result = FALSE;
			  break;
			}
		    }
		  else if (list[n] == ' ')
		    {
		      blank = TRUE;
		    }
		  else if (isDecimalPoint(list[n]))
		    {
		      if (++state > 1)
			{
			  result = FALSE;
			  break;
			}
		    }
		  else if (!isDigit(list[n]))
		    {
		      result = FALSE;
		      break;
		    }
		}
	      free(list);
	    }
	}
#else
      while (*bp)
	{
	  if (!isdigit(UChar(*bp)))
	    break;
	  bp++;
	}
      if (isDecimalPoint(*bp))
	{
	  bp++;
	  while (*bp)
	    {
	      if (!isdigit(UChar(*bp)))
		break;
	      bp++;
	    }
	}
      while (*bp && *bp == ' ')
	bp++;
      result = (*bp == '\0');
#endif
      if (result)
	{
	  val = atof(s);
	  if (low < high)
	    {
	      if (val < low || val > high)
		result = FALSE;
	    }
	  if (result)
	    {
	      sprintf(buf, "%.*f", (prec > 0 ? prec : 0), val);
	      set_field_buffer(field, 0, buf);
	    }
	}
    }
  return (result);
}

/*---------------------------------------------------------------------------
|   Facility      :  libnform
|   Function      :  static bool Check_This_Character(
|                                      int c,
|                                      const void * argp)
|
|   Description   :  Check a character for the numeric type.
|
|   Return Values :  TRUE  - character is valid
|                    FALSE - character is invalid
+--------------------------------------------------------------------------*/
static bool
Check_This_Character(int c, const void *argp)
{
  const thisARG *argn = (const thisARG *)argp;
  struct lconv *L = argn->L;

  return ((isDigit(c) ||
	   c == '+' ||
	   c == '-' ||
	   isDecimalPoint(c))
	  ? TRUE
	  : FALSE);
}

static FIELDTYPE typeTHIS =
{
  _HAS_ARGS | _RESIDENT,
  1,				/* this is mutable, so we can't be const */
  (FIELDTYPE *)0,
  (FIELDTYPE *)0,
  Make_This_Type,
  Copy_This_Type,
  Free_This_Type,
  Check_This_Field,
  Check_This_Character,
  NULL,
  NULL
};

NCURSES_EXPORT_VAR(FIELDTYPE*) TYPE_NUMERIC = &typeTHIS;

/* fty_num.c ends here */
