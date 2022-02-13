// Name: Haifaa Abushaaban[1146372]
/*
    Sample code "tree2.c" used for understanding how to create a document and nodes
    for an xml tree from: http://www.xmlsoft.org/examples/tree2.c
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
    createValidSVG first creates an xml tree, then validates the svg file against the schema file,
    then calls create svg for the svg file to create an svg struct if the file is valid
*/
SVG* createValidSVG(const char* fileName, const char* schemaFile){

    if (fileName == NULL || schemaFile == NULL) return NULL;

    // 1. parse the file and get the DOM
    xmlDocPtr doc = NULL;
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL){
        xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function
        return NULL;
    }

    // 2. validates the svg against the schema file
    bool valid = validateFileSVG(doc, schemaFile);
    if (valid == false){
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function

    // 3. create the svg, that is valid?
    SVG* svg = createSVG(fileName);
    if (svg == NULL){
        return NULL;
    }

    return svg;
}

/*
    writes an svg struct to a file in the svg format
    assumes that the struct has been validated!
*/
bool writeSVG(const SVG* img, const char* fileName){

    if (img == NULL || fileName == NULL) return false;

    // 1. create the xml doc from the svg
    xmlDocPtr doc = createXMLFromStruct(img);
    if (doc == NULL){
        xmlCleanupParser();
        return false;
    }

    // 2. save contents to a file
    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);

    // 3. free the document
    xmlFreeDoc(doc);
    xmlCleanupParser(); // free global variables allocated by parser in the createXMLFromStruct() function

    return true;
}

/*
    validates every struct and list
*/
bool validateSVG(const SVG* img, const char* schemaFile){

    if (img == NULL || schemaFile == NULL) return false;

    // 1. Convert the svg to an xml doc using write to file
    xmlDocPtr doc = createXMLFromStruct(img);
    if (doc == NULL){
        xmlCleanupParser();
        return false;
    }

    xmlCleanupParser(); // free global variables allocated by parser in the createXMLFromStruct() function

    // 2. Validate the xml doc against the schema similar to the create valid svg function
    bool valid = validateFileSVG(doc, schemaFile);
    if (valid == false){
        xmlFreeDoc(doc);
        xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function
        return false;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function

    // 3. Validate the svg struct against the svgparser.h specifications
    valid = validSVGStruct(img);
    if (valid == false) return false;

    return true;
}

bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute){

    if (img == NULL || newAttribute == NULL) return false;

    // 1. valid attribute strings
    if ((validChar(newAttribute->name) == 0) || (strcmp(newAttribute->name, "") == 0) || (validChar(newAttribute->value) == 0) || (strcmp(newAttribute->value, "") == 0)) return false;

    // 2. check the type
    if (elemType == SVG_IMG){
        bool valid = changeValueInAttr(img->otherAttributes, newAttribute);
        if (valid == false) return false;
    }
    else if (elemType == RECT){
        // 3. check if index is out of bounds
        if (getLength(img->rectangles) <= elemIndex) return false;
        // 4. separate function for changing value in order to check for validity of the new attribute
        //    the functions are in charge of freeing the newAttribute attribute depending on whether it was appended or not
        bool valid = changeValueInRect(img->rectangles, elemIndex, newAttribute);
        if (valid == false) return false;

    }
    else if (elemType == CIRC){
        if (getLength(img->circles) <= elemIndex) return false;
        bool valid = changeValueInCirc(img->circles, elemIndex, newAttribute);
        if (valid == false) return false;
    }
    else if (elemType == PATH){
        if (getLength(img->paths) <= elemIndex) return false;
        bool valid = changeValueInPath(img->paths, elemIndex, newAttribute);
        if (valid == false) return false;
    }
    else if (elemType == GROUP){
        if (getLength(img->groups) <= elemIndex) return false;
        bool valid = changeValueInGroup(img->groups, elemIndex, newAttribute);
        if (valid == false) return false;
    }
    else{ // not the appropriate element
        return false;
    }

    return true;

}

void addComponent(SVG* img, elementType type, void* newElement){

    if (img == NULL || newElement == NULL) return;

    // 1. determine component type
    if (type == RECT){
        if (img->rectangles == NULL) return;
        Rectangle* newRect = (Rectangle*) newElement;
        // 2. check if component is valid
        bool valid = validRectStruct(newRect);
        if (valid == false){
            deleteRectangle(newRect);
            return;
        }
        // 3. add comopnent to the end of the list
        insertBack(img->rectangles, (void*)newRect);
    }
    else if (type == CIRC){
        if (img->circles == NULL) return;
        Circle* newCirc = (Circle*) newElement;
        bool valid = validCircStruct(newCirc);
        if (valid == false){
            deleteCircle(newCirc);
            return;
        }
        insertBack(img->circles, (void*)newCirc);
    }
    else if (type == PATH){
        if (img->paths == NULL) return;
        Path* newPath = (Path*) newElement;
        bool valid = validPathStruct(newPath);
        if (valid == false){
            deletePath(newPath);
            return;
        }
        insertBack(img->paths, (void*)newPath);
    }

}

char* attrToJSON(const Attribute *a){ // const means it cannot be changed and it is safe to use

    if (a == NULL){
        char* attrString = malloc(strlen("{}") + 1);
        sprintf(attrString, "{}");
        return attrString;
    }

    // assuming that these attributes are validated using the validate svg function and that they are not NULL and are initialized
    char* attrString = malloc(strlen(a->name) + strlen(a->value) + 31); // 31 characters for words, quotes, commas, semicolons, \0
    sprintf(attrString, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
    return attrString;

}

char* circleToJSON(const Circle *c){

    if (c == NULL){
        char* circString = malloc(strlen("{}") + 1);
        sprintf(circString, "{}");
        return circString;
    }

    // 3 floats, 35 characters each, 1 int, 10 characters, 41 characters for words, quotes, commas, semicolons, \0
    char* circString = malloc(strlen(c->units) + 156);
    sprintf(circString, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, getLength(c->otherAttributes), c->units);
    return circString;

}

char* rectToJSON(const Rectangle *r){

    if (r == NULL){
        char* rectString = malloc(strlen("{}") + 1);
        sprintf(rectString, "{}");
        return rectString;
    }

    // 4 floats, 35 characters each, 1 int, 10 characters, 44 characters for words, quotes, commas, semicolons, \0
    char* rectString = malloc(strlen(r->units) + 194);
    sprintf(rectString, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, getLength(r->otherAttributes), r->units);
    return rectString;


}

char* pathToJSON(const Path *p){

    if (p == NULL){
        char* pathString = malloc(strlen("{}") + 1);
        sprintf(pathString, "{}");
        return pathString;
    }

    // 1 int, 10 characters, 21 characters for words, quotes, commas, semicolons, \0
    char* pathString = malloc(strlen(p->data) + 31);
    sprintf(pathString, "{\"d\":\"%.64s\",\"numAttr\":%d}", p->data, getLength(p->otherAttributes));

    return pathString;

}

char* groupToJSON(const Group *g){

    if (g == NULL){
        char* groupString = malloc(strlen("{}") + 1);
        sprintf(groupString, "{}");
        return groupString;
    }

    // 2 int, 10 characters, 25 characters for words, quotes, commas, semicolons, \0
    char* groupString = malloc(20 + 25);
    sprintf(groupString, "{\"children\":%d,\"numAttr\":%d}", (getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths) + getLength(g->groups)), getLength(g->otherAttributes));
    return groupString;

}

char* SVGtoJSON(const SVG* img){

    if (img == NULL){
        char* svgString = malloc(strlen("{}") + 1);
        sprintf(svgString, "{}");
        return svgString;
    }

    List* rectList = getRects(img);
    int numRect = getLength(rectList);
    freeList(rectList);

    List* circList = getCircles(img);
    int numCirc = getLength(circList);
    freeList(circList);

    List* pathList = getPaths(img);
    int numPaths = getLength(pathList);
    freeList(pathList);

    List* groupList = getGroups(img);
    int numGroups = getLength(groupList);
    freeList(groupList);

    // 4 int, 10 characters, 49 characters for words, quotes, commas, semicolons, \0
    char* svgString = malloc(40 + 49);
    sprintf(svgString, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", numRect, numCirc, numPaths, numGroups);

    return svgString;

}

char* attrListToJSON(const List *list){

    if (list == NULL){
        char* attrListString = malloc(strlen("[]") + 1);
        sprintf(attrListString, "[]");
        return attrListString;
    }

    // 1. initialize the string
    char* attrListString = malloc(strlen("[]") + 1);
    int size = 2; // so far the number of characters in the string is 2 for the '[]'
    strcpy(attrListString, "[");

    int index = 0; // index of the element to know when to add a comma

    // 2. traverse through the list
    List* attrList = (List *) list;

    void* elem;
    ListIterator iter = createIterator(attrList);
    while ((elem = nextElement(&iter)) != NULL){

        // 3. for each attribute, get the string
        Attribute* attr = (Attribute*) elem;
        char* currAttr = attrToJSON(attr);

        // 4. update the size of the number of characters that are to be in the list string
        size += strlen(currAttr) + 1; // 1 for the comma

        // 5. reallocate for the new size, 1 for null character
        attrListString = realloc(attrListString, size + 1);

        // 6. add the attribute string and the comma
        if (index > 0){
            strcat(attrListString, ",");
        }
        strcat(attrListString, currAttr);

        // 7. done with the current attribute
        free(currAttr);
        ++index;
    }

    // 8. add the ending bracket
    strcat(attrListString, "]");

    return attrListString;

}

char* circListToJSON(const List *list){

    if (list == NULL){
        char* circListString = malloc(strlen("[]") + 1);
        sprintf(circListString, "[]");
        return circListString;
    }

    // 1. initialize the string
    char* circListString = malloc(strlen("[]") + 1);
    int size = 2; // so far the number of characters in the string is 2 for the '[]'
    strcpy(circListString, "[");

    int index = 0; // index of the element to know when to add a comma

    // 2. traverse through the list
    List* circList = (List *) list;

    void* elem;
    ListIterator iter = createIterator(circList);
    while ((elem = nextElement(&iter)) != NULL){

        // 3. for each attribute, get the string
        Circle* circ = (Circle*) elem;
        char* currCirc = circleToJSON(circ);

        // 4. update the size of the number of characters that are to be in the list string
        size += strlen(currCirc) + 1; // 1 for the comma

        // 5. reallocate for the new size, 1 for null character
        circListString = realloc(circListString, size + 1);

        // 6. add the circle string and the comma
        if (index > 0){
            strcat(circListString, ",");
        }
        strcat(circListString, currCirc);

        // 7. done with the current circle
        free(currCirc);
        ++index;
    }

    // 8. add the ending bracket
    strcat(circListString, "]");

    return circListString;

}

char* rectListToJSON(const List *list){

    if (list == NULL){
        char* rectListString = malloc(strlen("[]") + 1);
        sprintf(rectListString, "[]");
        return rectListString;
    }

    // 1. initialize the string
    char* rectListString = malloc(strlen("[]") + 1);
    int size = 2; // so far the number of characters in the string is 2 for the '[]'
    strcpy(rectListString, "[");

    int index = 0; // index of the element to know when to add a comma

    // 2. traverse through the list
    List* rectList = (List *) list;

    void* elem;
    ListIterator iter = createIterator(rectList);
    while ((elem = nextElement(&iter)) != NULL){

        // 3. for each attribute, get the string
        Rectangle* rect = (Rectangle*) elem;
        char* currRect = rectToJSON(rect);

        // 4. update the size of the number of characters that are to be in the list string
        size += strlen(currRect) + 1; // 1 for the comma

        // 5. reallocate for the new size, 1 for null character
        rectListString = realloc(rectListString, size + 1);

        // 6. add the rectangle string and the comma
        if (index > 0){
            strcat(rectListString, ",");
        }
        strcat(rectListString, currRect);

        // 7. done with the current rectangle
        free(currRect);
        ++index;
    }

    // 8. add the ending bracket
    strcat(rectListString, "]");

    return rectListString;

}

char* pathListToJSON(const List *list){

    if (list == NULL){
        char* pathListString = malloc(strlen("[]") + 1);
        sprintf(pathListString, "[]");
        return pathListString;
    }

    // 1. initialize the string
    char* pathListString = malloc(strlen("[]") + 1);
    int size = 2; // so far the number of characters in the string is 2 for the '[]'
    strcpy(pathListString, "[");

    int index = 0; // index of the element to know when to add a comma

    // 2. traverse through the list
    List* pathList = (List *) list;

    void* elem;
    ListIterator iter = createIterator(pathList);
    while ((elem = nextElement(&iter)) != NULL){

        // 3. for each attribute, get the string
        Path* path = (Path*) elem;
        char* currPath = pathToJSON(path);

        // 4. update the size of the number of characters that are to be in the list string
        size += strlen(currPath) + 1; // 1 for the comma

        // 5. reallocate for the new size, 1 for null character
        pathListString = realloc(pathListString, size + 1);

        // 6. add the path string and the comma
        if (index > 0){
            strcat(pathListString, ",");
        }
        strcat(pathListString, currPath);

        // 7. done with the current path
        free(currPath);
        ++index;
    }

    // 8. add the ending bracket
    strcat(pathListString, "]");

    return pathListString;

}

char* groupListToJSON(const List *list){

    if (list == NULL){
        char* groupListString = malloc(strlen("[]") + 1);
        sprintf(groupListString, "[]");
        return groupListString;
    }

    // 1. initialize the string
    char* groupListString = malloc(strlen("[]") + 1);
    int size = 2; // so far the number of characters in the string is 2 for the '[]'
    strcpy(groupListString, "[");

    int index = 0; // index of the element to know when to add a comma

    // 2. traverse through the list
    List* groupList = (List *) list;

    void* elem;
    ListIterator iter = createIterator(groupList);
    while ((elem = nextElement(&iter)) != NULL){

        // 3. for each attribute, get the string
        Group* group = (Group*) elem;
        char* currGroup = groupToJSON(group);

        // 4. update the size of the number of characters that are to be in the list string
        size += strlen(currGroup) + 1; // 1 for the comma

        // 5. reallocate for the new size, 1 for null character
        groupListString = realloc(groupListString, size + 1);

        // 6. add the group string and the comma
        if (index > 0){
            strcat(groupListString, ",");
        }
        strcat(groupListString, currGroup);

        // 7. done with the current group
        free(currGroup);
        ++index;
    }

    // 8. add the ending bracket
    strcat(groupListString, "]");

    return groupListString;

}

/*
// Bonus functions
SVG* JSONtoSVG(const char* svgString);
Rect* JSONtoRect(const char* svgString);
Circle* JSONtoCircle(const char* svgString);
*/
