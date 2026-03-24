/*
 * json_parser.c - lightweight JSON helpers for project files.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "json_parser.h"

#include <string.h>


typedef enum GeanyJsonValueType
{
  GEANY_JSON_VALUE_INVALID = 0,
  GEANY_JSON_VALUE_STRING,
  GEANY_JSON_VALUE_ARRAY,
  GEANY_JSON_VALUE_OBJECT,
  GEANY_JSON_VALUE_OTHER
} GeanyJsonValueType;


typedef struct GeanyJsonSpan
{
  const gchar *start;
  const gchar *end;
  GeanyJsonValueType type;
} GeanyJsonSpan;


static const gchar *json_skip_whitespace(const gchar *p)
{
  while (*p != '\0' && g_ascii_isspace(*p))
    p++;

  return p;
}


static gboolean json_skip_string(const gchar **p)
{
  const gchar *s;

  g_return_val_if_fail(p != NULL && *p != NULL, FALSE);

  s = *p;
  if (*s != '"')
    return FALSE;

  s++;
  while (*s != '\0')
  {
    if (*s == '\\')
    {
      s++;
      if (*s == '\0')
        return FALSE;
      s++;
      continue;
    }

    if (*s == '"')
    {
      *p = s + 1;
      return TRUE;
    }

    s++;
  }

  return FALSE;
}


static gboolean json_skip_nested(const gchar **p, gchar open_char, gchar close_char)
{
  const gchar *s;
  gint depth = 1;

  g_return_val_if_fail(p != NULL && *p != NULL, FALSE);

  s = *p;
  while (*s != '\0')
  {
    if (*s == '"')
    {
      if (!json_skip_string(&s))
        return FALSE;
      continue;
    }

    if (*s == open_char)
      depth++;
    else if (*s == close_char)
      depth--;

    s++;

    if (depth == 0)
    {
      *p = s;
      return TRUE;
    }
  }

  return FALSE;
}


static gboolean json_skip_value(const gchar **p, GeanyJsonValueType *type)
{
  const gchar *s;

  g_return_val_if_fail(p != NULL && *p != NULL, FALSE);

  s = json_skip_whitespace(*p);
  if (*s == '\0')
    return FALSE;

  if (*s == '"')
  {
    if (!json_skip_string(&s))
      return FALSE;
    if (type != NULL)
      *type = GEANY_JSON_VALUE_STRING;
    *p = s;
    return TRUE;
  }

  if (*s == '[')
  {
    s++;
    if (!json_skip_nested(&s, '[', ']'))
      return FALSE;
    if (type != NULL)
      *type = GEANY_JSON_VALUE_ARRAY;
    *p = s;
    return TRUE;
  }

  if (*s == '{')
  {
    s++;
    if (!json_skip_nested(&s, '{', '}'))
      return FALSE;
    if (type != NULL)
      *type = GEANY_JSON_VALUE_OBJECT;
    *p = s;
    return TRUE;
  }

  while (*s != '\0' && *s != ',' && *s != ']' && *s != '}')
    s++;

  if (type != NULL)
    *type = GEANY_JSON_VALUE_OTHER;
  *p = s;
  return TRUE;
}


static gchar *json_unescape_string(const gchar *escaped)
{
  GString *result;
  const gchar *p;

  g_return_val_if_fail(escaped != NULL, NULL);

  result = g_string_new(NULL);
  for (p = escaped; *p != '\0'; p++)
  {
    if (*p == '\\' && *(p + 1) != '\0')
    {
      p++;
      switch (*p)
      {
        case '"':
        case '\\':
        case '/':
          g_string_append_c(result, *p);
          break;
        case 'b':
          g_string_append_c(result, '\b');
          break;
        case 'f':
          g_string_append_c(result, '\f');
          break;
        case 'n':
          g_string_append_c(result, '\n');
          break;
        case 'r':
          g_string_append_c(result, '\r');
          break;
        case 't':
          g_string_append_c(result, '\t');
          break;
        default:
          g_string_append_c(result, *p);
          break;
      }
    }
    else
      g_string_append_c(result, *p);
  }

  return g_string_free(result, FALSE);
}


static gboolean json_parse_quoted_string(const gchar **p, gchar **value)
{
  const gchar *start;
  const gchar *end;
  gchar *escaped;

  g_return_val_if_fail(p != NULL && *p != NULL && value != NULL, FALSE);

  start = json_skip_whitespace(*p);
  if (*start != '"')
    return FALSE;

  end = start;
  if (!json_skip_string(&end))
    return FALSE;

  escaped = g_strndup(start + 1, (gsize) (end - start - 2));
  *value = json_unescape_string(escaped);
  g_free(escaped);

  *p = end;
  return TRUE;
}


static gboolean json_find_top_level_member(const gchar *json_data, const gchar *member,
  GeanyJsonSpan *span)
{
  const gchar *p;

  g_return_val_if_fail(json_data != NULL && member != NULL && span != NULL, FALSE);

  p = json_skip_whitespace(json_data);
  if (*p != '{')
    return FALSE;

  p++;
  for (;;)
  {
    gchar *key = NULL;
    const gchar *value_start;
    const gchar *value_end;
    GeanyJsonValueType value_type = GEANY_JSON_VALUE_INVALID;

    p = json_skip_whitespace(p);
    if (*p == '}')
      break;

    if (!json_parse_quoted_string(&p, &key))
      return FALSE;

    p = json_skip_whitespace(p);
    if (*p != ':')
    {
      g_free(key);
      return FALSE;
    }

    p++;
    value_start = json_skip_whitespace(p);
    value_end = value_start;
    if (!json_skip_value(&value_end, &value_type))
    {
      g_free(key);
      return FALSE;
    }

    if (g_strcmp0(key, member) == 0)
    {
      span->start = value_start;
      span->end = value_end;
      span->type = value_type;
      g_free(key);
      return TRUE;
    }

    g_free(key);
    p = json_skip_whitespace(value_end);

    if (*p == ',')
    {
      p++;
      continue;
    }

    if (*p == '}')
      break;

    return FALSE;
  }

  return FALSE;
}


gboolean geany_json_extract_string_member(const gchar *json_data, const gchar *member,
  gchar **value)
{
  GeanyJsonSpan span;
  const gchar *p;

  g_return_val_if_fail(value != NULL, FALSE);

  if (!json_find_top_level_member(json_data, member, &span))
    return FALSE;

  if (span.type != GEANY_JSON_VALUE_STRING)
    return FALSE;

  p = span.start;
  return json_parse_quoted_string(&p, value);
}


GStrv geany_json_extract_string_array_member(const gchar *json_data, const gchar *member)
{
  GeanyJsonSpan span;
  const gchar *p;
  GPtrArray *items;

  if (!json_find_top_level_member(json_data, member, &span))
    return NULL;

  if (span.type != GEANY_JSON_VALUE_ARRAY)
    return NULL;

  p = span.start;
  p = json_skip_whitespace(p);
  if (*p != '[')
    return NULL;

  p++;
  items = g_ptr_array_new_with_free_func(g_free);

  for (;;)
  {
    gchar *item;

    p = json_skip_whitespace(p);
    if (*p == ']')
      break;

    if (!json_parse_quoted_string(&p, &item))
      goto fail;

    g_ptr_array_add(items, item);

    p = json_skip_whitespace(p);
    if (*p == ',')
    {
      p++;
      continue;
    }

    if (*p == ']')
      break;

    goto fail;
  }

  g_ptr_array_add(items, NULL);
  return (GStrv) g_ptr_array_free(items, FALSE);

fail:
  g_ptr_array_free(items, TRUE);
  return NULL;
}
