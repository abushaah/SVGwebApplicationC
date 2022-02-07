#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "LinkedListAPI.h"
#include "SVGParser.h"
#include "SVGHelperA1.h"

bool writeSVG(const SVG* img, const char* fileName);
void addAttrListToParentNode(List* list, xmlNodePtr* parent);
void addRectListToParentNode(List* rectList, xmlNodePtr* parent);
void addCircListToParentNode(List* circList, xmlNodePtr* parent);
void addPathListToParentNode(List* pathList, xmlNodePtr* parent);
void addGroupListToParentNode(List* groupList, xmlNodePtr* parent);
char* unitsWithNumber(float number, char units[]);

bool writeSVG(const SVG* img, const char* fileName){

    if (img == NULL || fileName == NULL) return false;

    FILE* file = fopen(fileName, "w");
    if (file == NULL){
        return false;
    }

    xmlDocPtr doc = NULL; // document pointer
    xmlNodePtr root_node = NULL; // root of tree

    LIBXML_TEST_VERSION;

    // 1. convert the SVG to an XML by creating a new document, a node and set it as a root node
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    // 2. set the namespace, title, and description using the xmlNewText
    xmlNewProp(root_node, BAD_CAST "xmlns", BAD_CAST img->namespace);

    if (strlen(img->title) != 0){
        xmlNodePtr titleNode = xmlNewNode(NULL, BAD_CAST "title");
        xmlNodePtr titleNodeTxt = xmlNewText(BAD_CAST img->title);
        xmlAddChild(titleNode, titleNodeTxt);
        xmlAddChild(root_node, titleNode);
    }
    if (strlen(img->description) != 0){
        xmlNodePtr descNode = xmlNewNode(NULL, BAD_CAST "desc");
        xmlNodePtr descNodeTxt = xmlNewText(BAD_CAST img->description);
        xmlAddChild(descNode, descNodeTxt);
        xmlAddChild(root_node, descNode);
    }

    /* 3.
       calling functions that will loop through list (other attributes, rect, circ, path, groups),
       and adds items to the parent, root_node svg
       these functions will be used for both the svg children and the group children
    */
    addAttrListToParentNode(img->otherAttributes, &root_node); // ask: first or last?
    addRectListToParentNode(img->rectangles, &root_node);
    addCircListToParentNode(img->circles, &root_node);
    addPathListToParentNode(img->paths, &root_node);
    addGroupListToParentNode(img->groups, &root_node);

    // 4. save contents to a file
    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);
    fclose(file);

    // 5. free the document, global variables that may have been allocated by the parser
    xmlFreeDoc(doc);
    xmlCleanupParser();

    // debug memory for regression tests
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

//bool validateSVG(const SVG* img, const char* schemaFile);
