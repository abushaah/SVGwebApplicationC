// Name: Hafaa Abushaaban [1146372]

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGHelper.h"
#include "SVGParser.h"

#define COUNT 500

void dummyDeleteRectangle(void* data){}
void dummyDeleteCircle(void* data){}
void dummyDeletePath(void* data){}
void dummyDeleteGroup(void* data){}

SVG* createSVG(const char* filename){

    if (filename == NULL) return NULL; // file is not given

    LIBXML_TEST_VERSION

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    int valid = 0;

    doc = xmlReadFile(filename, NULL, 0); // parse the file and get the DOM
    if (doc == NULL){
        return NULL;
    }

    SVG* svg = (SVG*) (malloc(sizeof(SVG)));
    if (svg == NULL){
        xmlFreeDoc(doc); // free document
        xmlCleanupParser(); // free global variables allocated by parser
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc); // root element node

    if (strcasecmp((char*)root_element->name, "svg") != 0){ // when the root node is not null
        free(svg);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    // must initialize all svg contents, namespace may not be empty
    valid = titleDescNS(svg->namespace, (char*)root_element->ns->href); // funciton to create namespace (must)
    if (valid == 0){
        free(svg);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }
    strcpy(svg->title, "");
    strcpy(svg->description, "");
    svg->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    svg->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    svg->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    svg->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    svg->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    Group **group = malloc(sizeof(Group*) * COUNT); // group that will point to Group*, start with it pointing to one only
    if (group == NULL){
        free(svg);
        xmlFreeDoc(doc); // free document
        xmlCleanupParser(); // free global variables allocated by parser
        return NULL;
    }

    int groupIdx = 0;
    int inserted = 0;

    valid = get_element_names(root_element, svg, group, &groupIdx, &inserted);
    /* root element is the root node, svg is the svg tree,
    group is the array that point to groups,
    group index is the index in the array of groups,
    inserted is whether a group is inserted */

    free(group);

    if (valid == 0){
        free(svg);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    xmlFreeDoc(doc); // free document
    xmlCleanupParser(); // free global variables allocated by parser

    return svg;

}

char* SVGToString(const SVG* img){

    char* tmpStr;
    SVG* tmp;
    int len;

    if (img == NULL){
        return NULL;
    }

    tmp = (SVG*)img;

    char* namespace = tmp->namespace;
    char* title = tmp->title;
    char* description = tmp->description;
    char* rectList = toString(tmp->rectangles);
    char* circList = toString(tmp->circles);
    char* pathList = toString(tmp->paths);
    char* groupList = toString(tmp->groups);
    char* attrList = toString(tmp->otherAttributes);

    len = strlen(namespace) + strlen(title) + strlen(description) + strlen(rectList) + strlen(circList) + strlen(pathList) + strlen(groupList) + strlen(attrList) + 9 + 24; // 8 '\n', \0
    tmpStr = (char*) malloc(sizeof(char) * len);
    sprintf(tmpStr, "N: %s\nT: %s\nD: %s\nR: %s\nC: %s\nP: %s\nG: %s\nA: %s\n", namespace, title, description, rectList, circList, pathList, groupList, attrList);

    free(rectList);
    free(circList);
    free(pathList);
    free(groupList);
    free(attrList);

    return tmpStr;

}

void deleteSVG(SVG* img){

    if (img->rectangles != NULL) freeList(img->rectangles);
    if (img->circles != NULL) freeList(img->circles);
    if (img->paths != NULL) freeList(img->paths);
    if (img->groups != NULL) freeList(img->groups);
    if (img->otherAttributes != NULL) freeList(img->otherAttributes);
    if (img != NULL) free(img);

}

void deleteAttribute( void* data){

    Attribute* tmp;
    if (data == NULL){
        return;
    }
    tmp = (Attribute*)data;
    free(tmp->name);
    free(tmp);

}

char* attributeToString( void* data){

    char* tmpStr;
    Attribute* tmp;
    int len;

    if (data == NULL){
        return NULL;
    }

    tmp = (Attribute*)data;

    len = strlen(tmp->name) + strlen(tmp->value) + 7; // 7 for the ", ' ', =, \n, \0

    tmpStr = (char*) malloc(sizeof(char) * len);

    sprintf(tmpStr, "%s = \"%s\"\n", tmp->name, tmp->value);

    return tmpStr;
}

int compareAttributes(const void *first, const void *second){ return 0; }

void deleteGroup(void* data){

    Group* tmp;
    if (data == NULL){
        return;
    }
    tmp = (Group*)data;

    if (tmp->rectangles != NULL) freeList(tmp->rectangles);
    if (tmp->circles != NULL) freeList(tmp->circles);
    if (tmp->paths != NULL) freeList(tmp->paths);
    if (tmp->groups != NULL) freeList(tmp->groups);
    if (tmp->otherAttributes != NULL) freeList(tmp->otherAttributes);
    if (tmp != NULL) free(tmp);

}

char* groupToString( void* data){

    char* tmpStr;
    Group* tmp;
    int len;

    if (data == NULL){
        return NULL;
    }

    tmp = (Group*)data;

    char* rectList = toString(tmp->rectangles);
    char* circList = toString(tmp->circles);
    char* pathList = toString(tmp->paths);
    char* groupList = toString(tmp->groups);
    char* attrList = toString(tmp->otherAttributes);

    len = strlen(rectList) + strlen(circList) + strlen(pathList) + strlen(groupList) + strlen(attrList) + 72; // '\n', \0
    tmpStr = (char*) malloc(sizeof(char) * len);
    sprintf(tmpStr, "Rectangles:\n(%s)\nCircles:\n(%s)\nPaths:\n(%s)\nSubgroups:\n(%s)\nOtherAttributes:\n(%s)\n", rectList, circList, pathList, groupList, attrList);

    free(rectList);
    free(circList);
    free(pathList);
    free(groupList);
    free(attrList);

    return tmpStr;

}

int compareGroups(const void *first, const void *second){ return 0; }

void deleteRectangle(void* data){

    Rectangle* tmp;
    if (data == NULL){
        return;
    }
    tmp = (Rectangle*)data;

    if (tmp->otherAttributes != NULL) freeList (tmp->otherAttributes);
    free(tmp);

}


char* rectangleToString(void* data){

    char* tmpStr;
    Rectangle* tmp;
    int len;

    if (data == NULL){
        return NULL;
    }

    tmp = (Rectangle*)data;

    char* attrList = toString(tmp->otherAttributes);

    // 4 floats (35 characters each * 4 = 140) + 50 char + (6 * 5 = 30) + 1 newline + 1 null = 222 + list attributes
    len = 222 + strlen(attrList);

    tmpStr = (char*) malloc(sizeof(char) * len);
    sprintf(tmpStr, "x = \"%f\"\ny = \"%f\"\nwidth = \"%f\"\nheight = \"%f\"\nunits = \"%s\"\n%s\n", tmp->x, tmp->y, tmp->width, tmp->height, tmp->units, attrList);

    free(attrList);
    return tmpStr;

}

int compareRectangles(const void *first, const void *second){ return 0; }

void deleteCircle(void* data){

    Circle* tmp;
    if (data == NULL){
        return;
    }
    tmp = (Circle*)data;

    if (tmp->otherAttributes != NULL) freeList (tmp->otherAttributes);
    free(tmp);

}

char* circleToString(void* data){

    char* tmpStr;
    Circle* tmp;
    int len;

    if (data == NULL){
        return NULL;
    }

    tmp = (Circle*)data;

    char* attrList = toString(tmp->otherAttributes);

    // 3 floats (35 characters each * 3 = 105) + 50 char + (6 * 4 = 24) + 4 = 181 + list attributes
    len = 183 + strlen(attrList);

    tmpStr = (char*) malloc(sizeof(char) * len);
    sprintf(tmpStr, "cx = \"%f\"\ncy = \"%f\"\nr = \"%f\"\nunits = \"%s\"\n%s\n", tmp->cx, tmp->cy, tmp->r, tmp->units, attrList);

    free(attrList);
    return tmpStr;

}

int compareCircles(const void *first, const void *second){ return 0; }

void deletePath(void* data){

    Path* tmp;
    if (data == NULL){
        return;
    }
    tmp = (Path*)data;
    if (tmp->otherAttributes != NULL) freeList (tmp->otherAttributes);
    free(tmp);

}

char* pathToString(void* data){

    char* tmpStr;
    Path* tmp;
    int len;

    if (data == NULL){
        return NULL;
    }

    tmp = (Path*)data;

    char* attrList = toString(tmp->otherAttributes);

    len = strlen(tmp->data) + strlen(attrList) + 9; // 7 for the ", ' ', =, \n, \0

    tmpStr = (char*) malloc(sizeof(char) * len);

    sprintf(tmpStr, "d = \"%s\"\n%s\n", tmp->data, attrList);

    free(attrList);
    return tmpStr;

}

int comparePaths(const void *first, const void *second){ return 0; }

/**
 * The get functions have similar format. They:
 * 1. traverse shape list in img
 * 2. call getElementGroups to:
 * 3. traverse groups shape list in img and
 * 4. recursively traverse groups groups shape list in img, and so on, until empty shape lists are reached
*/
List* getRects(const SVG* img){

    if (img == NULL) return NULL;

    List* rectangles = initializeList(&rectangleToString, &dummyDeleteRectangle, &compareRectangles);

    void* elem;
    ListIterator iter = createIterator(img->rectangles); // create iterator to traverse the list
    while ((elem = nextElement(&iter)) != NULL){
        Rectangle* rect = (Rectangle*) elem; // points to the element
        insertBack(rectangles, (void*)rect); // insert into the rectangle list
    }

    int valid = getElementGroups(img->groups, rectangles, "rect");
    if (valid == 0) return NULL;

    return rectangles;

}

List* getCircles(const SVG* img){

    if (img == NULL) return NULL;

    List* circles = initializeList(&circleToString, &dummyDeleteCircle, &compareCircles);

    void* elem;
    ListIterator iter = createIterator(img->circles);
    while ((elem = nextElement(&iter)) != NULL){
        Circle* circ = (Circle*) elem;
        insertBack(circles, (void*)circ);
    }

    int valid = getElementGroups(img->groups, circles, "circ");
    if (valid == 0) return NULL;

    return circles;

}

List* getPaths(const SVG* img){

    if (img == NULL) return NULL;

    List* paths = initializeList(&pathToString, &dummyDeletePath, &comparePaths);

    void* elem;
    ListIterator iter = createIterator(img->paths);
    while ((elem = nextElement(&iter)) != NULL){
        Path* path = (Path*) elem;
        insertBack(paths, (void*)path);
    }

    int valid = getElementGroups(img->groups, paths, "path");
    if (valid == 0) return NULL;

    return paths;

}

List* getGroups(const SVG* img){ // again with these groups smh

    if (img == NULL) return NULL;

    List* groups = initializeList(&groupToString, &dummyDeleteGroup, &compareGroups);

    void* elem;
    ListIterator iter = createIterator(img->groups);
    while ((elem = nextElement(&iter)) != NULL){
        Group* group = (Group*) elem;
        insertBack(groups, (void*)group);
    }

    int valid = getElementGroups(img->groups, groups, "group");
    if (valid == 0) return NULL;

    return groups;

}

/**
 * The summaries functions have similar format. They:
 * 1. traverse shape list in img to compare with the search value
 * 2. call compareInGroups to:
 * 3. traverse groups shape list in img and
 * 4. recursively traverse groups groups shape list in img, and so on, until empty shape lists are reached
 */
int numRectsWithArea(const SVG* img, float area){

    if (img == NULL) return 0;

    int count = 0;
    int areaR = (int)(ceil(area));
    count = findNumShape(img->rectangles, &compareRectAreas, &areaR);
    count += compareInGroups(img->groups, &compareRectAreas, &areaR, "rect");
    return count;
}

int numCirclesWithArea(const SVG* img, float area){

    if (img == NULL) return 0;

    int count = 0;
    int areaC = (int)(ceil(area));
    count = findNumShape(img->circles, &compareCircAreas, &areaC);
    count += compareInGroups(img->groups, &compareCircAreas, &areaC, "circ");
    return count;
}

int numPathsWithdata(const SVG* img, const char* data){

    if (img == NULL) return 0;

    int count = 0;
    count = findNumShape(img->paths, &comparePathData, data);
    count += compareInGroups(img->groups, &comparePathData, data, "path");
    return count;
}

/** traverses the groups list in the svg and sums the length of their primitive lists */
int numGroupsWithLen(const SVG* img, int len){

    if (img == NULL) return 0;

    int count = 0;
    count = findNumShape(img->groups, &compareGroupLen, &len);
    return count;

}

/**
  * other attr traverses
  * rect, path, circ, etc traverse the list and then traverse the struct
 * 1. traverse each lists structs for the other attribute list length in the svg
 * 2. recursively travser the groups lists other attributes list
 */
int numAttr(const SVG* img){

    if (img == NULL) return 0;

    int count = 0;
    count = getLength(img->otherAttributes);
    count += getRectAttrLen(img->rectangles);
    count += getCircAttrLen(img->circles);
    count += getPathAttrLen(img->paths);
    count += getGroupAttrLen(img->groups);
    return count;

}
