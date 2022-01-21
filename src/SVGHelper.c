// Haifaa Abushaaban [1146372]

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGParser.h"
#include "SVGHelper.h"

#define DELIMITERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ -=:;"
#define NUMDELIMITERS "0123456789."

// 0 means false!

void get_element_names(xmlNode * a_node, SVG * svg){

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {

        char* name = (char*)(cur_node->name);
        char* content = (char*)(cur_node->content);

        if (strcasecmp(name, "title") == 0){
            verifyCopy(svg->title, content, sizeof(svg->title), sizeof(cur_node->content));
            // title attributes are in the otherAttributes list
        }
        else if (strcasecmp(name, "desc") == 0){
            verifyCopy(svg->description, content, sizeof(svg->description), sizeof(cur_node->content));
            // desc attributes are in the otherAttributes list
        }
        else if (strcasecmp(name, "rect") == 0){ // create new rectangle
            Rectangle* rect = rectAttributes(cur_node); // fill in with attributes
            insertBack(svg->rectangles, (void*)rect); // insert into the rectangle list
        }
        else if (strcasecmp(name, "circle") == 0){ // create new circle
            Circle* circ = circAttributes(cur_node); // fill in with attributes
            insertBack(svg->circles, (void*)circ); // insert into the circle list
        }
        // repeat the same for circle, path, groups
        else{ // just call attr with no argument and place in otherAttributes list
            firstOtherAttributes(cur_node, svg->otherAttributes); // fill in with attributes
        }

        get_element_names(cur_node->children, svg);
    }

    // keep count of the number of elements created, since if it is 0 must create a new empty struct to avoid it being null

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
 * This function will return a rectangle struct with its attributes when given a node
 * caller must free the node
 */

Rectangle* rectAttributes(xmlNode *cur_node){ // fills in attributes for a rectangle

    // Iterate through every attribute of the current rectangle node
    xmlAttr *attr;
    Rectangle* rect = malloc(sizeof(Rectangle));

    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); // must initialize list, cannot be NULL but can be empty

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
    if (validChar(rect->units) == 0) strcpy(rect->units, ""); // u is not specified

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
    if (validChar(circ->units) == 0) strcpy(circ->units, ""); // u is not specified

    return circ;
}

void firstOtherAttributes(xmlNode *cur_node, List* otherAttributesList){ // giving the list only and not the svg

    // Iterate through every attribute of the current node
    xmlAttr *attr;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        insertBack(otherAttributesList, (void*)otherAttributes (attrName, cont)); // insert into the other attribute list
    }

}

/**
 * verifyCopy verifies the data fits into the fields
 * if it exceeds, truncuates the data to fit
 */
void verifyCopy(char* field, char* data, size_t fLength, size_t dLength){

    if (data == NULL){
        strcpy(field, "");
        return;
    }

    if (fLength < dLength){ // data is greater than allocated
        data[fLength - 1] = '\0';
    }

    strcpy(field, data);
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

    *number = atof (token);

    token = strtok(cpy, NUMDELIMITERS);
    if (token != NULL) strcpy(units, token);

    free(cpy);
    return 1; // valid
}

/**
 * This function returns whether a string is not empty
 * @return 0 when it is empty
 */

int validChar(char* word){ // checks if a character is valid

    if (word == NULL || word[0] == '\0' || word[0] == '\n' || strcmp(word, "") == 0) return 0; // 0 indicates false, not valid
    return 1;
}

/*
int nameSpace(char *field, const xmlChar * data, size_t fLength, size_t dLength){

    char *dataCpy = malloc(sizeof(data)); // must create a copy since it is a constant and cannot be changed
    strcpy(data, dataCpy);

    if (dataCpy == NULL || dataCpy[0] == '\0' || dataCpy[0] == '\n' || strcmp(dataCpy, "") == 0) return 0;

    if (fLength < dLength){ // data is greater than allocated
        dataCpy[fLength - 1] = '\0';
    }

    strcpy(field, data);
    free(dataCpy);

}
*/
