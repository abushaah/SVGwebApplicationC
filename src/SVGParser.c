// Name: Hafaa Abushaaban [1146372]

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGHelper3.h"
#include "SVGParser.h"

// REMEMBER TO REMOVE THE MAIN AND PRINT STATMENTS!

int main(void){

    SVG* svg = createSVG("quad02.svg");

    if (svg == NULL){
        printf("invalid svg file\n");
        return 0;
    }

    char* svgString = SVGToString(svg);
    printf("%s\n", svgString);
    free(svgString);

    deleteSVG(svg);

    return 0;

}

SVG* createSVG(const char* filename){

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVG* svg = (SVG*) (malloc(sizeof(SVG))); // CHECK MALLOC FOR FAIL!

    int valid = 0; // 0 means false, 1 means true

    if (!filename) return NULL; // file is not given

    LIBXML_TEST_VERSION

    doc = xmlReadFile(filename, NULL, 0); // parse the file and get the DOM

    if (doc == NULL){
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc); // root element node

    if (strcasecmp(root_element->name, "svg") != 0){
        return NULL;
    }

/*
    valid = nameSpace(svg->namespace, root_element->ns->href, sizeof(svg->namespace), sizeof(root_element->ns->href)); // funciton to create namespace (must)
    if (!valid){
        //deleteAll();
        return NULL;
    }
*/

    // must initialize all svg contents, may not be empty
    strcpy(svg->namespace, "");
    strcpy(svg->title, "");
    strcpy(svg->description, "");
    svg->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    svg->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    svg->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    svg->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    svg->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    Group **group;
    int groupIdx = 0;
    int inserted = 0;

    group = malloc(sizeof(Group*) * (groupIdx + 1)); // group that will point to Group*, start with it pointing to one only

    if (group == NULL){
        return NULL;
    }

    get_element_names(root_element, svg, group, &groupIdx, &inserted);
    // root element is the root node, svg is the svg tree, group is the first layer of a group, 0 indicates a layer of 0 groups

    free(group);

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

    len = strlen(rectList) + strlen(circList) + strlen(pathList) + strlen(groupList) + strlen(attrList) + 6; // 5 '\n', \0
    tmpStr = (char*) malloc(sizeof(char) * len);
    sprintf(tmpStr, "%s\n%s\n%s\n%s\n%s\n", rectList, circList, pathList, groupList, attrList);

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
