// Name: Hafaa Abushaaban [1146372]
// Sample code "libXmlExample" used for understanding the traversal of tree from: http://xmlsoft.org/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGHelper3.h"
#include "SVGParser.h"

#define DELIMITERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ !@#$%^&*()_+-=~`{}|[]:\";',/<>?"
#define NUMDELIMITERS "0123456789."
#define STRSIZE 256

// 0 means false!

void get_element_names(xmlNode* a_node, SVG* svg, Group** group, int* groupIdx, int* inserted){

    // groupIdx keeps track of the group depth
    /*
    Group**:
        * - first pointer is an array which points to multiple Group*
        * - second pointer are contents of the array, Group*
    */

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {

        char* name = (char*)(cur_node->name);

        char* parent = NULL;
        if (cur_node->parent != NULL){
            parent = (char*)(cur_node->parent->name); // if not the head node, there is a parent
        }

        char* nextParent = NULL;
        if (cur_node->next != NULL){
            nextParent = (char*)(cur_node->next->parent->name); // if not the head node node, there is a next singling
        }

        if (strcasecmp(name, "title") == 0){
            int valid = titleDescNS(svg->title, (char*)cur_node->children->content);
            if (valid == 0) strcpy(svg->title, "");
        }
        if (strcasecmp(name, "desc") == 0){
            int valid = titleDescNS(svg->description, (char*)cur_node->children->content);
            if (valid == 0) strcpy(svg->description, "");
        }

        // The primitives:
        if (strcasecmp(name, "rect") == 0){ // create new rectangle

            Rectangle* rect = rectAttributes(cur_node); // fill in with attributes
            insertBack(svg->rectangles, (void*)rect); // insert into the rectangle list

            if (parent != NULL && strcasecmp(parent, "g") == 0){ // if it belongs to a group, place in group
                Rectangle* cpy = rectAttributes(cur_node); // create a copy to add to another list
                insertBack((group[*groupIdx])->rectangles, (void*)cpy);
            }

        }
        else if (strcasecmp(name, "circle") == 0){ // create new circle

            Circle* circ = circAttributes(cur_node); // fill in with attributes
            insertBack(svg->circles, (void*)circ); // insert into the circle list

            if (parent != NULL && strcasecmp(parent, "g") == 0){ // if it belongs to a group, place in group
                Circle* cpy = circAttributes(cur_node);
                insertBack((group[*groupIdx])->circles, (void*)cpy);
            }

        }
        else if (strcasecmp(name, "path") == 0){ // create new path

            Path* path = pathAttributes(cur_node); // fill in with attributes
            insertBack(svg->paths, (void*)path); // insert into the path list

            if (parent != NULL && strcasecmp(parent, "g") == 0){ // if it belongs to a group, place in group
                Path* cpy = pathAttributes(cur_node);
                insertBack((group[*groupIdx])->paths, (void*)cpy);
            }

        }
        else if (strcasecmp(name, "g") == 0){ // create new group

            Group *newGroup = groupAttributes(cur_node); // fill in with attributes (not other primitives)
            *inserted = 0; // cannot insert into the svg->groups list since it is not complete

            if (parent != NULL && strcasecmp(parent, "g") == 0){ // if it belongs to a group, place in the previous group
                insertBack((group[*groupIdx])->groups, (void*)newGroup);
                ++*groupIdx; // depth of group has increased
            }
            group[*groupIdx] = newGroup;
        }
        else{ // just call attr with no argument and place in otherAttributes list
            firstOtherAttributes(cur_node, svg->otherAttributes); // fill in with attributes
        }

        if (((parent != NULL) && (strcasecmp(parent, "g") == 0)) && ((nextParent == NULL) || ((strcasecmp(nextParent, "svg") == 0)))){

            /* if the current elements parent is a g
               and
               the prev nodes parent is not in a group anymore
               then
               we shall place the original group into the group list in the svg
               and we should reset the group variable
            */
            if (*inserted == 0){
                insertBack(svg->groups, (void*)group[0]); // insert the group list (when parent is g and next parent is svg, that means the group tag has finished and we must insert it into the list
                *inserted = 1; // have inserted it
                *groupIdx = 0; // group has ended and we are going to be a child of svg
            }
        }

        get_element_names(cur_node->children, svg, group, groupIdx, inserted);

    }

}

/**
 * This function will return an attribute struct when given a node and its attributes
 * caller must free the node
 */
Attribute* otherAttributes (char *name, char *content){

    Attribute* anAtr = malloc(sizeof(Attribute) + strlen(content) + 1); // 1 for null
    anAtr->name = malloc(strlen(name) + 1); // 1 for null
    strcpy(anAtr->name, name);
    strcpy(anAtr->value, content);

    return anAtr;

}

/**
 * This function will add to the list of other attribute structures when given a node that doesnt belong to one of the geometric primitives
 */
void firstOtherAttributes(xmlNode *cur_node, List* otherAttributesList){ // giving the list only and not the svg

    // Iterate through every attribute of the current node
    xmlAttr *attr;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        insertBack(otherAttributesList, (void*)otherAttributes (attrName, cont)); // create a node and insert into the other attribute list
    }

}

/**
 * This function will return a rectangle struct with its attributes when given a node
 * caller must free the node
 */
Rectangle* rectAttributes(xmlNode *cur_node){ // fills in attributes for a rectangle

    // Iterate through every attribute of the current rectangle node
    xmlAttr *attr;
    Rectangle* rect = malloc(sizeof(Rectangle));

    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); // must initialize list, cannot be NULL but can be empty
    strcpy(rect->units, ""); // initialize units

    int valid[4] = {0, 0, 0, 0}; // x, y, w, h, if one of these is 1 in the end, then it is not valid and must be set to default value

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char*) (attr->name);
        char *cont = (char*) (value->content);

        if (strcasecmp(attrName, "x") == 0){
            valid[0] = numberWithUnits(&(rect->x), rect->units, cont);
        }
        else if (strcasecmp(attrName, "y") == 0){
            valid[1] = numberWithUnits(&(rect->y), rect->units, cont);
        }
        else if (strcasecmp(attrName, "width") == 0){
            valid[2] = numberWithUnits(&(rect->width), rect->units, cont);
        }
        else if (strcasecmp(attrName, "height") == 0){
            valid[3] = numberWithUnits(&(rect->height), rect->units, cont);
        }
        else{ // place in otherAttributes list
          insertBack(rect->otherAttributes, (void*)otherAttributes (attrName, cont)); // create an attribute node and place it in the list
        }
    }

    // if units are emtpy, emtpy string, etc assumes that all units are the same if specified
    if (valid[0] == 0) rect->x = 0; // x is not specified
    if (valid[1] == 0) rect->y = 0; // y is not specified
    if (valid[2] == 0) rect->width = 0; // w is not specified
    if (valid[3] == 0) rect->height = 0; // h is not specified

    return rect;
}

/**
 * This function will return a circle struct with its attributes when given a node
 * caller must free the node
 */
Circle* circAttributes(xmlNode *cur_node){ // fills in attributes for a rectangle

    // Iterate through every attribute of the current circle node
    xmlAttr *attr;
    Circle* circ = malloc(sizeof(Circle));

    circ->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); // must initialize list, cannot be NULL but can be empty
    strcpy(circ->units, "");

    int valid[3] = {0, 0, 0}; // x, y, r if one of these is 1 in the end, then it is not valid and must be set to default value

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char*)(attr->name);
        char *cont = (char*)(value->content);

        if (strcasecmp(attrName, "cx") == 0){
            valid[0] = numberWithUnits(&(circ->cx), circ->units, cont);
        }
        else if (strcasecmp(attrName, "cy") == 0){
            valid[1] = numberWithUnits(&(circ->cy), circ->units, cont);
        }
        else if (strcasecmp(attrName, "r") == 0){
            valid[2] = numberWithUnits(&(circ->r), circ->units, cont);
        }
        else{ // place in otherAttributes list
          insertBack(circ->otherAttributes, (void*)otherAttributes (attrName, cont)); // create an attribute node and place it in the list
        }
    }

    // if units are emtpy, emtpy string, etc assumes that all units are the same if specified
    if (valid[0] == 0) circ->cx = 0; // x is not specified
    if (valid[1] == 0) circ->cy = 0; // y is not specified
    if (valid[2] == 0) circ->r = 0; // r is not specified

    return circ;
}

/**
 * This function will return an attribute struct when given a node and its attributes
 * caller must free the node
 */
Path* pathAttributes (xmlNode *cur_node){

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    Path* path = malloc(sizeof(Path));

    path->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); // must initialize list, cannot be NULL but can be empty

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcasecmp(attrName, "d") == 0){ // path data
            path = realloc(path, sizeof(Path) + strlen(cont) + 1); // for the data
            strcpy(path->data, cont);
        }
        else{
            insertBack(path->otherAttributes, (void*)otherAttributes (attrName, cont)); // create a node and insert into the other attribute list
        }
    }

    return path;

}

Group* groupAttributes (xmlNode *cur_node){

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    Group* group = malloc(sizeof(Group));

    group->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    group->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    group->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    group->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    group->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        insertBack(group->otherAttributes, (void*)otherAttributes (attrName, cont)); // create a node and insert into the other attribute list
    }

    return group;

}

/**
 * This function separates the units from the floating number
 * @return 0 when no value is given
 */
int numberWithUnits(float* number, char* units, char* value){

    char* cpy = malloc (sizeof(value));
    strcpy(cpy, value);

    if (value == NULL) return 0;

    char* token = strtok(value, DELIMITERS);

    if (token == NULL) return 0; // no number found

    *number = atof (token); // if the number happens to be just '.', then the atof function will return 0.0

    token = strtok(cpy, NUMDELIMITERS);
    if (token != NULL) strcpy(units, token); // token not being null means there is a character specified after a number

    free(cpy);
    return 1; // valid
}

/**
 * This function returns whether a string is not empty or not
 * @return 0 when it is empty
 */
int validChar(char* word){ // checks if a character is valid

    if (word == NULL || word[0] == '\0' || word[0] == '\n') return 0; // 0 indicates false, not valid
    return 1;
}

/**
 * verifies the data fits into the fields
 * if it exceeds, truncuates the data to fit
 */
int titleDescNS(char *field, char* data){

    if (data == NULL) return 0;

    char *dataCpy = malloc(strlen(data) + 1); // must create a copy since it is a constant and cannot be changed
    strcpy(dataCpy, data);

    int dLength = strlen(data) + 1;

    if (validChar(dataCpy) == 0) return 0;
    if (emptyString(dataCpy) == 0) return 0;

    if (STRSIZE < dLength){ // data is greater than allocated
        dataCpy[STRSIZE - 1] = '\0';
    }

    strcpy(field, dataCpy);
    free(dataCpy);
    return 1;
}

int emptyString(char* word){ // checks if a character is valid

    if (word == NULL) return 0;

    char *dataCpy = malloc(strlen(word) + 1); // must create a copy since it is a constant and cannot be changed
    strcpy(dataCpy, word);

    char* token = strtok(dataCpy, " \n\t"); // must not be empty

    if (token == NULL){
        free(dataCpy);
        return 0;
    }

    free(dataCpy);
    return 1;

}
