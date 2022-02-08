// Name: Haifaa Abushaaban[1146372]
/*
    Sample code "xmlvalidation.c" used for understanding how to validate xmlDoc against
    an XSD file from: http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <libxml/xmlschemastypes.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "LinkedListAPI.h"
#include "SVGParser.h"
#include "SVGHelper.h"
#include "SVGHelperA2.h"

#define LIBXML_SCHEMAS_ENABLED

/*
    given an xml file name and a schema file,
    this function will parse the schema file,
    parse the svg tree,
    and if both files are valid,
    will validate the file against the schema file
    if any function is not valid or the file validation is not successful, will return false
    if all functions succeed and the file validation is successful, will return true
*/
bool validateFileSVG(const char* fileName, const char* schemaFile){

    if (fileName == NULL || schemaFile == NULL) return false;

    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr pCtxt;

    xmlLineNumbersDefault(1);

    // schema parser for the schema file
    pCtxt = xmlSchemaNewParserCtxt(schemaFile);
    if (pCtxt == NULL) return false; // error creating context

    xmlSchemaSetParserErrors(pCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

    schema = xmlSchemaParse(pCtxt);
    if (schema == NULL) return false; // error parsing schema file

    xmlSchemaFreeParserCtxt(pCtxt);

    // tree parsing for the svg file
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL) return false; // error parsing svg file

    xmlSchemaValidCtxtPtr vCtxt;
    int ret;

    // validate the schema against the svg tree
    vCtxt = xmlSchemaNewValidCtxt(schema);
    if (vCtxt == NULL) return false; // error creating context

    xmlSchemaSetValidErrors(vCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(vCtxt, doc);

    // xmlSchemaValidateDoc returns 0 if the document is schemas valid, any other number indicates fail
    if (ret != 0){
        xmlSchemaFreeValidCtxt(vCtxt);
        xmlFreeDoc(doc);
        if (schema != NULL) xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
        return false;
    }

    // ret == 0, the file is valid, can create a valid svg
    xmlSchemaFreeValidCtxt(vCtxt);
    xmlFreeDoc(doc);
    // free the resource
    if (schema != NULL) xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    return true;

}

/*
    this function is for adding items in a list to a parent when:
    - other attributes in an svg
    - other attributes in a shape struct such as circle rect and path
    - other attributes in a group
    - parent can be either SVG, group, or shape with other attirbute list
*/
void addAttrListToParentNode(List* list, xmlNodePtr* parent){

    if (list == NULL || *parent == NULL) return;

    void* elem;
    ListIterator iter = createIterator(list); // traverse through the other attributes
    while ((elem = nextElement(&iter)) != NULL){
        Attribute* attr = (Attribute*) elem;
        // parent node, name="value"
        xmlNewProp(*parent, BAD_CAST attr->name, BAD_CAST attr->value);
    }

}

/*
    given a list of rectangles, this function creates a new node for each rectangle struct,
    adds the properties/attributes in the node,
    and adds that node to the parent node given
*/
void addRectListToParentNode(List* rectList, xmlNodePtr* parent){

    if (rectList == NULL || *parent == NULL) return;

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(rectList); // traverse through the rectangle
    while ((elem = nextElement(&iter)) != NULL){

        Rectangle* rect = (Rectangle*) elem;

        // 2. rectangle node, adding it to the parent (svg or group)
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "rect", NULL);

        // 3. adding contents to the rectangle
        char* x = unitsWithNumber(rect->x, rect->units);
        char* y = unitsWithNumber(rect->y, rect->units);
        char* width = unitsWithNumber(rect->width, rect->units);
        char* height = unitsWithNumber(rect->height, rect->units);

        xmlNewProp(node, BAD_CAST "x", BAD_CAST x);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST y);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST width);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST height);

        // 4. adding other attributes to the rectangles
        addAttrListToParentNode(rect->otherAttributes, &node);

        free(x);
        free(y);
        free(width);
        free(height);
    }
}

/*
    given a list of circles, this function creates a new node for each circle struct,
    adds the properties/attributes in the node,
    and adds that node to the parent node given
*/
void addCircListToParentNode(List* circList, xmlNodePtr* parent){

    if (circList == NULL || *parent == NULL) return;

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(circList); // traverse through the rectangle
    while ((elem = nextElement(&iter)) != NULL){

        Circle* circ = (Circle*) elem;

        // 2. circle node, adding it to the parent (svg or group)
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "circle", NULL);

        // 3. adding contents to the circle
        char* cx = unitsWithNumber(circ->cx, circ->units);
        char* cy = unitsWithNumber(circ->cy, circ->units);
        char* r = unitsWithNumber(circ->r, circ->units);

        xmlNewProp(node, BAD_CAST "cx", BAD_CAST cx);
        xmlNewProp(node, BAD_CAST "cy", BAD_CAST cy);
        xmlNewProp(node, BAD_CAST "r", BAD_CAST r);

        // 4. adding other attributes to the circle
        addAttrListToParentNode(circ->otherAttributes, &node);

        free(cx);
        free(cy);
        free(r);
    }
}

/*
    given a list of paths, this function creates a new node for each path struct,
    adds the properties/attributes in the node,
    and adds that node to the parent node given
*/
void addPathListToParentNode(List* pathList, xmlNodePtr* parent){

    if (pathList == NULL || *parent == NULL) return;

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(pathList); // traverse through the rectangle
    while ((elem = nextElement(&iter)) != NULL){

        Path* path = (Path*) elem;

        // 2. path node, adding it to the parent (svg or group)
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "path", NULL);

        // 3. adding contents to the path
        xmlNewProp(node, BAD_CAST "d", BAD_CAST path->data);

        // 4. adding other attributes to the path
        addAttrListToParentNode(path->otherAttributes, &node);
    }
}

/*
    this function is for adding items from a group struct
    it is recursive, as it calls itself when it reaches an inner group list
*/
void addGroupListToParentNode(List* groupList, xmlNodePtr* parent){

    if (groupList == NULL || *parent == NULL) return; // base case

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(groupList); // traverse through the rectangle
    while ((elem = nextElement(&iter)) != NULL){

        Group* group = (Group*) elem;

        // 1. create the node for the group
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "g", NULL);

        // 2. call the other attribute rect, circle, path, functions with the
        // node as the parent in this case parent is a group node
        addAttrListToParentNode(group->otherAttributes, &node); // ask: first or last?
        addRectListToParentNode(group->rectangles, &node);
        addCircListToParentNode(group->circles, &node);
        addPathListToParentNode(group->paths, &node);
        addGroupListToParentNode(group->groups, &node);
    }

}

/*
    used for circles and rectangles, this funciton will return a string with the
    number and units concatinated
    units should not be null and should be initialized, according to the struct
    definition in the createSVG function in A1
    caller must free string
*/
char* unitsWithNumber(float number, char units[]){

    char* numWithUnits = numWithUnits = malloc(50 + (strlen(units)) + 1); // max length of float, units, and \0

    sprintf(numWithUnits, "%f", number);

    if (strlen(units) > 0){
        strcat(numWithUnits, units);
    }

    return numWithUnits;

}
