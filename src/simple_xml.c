#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "simple_xml.h"

/*
    Very small XML parser.

    Supported:
      - elements
      - attributes
      - nested nodes
      - self-closing tags

    Not supported:
      - CDATA
      - namespaces
      - entity decoding
      - text nodes

    Designed for simple XML formats like project files.
*/

/* ============================= */
/*        GLOBAL STATE           */
/* ============================= */

static XMLNode *g_xmlRoot = NULL;
static XMLNode *g_xmlCurrent = NULL;



/* ============================= */
/*        NODE UTILITIES         */
/* ============================= */

static XMLNode* xmlCreateNode()
{
    XMLNode *node = (XMLNode*)malloc(sizeof(XMLNode));
    memset(node, 0, sizeof(XMLNode));
    return node;
}


static void xmlAddChild(XMLNode *parent, XMLNode *child)
{
    if (parent->childCount < XML_MAX_CHILDREN)
    {
        parent->children[parent->childCount++] = child;
        child->parent = parent;
    }
}



/* ============================= */
/*        PARSER HELPERS         */
/* ============================= */

static void xmlSkipWhitespace(FILE *f)
{
    int c;

    while ((c = fgetc(f)) != EOF)
    {
        if (!isspace(c))
        {
            ungetc(c, f);
            return;
        }
    }
}


static void xmlReadName(FILE *f, char *out)
{
    int c;
    int i = 0;

    while ((c = fgetc(f)) != EOF)
    {
        if (isalnum(c) || c=='_' || c=='-' || c=='.')
        {
            out[i++] = (char)c;
        }
        else
        {
            out[i] = 0;
            ungetc(c, f);
            return;
        }
    }
}


static void xmlReadQuoted(FILE *f, char *out)
{
    int c;
    int i = 0;

    fgetc(f); /* skip opening quote */

    while ((c = fgetc(f)) != EOF)
    {
        if (c == '"')
        {
            out[i] = 0;
            return;
        }

        out[i++] = (char)c;
    }
}



/* ============================= */
/*       ATTRIBUTE PARSER        */
/* ============================= */

static void xmlParseAttributes(FILE *f, XMLNode *node)
{
    int c;

    while (1)
    {
        xmlSkipWhitespace(f);

        c = fgetc(f);

        if (c == '/' || c == '>')
        {
            ungetc(c, f);
            return;
        }

        ungetc(c, f);

        if (node->attrCount >= XML_MAX_ATTR)
            return;

        XMLAttribute *attr = &node->attrs[node->attrCount++];

        xmlReadName(f, attr->name);

        xmlSkipWhitespace(f);
        fgetc(f); /* = */
        xmlSkipWhitespace(f);

        xmlReadQuoted(f, attr->value);
    }
}



/* ============================= */
/*         XML PARSER            */
/* ============================= */

XMLNode* xmlParseFile(const char *path)
{
    FILE *f = fopen(path, "r");

    if (!f)
        return NULL;

    int c;

    g_xmlRoot = NULL;
    g_xmlCurrent = NULL;

    while ((c = fgetc(f)) != EOF)
    {
        if (c != '<')
            continue;

        c = fgetc(f);

        /* ============================= */
        /*        END NODE               */
        /* ============================= */

        if (c == '/')
        {
            char name[128];

            xmlReadName(f, name);

            while (fgetc(f) != '>');

            /*
                XML NODE CLOSED

                At this moment the node subtree is complete.
            */

            if (g_xmlCurrent)
                g_xmlCurrent = g_xmlCurrent->parent;
        }

        /* ============================= */
        /*        START NODE             */
        /* ============================= */

        else
        {
            XMLNode *node = xmlCreateNode();

            ungetc(c, f);

            xmlReadName(f, node->name);

            xmlParseAttributes(f, node);

            xmlSkipWhitespace(f);

            c = fgetc(f);

            int selfClosing = 0;

            if (c == '/')
            {
                selfClosing = 1;
                fgetc(f); /* > */
            }

            /*
                XML NODE LOADED

                At this point:
                  node->name       is valid
                  node->attrs[]    contain attributes
                  node->attrCount  is known

                Data can safely be read here.
            */

            if (!g_xmlRoot)
                g_xmlRoot = node;

            if (g_xmlCurrent)
                xmlAddChild(g_xmlCurrent, node);

            if (!selfClosing)
            {
                g_xmlCurrent = node;
            }
            else
            {
                /*
                    SELF-CLOSING NODE COMPLETE

                    Entire node is already finished here.
                */
            }
        }
    }

    fclose(f);

    return g_xmlRoot;
}


int xmlCountChildren(XMLNode *node)
{
  return node->childCount;
}

XMLNode *xmlGetChild(XMLNode *node, int iChild)
{
  return node->children[iChild];
}

const char *xmlGetName(XMLNode *node)
{
  return node->name;
}

const char *xmlReadAttribute(XMLNode* node, const char* szName)
{
  for (int i = 0; i < node->attrCount; i++) {
    if (strcmp(szName, node->attrs[i].name) == 0) {
      return node->attrs[i].value;
    }
  }
  return "";
}

/* ============================= */
/*       TREE ITERATION          */
/* ============================= */

void xmlPrintTree(XMLNode *node, int depth)
{
    int i;

    for (i = 0; i < depth; i++)
        printf("  ");

    printf("%s\n", node->name);

    for (i = 0; i < node->attrCount; i++)
    {
        int j;

        for (j = 0; j < depth + 1; j++)
            printf("  ");

        printf("%s = %s\n",
               node->attrs[i].name,
               node->attrs[i].value);
    }

    for (i = 0; i < node->childCount; i++)
        xmlPrintTree(node->children[i], depth + 1);
}
