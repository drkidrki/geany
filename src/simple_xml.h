
#ifndef SIMPLE_XML_H
#define SIMPLE_XML_H 1

// G_BEGIN_DECLS

#define XML_MAX_ATTR      16
#define XML_MAX_CHILDREN  64


typedef struct XMLAttribute
{
    char name[128];
    char value[256];
} XMLAttribute;


typedef struct XMLNode
{
    char name[128];

    XMLAttribute attrs[XML_MAX_ATTR];
    int attrCount;

    struct XMLNode *children[XML_MAX_CHILDREN];
    int childCount;

    struct XMLNode *parent;

} XMLNode;


XMLNode* xmlParseFile(const char *path);
int xmlCountChildren(XMLNode* node);
XMLNode* xmlGetChild(XMLNode* node, int iChild);
const char *xmlGetName(XMLNode *node);
const char* xmlReadAttribute(XMLNode* node, const char* szName);

void xmlPrintTree(XMLNode *node, int depth);

// G_END_DECLS

#endif /* SIMPLE_XML_H */
