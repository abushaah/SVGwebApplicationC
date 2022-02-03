// Name: Haifaa Abushaaban [1146372]
// Sample code "libXmlExample" used for understanding the traversal of tree from: http://xmlsoft.org/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGHelper.h"
#include "SVGParser.h"

#define DELIMITERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ !@#$%^&*()_~`{}|[]:\";',/<>?"
#define NUMDELIMITERS "0123456789.-"
#define STRSIZE 256
#define M_PI 3.14159265358979323846
// 0 means false!

// Module 1 helper functions:
/**
  * get element names traverses the tree and creates the svg by adding nodes to the list
  * caller must initialize the lists, root node, and svg
  */
int getElementNames(xmlNode* a_node, SVG* svg){

    if (a_node == NULL || svg == NULL){
        return 0;
    }
    if (svg->rectangles == NULL || svg->circles == NULL || svg->paths == NULL || svg->otherAttributes == NULL){
        return 0;
    }

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {

        char* name = (char*)(cur_node->name);

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

        }
        else if (strcasecmp(name, "circle") == 0){ // create new circle

            Circle* circ = circAttributes(cur_node); // fill in with attributes
            insertBack(svg->circles, (void*)circ); // insert into the circle list

        }
        else if (strcasecmp(name, "path") == 0){ // create new path

            Path* path = pathAttributes(cur_node); // fill in with attributes
            insertBack(svg->paths, (void*)path); // insert into the path list

        }
        else if (strcasecmp(name, "g") == 0){ // create new group

            Group *newGroup = groupAttributes(cur_node); // fill in with attributes (not other primitives)
            getElementNamesGroups(cur_node->children, newGroup);
            insertBack(svg->groups, (void*)newGroup);

        }
        else{ // just call attr with no argument and place in otherAttributes list
            firstOtherAttributes(cur_node, svg->otherAttributes); // fill in with attributes
        }

        // if its a group dont go to the children since we already traversed the children in the children in the getElementNamesGroups function
        if (strcasecmp(name, "g") != 0){
            getElementNames(cur_node->children, svg);
        }

    }

    return 1;

}

/**
 * this function will recursively go through a groups children and add them to the group
 * group is the group that the elements are added to
 */
void getElementNamesGroups(xmlNode* a_node, Group* group){

    if (a_node == NULL || group == NULL){ // base case
        return;
    }
    if (group->rectangles == NULL || group->circles == NULL || group->paths == NULL || group->otherAttributes == NULL){
        return;
    }

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {

        char* name = (char*)(cur_node->name);

        if (strcasecmp(name, "rect") == 0){ // create new rectangle
            Rectangle* rect = rectAttributes(cur_node); // fill in with attributes
            insertBack(group->rectangles, (void*)rect);
        }
        else if (strcasecmp(name, "circle") == 0){ // create new circle
            Circle* circ = circAttributes(cur_node); // fill in with attributes
            insertBack(group->circles, (void*)circ);
        }
        else if (strcasecmp(name, "path") == 0){ // create new path
            Path* path = pathAttributes(cur_node); // fill in with attributes
            insertBack(group->paths, (void*)path);
        }
        else if (strcasecmp(name, "g") == 0){ // create new group
            Group *newGroup = groupAttributes(cur_node); // fill in with attributes
            getElementNamesGroups(cur_node->children, newGroup);
            insertBack(group->groups, (void*)newGroup);
        }
        else{
            firstOtherAttributes(cur_node, group->otherAttributes);
        }
    }
}


/**
 * This function will return an attribute struct when given a node and its attributes
 * caller must free the node
 */
Attribute* otherAttributes (char *name, char *content){

    if (name == NULL || content == NULL) return NULL;

    Attribute* anAtr = malloc(sizeof(Attribute) + strlen(content) + 1); // 1 for null
    if (anAtr == NULL){
        return NULL;
    }
    anAtr->name = malloc(strlen(name) + 1); // 1 for null
    if (anAtr->name == NULL){
        free(anAtr);
        return NULL;
    }
    strcpy(anAtr->name, name);
    strcpy(anAtr->value, content);

    return anAtr;

}

/**
 * This function will add to the list of other attribute structures when given a node that doesnt belong to one of the geometric primitives
 */
void firstOtherAttributes(xmlNode *cur_node, List* otherAttributesList){ // giving the list only and not the svg

    if (cur_node == NULL || otherAttributesList == NULL) return;

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

    if (cur_node == NULL){
        return NULL;
    }

    // Iterate through every attribute of the current rectangle node
    xmlAttr *attr;
    Rectangle* rect = malloc(sizeof(Rectangle));
    if (rect == NULL){
        return NULL;
    }

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

    if (cur_node == NULL) return NULL;

    // Iterate through every attribute of the current circle node
    xmlAttr *attr;
    Circle* circ = malloc(sizeof(Circle));
    if (circ == NULL){
        return NULL;
    }

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

    if (cur_node == NULL) return NULL;

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    Path* path = malloc(sizeof(Path));
    if (path == NULL){
        return NULL;
    }

    path->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); // must initialize list, cannot be NULL but can be empty

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcasecmp(attrName, "d") == 0){ // path data
            path = realloc(path, sizeof(Path) + strlen(cont) + 1); // for the data
            if (path == NULL){
                return NULL;
            }
            strcpy(path->data, cont);
        }
        else{
            insertBack(path->otherAttributes, (void*)otherAttributes (attrName, cont)); // create a node and insert into the other attribute list
        }
    }

    return path;

}

Group* groupAttributes (xmlNode *cur_node){

    if (cur_node == NULL) return NULL;

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    Group* group = malloc(sizeof(Group));
    if (group == NULL){
        return NULL;
    }

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

    if (value == NULL) return 0;

    char* cpy = malloc (sizeof(value));
    if (cpy == NULL){
        return 0;
    }
    strcpy(cpy, value);

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
    if (dataCpy == NULL){
        return 0;
    }
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
    if (dataCpy == NULL){
        return 0;
    }
    strcpy(dataCpy, word);

    char* token = strtok(dataCpy, " \n\t"); // must not be empty

    if (token == NULL){
        free(dataCpy);
        return 0;
    }

    free(dataCpy);
    return 1;

}

// Module 2 helper functions:

/**
 * helper function for get uses 2 iterators: one that goes through the groups in the list one by one
 * one that goes through the rectangle list inside the groups
 */
int getElementGroups(List *source, List *dest, char* type){

    // a list of groups, a list for rectangle destination
    if (source == NULL || dest == NULL) return 0; // error case

    List* groupList = (List*) source;
    if (groupList == NULL){
        // base case - rectangles or groups list not initialized means that there are no more elements
        return 1;
    }

    // recursive case
    void* elemG;
    ListIterator iterG = createIterator(groupList); // traverse through the Group list
    while ((elemG = nextElement(&iterG)) != NULL){

        Group* group = (Group*) elemG;
        ListIterator iter;
        void* elem;

        // identify the iterator type for the list
        if (strcmp(type, "rect") == 0){
            iter = createIterator(group->rectangles);
        }
        else if (strcmp(type, "circ") == 0){ // repeat above for circle
            iter = createIterator(group->circles);
        }
        else if (strcmp(type, "path") == 0){
            iter = createIterator(group->paths);
        }
        else if (strcmp(type, "group") == 0){
            iter = createIterator(group->groups);
        }

        while ((elem = nextElement(&iter)) != NULL){
            insertBack(dest, elem); // insert into the specified list
        }

        int valid = getElementGroups(group->groups, dest, type);
        if (valid == 0) return 0;

    }

    return 1;
}

/*
    Development path:
    1. traverse through shape list for the item
    2. recursively traverse through the groups shape list for the item
*/

int findNumShape(List * list, bool (*customCompare)(const void* first, const void* second), const void* searchRecord){

    if (list == NULL || customCompare == NULL || searchRecord == NULL) return 0;

    int count = 0;
    ListIterator iter = createIterator(list);
    void* data;

    while ((data = nextElement(&iter)) != NULL){
        if (customCompare(data, searchRecord)){
            ++count;
        }
    }

    return count;

}

bool compareRectAreas(const void* data, const void* area){

    if (data == NULL || area == NULL) return 0;
    Rectangle* rect = (Rectangle*) data;
    int shapeArea = ceil(rect->width * rect->height);
    return (shapeArea == *(int*)area);

}

bool compareCircAreas(const void* data, const void* area){

    if (data == NULL || area == NULL) return 0;
    Circle* circ = (Circle*) data;
    int shapeArea = ceil(pow(circ->r, 2) * M_PI);
    return (shapeArea == *(int*)area);

}

bool comparePathData(const void* data, const void* string){

    if (data == NULL || string == NULL) return 0;
    Path* path = (Path*) data;
    return (!strcasecmp(path->data, (char*)string));

}

bool compareGroupLen(const void* data, const void* length){

    if (data == NULL || length == NULL) return 0;
    Group* group = (Group*) data;
    int count = 0;
    count += getLength(group->rectangles);
    count += getLength(group->circles);
    count += getLength(group->paths);
    count += getLength(group->groups);

    return (count == *(int*)length);

}

/**
 * the getShapeAttrLen(List* list) functions return the amount of attributes given in a shape list
 */
int getRectAttrLen(List* list){

    if (list == NULL) return 0;

    int count = 0;
    ListIterator iter = createIterator(list);
    Rectangle* rect;

    while ((rect = nextElement(&iter)) != NULL){
        count += getLength(rect->otherAttributes);
    }

    return count;
}

int getCircAttrLen(List* list){

    if (list == NULL) return 0;

    int count = 0;
    ListIterator iter = createIterator(list);
    Circle* circ;

    while ((circ = nextElement(&iter)) != NULL){
        count += getLength(circ->otherAttributes);
    }

    return count;
}

int getPathAttrLen(List* list){

    if (list == NULL) return 0;

    int count = 0;
    ListIterator iter = createIterator(list);
    Path* path;

    while ((path = nextElement(&iter)) != NULL){
        count += getLength(path->otherAttributes);
    }

    return count;
}

/**
 * the getGroupAttrLen(List* list) function returns the amount of attributes given in a group list and all its subgroups
 * it is a recursive function
 */
int getGroupAttrLen(List* list){

    if (list == NULL) return 0; // base case

    int count = 0;
    ListIterator iter = createIterator(list);
    Group* group;

    while ((group = nextElement(&iter)) != NULL){
        count += getLength(group->otherAttributes);
        count += getRectAttrLen(group->rectangles);
        count += getCircAttrLen(group->circles);
        count += getPathAttrLen(group->paths);
        count += getGroupAttrLen(group->groups);
    }

    return count;
}

/**
    Similar to get element groups, this funciton recursively computes the search for area/data/length in the groups
*/
int compareInGroups(List *group, bool (*customCompare)(const void* first, const void* second), const void* searchRecord, char* type){

    if (group == NULL || customCompare == NULL || searchRecord == NULL) return 0;
    // base case, not initialized

    List* groupList = group;

    int count = 0;

    // recursive case
    void* elemG;
    ListIterator iterG = createIterator(groupList); // traverse through the Group list
    while ((elemG = nextElement(&iterG)) != NULL){

        Group* group = (Group*) elemG;

        // identify the iterator type for the list
        if (strcmp(type, "rect") == 0){
            count += findNumShape(group->rectangles, customCompare, searchRecord);
        }
        else if (strcmp(type, "circ") == 0){ // repeat above for circle
            count += findNumShape(group->circles, customCompare, searchRecord);
        }
        else if (strcmp(type, "path") == 0){
            count += findNumShape(group->paths, customCompare, searchRecord);
        }
        count += compareInGroups(group->groups, customCompare, searchRecord, type);
    }

    return count;

}
