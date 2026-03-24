/*
 * json_parser.h - lightweight JSON helpers for project files.
 */

#ifndef GEANY_JSON_PARSER_H
#define GEANY_JSON_PARSER_H

#include <glib.h>

gboolean geany_json_extract_string_member(const gchar *json_data, const gchar *member,
  gchar **value);
GStrv geany_json_extract_string_array_member(const gchar *json_data, const gchar *member);

#endif /* GEANY_JSON_PARSER_H */
